//
//  Window.cpp
//

#include <string>
#include <cassert>

#include <Window.h>

#if defined(_DIRECTX)
#include <WindowsX.h>
#endif

std::map<dg::Window::window_map_key_type, std::weak_ptr<dg::Window>>
    dg::Window::windowMap;

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
#if defined(_OPENGL)
  currentCursorPosition = glm::vec2((float)(int)x, (float)(int)y);
#elif defined(_DIRECTX)

  if (cursorIsLocked) {
    POINT point;
    point.x = (long)x;
    point.y = (long)y;

    // Convert from client space to screen space.
    ClientToScreen(hWnd, &point);

    // Center of window in screen space coordinates.
    POINT center_SS = GetWindowCenterScreenSpace();

    currentCursorPosition += glm::vec2(
      point.x - center_SS.x, point.y - center_SS.y);
    SetCursorPos(center_SS.x, center_SS.y);
  } else {
    currentCursorPosition = glm::vec2((float)x, (float)y);
  }
#endif
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
  window->width = width;
  window->height = height;
#endif
  window->title = title;
  window->Open(width, height);
#if defined(_OPENGL)
  windowMap[window->GetHandle()] = window;
#elif defined(_DIRECTX)
  windowMap[window->hWnd] = window;
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
#if defined(_OPENGL)
  if (!hasInitialCursorPosition) {
    hasInitialCursorPosition = true;
    double x, y;
    glfwGetCursorPos(glfwWindow, &x, &y);
    HandleCursorPosition(x, y);
  }
#endif

  lastCursorPosition = currentCursorPosition;
  lastKeyStates = currentKeyStates;
  lastMouseButtonStates = currentMouseButtonStates;

#if defined(_OPENGL)
  glfwPollEvents();
#elif defined(_DIRECTX)
  MSG msg = {};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT) {
      shouldClose = true;
    }
  }
#endif

#if defined(_DIRECTX)
  cursorWasLocked = cursorIsLocked;
#endif

  cursorDelta = currentCursorPosition - lastCursorPosition;
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
#elif defined(_DIRECTX)
  if (cursorIsLocked) {
    return;
  }
  cursorIsLocked = true;
  ShowCursor(false);
  SetCapture(hWnd);
  RECT rect;
  GetWindowRect(hWnd, &rect);
  ClipCursor(&rect);
  cursorLockOffset.x = currentCursorPosition.x - (width / 2);
  cursorLockOffset.y = currentCursorPosition.y - (height / 2);

  lastCursorPosition -= cursorLockOffset * 2.f;
  currentCursorPosition -= cursorLockOffset;

  POINT center_SS = GetWindowCenterScreenSpace();
  SetCursorPos(center_SS.x, center_SS.y);
#endif
}

void dg::Window::UnlockCursor() {
#if defined(_OPENGL)
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#elif defined(_DIRECTX)
  if (!cursorIsLocked) {
    return;
  }
  cursorIsLocked = false;
  POINT center_SS = GetWindowCenterScreenSpace();
  SetCursorPos(
    center_SS.x + cursorLockOffset.x,
    center_SS.y + cursorLockOffset.y);
  ShowCursor(true);
  ReleaseCapture();
  ClipCursor(NULL);

  currentCursorPosition += cursorLockOffset;

  cursorLockOffset = glm::vec2(0);

  lastCursorPosition += cursorLockOffset;
#endif
}

