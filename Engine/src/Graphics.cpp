//
//  Graphics.cpp
//

#include "dg/Graphics.h"
#include <cassert>
#include "dg/Exceptions.h"
#include "dg/FrameBuffer.h"
#include "dg/Mesh.h"
#include "dg/RasterizerState.h"
#include "dg/Shader.h"
#include "dg/Window.h"

#if defined(_DIRECTX)
#include <WindowsX.h>
#endif

#pragma region Base Class

std::unique_ptr<dg::Graphics::graphics_class> dg::Graphics::Instance;

void dg::Graphics::Initialize(Window& window) {
  assert(Instance == nullptr);
  Instance = std::unique_ptr<graphics_class>(new graphics_class(window));
  Instance->InitializeGraphics();
  Instance->InitializeResources();
}

void dg::Graphics::InitializeResources() {
  // Create primitive meshes.
  dg::Mesh::CreatePrimitives();
}

void dg::Graphics::PushRasterizerState(const RasterizerState &state) {
  if (states.empty()) {
    states.push_front(
        std::unique_ptr<RasterizerState>(new RasterizerState(state)));
  } else {
    states.push_front(std::unique_ptr<RasterizerState>(
        new RasterizerState(*states.front() + state)));
  }
}

void dg::Graphics::PopRasterizerState() {
  states.pop_front();
}

void dg::Graphics::ApplyCurrentRasterizerState() {
  if (states.empty()) {
    return;
  }

  auto &state = *states.front();

#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
  if (!state.Declares##public_name()) { \
    throw UndeclaredRasterizerStateAttribute(#public_name); \
  }
#include "dg/RasterizerStateAttributes.def"

  ApplyRasterizerState(state);
}

const dg::RasterizerState *dg::Graphics::GetEffectiveRasterizerState() const {
  if (states.empty()) {
    return &emptyRasterizerState;
  } else {
    return states.front().get();
  }
}

void dg::Graphics::Shutdown() {
  if (Instance != nullptr) {
    Instance = nullptr;
  }
}

const std::string &dg::Graphics::GetAPIName() { return apiName; }

#pragma endregion

#pragma region OpenGL Graphics
#if defined(_OPENGL)

const std::string dg::Graphics::apiName = "OpenGL";

dg::OpenGLGraphics::OpenGLGraphics(Window& window) {}

void dg::OpenGLGraphics::InitializeGraphics() {
  // Load GLAD procedures.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD.");
  }
}

void dg::OpenGLGraphics::InitializeResources() {
  Graphics::InitializeResources();
}

void dg::OpenGLGraphics::SetRenderTarget(FrameBuffer &frameBuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.GetHandle());
  SetViewport(0, 0, frameBuffer.GetWidth(), frameBuffer.GetHeight());
  unsigned int attachments[10];
  for (int i = 0; i < sizeof(attachments) / sizeof(attachments[0]); i++) {
    attachments[i] = GL_COLOR_ATTACHMENT0 + i;
  }
  glDrawBuffers(frameBuffer.ColorTextureCount(), attachments);
}

void dg::OpenGLGraphics::SetRenderTarget(Window& window) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glm::vec2 size = window.GetFramebufferSize();
  SetViewport(0, 0, (int)size.x, (int)size.y);

  unsigned int attachment = GL_COLOR_ATTACHMENT0;
  glDrawBuffers(1, &attachment);
}

void dg::OpenGLGraphics::SetViewport(int x, int y, int width, int height) {
  viewportDimensions = glm::vec2((float)width, (float)height);
  glViewport(x, y, width, height);
}

void dg::OpenGLGraphics::ClearColor(glm::vec3 color, bool clearDepth,
                                    bool clearStencil) {
  GLenum clearBits = GL_COLOR_BUFFER_BIT;
  if (clearDepth) {
    clearBits |= GL_DEPTH_BUFFER_BIT;
  }
  if (clearStencil) {
    clearBits |= GL_STENCIL_BUFFER_BIT;
  }

  if (clearDepth || clearStencil) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
  }
  glClearColor(color.x, color.y, color.z, 1);
  glClear(clearBits);
}

