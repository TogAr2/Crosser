#ifndef CROSSER_PLUGINMANAGER_HPP
#define CROSSER_PLUGINMANAGER_HPP

#include "IPlugin.hpp"
#include "events.hpp"

typedef crs::IPlugin* create_t();
typedef void destroy_t(crs::IPlugin*);

class PluginManager {
	crs::IPlugin *plugins[20];
	void (*destructors[20])(crs::IPlugin *);

public:
	PluginManager();

	void start();
	void end();

	bool loadDynamicPlugin(const char *path);
	void destroyPlugin(int id);

	void broadcastEvent(crs::Event* &event);

	static PluginManager instance;
};

#endif //CROSSER_PLUGINMANAGER_HPP
