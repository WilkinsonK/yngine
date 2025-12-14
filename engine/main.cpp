#include "EngineCore.hpp"
#include "EngineUI.hpp"

int main(void) {
    Engine::Core::InitializeLoggers();
    Engine::UI::MainWindow();
    return 0;
}
