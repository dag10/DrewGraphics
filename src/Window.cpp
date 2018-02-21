//
//  Window.cpp
//

#include <string>
#include <cassert>

#include <Window.h>

#if defined(_DIRECTX)
#include <WindowsX.h>
#endif

#if defined(_OPENGL)
std::map<GLFWwindow*, std::weak_ptr<dg::Window>> dg::Window::windowMap;
#endif

#if defined(_OPENGL)
void dg::Window::glfwKeyCallback(
    GLFWwindow *glfwWindow, int key, int scancode, int action, int mods) {
  if (key < 0 || key >= (int)Key::LAST) return;
  auto pair = windowMap.find(glfwWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleKey((Key)key, (InputState)action);
  }
}
#endif

#if defined(_OPENGL)
void dg::Window::glfwMouseButtonCallback(
    GLFWwindow *glfwWindow, int button, int action, int mods) {
  if (button < 0 || button >= (int)MouseButton::LAST) return;
  auto pair = windowMap.find(glfwWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleMouseButton((MouseButton)button, (InputState)action);
  }
}
#endif

#if defined(_OPENGL)
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
#endif

void dg::Window::HandleKey(Key key, InputState action) {
  currentKeyStates[(int)key] = (InputState)action;
}

void dg::Window::HandleMouseButton(MouseButton button, InputState action) {
  currentMouseButtonStates[(int)button] = (InputState)action;
}

void dg::Window::HandleCursorPosition(double x, double y) {
  currentCursorPosition = glm::vec2((float)x, (float)y);
}

// Opens a window with a title and size.
// Sizes are assuming 1x DPI scale, even if on a higher-DPI display.
std::shared_ptr<dg::Window> dg::Window::Open(
#if defined(_OPENGL)
    unsigned int width,
    unsigned int height,
    std::string title)
#elif defined(_DIRECTX)
    unsigned int width,
    unsigned int height,
    std::string title,
    HINSTANCE hInstance)
#endif
{

  std::shared_ptr<Window> window = std::make_shared<Window>();

#if defined(_DIRECTX)
  window->hInstance = hInstance;
#endif
  window->title = title;
  window->Open(width, height);
#if defined(_OPENGL)
  windowMap[window->GetHandle()] = window;
#endif

  return window;
}

dg::Window::Window() {
  lastKeyStates = std::vector<InputState>(
      (int)Key::LAST + 1, InputState::RELEASE);
  currentKeyStates = std::vector<InputState>(
      (int)Key::LAST + 1, InputState::RELEASE);
  lastMouseButtonStates = std::vector<InputState>(
      (int)MouseButton::LAST + 1, InputState::RELEASE);
  currentMouseButtonStates = std::vector<InputState>(
      (int)MouseButton::LAST + 1, InputState::RELEASE);
}

void dg::Window::PollEvents() {
  if (!hasInitialCursorPosition) {
    hasInitialCursorPosition = true;
    double x, y;
#if defined(_OPENGL)
    glfwGetCursorPos(glfwWindow, &x, &y);
#elif defined _DIRECTX
    // TODO: Get cursor position in DirectInput.
    x = y = 0;
#endif
    HandleCursorPosition(x, y);
  }
  lastCursorPosition = currentCursorPosition;
  lastKeyStates = currentKeyStates;
  lastMouseButtonStates = currentMouseButtonStates;
#if defined(_OPENGL)
  glfwPollEvents();
#endif
}

bool dg::Window::IsKeyPressed(Key key) const {
  InputState state = currentKeyStates[(int)key];
  return state == InputState::PRESS || state == InputState::REPEAT;
}

bool dg::Window::IsKeyJustPressed(Key key) const {
  InputState state = currentKeyStates[(int)key];
  return (state == InputState::PRESS || state == InputState::REPEAT) &&
    lastKeyStates[(int)key] == InputState::RELEASE;
}

bool dg::Window::IsMouseButtonPressed(MouseButton button) const {
  return currentMouseButtonStates[(int)button] == InputState::PRESS;
}

