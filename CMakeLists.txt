project(sc2switcher)

include_directories(${OBS_JANSSON_INCLUDE_DIRS})

set(sc2switcher_HEADERS
	${sc2switcher_HEADERS}
	auto-scene-switcher.hpp
	tool-helpers.hpp
	)
set(sc2switcher_SOURCES
	${sc2switcher_SOURCES}
	sc2switcher.c
	auto-scene-switcher.cpp
	)
set(sc2switcher_UI
	${sc2switcher_UI}
	forms/auto-scene-switcher.ui
	)
set(sc2switcher_PLATFORM_LIBS
	${OBS_JANSSON_IMPORT}
	)

qt5_wrap_ui(sc2switcher_UI_HEADERS
	${sc2switcher_UI}
	${sc2switcher_PLATFORM_UI})

add_library(sc2switcher MODULE
	${sc2switcher_HEADERS}
	${sc2switcher_SOURCES}
	${sc2switcher_UI_HEADERS}
	${sc2switcher_PLATFORM_SOURCES}
	${sc2switcher_PLATFORM_HEADERS}
	)
target_link_libraries(sc2switcher
	${sc2switcher_PLATFORM_LIBS}
	obs-frontend-api
	Qt5::Widgets
	libobs
    ${LIBCURL_LIBRARIES})

install_obs_plugin(sc2switcher)
