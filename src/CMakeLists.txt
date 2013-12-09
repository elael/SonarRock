rock_library(osgSonartest
    SOURCES SonarSimple.cpp
    HEADERS SonarSimple.hpp
    DEPS_PKGCONFIG openscenegraph)

rock_executable(osgSonartest_bin SonarSimple.cpp
    DEPS osgSonartest
    LIBS curses
    DEPS_PKGCONFIG openscenegraph)

