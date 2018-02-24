//
//  Window.cpp
//

#include <Graphics.h>
#include <Window.h>
#include <cassert>
#include <iostream>
#include <string>

#if defined(_DIRECTX)
#include <WindowsX.h>
#endif

#pragma region Base Window

void dg::Window::HandleKey(Key key, InputState action) {
  currentKeyStates[(int)key] = (InputState)action;
}

void dg::Window::HandleMouseButton(MouseButton button, InputState action) {
  currentMouseButtonStates[(int)button] = (InputState)action;
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

glm::vec2 dg::Window::GetCursorDelta() const {
  return cursorDelta;
}

const std::string dg::Window::GetTitle() const {
  return title;
}

dg::Window::Window(dg::Window&& other) {
  *this = std::move(other);
}

float dg::Window::GetWidth() const {
  return GetContentSize().x;
}

float dg::Window::GetHeight() const {
  return GetContentSize().y;
}

float dg::Window::GetAspectRatio() const {
  glm::vec2 size = GetContentSize();
  return size.x / size.y;
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
  swap(first.title, second.title);
  swap(first.hasInitialCursorPosition, second.hasInitialCursorPosition);
  swap(first.lastCursorPosition, second.lastCursorPosition);
  swap(first.currentCursorPosition, second.currentCursorPosition);
  swap(first.cursorDelta, second.cursorDelta);
}

#pragma endregion
#pragma region GLFW Window
#if defined(_OPENGL)

std::map<GLFWwindow*, std::weak_ptr<dg::OpenGLWindow>>
    dg::OpenGLWindow::windowMap;

bool dg::OpenGLWindow::glfwIsInitialized = false;

void dg::OpenGLWindow::InitializeGLFW() {
  // Print GLFW errors to stderr.
  glfwSetErrorCallback([](int code, const char *desc) {
    std::cerr << "GLFW Error: " << desc << std::endl;
  });

  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW.");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glfwIsInitialized = true;
}

dg::OpenGLWindow::OpenGLWindow() : Window() {}

dg::OpenGLWindow::OpenGLWindow(dg::OpenGLWindow&& other) {
  *this = std::move(other);
}

dg::OpenGLWindow::~OpenGLWindow() {
  if (glfwWindow != nullptr) {
    windowMap.erase(glfwWindow);
    glfwDestroyWindow(glfwWindow);
    glfwWindow = nullptr;
  }
}

dg::OpenGLWindow& dg::OpenGLWindow::operator=(dg::OpenGLWindow&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(dg::OpenGLWindow& first, dg::OpenGLWindow& second) {
  using std::swap;
  swap(first.glfwWindow, second.glfwWindow);
}

std::shared_ptr<dg::Window> dg::OpenGLWindow::Open(
    unsigned int width, unsigned int height, std::string title) {

  std::shared_ptr<OpenGLWindow> window =
      std::shared_ptr<OpenGLWindow>(new OpenGLWindow());

  window->title = title;
  window->Open(width, height);
  windowMap[window->glfwWindow] = window;

  return window;
}

void dg::OpenGLWindow::Open(int width, int height) {
  assert(glfwWindow == nullptr);

  if (!glfwIsInitialized) {
    InitializeGLFW();
  }

  // Only Windows sets window sizes by pixel sizes.
  // Mac automatically adjusts for DPI scale.
#ifdef _WIN32
  glm::vec2 scale = GetContentScale();
  width = (int)((float)width * scale.x);
  height = (int)((float)height * scale.y);
#endif

  glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (glfwWindow == nullptr) {
    throw std::runtime_error("Failed to create GLFW window.");
  }

  glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
  glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
  glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);

  UseContext();
}

void dg::OpenGLWindow::glfwKeyCallback(
    GLFWwindow *glfwOpenGLWindow, int key, int scancode, int action, int mods) {
  if (key < 0 || key >= (int)Key::LAST) return;
  auto pair = windowMap.find(glfwOpenGLWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleKey((Key)key, (InputState)action);
  }
}

void dg::OpenGLWindow::glfwMouseButtonCallback(
    GLFWwindow *glfwOpenGLWindow, int button, int action, int mods) {
  if (button < 0 || button >= (int)MouseButton::LAST) return;
  auto pair = windowMap.find(glfwOpenGLWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleMouseButton((MouseButton)button, (InputState)action);
  }
}

void dg::OpenGLWindow::glfwCursorPositionCallback(
    GLFWwindow *glfwOpenGLWindow, double x, double y) {
  auto pair = windowMap.find(glfwOpenGLWindow);
  if (pair == windowMap.end()) {
    return;
  }
  if (auto window = pair->second.lock()) {
    window->HandleCursorPosition(x, y);
  }
}

void dg::OpenGLWindow::UseContext() {
  glfwMakeContextCurrent(glfwWindow);
}

void dg::OpenGLWindow::HandleCursorPosition(double x, double y) {
  currentCursorPosition = glm::vec2((float)(int)x, (float)(int)y);
}

void dg::OpenGLWindow::PollEvents() {
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

  cursorDelta = currentCursorPosition - lastCursorPosition;
}

dg::Window::handle_type dg::OpenGLWindow::GetHandle() const {
  return glfwWindow;
}

void dg::OpenGLWindow::LockCursor() {
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void dg::OpenGLWindow::UnlockCursor() {
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool dg::OpenGLWindow::IsCursorLocked() const {
  return glfwGetInputMode(glfwWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

glm::vec2 dg::OpenGLWindow::GetCursorPosition() const {
  glm::vec2 pos = currentCursorPosition;
#if defined(_WIN32)
  pos /= GetContentScale();
  pos.x = floor(pos.x);
  pos.y = floor(pos.y);
#endif
  return pos;
}

void dg::OpenGLWindow::Hide() {
  glfwHideWindow(glfwWindow);
}

void dg::OpenGLWindow::Show() {
  glfwShowWindow(glfwWindow);
}

bool dg::OpenGLWindow::ShouldClose() const {
  return glfwWindowShouldClose(glfwWindow);
}

void dg::OpenGLWindow::SetShouldClose(bool shouldClose) {
  glfwSetWindowShouldClose(glfwWindow, shouldClose);
}

void dg::OpenGLWindow::SetTitle(const std::string& title) {
  this->title = title;
  glfwSetWindowTitle(glfwWindow, title.c_str());
}

void dg::OpenGLWindow::StartRender() {
  assert(glfwWindow != nullptr);

  UseContext();
  ResetViewport();
}

void dg::OpenGLWindow::FinishRender() {
  assert(glfwWindow != nullptr);
  glfwSwapBuffers(glfwWindow);
}

void dg::OpenGLWindow::ResetViewport() {
  // Get the latest true pixel dimension of the window. This
  // takes into account any DPIs or current window size.
  int width, height;
  glfwGetFramebufferSize(glfwWindow, &width, &height);
  glViewport(0, 0, width, height);
}

glm::vec2 dg::OpenGLWindow::GetContentSize() const {
  int x, y;
  glfwGetWindowSize(glfwWindow, &x, &y);
  glm::vec2 size(x, y);
  // Only Windows sets window sizes by pixel sizes.
  // Mac automatically adjusts for DPI scale.
#ifdef _WIN32
  size /= GetContentScale();
#endif
  return size;
}

void dg::OpenGLWindow::SetClientSize(glm::vec2 size) {
  // Only Windows sets window sizes by pixel sizes.
  // Mac automatically adjusts for DPI scale.
#ifdef _WIN32
  size *= GetContentScale();
#endif
  glfwSetWindowSize(glfwWindow, (int)size.x, (int)size.y);
}

glm::vec2 dg::OpenGLWindow::GetContentScale() const {
  float x, y;
  if (glfwWindow == nullptr) {
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &x, &y);
  } else {
    glfwGetWindowContentScale(glfwWindow, &x, &y);
  }
  return glm::vec2(x, y);
}

#pragma endregion
#endif
#pragma region Win32 Window
#if defined(_DIRECTX)

std::map<HWND, std::weak_ptr<dg::Win32Window>> dg::Win32Window::windowMap;

void dg::Win32Window::CreateConsoleWindow(
    int bufferLines, int bufferColumns, int windowLines, int windowColumns) {
  CONSOLE_SCREEN_BUFFER_INFO coninfo;

  AllocConsole();
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
  coninfo.dwSize.Y = bufferLines;
  coninfo.dwSize.X = bufferColumns;
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  SMALL_RECT rect;
  rect.Left = 0;
  rect.Top = 0;
  rect.Right = windowColumns;
  rect.Bottom = windowLines;
  SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

  FILE* stream;
  freopen_s(&stream, "CONIN$", "r", stdin);
  freopen_s(&stream, "CONOUT$", "w", stdout);
  freopen_s(&stream, "CONOUT$", "w", stderr);

  // Prevent accidental console window closure.
  HWND consoleHandle = GetConsoleWindow();
  HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
  EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

dg::Win32Window::Win32Window() : Window() {}

dg::Win32Window::Win32Window(dg::Win32Window&& other) {
  *this = std::move(other);
}

dg::Win32Window& dg::Win32Window::operator=(dg::Win32Window&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(dg::Win32Window& first, dg::Win32Window& second) {
  using std::swap;
  swap((Window&)first, (Window&)second);
  swap(first.hInstance, second.hInstance);
  swap(first.hWnd, second.hWnd);
  swap(first.width, second.width);
  swap(first.height, second.height);
  swap(first.shouldClose, second.shouldClose);
  swap(first.cursorIsLocked, second.cursorIsLocked);
  swap(first.cursorWasLocked, second.cursorWasLocked);
  swap(first.cursorLockOffset, second.cursorLockOffset);
}

std::shared_ptr<dg::Window> dg::Win32Window::Open(
    unsigned int width, unsigned int height, std::string title,
    HINSTANCE hInstance) {

  std::shared_ptr<Win32Window> window =
      std::shared_ptr<Win32Window>(new Win32Window());

  window->hInstance = hInstance;
  window->width = width;
  window->height = height;
  window->title = title;
  window->Open(width, height);
  windowMap[window->hWnd] = window;

  return window;
}

#include <iostream>
void dg::Win32Window::Open(int width, int height) {

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

  Show();
}

LRESULT dg::Win32Window::ProcessMessage(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  auto pair = windowMap.find(hWnd);
  if (pair != windowMap.end()) {
    if (auto window = pair->second.lock()) {
      return window->ProcessMessage(uMsg, wParam, lParam);
    }
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK dg::Win32Window::ProcessMessage(
    UINT uMsg, WPARAM wParam, LPARAM lParam) {

  switch (uMsg) {
    // Win32Window is closing.
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

    // Win32Window size changes.
    case WM_SIZE: {
      // If we're minimizing, we'll be going to a size of zero, so ignore.
      if (wParam == SIZE_MINIMIZED) {
        return 0;
      }
      RECT clientRect;
      GetClientRect(hWnd, &clientRect);
      width = clientRect.right - clientRect.left;
      height = clientRect.bottom - clientRect.top;
      if (Graphics::Instance != nullptr) {
        Graphics::Instance->OnWindowResize(*this);
      }
      return 0;
    }

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

POINT dg::Win32Window::GetWindowCenterScreenSpace() const {
  POINT clientCenter;
  clientCenter.x = width / 2;
  clientCenter.y = height / 2;
  ClientToScreen(hWnd, &clientCenter);
  return clientCenter;
}

void dg::Win32Window::HandleCursorPosition(double x, double y) {
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
    currentCursorPosition = glm::vec2((float)(int)x, (float)(int)y);
  }
}

void dg::Win32Window::PollEvents() {
  lastCursorPosition = currentCursorPosition;
  lastKeyStates = currentKeyStates;
  lastMouseButtonStates = currentMouseButtonStates;

  MSG msg = {};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT) {
      shouldClose = true;
    }
  }

  cursorWasLocked = cursorIsLocked;
  if (hasInitialCursorPosition) {
    cursorDelta = currentCursorPosition - lastCursorPosition;
  }
  hasInitialCursorPosition = true;
}

dg::Window::handle_type dg::Win32Window::GetHandle() const {
  return hWnd;
}

void dg::Win32Window::LockCursor() {
  if (cursorIsLocked) {
    return;
  }
  cursorIsLocked = true;
  while (ShowCursor(false) > 0) continue;
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
}

void dg::Win32Window::UnlockCursor() {
  if (!cursorIsLocked) {
    return;
  }
  cursorIsLocked = false;
  POINT center_SS = GetWindowCenterScreenSpace();
  SetCursorPos(
    center_SS.x + (int)cursorLockOffset.x,
    center_SS.y + (int)cursorLockOffset.y);
  ShowCursor(true);
  ReleaseCapture();
  ClipCursor(NULL);

  currentCursorPosition += cursorLockOffset;
  cursorLockOffset = glm::vec2(0);
  lastCursorPosition += cursorLockOffset;
}

bool dg::Win32Window::IsCursorLocked() const {
  return (cursorIsLocked && GetCapture() == hWnd);
}

glm::vec2 dg::Win32Window::GetCursorPosition() const {
  glm::vec2 pos = currentCursorPosition;
  if (cursorIsLocked || cursorWasLocked) {
    pos += cursorLockOffset;
  }
  return pos;
}

void dg::Win32Window::Hide() {
  ShowWindow(hWnd, SW_MINIMIZE);
}

void dg::Win32Window::Show() {
  ShowWindow(hWnd, SW_SHOW);
}

bool dg::Win32Window::ShouldClose() const {
  return shouldClose;
}

void dg::Win32Window::SetShouldClose(bool shouldClose) {
  if (shouldClose) {
    PostQuitMessage(0);
  }
}

void dg::Win32Window::SetTitle(const std::string& title) {
  this->title = title;
  SetWindowText(hWnd, title.c_str());
}

void dg::Win32Window::StartRender() {
  assert(hWnd != NULL);
  ResetViewport();
}

void dg::Win32Window::FinishRender() {
  assert(hWnd != nullptr);
  Graphics::Instance->swapChain->Present(0, 0);
}

void dg::Win32Window::ResetViewport() {
  // TODO
}

glm::vec2 dg::Win32Window::GetContentSize() const {
  return glm::vec2(width, height);
}

void dg::Win32Window::SetClientSize(glm::vec2 size) {
  // TODO: Set client size
}

#endif
#pragma endregion