void dg::OpenGLGraphics::ClearDepthStencil(bool clearDepth, bool clearStencil) {
  if (!clearDepth && !clearStencil) {
    return;
  }
  GLenum clearBits = GL_NONE;
  if (clearDepth) {
    clearBits |= GL_DEPTH_BUFFER_BIT;
  }
  if (clearStencil) {
    clearBits |= GL_STENCIL_BUFFER_BIT;
  }
  if (clearDepth || clearStencil) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
  }
  glClear(clearBits);
}

void dg::OpenGLGraphics::ApplyRasterizerState(const RasterizerState &state) {
  auto cullMode = state.GetCullMode();
  switch (cullMode) {
    case RasterizerState::CullMode::OFF:
      glDisable(GL_CULL_FACE);
      break;
    case RasterizerState::CullMode::FRONT:
    case RasterizerState::CullMode::BACK:
      glEnable(GL_CULL_FACE);
      glCullFace(ToGLEnum(cullMode));
      break;
  }

  bool writeDepth = state.GetWriteDepth();
  auto depthFunc = state.GetDepthFunc();
  if (!writeDepth && depthFunc == RasterizerState::DepthFunc::ALWAYS) {
    glDisable(GL_DEPTH_TEST);
  } else {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(writeDepth ? GL_TRUE : GL_FALSE);
    glDepthFunc(ToGLEnum(depthFunc));
  }

  if (state.GetBlendEnabled()) {
    glEnable(GL_BLEND);
    glBlendEquationSeparate(ToGLEnum(state.GetRGBBlendEquation()),
                            ToGLEnum(state.GetAlphaBlendEquation()));
    glBlendFuncSeparate(ToGLEnum(state.GetSrcRGBBlendFunc()),
                        ToGLEnum(state.GetDstRGBBlendFunc()),
                        ToGLEnum(state.GetSrcAlphaBlendFunc()),
                        ToGLEnum(state.GetDstAlphaBlendFunc()));
  } else {
    glDisable(GL_BLEND);
  }

  glPolygonMode(GL_FRONT_AND_BACK, ToGLEnum(state.GetFillMode()));
}

GLenum dg::OpenGLGraphics::ToGLEnum(RasterizerState::CullMode cullMode) {
  switch (cullMode) {
    case RasterizerState::CullMode::OFF:
      return GL_NONE;
    case RasterizerState::CullMode::FRONT:
      return GL_FRONT;
    case RasterizerState::CullMode::BACK:
      return GL_BACK;
  }
  return GL_NONE;
}

GLenum dg::OpenGLGraphics::ToGLEnum(RasterizerState::DepthFunc depthFunc) {
  switch (depthFunc) {
    case RasterizerState::DepthFunc::ALWAYS:
      return GL_ALWAYS;
    case RasterizerState::DepthFunc::LESS:
      return GL_LESS;
    case RasterizerState::DepthFunc::EQUAL:
      return GL_EQUAL;
    case RasterizerState::DepthFunc::LEQUAL:
      return GL_LEQUAL;
    case RasterizerState::DepthFunc::GREATER:
      return GL_GREATER;
    case RasterizerState::DepthFunc::NOTEQUAL:
      return GL_NOTEQUAL;
    case RasterizerState::DepthFunc::GEQUAL:
      return GL_GEQUAL;
  }
  return GL_NONE;
}

GLenum dg::OpenGLGraphics::ToGLEnum(
    RasterizerState::BlendEquation blendEquation) {
  switch (blendEquation) {
    case RasterizerState::BlendEquation::ADD:
      return GL_FUNC_ADD;
    case RasterizerState::BlendEquation::SUBTRACT:
      return GL_FUNC_SUBTRACT;
    case RasterizerState::BlendEquation::REVERSE_SUBTRACT:
      return GL_FUNC_REVERSE_SUBTRACT;
    case RasterizerState::BlendEquation::MIN:
      return GL_MIN;
    case RasterizerState::BlendEquation::MAX:
      return GL_MAX;
  }
  return GL_NONE;
}

