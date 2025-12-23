#include <exception>
#include <memory>

#include "Plugin.hpp"

using namespace plugin;

class HelloWorld : public PluginBase {
    public:
    HelloWorld(const Context_ context) : PluginBase(context) {}
    PluginResult(int) OnLoad(Context context) {
        std::cout << "Hello, World!" << std::endl;
        return 0;
    }
};

PLUGIN_PREAMBLE(HelloWorld, 1.0.0);
