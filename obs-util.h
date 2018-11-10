#pragma once

#include <obs.hpp>
#include <string>

static inline OBSWeakSource GetWeakSourceByName(const char *name)
{
	OBSWeakSource weak;
	obs_source_t *source = obs_get_source_by_name(name);
	if (source) {
		weak = obs_source_get_weak_source(source);
		obs_source_release(source);
	}

	return weak;
}

static inline std::string GetWeakSourceName(obs_weak_source_t *weak_source)
{
	std::string name;

	obs_source_t *source = obs_weak_source_get_source(weak_source);
	if (source) {
		name = obs_source_get_name(source);
		obs_source_release(source);
	}

	return name;
}