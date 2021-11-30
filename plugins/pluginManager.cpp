#include "pluginManager.hpp"
#include "config.hpp"
#include "logger.hpp"

#include <dlfcn.h>
#include <filesystem>
#include <iostream>

int nextId = 0;

PluginManager::PluginManager() : plugins(), destructors() {
	for (auto & plugin : plugins) {
		plugin = nullptr;
	}
}

bool PluginManager::loadDynamicPlugin(const char *path) {
	void *handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
	if (!handle) {
		std::cerr << Logger::error << "Cannot load plugin: " << dlerror() << std::endl;
		return false;
	}

	//Reset errors
	dlerror();

	auto* create = (create_t*) dlsym(handle, "createPlugin");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << Logger::error << "Cannot load 'createPlugin' symbol: " << dlsym_error << std::endl;
		return false;
	}
	auto* destroy = (destroy_t*) dlsym(handle, "destroyPlugin");
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << Logger::error << "Cannot load 'destroyPlugin' symbol: " << dlsym_error << std::endl;
		return false;
	}

	//Create the plugin instance
	crs::IPlugin* plugin = create();

	plugin->id = nextId;
	nextId++;

	plugins[plugin->id] = plugin;
	destructors[plugin->id] = destroy;

	plugin->setup();

	return true;
}

void PluginManager::destroyPlugin(int id) {
	crs::IPlugin* plugin = plugins[id];
	plugin->end();

	destructors[id](plugin);

	plugins[id] = nullptr;
}

void PluginManager::broadcastEvent(crs::Event *&event) {
	for (auto & plugin : plugins) {
		if (plugin != nullptr) {
			plugin->onEvent(event);
		}
	}
}

void PluginManager::start() {
	std::filesystem::path folder(PLUGIN_FOLDER);
	if (!std::filesystem::exists(folder)) {
		std::filesystem::create_directory(folder);
	}

	for (const auto & entry : std::filesystem::directory_iterator(folder)) {
		if (entry.path().filename().has_filename()) {
			loadDynamicPlugin(entry.path().string().c_str());
			std::cout << Logger::info << "Loaded plugin " << entry.path().filename().string() << std::endl;
		}
	}
}

void PluginManager::end() {
	for (auto & plugin : plugins) {
		if (plugin != nullptr) {
			destroyPlugin(plugin->id);
		}
	}
}

PluginManager PluginManager::instance = PluginManager();
