//
//  raytracing/TraceableCheckerboardMaterial.cpp
//

#include <raytracing/Rays.h>
#include <raytracing/TraceableCheckerboardMaterial.h>
#include <raytracing/TraceableStandardMaterial.h>

dg::TraceableCheckerboardMaterial::TraceableCheckerboardMaterial()
    : CheckerboardMaterial(), TraceableMaterial() {}

dg::TraceableCheckerboardMaterial::TraceableCheckerboardMaterial(
    TraceableCheckerboardMaterial &other)
    : CheckerboardMaterial(other) {}

dg::TraceableCheckerboardMaterial::TraceableCheckerboardMaterial(
    TraceableCheckerboardMaterial &&other) {
  *this = std::move(other);
}

dg::TraceableCheckerboardMaterial &dg::TraceableCheckerboardMaterial::operator=(
    TraceableCheckerboardMaterial &other) {
  *this = TraceableCheckerboardMaterial(other);
  return *this;
}

dg::TraceableCheckerboardMaterial &dg::TraceableCheckerboardMaterial::operator=(
    TraceableCheckerboardMaterial &&other) {
  swap(*this, other);
  return *this;
}

void dg::swap(TraceableCheckerboardMaterial &first,
              TraceableCheckerboardMaterial &second) {
  using std::swap;
  swap((Material&)first, (Material&)second);
}

glm::vec3 dg::TraceableCheckerboardMaterial::Shade(
    const RayResult &rayres) const {
  glm::vec2 _Size = GetRequiredProperty("_Size")->value._vec2;
  glm::vec3 _Colors[2];
  _Colors[0] = GetRequiredProperty("_Colors[0]")->value._vec3;
  _Colors[1] = GetRequiredProperty("_Colors[1]")->value._vec3;

  glm::vec2 coord = rayres.interpolatedVertex.texCoord * _Size;
  int parity = int(coord.x) % 2 ^ int(coord.y) % 2;
  glm::vec3 diffuseColor = _Colors[parity];
  glm::vec3 specularColor(0);
  float shininess = 0;
  glm::vec3 scenePos =
      rayres.ray.origin + rayres.ray.direction * rayres.distance;
  glm::vec3 cameraPos = GetRequiredProperty("_CameraPosition")->value._vec3;
  glm::vec3 normal = rayres.normal;

  auto mutLightDirectIllumination =
      (std::unordered_map<int, bool> &)rayres.lightDirectIllumination;

  glm::vec3 cumulative(0);
  for (auto& light : lights) {
    glm::vec3 lightVal = TraceableStandardMaterial::CalculateLight(
        light.second, normal, diffuseColor, specularColor, scenePos, cameraPos,
        shininess);
    if (!mutLightDirectIllumination[light.first]) {
      lightVal *= 0.5f;
    }
    cumulative += lightVal;
  }

  return cumulative;
}
