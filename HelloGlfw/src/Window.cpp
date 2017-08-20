//
//  Window.cpp
//

#include <string>
#include <cassert>

#include "Window.h"

dg::Window dg::Window::Open(
    unsigned int width, unsigned int height, std::string title) {
  dg::Window window;
  window.width = width;
  window.height = height;
  window.title = title;
  window.Open();
  return window;
}

bool dg::Window::ShouldClose() const {
  return glfwWindowShouldClose(glfwWindow);
}

void dg::Window::SetShouldClose(bool shouldClose) {
  glfwSetWindowShouldClose(glfwWindow, shouldClose);
}

void dg::Window::StartRender() {
  assert(glfwWindow != nullptr);

  UseContext();
  glViewport(0, 0, width, height);
}

void dg::Window::FinishRender() {
  assert(glfwWindow != nullptr);

  glfwSwapBuffers(glfwWindow);
}

dg::Window::Window(dg::Window&& other) {
  *this = std::move(other);
}

GLFWwindow *dg::Window::GetHandle() const {
  return glfwWindow;
}

dg::Window::~Window() {
  if (glfwWindow != nullptr) {
    glfwDestroyWindow(glfwWindow);
    glfwWindow = nullptr;
  }
}

dg::Window& dg::Window::operator=(dg::Window&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(dg::Window& first, dg::Window& second) {
  using std::swap;
  swap(first.glfwWindow, second.glfwWindow);
  swap(first.width, second.width);
  swap(first.height, second.height);
  swap(first.title, second.title);
}

void dg::Window::Open() {
  assert(glfwWindow == nullptr);

  glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (glfwWindow == nullptr) {
    throw std::runtime_error("Failed to create GLFW window.");
  }

  UseContext();
}

void dg::Window::UseContext() {
  glfwMakeContextCurrent(glfwWindow);
}