bool dg::Window::IsMouseButtonJustPressed(MouseButton button) const {
  return currentMouseButtonStates[(int)button] == InputState::PRESS &&
    lastMouseButtonStates[(int)button] == InputState::RELEASE;
}

void dg::Window::LockCursor() {
#if defined(_OPENGL)
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#elif defined _DIRECTX
  // TODO
#endif
}

void dg::Window::UnlockCursor() {
#if defined(_OPENGL)
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#elif defined _DIRECTX
  // TODO
#endif
}

bool dg::Window::IsCursorLocked() const {
#if defined(_OPENGL)
  return glfwGetInputMode(glfwWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
#elif defined _DIRECTX
  // TODO
  return false;
#endif
}

glm::vec2 dg::Window::GetCursorPosition() const {
  glm::vec2 pos = currentCursorPosition;
#ifdef _WIN32
  pos /= GetContentScale();
#endif
  return pos;
}

glm::vec2 dg::Window::GetCursorDelta() const {
  return currentCursorPosition - lastCursorPosition;
}

void dg::Window::Hide() {
#if defined(_OPENGL)
  glfwHideWindow(glfwWindow);
#elif defined _DIRECTX
  // TODO
#endif
}

void dg::Window::Show() {
#if defined(_OPENGL)
  glfwShowWindow(glfwWindow);
#elif defined _DIRECTX
  // TODO
#endif
}

bool dg::Window::ShouldClose() const {
#if defined(_OPENGL)
  return glfwWindowShouldClose(glfwWindow);
#elif defined _DIRECTX
  // TODO
  return false;
#endif
}

void dg::Window::SetShouldClose(bool shouldClose) {
#if defined(_OPENGL)
  glfwSetWindowShouldClose(glfwWindow, shouldClose);
#elif defined _DIRECTX
  // TODO
#endif
}

const std::string dg::Window::GetTitle() const {
  return title;
}

void dg::Window::SetTitle(const std::string& title) {
  this->title = title;
#if defined(_OPENGL)
  glfwSetWindowTitle(glfwWindow, title.c_str());
#elif defined _DIRECTX
  // TODO
#endif
}

void dg::Window::StartRender() {
#if defined(_OPENGL)
  assert(glfwWindow != nullptr);
#endif

  // TODO: DirectX

  UseContext();
  ResetViewport();
}

void dg::Window::FinishRender() {
#if defined(_OPENGL)
  assert(glfwWindow != nullptr);
  glfwSwapBuffers(glfwWindow);
#endif

  // TODO: DirectX
}

void dg::Window::ResetViewport() {
  // Get the latest true pixel dimension of the window. This
  // takes into account any DPIs or current window size.
#if defined(_OPENGL)
  int width, height;
  glfwGetFramebufferSize(glfwWindow, &width, &height);
  glViewport(0, 0, width, height);
#elif defined _DIRECTX
  // TODO
#endif
}

dg::Window::Window(dg::Window&& other) {
  *this = std::move(other);
}

float dg::Window::GetWidth() const {
  return GetSize().x;
}

float dg::Window::GetHeight() const {
  return GetSize().y;
}

/// Returns the size of the window as if monitor is 1x DPI scale, even if it's high-DPI.
glm::vec2 dg::Window::GetSize() const {
  int x, y;
#if defined(_OPENGL)
  glfwGetWindowSize(glfwWindow, &x, &y);
#elif defined _DIRECTX
  // TODO
  x = y = 0;
#endif
  glm::vec2 size(x, y);
  // Only Windows sets window sizes by pixel sizes. Mac automatically adjusts for DPI scale.
#ifdef _WIN32
  size /= GetContentScale();
#endif
  return size;
}

/// Sets the size of the window as if monitor is 1x DPI scale, even if it's high-DPI.
void dg::Window::SetSize(glm::vec2 size) {
  // Only Windows sets window sizes by pixel sizes. Mac automatically adjusts for DPI scale.
#ifdef _WIN32
  size *= GetContentScale();
#endif
#if defined(_OPENGL)
  glfwSetWindowSize(glfwWindow, (int)size.x, (int)size.y);
#elif defined _DIRECTX
  // TODO
#endif
}

/// Gets the DPI scale for the window if it exists, or of the primary monitor if
/// the window does not yet exist.
glm::vec2 dg::Window::GetContentScale() const {
#if defined(_OPENGL)
  float x, y;
  if (glfwWindow == nullptr) {
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &x, &y);
  } else {
    glfwGetWindowContentScale(glfwWindow, &x, &y);
  }
  return glm::vec2(x, y);
#elif defined _DIRECTX
  // TODO
  return glm::vec2(1);
#endif
}

