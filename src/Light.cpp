//
//  Light.cpp
//

#include <Light.h>
#include <Material.h>

// NOTE: Keep these consistent with MAX_LIGHTS in
//       assets/shaders/includes/fragment_head.glsl.
const char *dg::Light::LIGHTS_ARRAY_NAME = "_Lights";
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
  material.SetProperty(LightProperty(index, "type"), (int)NullLightType);
}

const std::string dg::Light::LightProperty(
    int index, const std::string& property) {
  char buffer[128];
  assert(index >= 0 && index < MAX_LIGHTS);
  assert(snprintf(
        buffer, sizeof(buffer), "%s[%d].%s",
        LIGHTS_ARRAY_NAME, index, property.c_str()) >= 0);
  return std::string(buffer);
}

