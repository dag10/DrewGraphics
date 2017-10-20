//
//  Window.h
//
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>
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

      bool IsKeyPressed(GLenum key) const;
      bool IsKeyJustPressed(GLenum key) const;

      bool IsMouseButtonPressed(GLenum key) const;
      bool IsMouseButtonJustPressed(GLenum key) const;

      void LockCursor();
      void UnlockCursor();
      bool IsCursorLocked() const;
      glm::vec2 GetCursorPosition() const;
      glm::vec2 GetCursorDelta() const;

      bool ShouldClose() const;
      void SetShouldClose(bool shouldClose);

      void StartRender();
      void FinishRender();

      float GetWidth() const;
      float GetHeight() const;
      glm::vec2 GetSize() const;

      GLFWwindow *GetHandle() const;

    private:
      static std::map<GLFWwindow*, std::weak_ptr<Window>> windowMap;

      static void glfwKeyCallback(
          GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
      static void glfwMouseButtonCallback(
          GLFWwindow *glfwWindow, int button, int action, int mods);
      static void glfwCursorPositionCallback(
          GLFWwindow *glfwWindow, double x, double y);

      void HandleKey(int key, int action);
      void HandleMouseButton(int button, int action);
      void HandleCursorPosition(double x, double y);

      void Open();
      void UseContext();

      std::vector<uint8_t> lastKeyStates;
      std::vector<uint8_t> currentKeyStates;
      std::vector<uint8_t> lastMouseButtonStates;
      std::vector<uint8_t> currentMouseButtonStates;
      GLFWwindow *glfwWindow = nullptr;
      int width = 0;
      int height = 0;
      std::string title;
      bool hasInitialCursorPosition = false;
      glm::vec2 lastCursorPosition;
      glm::vec2 currentCursorPosition;
  }; // class Window

} // namespace dg
