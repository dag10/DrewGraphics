//
//  Light.cpp
//

#include <Light.h>
#include <Material.h>

// These are the names of all members of the Light struct
// as defined in assets/shaders/includes/fragment_head.glsl.
// This list excludes "type" because we want to explicitly set that
// every time we render.
static const auto allLightProperties = std::vector<std::string> {
  "diffuse",
  "ambient",
  "specular",
  "position",
  "direction",
  "constant",
  "linear",
  "quadratic",
};

// The name and size of the lights array as defined in
// assets/shaders/includes/fragment_head.glsl.
static const std::string lightsArrayName = "_Lights";
const int dg::Light::MAX_LIGHTS = 2;

dg::Light::Light(
    glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {}

dg::Light::Light(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : ambient(ambient), diffuse(diffuse), specular(specular) {}

void dg::Light::SetMaterialProperties(int index, Material& material) const {
  material.SetProperty(LightProperty(index, "type"), (int)type);
  material.SetProperty(LightProperty(index, "ambient"), ambient);
  material.SetProperty(LightProperty(index, "diffuse"), diffuse);
  material.SetProperty(LightProperty(index, "specular"), specular);
}

void dg::Light::ClearMaterialProperties(int index, Material& material) {
  material.SetProperty(LightProperty(index, "type"), (int)UnknownLightType);
}

const std::string dg::Light::LightProperty(
    int index, const std::string& property) {
  char buffer[128];
  assert(index >= 0 && index < MAX_LIGHTS);
  assert(snprintf(
        buffer, sizeof(buffer), "%s[%d].%s",
        lightsArrayName.c_str(), index, property.c_str()) >= 0);
  return std::string(buffer);
}

