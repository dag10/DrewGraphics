//
//  Graphics.cpp
//

#include "dg/Graphics.h"
#include <cassert>
#include "dg/Mesh.h"
#include "dg/Shader.h"
#include "dg/Window.h"

#include <WindowsX.h>

#pragma region Base Class

std::unique_ptr<dg::Graphics::graphics_class> dg::Graphics::Instance;

void dg::Graphics::Initialize(const Window& window) {
  assert(Instance == nullptr);
  Instance = std::unique_ptr<graphics_class>(new graphics_class(window));
  Instance->InitializeGraphics();
  Instance->InitializeResources();
}

void dg::Graphics::InitializeResources() {
  // Create primitive meshes.
  dg::Mesh::CreatePrimitives();
}

void dg::Graphics::Shutdown() {
  if (Instance != nullptr) {
    Instance = nullptr;
  }
}

#pragma endregion

#pragma region OpenGL Graphics
#pragma endregion
#pragma region DirectX Graphics

dg::DirectXGraphics::DirectXGraphics(const Window& window) : window(window) {}

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

  // The above function created the back buffer render target
  // for us, but we need a reference to it
  ID3D11Texture2D* backBufferTexture;
  swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                       (void**)&backBufferTexture);

  // Now that we have the texture, create a render target view
  // for the back buffer so we can render into it.  Then release
  // our local reference to the texture, since we have the view.
  device->CreateRenderTargetView(backBufferTexture, 0, &backBufferRTV);
  backBufferTexture->Release();

  // Set up the description of the texture to use for the depth buffer
  D3D11_TEXTURE2D_DESC depthStencilDesc = {};
  depthStencilDesc.Width = (unsigned int)contentSize.x;
  depthStencilDesc.Height = (unsigned int)contentSize.y;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;

  // Create the depth buffer and its view, then
  // release our reference to the texture
  ID3D11Texture2D* depthBufferTexture;
  device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
  device->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
  depthBufferTexture->Release();

  // Bind the views to the pipeline, so rendering properly
  // uses their underlying textures
  context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

  // Lastly, set up a viewport so we render into
  // to correct portion of the window
  D3D11_VIEWPORT viewport = {};
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = contentSize.x;
  viewport.Height = contentSize.y;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  context->RSSetViewports(1, &viewport);
}

dg::DirectXGraphics::~DirectXGraphics() {
  if (depthStencilView) {
    depthStencilView->Release();
  }
  if (backBufferRTV) {
    backBufferRTV->Release();
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
  contentSize = window.GetContentSize();

  // Release existing DirectX views and buffers.
  if (depthStencilView) {
    depthStencilView->Release();
  }
  if (backBufferRTV) {
    backBufferRTV->Release();
  }

  // Resize the underlying swap chain buffers.
  swapChain->ResizeBuffers(1, (unsigned int)contentSize.x,
                           (unsigned int)contentSize.y,
                           DXGI_FORMAT_R8G8B8A8_UNORM, 0);

  // Recreate the render target view for the back buffer
  // texture, then release our local texture reference.
  ID3D11Texture2D* backBufferTexture;
  swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                       reinterpret_cast<void**>(&backBufferTexture));
  device->CreateRenderTargetView(backBufferTexture, 0, &backBufferRTV);
  backBufferTexture->Release();

  // Set up the description of the texture to use for the depth buffer
  D3D11_TEXTURE2D_DESC depthStencilDesc;
  depthStencilDesc.Width = (unsigned int)contentSize.x;
  depthStencilDesc.Height = (unsigned int)contentSize.y;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;

  // Create the depth buffer and its view, then
  // release our reference to the texture
  ID3D11Texture2D* depthBufferTexture;
  device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
  device->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
  depthBufferTexture->Release();

  // Bind the views to the pipeline, so rendering properly
  // uses their underlying textures
  context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

  // Lastly, set up a viewport so we render into
  // to correct portion of the window
  D3D11_VIEWPORT viewport = {};
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = contentSize.x;
  viewport.Height = contentSize.y;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  context->RSSetViewports(1, &viewport);
}

void dg::DirectXGraphics::Clear(glm::vec3 color) {
  const float colorArray[4] = { color.x, color.y, color.z, 0 };
  context->ClearRenderTargetView(backBufferRTV, colorArray);
  context->ClearDepthStencilView(
      depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

#pragma endregion