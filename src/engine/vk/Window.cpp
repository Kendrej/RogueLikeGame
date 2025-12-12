#include "engine/gfx/VulkanImGuiApp.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>


void VulkanImGuiApp::initWindow()
{
    if (!glfwInit())
        throw std::runtime_error("GLFW init failed");

    // (opcjonalnie, ale polecam mie�)
    glfwSetErrorCallback([](int err, const char* desc)
                         { std::cerr << "GLFW Error " << err << ": " << desc << std::endl; });

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor)
        throw std::runtime_error("No primary monitor");

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
        throw std::runtime_error("No video mode");

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    window_ = glfwCreateWindow(mode->width, mode->height, "RogueLikeGame", monitor, nullptr);
    if (!window_)
        throw std::runtime_error("Failed to create fullscreen window");

    glfwSetWindowUserPointer(window_, this);
}
