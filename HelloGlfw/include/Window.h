//
//  Window.h
//
#pragma once

#include <memory>
#include <string>
#include <GLFW/glfw3.h>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the GLFW window resource.
  class Window {

    public:
      static Window Open(
          unsigned int width, unsigned int height, std::string title);

      Window() = default;
      Window(Window& other) = delete;
      Window(Window&& other);
      ~Window();
      Window& operator=(Window& other) = delete;
      Window& operator=(Window&& other);
      friend void swap(Window& first, Window& second); // nothrow

      bool ShouldClose() const;
      void SetShouldClose(bool shouldClose);

      bool IsKeyPressed(GLenum key) const;

      void StartRender();
      void FinishRender();

      float GetWidth() const;
      float GetHeight() const;

      GLFWwindow *GetHandle() const;

    private:
      void Open();
      void UseContext();

      GLFWwindow *glfwWindow = nullptr;
      int width = 0;
      int height = 0;
      std::string title;
  }; // class Window

} // namespace dg
