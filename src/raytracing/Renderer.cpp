//
//  raytracer/Renderer.cpp
//

#include <raytracing/Renderer.h>
#include <Texture.h>
#include <Canvas.h>
#include <Scene.h>
#include <glm/gtc/matrix_access.hpp>

dg::Renderer::Renderer(unsigned int width, unsigned int height, Scene *scene)
  : scene(scene) {
  canvas = std::make_shared<Canvas>(width, height);
}

std::shared_ptr<dg::Texture> dg::Renderer::GetTexture() const {
  return canvas->GetTexture();
}

void dg::Renderer::Render() {
  std::shared_ptr<Camera> camera = scene->GetMainCamera();
  Transform xfCamera = camera->SceneSpace();

  glm::mat4x4 plane_CS(
      glm::vec4(0, 0, 0, 1), // Center position of near clip plane
      glm::vec4(1, 0, 0, 1), // Right extent of near clip plane
      glm::vec4(0, 1, 0, 1), // Up extent of near clip plane
      glm::vec4(0, 0, 0, 0)  // Nothing
      );
  glm::mat4x4 plane_SS =
    xfCamera.ToMat4() *
    glm::inverse(camera->GetProjectionMatrix(
          (float)canvas->GetWidth() / (float)canvas->GetHeight())) *
    plane_CS;

  glm::vec4 planeCenter_h = glm::column(plane_SS, 0);
  planeCenter_h /= planeCenter_h.w; // Normalize homogenous coords
  glm::vec3 planeCenter(planeCenter_h.x, planeCenter_h.y, planeCenter_h.z);

  glm::vec4 planeRightPos_h = glm::column(plane_SS, 1);
  planeRightPos_h /= planeRightPos_h.w; // Normalize homogenous coords
  glm::vec3 planeRightPos(planeRightPos_h.x, planeRightPos_h.y, planeRightPos_h.z);
  glm::vec3 planeRight = planeRightPos - planeCenter;

  glm::vec4 planeUpPos_h = glm::column(plane_SS, 2);
  planeUpPos_h /= planeUpPos_h.w; // Normalize homogenous coords
  glm::vec3 planeUpPos(planeUpPos_h.x, planeUpPos_h.y, planeUpPos_h.z);
  glm::vec3 planeUp = planeUpPos - planeCenter;

  unsigned int width = canvas->GetWidth();
  unsigned int height = canvas->GetHeight();
  glm::vec3 halfPixelRight = planeRight / (float)width;
  glm::vec3 halfPixelUp = planeUp / (float)height;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      float i = ((float)x / width * 2) - 1;
      float j = ((float)y / height * 2) - 1;

      Transform xfPixel_SS = Transform::T(
          halfPixelUp + halfPixelRight + planeCenter +
          (i * planeRight) + (j * planeUp));

      Ray ray(
        xfCamera.translation,
        glm::normalize(xfPixel_SS.translation - xfCamera.translation)
      );
      RayResult res = TraceRay(ray);
      Pixel pixel = RenderPixel(res);
      canvas->SetPixel(x, y, pixel.red, pixel.green, pixel.blue);
    }
  }
  canvas->Submit();
}

dg::Renderer::RayResult dg::Renderer::TraceRay(Ray ray) {
  return RayResult(ray, 0); // TODO
}

dg::Renderer::Pixel dg::Renderer::RenderPixel(RayResult rayres) {
  // Encode direction as pixel.
  return Pixel((rayres.ray.direction + 1.f) * 0.5f);

  // Encode distance as pixel.
  //uint8_t value = rayres.distance * 255 / 10;
  //return Pixel(value, value, value);
}