GLenum dg::OpenGLGraphics::ToGLEnum(
    RasterizerState::BlendFunc blendFunction) {
  switch (blendFunction) {
    case RasterizerState::BlendFunc::ZERO:
      return GL_ZERO;
    case RasterizerState::BlendFunc::ONE:
      return GL_ONE;
    case RasterizerState::BlendFunc::SRC_COLOR:
      return GL_SRC_COLOR;
    case RasterizerState::BlendFunc::ONE_MINUS_SRC_COLOR:
      return GL_ONE_MINUS_SRC_COLOR;
    case RasterizerState::BlendFunc::DST_COLOR:
      return GL_DST_COLOR;
    case RasterizerState::BlendFunc::ONE_MINUS_DST_COLOR:
      return GL_ONE_MINUS_DST_COLOR;
    case RasterizerState::BlendFunc::SRC_ALPHA:
      return GL_SRC_ALPHA;
    case RasterizerState::BlendFunc::ONE_MINUS_SRC_ALPHA:
      return GL_ONE_MINUS_SRC_ALPHA;
    case RasterizerState::BlendFunc::DST_ALPHA:
      return GL_DST_ALPHA;
    case RasterizerState::BlendFunc::ONE_MINUS_DST_ALPHA:
      return GL_ONE_MINUS_DST_ALPHA;
  }
  return GL_NONE;
}

GLenum dg::OpenGLGraphics::ToGLEnum(RasterizerState::FillMode fillMode) {
  switch (fillMode) {
    case RasterizerState::FillMode::LINE:
      return GL_LINE;
    case RasterizerState::FillMode::FILL:
      return GL_FILL;
  }
  return GL_NONE;
}

#endif
#pragma endregion
#pragma region DirectX Graphics
#if defined(_DIRECTX)

const std::string dg::Graphics::apiName = "DirectX";

dg::DirectXGraphics::DirectXGraphics(Window& window) : window(window) {}

void dg::DirectXGraphics::InitializeGraphics() {
  unsigned int deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
  deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  contentSize = window.GetContentSize();

  DXGI_SWAP_CHAIN_DESC swapDesc = {};
  swapDesc.BufferCount = 1;
  swapDesc.BufferDesc.Width = (unsigned int)contentSize.x;
  swapDesc.BufferDesc.Height = (unsigned int)contentSize.y;
  swapDesc.BufferDesc.RefreshRate.Numerator = 60;
  swapDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapDesc.Flags = 0;
  swapDesc.OutputWindow = window.GetHandle();
  swapDesc.SampleDesc.Count = 1;
  swapDesc.SampleDesc.Quality = 0;
  swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  swapDesc.Windowed = true;

  HRESULT hr = S_OK;

  // Attempt to initialize DirectX
  hr = D3D11CreateDeviceAndSwapChain(
      0,  // Video adapter (physical GPU) to use, or null for default
      D3D_DRIVER_TYPE_HARDWARE,  // We want to use the hardware (GPU)
      0,                         // Used when doing software rendering
      deviceFlags,               // Any special options
      0,  // Optional array of possible verisons we want as fallbacks
      0,  // The number of fallbacks in the above param
      D3D11_SDK_VERSION,  // Current version of the SDK
      &swapDesc,          // Address of swap chain options
      &swapChain,         // Pointer to our Swap Chain pointer
      &device,            // Pointer to our Device pointer
      &dxFeatureLevel,  // This will hold the actual feature level the app will
                        // use
      &context);        // Pointer to our Device Context pointer
  if (FAILED(hr)) {
    throw std::runtime_error("Failed to create D3D11 device and swap chain.");
  }

  ID3D11Texture2D* backBufferTexture;
  swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                       (void**)&backBufferTexture);
  device->CreateRenderTargetView(backBufferTexture, 0, &windowRenderTargetView);
  backBufferTexture->Release();

  TextureOptions depthTexOpts;
  depthTexOpts.width = (unsigned int)contentSize.x;
  depthTexOpts.height = (unsigned int)contentSize.y;
  depthTexOpts.format = TexturePixelFormat::DEPTH_STENCIL;
  depthTexOpts.type = TexturePixelType::INT;
  depthTexOpts.wrap = TextureWrap::CLAMP_EDGE;
  depthTexOpts.mipmap = false;
  depthTexOpts.shaderReadable = false;
  auto depthTexture = Texture::Generate(depthTexOpts);

  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Format =
      depthTexOpts.GetDirectXDepthStencilFormat();
  device->CreateDepthStencilView(depthTexture->GetTexture(),
                                 &depthStencilViewDesc,
                                 &windowDepthStencilView);

  SetRenderTarget(window);
}

dg::DirectXGraphics::~DirectXGraphics() {
  if (windowDepthStencilView) {
    windowDepthStencilView->Release();
  }
  if (windowRenderTargetView) {
    windowRenderTargetView->Release();
  }
  if (swapChain) {
    swapChain->Release();
  }
  if (context) {
    context->Release();
  }
  if (device) {
    device->Release();
  }
}

