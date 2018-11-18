project(sc2switcher3)
include_directories(${OBS_JANSSON_INCLUDE_DIRS})

set(sc2switcher3_HEADERS
	${sc2switcher3_HEADERS}
	SC2Data.h
	Observer.h
	SC2State.h
	Constants.h
	SceneSwitcher.h
	ScoreTracker.h
	Webhook.h
	Config.h
	forms/SettingsDialog.h
	log.h
	)
set(sc2switcher3_SOURCES
	${sc2switcher3_SOURCES}
	sc2switcher.cpp
	SC2Data.cpp
	SC2State.cpp
	SceneSwitcher.cpp
	ScoreTracker.cpp
	Webhook.cpp
	Config.cpp
	forms/SettingsDialog.cpp
	)
set(sc2switcher3_UI
	${sc2switcher3_UI}
	forms/SettingsDialog.ui
	)
set(sc2switcher3_PLATFORM_LIBS
	${OBS_JANSSON_IMPORT}
	)

qt5_wrap_ui(sc2switcher3_UI_HEADERS
	${sc2switcher3_UI}
	${sc2switcher3_PLATFORM_UI})

add_library(sc2switcher3 MODULE
	${sc2switcher3_HEADERS}
	${sc2switcher3_SOURCES}
	${sc2switcher3_UI_HEADERS}
	${sc2switcher3_PLATFORM_SOURCES}
	${sc2switcher3_PLATFORM_HEADERS}
	)
target_link_libraries(sc2switcher3
	${sc2switcher3_PLATFORM_LIBS}
	obs-frontend-api
	Qt5::Widgets
	${LIBCURL_LIBRARIES}
	libobs)

install_obs_plugin(sc2switcher3)
