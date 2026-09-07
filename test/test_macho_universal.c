/*   This test code is hereby placed in the public domain. */

/*  Regression test for the Mach-O universal ("fat") binary header
    reader.

    _dwarf_object_detector_universal_head_fd() read the 64-bit arch
    table with sizeof(fa), the size of the pointer, rather than
    sizeof(*fa), the 32 bytes of struct fat_arch_64.  Only the first
    quarter of the table was filled, offset and size came back as the
    zeros calloc left, and no FAT_MAGIC_64 universal binary could be
    read at all.

    The two files written here describe the same inner object at the
    same offset with the same size, once with FAT_MAGIC and 32-bit
    entries and once with FAT_MAGIC_64 and 64-bit ones, so
    dwarf_init_path_a() has to answer the same way for both.  */

#include <config.h>
#include <stdio.h>  /* fopen fclose fwrite remove printf */
#include <string.h> /* memset */
#include "dwarf.h"
#include "libdwarf.h"

#define FAT_MAGIC     0xcafebabe
#define FAT_MAGIC_64  0xcafebabf
#define MH_MAGIC_64   0xfeedfacf
#define PAYLOAD_OFF   4096
#define PAYLOAD_LEN   232

static void
put_be32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >>  8);
    p[3] = (unsigned char)(v);
}

static void
put_le32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)(v);
    p[1] = (unsigned char)(v >>  8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

/*  Writes a one-architecture universal binary.  With FAT_MAGIC_64 the
    arch entry carries 64-bit offset and size fields, so the entry is
    32 bytes instead of 20.  */
static int
write_fat(const char *path, int is64)
{
    unsigned char buf[PAYLOAD_OFF + PAYLOAD_LEN];
    unsigned char *p = buf;
    FILE *f = 0;
    size_t n = 0;

    memset(buf, 0, sizeof(buf));
    put_be32(p, is64 ? FAT_MAGIC_64 : FAT_MAGIC);
    put_be32(p + 4, 1);                    /* nfat_arch */
    p += 8;
    put_be32(p, 0x0100000c);               /* cputype   */
    put_be32(p + 4, 0);                    /* cpusubtype*/
    if (is64) {
        put_be32(p +  8, 0);               /* offset, high word */
        put_be32(p + 12, PAYLOAD_OFF);     /* offset, low word  */
        put_be32(p + 16, 0);               /* size, high word   */
        put_be32(p + 20, PAYLOAD_LEN);     /* size, low word    */
    } else {
        put_be32(p +  8, PAYLOAD_OFF);
        put_be32(p + 12, PAYLOAD_LEN);
    }
    /*  A minimal 64-bit Mach-O so the arch entry points at something
        with a recognizable magic number. */
    p = buf + PAYLOAD_OFF;
    put_le32(p, MH_MAGIC_64);
    put_le32(p + 4, 0x0100000c);

    f = fopen(path, "wb");
    if (!f) {
        return 1;
    }
    n = fwrite(buf, 1, sizeof(buf), f);
    fclose(f);
    return n == sizeof(buf) ? 0 : 1;
}

static int
open_it(const char *path, int *errnum)
{
    Dwarf_Debug dbg = 0;
    Dwarf_Error err = 0;
    char tp[2048];
    int res = 0;

    *errnum = 0;
    res = dwarf_init_path_a(path, tp, sizeof(tp), DW_GROUPNUMBER_ANY,
        0, 0, 0, &dbg, &err);
    if (res == DW_DLV_ERROR) {
        *errnum = (int)dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
    }
    if (res == DW_DLV_OK) {
        dwarf_finish(dbg);
    }
    return res;
}

int
main(void)
{
    const char *f32 = "test_macho_universal_32.bin";
    const char *f64 = "test_macho_universal_64.bin";
    int res32 = 0;
    int res64 = 0;
    int err32 = 0;
    int err64 = 0;
    int failcount = 0;

    if (write_fat(f32, 0) || write_fat(f64, 1)) {
        printf("FAIL test_macho_universal: cannot write test files\n");
        return 1;
    }

    res32 = open_it(f32, &err32);
    res64 = open_it(f64, &err64);

    if (res32 != res64 || err32 != err64) {
        printf("FAIL test_macho_universal: FAT_MAGIC gives res %d err %d "
            "but FAT_MAGIC_64 gives res %d err %d. "
            "The 64-bit arch table was misread.\n",
            res32, err32, res64, err64);
        ++failcount;
    }
    remove(f32);
    remove(f64);
    if (failcount) {
        return 1;
    }
    printf("PASS test_macho_universal\n");
    return 0;
}