void dg::DirectXGraphics::OnWindowResize(const Window& window) {
  bool windowIsTarget = (currentRenderTargetView == windowRenderTargetView);
  contentSize = window.GetContentSize();

  if (windowDepthStencilView) {
    windowDepthStencilView->Release();
  }
  if (windowRenderTargetView) {
    windowRenderTargetView->Release();
  }

  swapChain->ResizeBuffers(1, (unsigned int)contentSize.x,
                           (unsigned int)contentSize.y,
                           DXGI_FORMAT_R8G8B8A8_UNORM, 0);

  ID3D11Texture2D* backBufferTexture;
  swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                       reinterpret_cast<void**>(&backBufferTexture));
  device->CreateRenderTargetView(backBufferTexture, 0, &windowRenderTargetView);
  backBufferTexture->Release();

  TextureOptions depthTexOpts;
  depthTexOpts.width = (unsigned int)contentSize.x;
  depthTexOpts.height = (unsigned int)contentSize.y;
  depthTexOpts.format = TexturePixelFormat::DEPTH_STENCIL;
  depthTexOpts.type = TexturePixelType::INT;
  depthTexOpts.wrap = TextureWrap::CLAMP_EDGE;
  depthTexOpts.mipmap = false;
  depthTexOpts.shaderReadable = false;
  auto depthTexture = Texture::Generate(depthTexOpts);

  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Format = depthTexOpts.GetDirectXDepthStencilFormat();
  device->CreateDepthStencilView(depthTexture->GetTexture(),
                                 &depthStencilViewDesc,
                                 &windowDepthStencilView);

  if (windowIsTarget) {
    SetViewport(0, 0, (int)contentSize.x, (int)contentSize.y);
  }
}

void dg::DirectXGraphics::SetRenderTarget(FrameBuffer &frameBuffer) {
  SetViewport(0, 0, frameBuffer.GetWidth(), frameBuffer.GetHeight());
  currentRenderTargetView = frameBuffer.GetRenderTargetView();
  currentDepthStencilView = frameBuffer.GetDepthStencilView();
  context->OMSetRenderTargets(1, &currentRenderTargetView,
                              currentDepthStencilView);
}

void dg::DirectXGraphics::SetRenderTarget(Window& window) {
  SetViewport(0, 0, (int)window.GetWidth(), (int)window.GetHeight());
  currentRenderTargetView = windowRenderTargetView;
  currentDepthStencilView = windowDepthStencilView;
  context->OMSetRenderTargets(1, &currentRenderTargetView,
                              currentDepthStencilView);
}

void dg::DirectXGraphics::SetViewport(int x, int y, int width, int height) {
  viewportDimensions = glm::vec2((float)width, (float)height);
  D3D11_VIEWPORT viewport = {};
  viewport.TopLeftX = (float)x;
  viewport.TopLeftY = (float)y;
  viewport.Width = (float)width;
  viewport.Height = (float)height;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  context->RSSetViewports(1, &viewport);
}

void dg::DirectXGraphics::ClearColor(glm::vec3 color, bool clearDepth,
                                     bool clearStencil) {
  const float colorArray[4] = { color.x, color.y, color.z, 0 };
  context->ClearRenderTargetView(currentRenderTargetView, colorArray);
  if (clearDepth || clearStencil) {
    int clearBits = 0;
    if (clearDepth) {
      clearBits |= D3D11_CLEAR_DEPTH;
    }
    if (clearStencil) {
      clearBits |= D3D11_CLEAR_STENCIL;
    }
    context->ClearDepthStencilView(currentDepthStencilView, clearBits, 1.0f, 0);
  }
}

void dg::DirectXGraphics::ClearDepthStencil(bool clearDepth,
                                              bool clearStencil) {
  context->ClearDepthStencilView(currentDepthStencilView,
                                 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,
                                 0);
}


