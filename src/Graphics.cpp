//
//  Graphics.cpp
//

#include <Graphics.h>
#include <Window.h>
#include <Mesh.h>
#include <Shader.h>
#include <cassert>

#if defined(_OPENGL)
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#endif

#pragma region Base Class

std::unique_ptr<dg::Graphics> dg::Graphics::Instance;

void dg::Graphics::Initialize(const Window& window) {
  assert(Instance == nullptr);
#if defined(_DIRECTX)
  Instance = std::unique_ptr<DirectXGraphics>(new DirectXGraphics(window));
#else
  Instance = std::unique_ptr<OpenGLGraphics>(new OpenGLGraphics(window));
#endif
}

void dg::Graphics::InitializeResources() {
#if defined(_OPENGL) // TODO: Remove ifdef once Mesh supports DirectX.
  // Create primitive meshes.
  dg::Mesh::CreatePrimitives();
#endif

#if defined(_OPENGL) // TODO: Remove ifdef once Shader supports DirectX.
  // Staticially initialize shader class.
  dg::Shader::Initialize();
#endif
}

void dg::Graphics::Shutdown() {
  if (Instance != nullptr) {
    Instance = nullptr;
  }
}

#pragma endregion

#pragma region OpenGL Graphics
#if defined(_OPENGL)

dg::OpenGLGraphics::OpenGLGraphics(const Window& window) {
  // Load GLAD procedures.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD.");
  }

  InitializeResources();

  // Configure global includes for all shader files.
  dg::Shader::SetVertexHead("assets/shaders/includes/vertex_head.glsl");
  dg::Shader::AddVertexSource("assets/shaders/includes/vertex_main.glsl");
  dg::Shader::SetFragmentHead("assets/shaders/includes/fragment_head.glsl");
  dg::Shader::AddFragmentSource("assets/shaders/includes/fragment_main.glsl");
}

dg::OpenGLGraphics::~OpenGLGraphics() {
  // Even though GLFW is initialized in OpenGLWindow::InitializeGLFW(), we'll
  // shut down here because this is called at final termination of the program.
  glfwTerminate();
}

#endif
#pragma endregion
#pragma region DirectX Graphics
#if defined(_DIRECTX)

dg::DirectXGraphics::DirectXGraphics(const Window& window) {
  // TODO

  InitializeResources();
}

#endif
#pragma endregion