bool dg::Window::IsCursorLocked() const {
#if defined(_OPENGL)
  return glfwGetInputMode(glfwWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
#elif defined(_DIRECTX)
  return (cursorIsLocked && GetCapture() == hWnd);
#endif
}

glm::vec2 dg::Window::GetCursorPosition() const {
  glm::vec2 pos = currentCursorPosition;
#if defined(_OPENGL)
# if defined(_WIN32)
  pos /= GetContentScale();
  pos.x = floor(pos.x);
  pos.y = floor(pos.y);
# endif
#elif defined(_DIRECTX)
  if (cursorIsLocked || cursorWasLocked) {
    pos += cursorLockOffset;
  }
#endif
  return pos;
}

glm::vec2 dg::Window::GetCursorDelta() const {
  return cursorDelta;
}

void dg::Window::Hide() {
#if defined(_OPENGL)
  glfwHideWindow(glfwWindow);
#elif defined(_DIRECTX)
  ShowWindow(hWnd, SW_MINIMIZE);
#endif
}

void dg::Window::Show() {
#if defined(_OPENGL)
  glfwShowWindow(glfwWindow);
#elif defined(_DIRECTX)
  ShowWindow(hWnd, SW_SHOW);
#endif
}

bool dg::Window::ShouldClose() const {
#if defined(_OPENGL)
  return glfwWindowShouldClose(glfwWindow);
#elif defined _DIRECTX
  return shouldClose;
#endif
}

void dg::Window::SetShouldClose(bool shouldClose) {
#if defined(_OPENGL)
  glfwSetWindowShouldClose(glfwWindow, shouldClose);
#elif defined _DIRECTX
  if (shouldClose) {
    PostQuitMessage(0);
  }
#endif
}

const std::string dg::Window::GetTitle() const {
  return title;
}

void dg::Window::SetTitle(const std::string& title) {
  this->title = title;
#if defined(_OPENGL)
  glfwSetWindowTitle(glfwWindow, title.c_str());
#elif defined(_DIRECTX)
  SetWindowText(hWnd, title.c_str());
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

#if defined(_DIRECTX)
POINT dg::Window::GetWindowCenterScreenSpace() const {
  POINT clientCenter;
  clientCenter.x = width / 2;
  clientCenter.y = height / 2;
  ClientToScreen(hWnd, &clientCenter);
  return clientCenter;
}
#endif

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
  swap(first.width, second.width);
  swap(first.height, second.height);
  swap(first.shouldClose, second.shouldClose);
  swap(first.cursorIsLocked, second.cursorIsLocked);
  swap(first.cursorWasLocked, second.cursorWasLocked);
  swap(first.cursorLockOffset, second.cursorLockOffset);
#endif
  swap(first.title, second.title);
  swap(first.hasInitialCursorPosition, second.hasInitialCursorPosition);
  swap(first.lastCursorPosition, second.lastCursorPosition);
  swap(first.currentCursorPosition, second.currentCursorPosition);
  swap(first.cursorDelta, second.cursorDelta);
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

#elif defined(_DIRECTX)

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
  auto pair = windowMap.find(hWnd);
  if (pair != windowMap.end()) {
    if (auto window = pair->second.lock()) {
      return window->ProcessMessage(uMsg, wParam, lParam);
    }
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK dg::Window::ProcessMessage(
    UINT uMsg, WPARAM wParam, LPARAM lParam) {

  switch (uMsg) {
    // Window is closing.
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    // Prevent beeping when we "alt-enter" into fullscreen.
    case WM_MENUCHAR:
      return MAKELRESULT(0, MNC_CLOSE);

    // Prevent the overall window from becoming too small.
    case WM_GETMINMAXINFO:
      ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
      ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
      return 0;

    // Window size changes.
    case WM_SIZE:
      // If we're minimizing, we'll be going to a size of zero, so ignore.
      if (wParam == SIZE_MINIMIZED) {
        return 0;
      }
      width = LOWORD(lParam);
      height = HIWORD(lParam);
      // TODO: If DirectX is initialized, resize buffers.
      return 0;

    // Mouse button pressed (while over window).
    case WM_LBUTTONDOWN:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      HandleMouseButton(BUTTON_LEFT, InputState::PRESS);
      return 0;
    case WM_MBUTTONDOWN:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      HandleMouseButton(BUTTON_MIDDLE, InputState::PRESS);
      return 0;
    case WM_RBUTTONDOWN:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      HandleMouseButton(BUTTON_RIGHT, InputState::PRESS);
      return 0;

    // Mouse button released (while over window).
    case WM_LBUTTONUP:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      HandleMouseButton(BUTTON_LEFT, InputState::RELEASE);
      return 0;
    case WM_MBUTTONUP:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      HandleMouseButton(BUTTON_MIDDLE, InputState::RELEASE);
      return 0;
    case WM_RBUTTONUP:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      HandleMouseButton(BUTTON_RIGHT, InputState::RELEASE);
      return 0;

    // Cursor moves.
    case WM_MOUSEMOVE:
      HandleCursorPosition(
          (double)GET_X_LPARAM(lParam), (double)GET_Y_LPARAM(lParam));
      return 0;

    // Key pressed.
    case WM_KEYDOWN:
      HandleKey((Key)wParam, InputState::PRESS);
      return 0;

    // Key released.
    case WM_KEYUP:
      HandleKey((Key)wParam, InputState::RELEASE);
      return 0;

    // TODO: Handle mouse wheel (WM_MOUSEWHEEL).
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
#endif