float dg::Window::GetAspectRatio() const {
  glm::vec2 size = GetSize();
  return size.x / size.y;
}

#if defined(_OPENGL)
GLFWwindow *dg::Window::GetHandle() const {
  return glfwWindow;
}
#endif

dg::Window::~Window() {
#if defined(_OPENGL)
  if (glfwWindow != nullptr) {
    windowMap.erase(glfwWindow);
    glfwDestroyWindow(glfwWindow);
    glfwWindow = nullptr;
  }
#endif
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
#if defined(_OPENGL)
  swap(first.glfwWindow, second.glfwWindow);
#elif defined(_DIRECTX)
  swap(first.hInstance, second.hInstance);
  swap(first.hWnd, second.hWnd);
#endif
  swap(first.title, second.title);
  swap(first.hasInitialCursorPosition, second.hasInitialCursorPosition);
  swap(first.lastCursorPosition, second.lastCursorPosition);
  swap(first.currentCursorPosition, second.currentCursorPosition);
}

void dg::Window::Open(int width, int height) {
#if defined(_OPENGL)

  assert(glfwWindow == nullptr);

  // Only Windows sets window sizes by pixel sizes. Mac automatically adjusts for DPI scale.
# ifdef _WIN32
  glm::vec2 scale = GetContentScale();
  width = (int)((float)width * scale.x);
  height = (int)((float)height * scale.y);
# endif

  glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (glfwWindow == nullptr) {
    throw std::runtime_error("Failed to create GLFW window.");
  }

  glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
  glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
  glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);

  UseContext();

#elif defined _DIRECTX

  WNDCLASS wndClass = {};
  wndClass.style = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = ProcessMessage;
  wndClass.cbClsExtra = 0;
  wndClass.cbWndExtra = 0;
  wndClass.hInstance = hInstance;
  wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndClass.lpszMenuName = NULL;
  wndClass.lpszClassName = "Direct3DWindowClass";

  if (!RegisterClass(&wndClass)) {
    DWORD error = GetLastError();
    if (error != ERROR_CLASS_ALREADY_EXISTS) {
      throw std::runtime_error(
        "Failed to register Window class. (" + std::to_string(error) + ")");
    }
  }

  RECT clientRect;
  SetRect(&clientRect, 0, 0, width, height);
  AdjustWindowRect(
    &clientRect,
    WS_OVERLAPPEDWINDOW, // Standard window buttons
    false); // No menu bar

  // Center the window.
  RECT desktopRect;
  GetClientRect(GetDesktopWindow(), &desktopRect);
  int centeredX = (desktopRect.right / 2) - (clientRect.right / 2);
  int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2);

  hWnd = CreateWindow(wndClass.lpszClassName, title.c_str(),
                      WS_OVERLAPPEDWINDOW, centeredX, centeredY,
                      clientRect.right - clientRect.left,  // Calculated width
                      clientRect.bottom - clientRect.top,  // Calculated height
                      0,                                   // No parent window
                      0,                                   // No menu
                      hInstance,                           // The app's handle
                      0);  // No other windows in our application

  if (hWnd == NULL) {
    throw std::runtime_error(
      "Failed to create Window. (" + std::to_string(GetLastError()) + ")");
  }

  ShowWindow(hWnd, SW_SHOW);

#endif
}

void dg::Window::UseContext() {
#if defined(_OPENGL)
  glfwMakeContextCurrent(glfwWindow);
#endif
}

#if defined(_DIRECTX)
LRESULT dg::Window::ProcessMessage(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
#endif


