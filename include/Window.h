//
//  Window.h
//

#pragma once

#if defined(_OPENGL)
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#elif defined(_DIRECTX)
#include <Windows.h>
#endif

#include "InputCodes.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <glm/vec2.hpp>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the GLFW window resource.
  class Window {

    public:

#if defined(_OPENGL)
      typedef GLFWwindow* handle_type;
#elif defined(_DIRECTX)
      typedef HWND handle_type;
#endif

      // TODO: Make these protected :)
      Window(Window& other) = delete;
      Window(Window&& other);
      virtual ~Window() = default;
      Window& operator=(Window& other) = delete;
      Window& operator=(Window&& other);
      friend void swap(Window& first, Window& second); // nothrow

      virtual void PollEvents() = 0;

      virtual handle_type GetHandle() const = 0;

      bool IsKeyPressed(Key key) const;
      bool IsKeyJustPressed(Key key) const;

      bool IsMouseButtonPressed(MouseButton button) const;
      bool IsMouseButtonJustPressed(MouseButton button) const;

      virtual void LockCursor() = 0;
      virtual void UnlockCursor() = 0;
      virtual bool IsCursorLocked() const = 0;
      virtual glm::vec2 GetCursorPosition() const = 0;
      glm::vec2 GetCursorDelta() const;

      virtual void Hide() = 0;
      virtual void Show() = 0;

      virtual bool ShouldClose() const = 0;
      virtual void SetShouldClose(bool shouldClose) = 0;

      const std::string GetTitle() const;
      virtual void SetTitle(const std::string& title) = 0;

      virtual void StartRender() = 0;
      virtual void FinishRender() = 0;

      virtual void ResetViewport() = 0;

      float GetWidth() const;
      float GetHeight() const;

      // Returns the size of the window as if monitor is 1x DPI scale,
      // even if it's high-DPI.
      virtual glm::vec2 GetContentSize() const = 0;
      // Sets the size of the window as if monitor is 1x DPI scale, even if
      // it's high-DPI.
      virtual void SetClientSize(glm::vec2 size) = 0;

      float GetAspectRatio() const;

    protected:

      Window();

      enum class InputState : int8_t {
#if defined(_OPENGL)
        PRESS   = GLFW_PRESS,
        RELEASE = GLFW_RELEASE,
        REPEAT  = GLFW_REPEAT,
#elif defined _DIRECTX
        PRESS,
        RELEASE,
        REPEAT,
#endif
      };

      void HandleKey(Key key, InputState action);
      void HandleMouseButton(MouseButton button, InputState action);
      virtual void HandleCursorPosition(double x, double y) = 0;

      virtual void Open(int width, int height) = 0;

      std::vector<InputState> lastKeyStates;
      std::vector<InputState> currentKeyStates;
      std::vector<InputState> lastMouseButtonStates;
      std::vector<InputState> currentMouseButtonStates;
      std::string title;
      bool hasInitialCursorPosition = false;
      glm::vec2 lastCursorPosition;
      glm::vec2 currentCursorPosition;
      glm::vec2 cursorDelta;

  }; // class Window

