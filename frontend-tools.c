#include <obs-module.h>
#include "frontend-tools-config.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("sc2switcher", "en-US")


void InitSceneSwitcher();
void FreeSceneSwitcher();

bool obs_module_load(void)
{
	InitSceneSwitcher();
}

void obs_module_unload(void)
{
	FreeSceneSwitcher();
}