//
//  Window.h
//
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>
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

      void Hide();
      void Show();

      bool ShouldClose() const;
      void SetShouldClose(bool shouldClose);

      const std::string GetTitle() const;
      void SetTitle(const std::string& title);

      void StartRender();
      void FinishRender();

      float GetWidth() const;
      float GetHeight() const;
      glm::vec2 GetSize() const;
      void SetSize(glm::vec2 size);
      glm::vec2 GetContentScale() const;
      float GetAspectRatio() const;

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

      void Open(int width, int height);
      void UseContext();

      std::vector<uint8_t> lastKeyStates;
      std::vector<uint8_t> currentKeyStates;
      std::vector<uint8_t> lastMouseButtonStates;
      std::vector<uint8_t> currentMouseButtonStates;
      GLFWwindow *glfwWindow = nullptr;
      std::string title;
      bool hasInitialCursorPosition = false;
      glm::vec2 lastCursorPosition;
      glm::vec2 currentCursorPosition;
  }; // class Window

} // namespace dg
