//
//  raytracer/Renderer.cpp
//

#include <Canvas.h>
#include <Scene.h>
#include <Texture.h>
#include <raytracing/Renderer.h>
#include <raytracing/TraceableMaterial.h>
#include <raytracing/TraceableModel.h>
#include <forward_list>
#include <glm/gtc/matrix_access.hpp>
#include <iostream>

dg::Renderer::Renderer(unsigned int width, unsigned int height, Scene *scene)
  : scene(scene) {
  const float resScale = 2.f;
  width *= resScale;
  height *= resScale;
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
  planeRightPos_h /= planeRightPos_h.w; // Normalize homogenous coord
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

      // Bounce the ray around the scene.
      Ray initialRay(
        xfCamera.translation,
        glm::normalize(xfPixel_SS.translation - xfCamera.translation)
      );
      std::forward_list<RayResult> results;
      Ray ray = initialRay;
      int maxDepth = 20;
      while (maxDepth > 0) {
        maxDepth--;
        RayResult res = TraceRay(ray);
        results.push_front(res);
        if (res.hit && res.model->material->reflection > 0) {
          ray = res.GetReflectedRay();
          maxDepth = std::min(maxDepth, res.model->material->maxDepth);
        } else if (res.hit && res.model->material->transmission > 0) {
          ray = res.GetRefractedRay();
          maxDepth = std::min(maxDepth, res.model->material->maxDepth);
        } else {
          break;
        }
      }

      // Construct the final color of the ray by looking at its hits
      // in reverse order.
      glm::vec3 color(0.f);
      bool isInitial = true;
      while (!results.empty()) {
        auto &res = results.front();
        float blendWeight = res.model->material->reflection > 0
                                ? res.model->material->reflection
                                : res.model->material->transmission;
        float weight = isInitial ? 1 : (1.f - blendWeight);
        switch (res.model->material->rayBlendMode) {
          case RayBlendMode::Additive:
            color *= (1.f - weight);
            color += ShadeRayResult(res);
            break;
          case RayBlendMode::Weighted:
            color *= (1.f - weight);
            color += weight * ShadeRayResult(res);
            break;
        }
        results.pop_front();
        isInitial = false;
      }

      // We don't have tone management, yet, but clip out of range colors
      // to white.
      if (color.r > 1) {
        color /= color.r;
      }
      if (color.g > 1) {
        color /= color.g;
      }
      if (color.b > 1) {
        color /= color.b;
      }
      canvas->SetPixel(x, y, color.r * 255, color.g * 255, color.b * 255);
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
      } else if (auto light = std::dynamic_pointer_cast<Light>(*child)) {
        lights.push_front(light.get()->GetShaderData());
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
    RayResult res = (*model)->RayTest(ray);
    shortest = RayResult::Closest(res, shortest);
  }

  // See if intersection point is in shadow.
  if (shortest.hit) {
    for (auto &light : shortest.model->material->lights) {
      shortest.lightDirectIllumination[light.first] = false;
      if (light.second.type == Light::LightType::NONE) {
        continue;
      }
      Ray lightRay = shortest.RayToLight(light.second);
      RayResult res = RayResult::Miss(ray);
      int maxDepth = 10;
      while (maxDepth > 0) {
        maxDepth--;
        for (auto model = objects.begin(); model != objects.end(); model++) {
          if (*model == shortest.model) continue;
          res = RayResult::Closest(res, (*model)->RayTest(lightRay));
        }
        if (res.hit && res.model->material->reflection > 0) {
          lightRay = res.GetReflectedRay();
          maxDepth = std::min(maxDepth, res.model->material->maxDepth);
        } else if (res.hit && res.model->material->transmission > 0) {
          lightRay = res.GetRefractedRay();
          maxDepth = std::min(maxDepth, res.model->material->maxDepth);
        } else {
          break;
        }
        res = RayResult::Miss(lightRay);
      }
      if (!res.hit) {
        if (lightRay.IntersectLight(light.second).hit) {
          shortest.lightDirectIllumination[light.first] = true;
        }
      }
    }
  }

  return shortest;
}

glm::vec3 dg::Renderer::ShadeRayResult(RayResult rayres) {
  if (rayres.hit) {
    glm::vec3 color;

    // Shade using material.
    auto traceableMat =
        std::dynamic_pointer_cast<TraceableMaterial>(rayres.model->material);
    if (traceableMat != nullptr) {
      try {
        color = traceableMat->Shade(rayres);
      } catch (const std::exception &e) {
        throw std::runtime_error("Failed to shade: " + std::string(e.what()));
      }
    }

    return color;
  }

  // Background color.
  return glm::vec3(115, 163, 225) / 255.f;

  // Encode direction as pixel.
  //return (rayres.ray.direction + 1.f) * 0.5f;
}
