set_source_group(SOURCES "Source Files" createirepformfrombinary.cc
    createirepfrombinary.cc
    dwarfgen.cc irepattrtodbg.cc ireptodbg.cc
    dg_getopt.cc)

set_source_group(HEADERS "Header Files" createirepfrombinary.h
    general.h irepattrtodbg.h
    irepdie.h irepform.h irepframe.h
    irepline.h irepmacro.h ireppubnames.h
    irepresentation.h ireptodbg.h strtabdata.h
    ../../lib/libdwarf/libdwarf_private.h
    dg_getopt.h
    ../../lib/libdwarf/dwarf.h
    ../../lib/libdwarfp/libdwarfp.h)

set_source_group(CONFIGURATION_FILES "Configuration Files"
    ${PROJECT_SOURCE_DIR}/cmake/config.h.in
    ${PROJECT_BINARY_DIR}/config.h)

add_executable(dwarfgen ${SOURCES} ${HEADERS} ${CONFIGURATION_FILES})

set_folder(dwarfgen src/bin/dwarfgen)
dwarf_common_options(dwarfgen)

target_link_libraries(dwarfgen PRIVATE dwarfp-static dwarf-static)

install(TARGETS dwarfgen DESTINATION
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

#install(FILES dwarfgen.1 DESTINATION share/man/man1)
