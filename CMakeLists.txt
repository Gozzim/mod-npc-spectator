# Add script files
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/ArenaSpectatorNPC.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/ArenaSpectatorNPC.h")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/ArenaSpectatorNPC_SC.cpp")

# Add scripts to script loader
AC_ADD_SCRIPT_LOADER("ArenaSpectatorNPC" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")