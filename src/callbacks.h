#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <glfw/glfw3.h>

#include "callbacks.h"
#include "camera.h"
#include "constants.h"

// Whenever the window size changes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Mouse/cursor controls
void cursorPosCallback(GLFWwindow* window, double posX, double posY);

// `offsetY` means how much we scrolled vertically
void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);

#endif
