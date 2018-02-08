//
//  raytracer/Renderer.cpp
//

#include <raytracing/Renderer.h>
#include <Texture.h>
#include <Canvas.h>
#include <Scene.h>

dg::Renderer::Renderer(unsigned int width, unsigned int height, Scene *scene)
  : scene(scene) {
  canvas = std::make_shared<Canvas>(width, height);
}

void dg::Renderer::Render() {
  // TODO: Render the scene! This is just temporary so that we at least
  //       some pixels up on the screen.
  unsigned int width = canvas->GetWidth();
  unsigned int height = canvas->GetHeight();
  for (int i = 0; i < width; i++) {
    canvas->SetPixel(i, height / 2, 255 * i / width, 0, 100);
  }
  canvas->Submit();
}

std::shared_ptr<dg::Texture> dg::Renderer::GetTexture() const {
  return canvas->GetTexture();
}

