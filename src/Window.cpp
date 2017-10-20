//
//  Window.cpp
//

#include <string>
#include <cassert>

#include <Window.h>

std::map<GLFWwindow*, std::weak_ptr<dg::Window>> dg::Window::windowMap;

void dg::Window::glfwKeyCallback(
    GLFWwindow *glfwWindow, int key, int scancode, int action, int mods) {
  auto pair = windowMap.find(glfwWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleKey(key, action);
  }
}

void dg::Window::glfwMouseButtonCallback(
    GLFWwindow *glfwWindow, int button, int action, int mods) {
  auto pair = windowMap.find(glfwWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleMouseButton(button, action);
  }
}

void dg::Window::glfwCursorPositionCallback(
    GLFWwindow *glfwWindow, double x, double y) {
  auto pair = windowMap.find(glfwWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleCursorPosition(x, y);
  }
}

void dg::Window::HandleKey(int key, int action) {
  currentKeyStates[key] = action;
}

void dg::Window::HandleMouseButton(int button, int action) {
  currentMouseButtonStates[button] = action;
}

void dg::Window::HandleCursorPosition(double x, double y) {
  currentCursorPosition = glm::vec2((float)x, (float)y);
}

std::shared_ptr<dg::Window> dg::Window::Open(
    unsigned int width, unsigned int height, std::string title) {
  std::shared_ptr<Window> window = std::make_shared<Window>();
  window->width = width;
  window->height = height;
  window->title = title;
  window->lastKeyStates = std::vector<uint8_t>(
      GLFW_KEY_LAST + 1, GLFW_RELEASE);
  window->currentKeyStates = std::vector<uint8_t>(
      GLFW_KEY_LAST + 1, GLFW_RELEASE);
  window->lastMouseButtonStates = std::vector<uint8_t>(
      GLFW_MOUSE_BUTTON_LAST + 1, GLFW_RELEASE);
  window->currentMouseButtonStates = std::vector<uint8_t>(
      GLFW_MOUSE_BUTTON_LAST + 1, GLFW_RELEASE);
  window->Open();
  windowMap[window->GetHandle()] = window;
  return window;
}

void dg::Window::PollEvents() {
  if (!hasInitialCursorPosition) {
    hasInitialCursorPosition = true;
    double x, y;
    glfwGetCursorPos(glfwWindow, &x, &y);
    HandleCursorPosition(x, y);
  }
  lastCursorPosition = currentCursorPosition;
  lastKeyStates = currentKeyStates;
  lastMouseButtonStates = currentMouseButtonStates;
  glfwPollEvents();
}

bool dg::Window::IsKeyPressed(GLenum key) const {
  int state = currentKeyStates[key];
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool dg::Window::IsKeyJustPressed(GLenum key) const {
  int state = currentKeyStates[key];
  return (state == GLFW_PRESS || state == GLFW_REPEAT) &&
    lastKeyStates[key] == GLFW_RELEASE;
}

bool dg::Window::IsMouseButtonPressed(GLenum button) const {
  return currentMouseButtonStates[button] == GLFW_PRESS;
}

bool dg::Window::IsMouseButtonJustPressed(GLenum button) const {
  return currentMouseButtonStates[button] == GLFW_PRESS &&
    lastMouseButtonStates[button] == GLFW_RELEASE;
}

void dg::Window::LockCursor() {
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void dg::Window::UnlockCursor() {
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool dg::Window::IsCursorLocked() const {
  return glfwGetInputMode(glfwWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

glm::vec2 dg::Window::GetCursorPosition() const {
  double x, y;
  glfwGetCursorPos(glfwWindow, &x, &y);
  return glm::vec2(x, y);
}

glm::vec2 dg::Window::GetCursorDelta() const {
  return currentCursorPosition - lastCursorPosition;
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

  // Get the latest true pixel dimension of the window. This
  // takes into account any DPIs or current window size.
  glfwGetFramebufferSize(glfwWindow, &width, &height);
  glViewport(0, 0, width, height);
}

void dg::Window::FinishRender() {
  assert(glfwWindow != nullptr);
  glfwSwapBuffers(glfwWindow);
}

dg::Window::Window(dg::Window&& other) {
  *this = std::move(other);
}

float dg::Window::GetWidth() const {
  return width;
}

float dg::Window::GetHeight() const {
  return height;
}

glm::vec2 dg::Window::GetSize() const {
  return glm::vec2(width, height);
}

GLFWwindow *dg::Window::GetHandle() const {
  return glfwWindow;
}

dg::Window::~Window() {
  if (glfwWindow != nullptr) {
    windowMap.erase(glfwWindow);
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
  swap(first.lastKeyStates, second.lastKeyStates);
  swap(first.currentKeyStates, second.currentKeyStates);
  swap(first.lastMouseButtonStates, second.lastMouseButtonStates);
  swap(first.currentMouseButtonStates, second.currentMouseButtonStates);
  swap(first.glfwWindow, second.glfwWindow);
  swap(first.width, second.width);
  swap(first.height, second.height);
  swap(first.title, second.title);
  swap(first.hasInitialCursorPosition, second.hasInitialCursorPosition);
  swap(first.lastCursorPosition, second.lastCursorPosition);
  swap(first.currentCursorPosition, second.currentCursorPosition);
}

void dg::Window::Open() {
  assert(glfwWindow == nullptr);

  glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (glfwWindow == nullptr) {
    throw std::runtime_error("Failed to create GLFW window.");
  }

  glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
  glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
  glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);

  UseContext();
}

void dg::Window::UseContext() {
  glfwMakeContextCurrent(glfwWindow);
}

