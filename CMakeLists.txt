project(sc2switcher3)

add_library(sc2switcher3 MODULE)
add_library(OBS::sc2switcher3 ALIAS sc2switcher3)

find_qt(COMPONENTS Widgets)
find_package(CURL REQUIRED)

set_target_properties(
  sc2switcher3
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON
             AUTOUIC_SEARCH_PATHS "forms")

target_sources(sc2switcher3 PRIVATE forms/SettingsDialog.ui)

target_sources(
  sc2switcher3
  PRIVATE
  	SC2Data.h
	Observer.h
	SC2State.h
	Constants.h
	SceneSwitcher.h
	ScoreTracker.h
	Webhook.h
	Config.h
	SettingsDialog.h
	log.h
  	sc2switcher.cpp
	SC2Data.cpp
	SC2State.cpp
	SceneSwitcher.cpp
	ScoreTracker.cpp
	Webhook.cpp
	Config.cpp
	SettingsDialog.cpp
	forms/SettingsDialog.ui
)	

target_link_libraries(sc2switcher3
	obs-frontend-api
	jansson
	Qt::Widgets
	CURL::libcurl
	libobs)


set(MODULE_DESCRIPTION "OBS SC2 Switcher UI")
configure_file(${CMAKE_SOURCE_DIR}/cmake/bundle/windows/obs-module.rc.in
			 sc2switcher3.rc)
target_sources(sc2switcher3 PRIVATE sc2switcher3.rc)

	
get_target_property(_SOURCES sc2switcher3 SOURCES)
set(_UI ${_SOURCES})
list(FILTER _UI INCLUDE REGEX ".*\\.ui?")

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/forms"
  PREFIX "UI Files"
  FILES ${_UI})
unset(_SOURCES)
unset(_UI)

setup_plugin_target(sc2switcher3)

