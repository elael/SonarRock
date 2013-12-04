rock_library(osgSonartest
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(osgSonartest_bin Main.cpp
    DEPS osgSonartest
    LIBS curses
    DEPS_PKGCONFIG openscenegraph)