#if defined(_OPENGL)
  class OpenGLWindow : public Window {

    public:

      // Opens a window with a title and size.
      // Sizes are assuming 1x DPI scale, even if on a higher-DPI display.
      static std::shared_ptr<Window> Open(
          unsigned int width, unsigned int height, std::string title);

      OpenGLWindow(OpenGLWindow& other) = delete;
      OpenGLWindow(OpenGLWindow&& other);
      virtual ~OpenGLWindow();
      OpenGLWindow& operator=(OpenGLWindow& other) = delete;
      OpenGLWindow& operator=(OpenGLWindow&& other);
      friend void swap(OpenGLWindow& first, OpenGLWindow& second); // nothrow

      virtual void PollEvents();

      virtual handle_type GetHandle() const;

      virtual void LockCursor();
      virtual void UnlockCursor();
      virtual bool IsCursorLocked() const;
      virtual glm::vec2 GetCursorPosition() const;

      virtual void Hide();
      virtual void Show();

      virtual bool ShouldClose() const;
      virtual void SetShouldClose(bool shouldClose);

      virtual void SetTitle(const std::string& title);

      virtual void StartRender();
      virtual void FinishRender();

      virtual void ResetViewport();

      // Returns the size of the window as if monitor is 1x DPI scale,
      // even if it's high-DPI.
      virtual glm::vec2 GetContentSize() const;
      // Sets the size of the window as if monitor is 1x DPI scale, even if
      // it's high-DPI.
      virtual void SetClientSize(glm::vec2 size);

    private:

      static std::map<GLFWwindow*, std::weak_ptr<OpenGLWindow>> windowMap;
      static bool glfwIsInitialized;
      static void InitializeGLFW();

      virtual void Open(int width, int height);

      virtual void HandleCursorPosition(double x, double y);

      static void glfwKeyCallback(
          GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
      static void glfwMouseButtonCallback(
          GLFWwindow *glfwWindow, int button, int action, int mods);
      static void glfwCursorPositionCallback(
          GLFWwindow *glfwWindow, double x, double y);

      OpenGLWindow();

      void UseContext();

      // Gets the DPI scale for the window if it exists, or of the primary
      // monitor if the window does not yet exist.
      glm::vec2 GetContentScale() const;

      GLFWwindow *glfwWindow = nullptr;

  }; // class OpenGLWindow
#endif

#if defined(_DIRECTX)
  class Win32Window : public Window {

    public:

      // Opens a window with a title and size.
      // Sizes are assuming 1x DPI scale, even if on a higher-DPI display.
      static std::shared_ptr<Window> Open(
          unsigned int width, unsigned int height, std::string title,
          HINSTANCE hInstance);

      static void dg::Win32Window::CreateConsoleWindow(
        int bufferLines, int bufferColumns, int windowLines, int windowColumns);

      Win32Window(Win32Window& other) = delete;
      Win32Window(Win32Window&& other);
      Win32Window& operator=(Win32Window& other) = delete;
      Win32Window& operator=(Win32Window&& other);
      friend void swap(Win32Window& first, Win32Window& second); // nothrow

      virtual void PollEvents();
      virtual handle_type GetHandle() const;

      virtual void LockCursor();
      virtual void UnlockCursor();
      virtual bool IsCursorLocked() const;
      virtual glm::vec2 GetCursorPosition() const;

      virtual void Hide();
      virtual void Show();

      virtual bool ShouldClose() const;
      virtual void SetShouldClose(bool shouldClose);

      virtual void SetTitle(const std::string& title);

      virtual void StartRender();
      virtual void FinishRender();

      virtual void ResetViewport();

      // Returns the size of the window as if monitor is 1x DPI scale,
      // even if it's high-DPI.
      virtual glm::vec2 GetContentSize() const;
      // Sets the size of the window as if monitor is 1x DPI scale, even if
      // it's high-DPI.
      virtual void SetClientSize(glm::vec2 size);

    private:

      static std::map<HWND, std::weak_ptr<Win32Window>> windowMap;

      virtual void Open(int width, int height);

      static LRESULT CALLBACK ProcessMessage(
          HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
      LRESULT CALLBACK ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

      Win32Window();

      virtual void HandleCursorPosition(double x, double y);
      POINT GetWindowCenterScreenSpace() const;

      HINSTANCE hInstance = nullptr;
      HWND hWnd = nullptr;
      unsigned int width;
      unsigned int height;
      bool shouldClose = false;
      bool cursorIsLocked = false;
      bool cursorWasLocked = false;
      glm::vec2 cursorLockOffset;
      unsigned int dpi;

  }; // class Win32Window
#endif

} // namespace dg
