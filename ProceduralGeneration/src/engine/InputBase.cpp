#include "Input.h"

#include <memory>

using namespace Engine;

// Pointer to the input class that inherits InputBase in the acutal game
namespace Engine {
	std::shared_ptr<InputBase> inputPtrCallback;
}

void Engine::setInputPtr(std::shared_ptr<InputBase> _inputPtrCallback) {
	inputPtrCallback = _inputPtrCallback;
}

// Mouse movement
void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (inputPtrCallback)
		inputPtrCallback->mouseCallback(window, xpos, ypos);
}

// Scrolling
void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (inputPtrCallback)
		inputPtrCallback->scrollCallback(window, xoffset, yoffset);
}