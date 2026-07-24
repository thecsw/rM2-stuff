find_package(PkgConfig)
# libsystemd is only used by the rm2fb server, not by rMlib/yaft. Make it
# optional so cross-builds (and minimal toolchains without a usable
# libsystemd pkg-config) still configure. Targets that actually need it
# (rm2fb_server) will fail at link time if it is genuinely missing.
pkg_check_modules(SYSTEMD libsystemd)

add_library(Systemd::Lib INTERFACE IMPORTED GLOBAL)
if(SYSTEMD_FOUND)
  set_target_properties(
    Systemd::Lib
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SYSTEMD_INCLUDE_DIRS}"
              INTERFACE_LINK_LIBRARIES "${SYSTEMD_LIBRARIES}")
endif()