void dg::DirectXGraphics::ApplyRasterizerState(const RasterizerState &state) {
  auto hash = std::hash<RasterizerState>{}(state);
  auto stateResourcesIter = rasterizerStateResources.find(hash);
  RasterizerStateResources *stateResourcesPtr;
  if (stateResourcesIter != rasterizerStateResources.end()) {
    stateResourcesPtr = stateResourcesIter->second.get();
  } else {
    rasterizerStateResources[hash] = CreateRasterizerStateResources(state);
    stateResourcesPtr = rasterizerStateResources[hash].get();
  }

  context->RSSetState(stateResourcesPtr->rsState);
  context->OMSetDepthStencilState(stateResourcesPtr->dsState, 0);
  context->OMSetBlendState(stateResourcesPtr->blendState, NULL, 0xffffffff);
}

std::shared_ptr<dg::DirectXGraphics::RasterizerStateResources>
dg::DirectXGraphics::CreateRasterizerStateResources(
    const RasterizerState& state) {
  auto resources =
      std::shared_ptr<RasterizerStateResources>(new RasterizerStateResources());

  CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
  rasterizerDesc.CullMode = CullModeToDXEnum(state.GetCullMode());
  rasterizerDesc.FillMode = FillModeToDXEnum(state.GetFillMode());
  rasterizerDesc.FrontCounterClockwise = state.GetFlipRenderY();
  HRESULT hr =
      device->CreateRasterizerState(&rasterizerDesc, &resources->rsState);
  if (FAILED(hr)) {
    throw std::runtime_error(
        "Failed to create DirectX RasterizerState resource for state:\n" +
        std::to_string(state));
  }

  CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
  bool writeDepth = state.GetWriteDepth();
  auto depthFunc = state.GetDepthFunc();
  if (!writeDepth && depthFunc == RasterizerState::DepthFunc::ALWAYS) {
    depthStencilDesc.DepthEnable = false;
  } else {
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = WriteDepthToDXEnum(writeDepth);
    depthStencilDesc.DepthFunc = DepthFuncToDXEnum(depthFunc);
  }
  hr = device->CreateDepthStencilState(&depthStencilDesc, &resources->dsState);
  if (FAILED(hr)) {
    throw std::runtime_error(
        "Failed to create DirectX DepthStencilState resource for state:\n" +
        std::to_string(state));
  }

  CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);
  if (state.GetBlendEnabled()) {
    blendDesc.RenderTarget->BlendEnable = true;
    blendDesc.IndependentBlendEnable = true;
    blendDesc.RenderTarget->BlendOp =
        BlendEquationToDXEnum(state.GetRGBBlendEquation());
    blendDesc.RenderTarget->BlendOpAlpha =
        BlendEquationToDXEnum(state.GetAlphaBlendEquation());
    blendDesc.RenderTarget->SrcBlend =
        BlendFuncToDXEnum(state.GetSrcRGBBlendFunc());
    blendDesc.RenderTarget->SrcBlendAlpha =
        BlendFuncToDXEnum(state.GetSrcAlphaBlendFunc());
    blendDesc.RenderTarget->DestBlend =
        BlendFuncToDXEnum(state.GetDstRGBBlendFunc());
    blendDesc.RenderTarget->DestBlendAlpha =
        BlendFuncToDXEnum(state.GetDstAlphaBlendFunc());
    blendDesc.RenderTarget->RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;
  } else {
    blendDesc.RenderTarget->BlendEnable = false;
  }
  hr = device->CreateBlendState(&blendDesc, &resources->blendState);
  if (FAILED(hr)) {
    throw std::runtime_error(
        "Failed to create DirectX BlendState resource for state:\n" +
        std::to_string(state));
  }

  return resources;
}

D3D11_CULL_MODE dg::DirectXGraphics::CullModeToDXEnum(
    RasterizerState::CullMode cullMode) {
  switch (cullMode) {
    case RasterizerState::CullMode::OFF:
      return D3D11_CULL_NONE;
    case RasterizerState::CullMode::FRONT:
      return D3D11_CULL_FRONT;
    case RasterizerState::CullMode::BACK:
      return D3D11_CULL_BACK;
  }
  throw EngineError("Can't get D3D11_CULL_MODE for unknown CullMode: " +
                    std::to_string((int)cullMode));
}

D3D11_DEPTH_WRITE_MASK dg::DirectXGraphics::WriteDepthToDXEnum(
    bool writeDepth) {
  return writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
}

