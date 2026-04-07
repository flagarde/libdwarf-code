set_source_group(SOURCES "Source Files"
"${PROJECT_SOURCE_DIR}/src/lib/libdwarf/dwarf_memcpy_swap.c"
"${PROJECT_SOURCE_DIR}/src/lib/libdwarf/dwarf_string.c"
"${PROJECT_SOURCE_DIR}/src/lib/libdwarf/dwarf_tsearchhash.c"
dwarf_pro_alloc.c
dwarf_pro_arange.c
dwarf_pro_debug_sup.c
dwarf_pro_die.c
dwarf_pro_dnames.c
dwarf_pro_error.c
dwarf_pro_expr.c
dwarf_pro_finish.c
dwarf_pro_forms.c
dwarf_pro_funcs.c
dwarf_pro_frame.c
dwarf_pro_init.c
dwarf_pro_line.c
dwarf_pro_reloc.c
dwarf_pro_reloc_stream.c
dwarf_pro_reloc_symbolic.c
dwarf_pro_pubnames.c
dwarf_pro_section.c
dwarf_pro_types.c
dwarf_pro_vars.c
dwarf_pro_macinfo.c
dwarf_pro_weaks.c)

set_source_group(HEADERS "Header Files"
libdwarfp.h
dwarf_pro_alloc.h
dwarf_pro_arange.h
dwarf_pro_die.h
dwarf_pro_error.h
dwarf_pro_expr.h
dwarf_pro_frame.h
dwarf_pro_incl.h
dwarf_pro_line.h
dwarf_pro_log_extra_flag_strings.c
dwarf_pro_macinfo.h
dwarf_pro_opaque.h
dwarf_pro_reloc.h
dwarf_pro_reloc_stream.h
dwarf_pro_reloc_symbolic.h
dwarf_pro_section.h
dwarf_pro_types.h
dwarf_pro_util.h)

add_library(dwarfp-shared SHARED ${SOURCES} ${HEADERS})
add_library(libdwarf::dwarfp-shared ALIAS dwarfp-shared)
target_link_libraries(dwarfp-shared PRIVATE dwarf-shared)
set_target_properties(dwarfp-shared PROPERTIES
                                   OUTPUT_NAME dwarfp
                                   PUBLIC_HEADER "libdwarfp.h"
                                   VERSION "${PROJECT_VERSION}"
                                   SOVERSION "${PROJECT_VERSION_MAJOR}"
                                   FOLDER src/lib/libdwarfp
                                   C_VISIBILITY_PRESET hidden
                                   POSITION_INDEPENDENT_CODE ON
                                   )
target_compile_options(dwarfp-shared PRIVATE ${COMPILER_FLAGS} ${DW_FWALL} )
target_compile_definitions(dwarfp-shared PUBLIC _GNU_SOURCE LIBDWARF_BUILD PIC)
target_include_directories(dwarfp-shared PUBLIC
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/lib/libdwarfp> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                          )
msvc_posix(dwarfp-shared)

add_library(dwarfp-static STATIC ${SOURCES} ${HEADERS} ${CONFIGURATION_FILES})
add_library(libdwarf::dwarfp-static ALIAS dwarfp-static)
target_link_libraries(dwarfp-static PRIVATE dwarf-static)
set_target_properties(dwarfp-static PROPERTIES OUTPUT_NAME dwarfp
                                              PUBLIC_HEADER "libdwarfp.h"
                                              FOLDER src/lib/libdwarfp
                                              POSITION_INDEPENDENT_CODE $<BOOL:${PIC_ALWAYS}>
                                              )
target_compile_definitions(dwarfp-static PRIVATE _GNU_SOURCE)
target_compile_options(dwarfp-static PRIVATE ${COMPILER_FLAGS} ${DW_FWALL} )
target_compile_definitions(dwarfp-static PUBLIC _GNU_SOURCE LIBDWARF_STATIC $<$<BOOL:${PIC_ALWAYS}>:PIC>)
target_include_directories(dwarfp-static PUBLIC
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/lib/libdwarfp> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                          )
msvc_posix(dwarfp-static)

install(TARGETS dwarfp-static dwarfp-shared
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        )

# The install has to be here, not in
# ../CMakeLists.txt  to make install work properly
# for cmake before cmake 3.13. This also works
# for newer cmake.
configure_file(libdwarfp.pc.in libdwarfp.pc @ONLY)

install(TARGETS dwarfp-static dwarfp-shared  EXPORT libdwarfpTargets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        )
install(EXPORT libdwarfpTargets FILE libdwarfp-targets.cmake
        NAMESPACE libdwarfp::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libdwarfp")
install(FILES cmake/libdwarfp-config.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libdwarfp")
install(FILES "${PROJECT_BINARY_DIR}/src/lib/libdwarfp/libdwarfp.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
