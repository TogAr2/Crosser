#include "pluginManager.hpp"
#include "config.hpp"
#include "logger.hpp"

#include <dlfcn.h>
#include <filesystem>

int nextId = 0;

PluginManager::PluginManager() : plugins(), destructors() {
	for (auto & plugin : plugins) {
		plugin = nullptr;
	}
}

bool PluginManager::loadDynamicPlugin(const char *path) {
	void *handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
	if (!handle) {
		std::string string = "Cannot load plugin: ";
		string.append(dlerror());
		Logger::error(string);
		return false;
	}

	//Reset errors
	dlerror();

	auto* create = (create_t*) dlsym(handle, "createPlugin");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
		std::string string = "Cannot load 'createPlugin' symbol: ";
		string.append(dlsym_error);
		Logger::error(string);
		return false;
	}
	auto* destroy = (destroy_t*) dlsym(handle, "destroyPlugin");
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::string string = "Cannot load 'destroyPlugin' symbol: ";
		string.append(dlsym_error);
		Logger::error(string);
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
	for (const auto & entry : std::filesystem::directory_iterator(PLUGIN_FOLDER)) {
		loadDynamicPlugin(entry.path().string().c_str());
		Logger::info("Loaded plugin " + entry.path().filename().string());
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
