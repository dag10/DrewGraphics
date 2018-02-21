//
//  Window.h
//
#pragma once

#ifdef _OPENGL
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#endif

#include <InputCodes.h>
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
      static std::shared_ptr<Window> Open(
          unsigned int width, unsigned int height, std::string title);

      Window() = default;
      Window(Window& other) = delete;
      Window(Window&& other);
      ~Window();
      Window& operator=(Window& other) = delete;
      Window& operator=(Window&& other);
      friend void swap(Window& first, Window& second); // nothrow

      void PollEvents();

      bool IsKeyPressed(Key key) const;
      bool IsKeyJustPressed(Key key) const;

      bool IsMouseButtonPressed(MouseButton button) const;
      bool IsMouseButtonJustPressed(MouseButton button) const;

      void LockCursor();
      void UnlockCursor();
      bool IsCursorLocked() const;
      glm::vec2 GetCursorPosition() const;
      glm::vec2 GetCursorDelta() const;

      void Hide();
      void Show();

      bool ShouldClose() const;
      void SetShouldClose(bool shouldClose);

      const std::string GetTitle() const;
      void SetTitle(const std::string& title);

      void StartRender();
      void FinishRender();

      void ResetViewport();

      float GetWidth() const;
      float GetHeight() const;
      glm::vec2 GetSize() const;
      void SetSize(glm::vec2 size);
      glm::vec2 GetContentScale() const;
      float GetAspectRatio() const;

#ifdef _OPENGL
      GLFWwindow *GetHandle() const;
#endif

    private:

      enum class InputState : int8_t {
#ifdef _OPENGL
        PRESS   = GLFW_PRESS,
        RELEASE = GLFW_RELEASE,
        REPEAT  = GLFW_REPEAT,
#elif defined _DIRECTX
        PRESS,
        RELEASE,
        REPEAT,
#endif
      };

#ifdef _OPENGL
      static std::map<GLFWwindow*, std::weak_ptr<Window>> windowMap;

      static void glfwKeyCallback(
          GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
      static void glfwMouseButtonCallback(
          GLFWwindow *glfwWindow, int button, int action, int mods);
      static void glfwCursorPositionCallback(
          GLFWwindow *glfwWindow, double x, double y);
#endif

      void HandleKey(Key key, InputState action);
      void HandleMouseButton(MouseButton button, InputState action);
      void HandleCursorPosition(double x, double y);

      void Open(int width, int height);
      void UseContext();

#ifdef _OPENGL
      GLFWwindow *glfwWindow = nullptr;
#endif

      std::vector<InputState> lastKeyStates;
      std::vector<InputState> currentKeyStates;
      std::vector<InputState> lastMouseButtonStates;
      std::vector<InputState> currentMouseButtonStates;
      std::string title;
      bool hasInitialCursorPosition = false;
      glm::vec2 lastCursorPosition;
      glm::vec2 currentCursorPosition;
  }; // class Window

} // namespace dg