D3D11_COMPARISON_FUNC dg::DirectXGraphics::DepthFuncToDXEnum(
    RasterizerState::DepthFunc depthFunc) {
  switch (depthFunc) {
    case RasterizerState::DepthFunc::ALWAYS:
      return D3D11_COMPARISON_ALWAYS;
    case RasterizerState::DepthFunc::LESS:
      return D3D11_COMPARISON_LESS;
    case RasterizerState::DepthFunc::EQUAL:
      return D3D11_COMPARISON_EQUAL;
    case RasterizerState::DepthFunc::LEQUAL:
      return D3D11_COMPARISON_LESS_EQUAL;
    case RasterizerState::DepthFunc::GREATER:
      return D3D11_COMPARISON_GREATER;
    case RasterizerState::DepthFunc::NOTEQUAL:
      return D3D11_COMPARISON_NOT_EQUAL;
    case RasterizerState::DepthFunc::GEQUAL:
      return D3D11_COMPARISON_GREATER_EQUAL;
  }
  throw EngineError("Can't get D3D11_DEPTH_WRITE_MASK for unknown DepthFunc: " +
                    std::to_string((int)depthFunc));
}

D3D11_BLEND_OP dg::DirectXGraphics::BlendEquationToDXEnum(
    RasterizerState::BlendEquation blendEquation) {
  switch (blendEquation) {
    case RasterizerState::BlendEquation::ADD:
      return D3D11_BLEND_OP_ADD;
    case RasterizerState::BlendEquation::SUBTRACT:
      return D3D11_BLEND_OP_SUBTRACT;
    case RasterizerState::BlendEquation::REVERSE_SUBTRACT:
      return D3D11_BLEND_OP_REV_SUBTRACT;
    case RasterizerState::BlendEquation::MIN:
      return D3D11_BLEND_OP_MIN;
    case RasterizerState::BlendEquation::MAX:
      return D3D11_BLEND_OP_MAX;
  }
  throw EngineError("Can't get D3D11_BLEND_OP for unknown BlendEquation: " +
                    std::to_string((int)blendEquation));
}

D3D11_BLEND dg::DirectXGraphics::BlendFuncToDXEnum(
    RasterizerState::BlendFunc blendFunction) {
  switch (blendFunction) {
    case RasterizerState::BlendFunc::ZERO:
      return D3D11_BLEND_ZERO;
    case RasterizerState::BlendFunc::ONE:
      return D3D11_BLEND_ONE;
    case RasterizerState::BlendFunc::SRC_COLOR:
      return D3D11_BLEND_SRC_COLOR;
    case RasterizerState::BlendFunc::ONE_MINUS_SRC_COLOR:
      return D3D11_BLEND_INV_SRC_COLOR;
    case RasterizerState::BlendFunc::DST_COLOR:
      return D3D11_BLEND_DEST_COLOR;
    case RasterizerState::BlendFunc::ONE_MINUS_DST_COLOR:
      return D3D11_BLEND_INV_DEST_COLOR;
    case RasterizerState::BlendFunc::SRC_ALPHA:
      return D3D11_BLEND_SRC_ALPHA;
    case RasterizerState::BlendFunc::ONE_MINUS_SRC_ALPHA:
      return D3D11_BLEND_INV_SRC_ALPHA;
    case RasterizerState::BlendFunc::DST_ALPHA:
      return D3D11_BLEND_DEST_ALPHA;
    case RasterizerState::BlendFunc::ONE_MINUS_DST_ALPHA:
      return D3D11_BLEND_INV_DEST_ALPHA;
  }
  throw EngineError("Can't get D3D11_BLEND for unknown BlendFunc: " +
                    std::to_string((int)blendFunction));
}

D3D11_FILL_MODE dg::DirectXGraphics::FillModeToDXEnum(
    RasterizerState::FillMode fillMode) {
  switch (fillMode) {
    case RasterizerState::FillMode::LINE:
      return D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    case RasterizerState::FillMode::FILL:
      return D3D11_FILL_MODE::D3D11_FILL_SOLID;
  }
  throw EngineError("Can't get D3D11_FILL_MODE for unknown FillMode: " +
                    std::to_string((int)fillMode));
}

dg::DirectXGraphics::RasterizerStateResources::~RasterizerStateResources() {
  if (rsState != NULL) {
    rsState->Release();
    rsState = NULL;
  }
  if (dsState != NULL) {
    dsState->Release();
    dsState = NULL;
  }
  if (blendState != NULL) {
    blendState->Release();
    blendState = NULL;
  }
}

#endif
#pragma endregion
