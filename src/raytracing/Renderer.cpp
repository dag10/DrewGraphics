//
//  raytracer/Renderer.cpp
//

#include <raytracing/Renderer.h>
#include <raytracing/TraceableModel.h>
#include <Texture.h>
#include <Canvas.h>
#include <Scene.h>
#include <glm/gtc/matrix_access.hpp>
#include <iostream>

dg::Renderer::Renderer(unsigned int width, unsigned int height, Scene *scene)
  : scene(scene) {
  canvas = std::make_shared<Canvas>(width, height);
}

std::shared_ptr<dg::Texture> dg::Renderer::GetTexture() const {
  return canvas->GetTexture();
}

void dg::Renderer::Render() {
  double startTime = glfwGetTime();

  ProcessSceneObjects();

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

  double endTime = glfwGetTime();
  double deltaTime = endTime - startTime;
  double deltaPerPixel = deltaTime / (width * height);
  double deltaPerObject = deltaTime / numObjects;
  double deltaPerObjectPixel = deltaTime / (width * height) / numObjects;
  std::cout << std::fixed
    << "Render finished in " << deltaTime << " seconds." << std::endl
    << std::endl
    << deltaPerPixel << " seconds per pixel" << std::endl
    << deltaPerObject << " seconds per object (cumulative)" << std::endl
    << deltaPerObjectPixel << " seconds per object per pixel" << std::endl
    << std::endl;
}

void dg::Renderer::ProcessSceneObjects() {
  std::forward_list<const SceneObject*> remainingObjects;
  remainingObjects.push_front(scene);
  while (!remainingObjects.empty()) {
    const SceneObject *obj = remainingObjects.front();
    remainingObjects.pop_front();
    for (auto child = obj->Children().begin();
         child != obj->Children().end();
         child++) {
      if (!(*child)->enabled) continue;
      remainingObjects.push_front(child->get());
      if (auto model = std::dynamic_pointer_cast<TraceableModel>(*child)) {
        model->CacheTransforms();
        objects.push_front((const TraceableModel*)(model.get()));
        numObjects++;
      }
    }
  }

  std::cout << "Rendering " << numObjects << " objects..." << std::endl;
}

dg::RayResult dg::Renderer::TraceRay(Ray ray) {
  std::shared_ptr<Camera> camera = scene->GetMainCamera();
  RayResult shortest = RayResult::Miss(ray);

  for (auto model = objects.begin(); model != objects.end(); model++) {
    shortest = RayResult::Closest((*model)->RayTest(ray), shortest);
  }

  return shortest;
}

dg::Renderer::Pixel dg::Renderer::RenderPixel(RayResult rayres) {
  if (rayres.hit) {
    glm::vec3 color = glm::vec3(rayres.distance / 50.f);

    if (rayres.model->mesh == Mesh::Sphere) {
      color *= glm::vec3(1, 0, 0);
    } else if (rayres.model->mesh == Mesh::Cube) {
      color *= glm::vec3(0, 1, 0);
    }

    return Pixel(color);
  }

  // Encode direction as pixel.
  return Pixel((rayres.ray.direction + 1.f) * 0.5f);
}

