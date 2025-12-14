#include "log4cplus/configurator.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
using namespace log4cplus;

#include "config.hpp"
#include "EngineCore.hpp"
using namespace Engine;

static Logger logger = Logger::getInstance("Engine::Core");

void Engine::Core::InitializeLoggers(void) {
    log4cplus::PropertyConfigurator::doConfigure(ENGINE_LOGPROPERTIES);
    LOG4CPLUS_INFO(logger, "Loggers configured");
}
