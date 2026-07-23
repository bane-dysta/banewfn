set(BANEWFN_DEFAULT_MULTIWFN_EXEC "Multiwfn" CACHE STRING
    "Default Multiwfn executable written to generated banewfn.rc")
set(BANEWFN_DEFAULT_CORES "32" CACHE STRING
    "Default core count written to generated banewfn.rc")

# Windows release/installer staging intentionally keeps the historical portable
# layout consumed by packaging/windows/inno/banewfn.iss. Native Unix installs
# use the normal FHS-style layout.
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(BANEWFN_INSTALL_BINDIR ".")
    set(BANEWFN_INSTALL_CONFDIR "conf")
    set(BANEWFN_INSTALL_SCRIPTSDIR "scripts")
    set(BANEWFN_INSTALL_DOCDIR "docs/out")
    set(BANEWFN_INSTALL_RCDIR ".")
    set(BANEWFN_GENERATED_CONFPATH "conf")
else()
    set(BANEWFN_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}")
    set(BANEWFN_INSTALL_CONFDIR "${CMAKE_INSTALL_DATADIR}/banewfn/conf")
    set(BANEWFN_INSTALL_SCRIPTSDIR "${CMAKE_INSTALL_DATADIR}/banewfn/scripts")
    set(BANEWFN_INSTALL_DOCDIR "${CMAKE_INSTALL_DOCDIR}")
    set(BANEWFN_INSTALL_RCDIR "${CMAKE_INSTALL_SYSCONFDIR}/banewfn")
    set(BANEWFN_GENERATED_CONFPATH "${CMAKE_INSTALL_FULL_DATADIR}/banewfn/conf")
endif()

set(BANEWFN_GENERATED_RC "${CMAKE_CURRENT_BINARY_DIR}/banewfn.rc")
file(WRITE "${BANEWFN_GENERATED_RC}"
    "Multiwfn_exec=${BANEWFN_DEFAULT_MULTIWFN_EXEC}\n"
    "confpath=${BANEWFN_GENERATED_CONFPATH}\n"
    "cores=${BANEWFN_DEFAULT_CORES}\n"
    "citations_output=references.bib\n"
)

install(TARGETS banewfn bwpack
    RUNTIME DESTINATION "${BANEWFN_INSTALL_BINDIR}"
)

if(IS_DIRECTORY "${BANEWFN_PACKAGE_CONF_SOURCE}")
    install(DIRECTORY "${BANEWFN_PACKAGE_CONF_SOURCE}/"
        DESTINATION "${BANEWFN_INSTALL_CONFDIR}"
    )
    install(FILES "${BANEWFN_GENERATED_RC}"
        DESTINATION "${BANEWFN_INSTALL_CONFDIR}"
    )
else()
    message(WARNING "banewfn: conf source directory does not exist and will not be installed: ${BANEWFN_PACKAGE_CONF_SOURCE}")
endif()

install(FILES "${BANEWFN_GENERATED_RC}"
    DESTINATION "${BANEWFN_INSTALL_RCDIR}"
)

if(IS_DIRECTORY "${BANEWFN_PACKAGE_SCRIPT_SOURCE}")
    install(DIRECTORY "${BANEWFN_PACKAGE_SCRIPT_SOURCE}/"
        DESTINATION "${BANEWFN_INSTALL_SCRIPTSDIR}"
    )
else()
    message(WARNING "banewfn: scripts source directory does not exist and will not be installed: ${BANEWFN_PACKAGE_SCRIPT_SOURCE}")
endif()

if(IS_DIRECTORY "${BANEWFN_PACKAGE_DOC_SOURCE}")
    file(GLOB BANEWFN_PACKAGE_MANUALS "${BANEWFN_PACKAGE_DOC_SOURCE}/${BANEWFN_PACKAGE_DOC_PATTERN}")
    if(BANEWFN_PACKAGE_MANUALS)
        install(DIRECTORY "${BANEWFN_PACKAGE_DOC_SOURCE}/"
            DESTINATION "${BANEWFN_INSTALL_DOCDIR}"
            FILES_MATCHING
            PATTERN "${BANEWFN_PACKAGE_DOC_PATTERN}"
        )
    else()
        message(WARNING "banewfn: no manuals matched ${BANEWFN_PACKAGE_DOC_PATTERN} under ${BANEWFN_PACKAGE_DOC_SOURCE}; docs will be empty in packages")
    endif()
else()
    message(WARNING "banewfn: docs source directory does not exist and will not be installed: ${BANEWFN_PACKAGE_DOC_SOURCE}")
endif()
