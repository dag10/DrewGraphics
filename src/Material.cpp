//
//  Material.cpp
//

#include "dg/Material.h"

dg::Material::Material(Material& other) {
  this->shader = other.shader;
  this->properties = other.properties;
  this->highestTexUnitHint = other.highestTexUnitHint;
  this->rasterizerOverride = other.rasterizerOverride;
}

dg::Material::Material(Material&& other) {
  *this = std::move(other);
}

dg::Material& dg::Material::operator=(Material& other) {
  *this = Material(other);
  return *this;
}

dg::Material& dg::Material::operator=(Material&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Material& first, Material& second) {
  using std::swap;
  swap(first.shader, second.shader);
  swap(first.properties, second.properties);
  swap(first.highestTexUnitHint, second.highestTexUnitHint);
  swap(first.rasterizerOverride, second.rasterizerOverride);
}

void dg::Material::SetProperty(const std::string& name, bool value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::BOOL;
  prop.value._bool = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, int value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::INT;
  prop.value._int = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, float value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::FLOAT;
  prop.value._float = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::vec2 value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::VEC2;
  prop.value._vec2 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::vec3 value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::VEC3;
  prop.value._vec3 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::vec4 value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::VEC4;
  prop.value._vec4 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::mat4x4 value) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::MAT4X4;
  prop.value._mat4x4 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(
    const std::string& name, std::shared_ptr<Texture> value) {
  SetProperty(name, value, -1);
}

void dg::Material::SetProperty(
    const std::string& name, std::shared_ptr<Texture> value,
    int texUnitHint) {
  MaterialProperty prop;
  prop.type = MaterialPropertyType::TEXTURE;
  prop.texture = value;
  prop.texUnitHint = texUnitHint;
  if (texUnitHint > (int)highestTexUnitHint) {
    highestTexUnitHint = texUnitHint;
  }
  properties.insert_or_assign(name, prop);
}

void dg::Material::ClearProperty(const std::string& name) {
  properties.erase(name);
}

void dg::Material::SendCameraPosition(glm::vec3 position) {
  shader->SetVec3("_CameraPosition", position);
}

void dg::Material::SendMatrixMVP(glm::mat4x4 mvp) {
  shader->SetMat4("_Matrix_MVP", mvp);
}

void dg::Material::SendMatrixM(glm::mat4x4 m) {
  shader->SetMat4("_Matrix_M", m);
}

void dg::Material::SendMatrixNormal(glm::mat4x4 normal) {
  shader->SetMat4("_Matrix_Normal", normal);
}

#if defined(_OPENGL)
void dg::Material::SendLight(int index, const Light::ShaderData& data) {
  shader->SetVec3(LightProperty(index, "diffuse"), data.diffuse);
  shader->SetInt(LightProperty(index, "type"), (int)data.type);
  shader->SetVec3(LightProperty(index, "ambient"), data.ambient);
  shader->SetFloat(LightProperty(index, "innerCutoff"), data.innerCutoff);
  shader->SetVec3(LightProperty(index, "specular"), data.specular);
  shader->SetFloat(LightProperty(index, "outerCutoff"), data.outerCutoff);
  shader->SetVec3(LightProperty(index, "position"), data.position);
  shader->SetFloat(LightProperty(index, "constantCoeff"), data.constantCoeff);
  shader->SetVec3(LightProperty(index, "direction"), data.direction);
  shader->SetFloat(LightProperty(index, "linearCoeff"), data.linearCoeff);
  shader->SetFloat(LightProperty(index, "quadraticCoeff"), data.quadraticCoeff);
}

void dg::Material::ClearLights() {
  for (int i = 0; i < Light::MAX_LIGHTS; i++) {
    ClearLight(i);
  }
}

void dg::Material::ClearLight(int index) {
  shader->SetInt(LightProperty(index, "type"), (int)Light::LightType::NONE);
}
#endif

void dg::Material::SendLights(
    const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) {
#if defined(_OPENGL)
  ClearLights();
  for (int i = 0; i < Light::MAX_LIGHTS; i++) {
    if (lights[i].type != Light::LightType::NONE) {
      SendLight(i, lights[i]);
    }
  }
#elif defined(_DIRECTX)
  shader->SetData(Light::LIGHTS_ARRAY_NAME, lights);
#endif
}

const std::string dg::Material::LightProperty(
    int index, const std::string& property) {
  char buffer[128];
  assert(index >= 0 && index < Light::MAX_LIGHTS);
  assert(snprintf(
        buffer, sizeof(buffer), "%s[%d].%s",
        Light::LIGHTS_ARRAY_NAME, index, property.c_str()) >= 0);
  return std::string(buffer);
}

void dg::Material::SetInvPortal(glm::mat4x4 invPortal) {
  SetProperty("_InvPortal", invPortal);
}

void dg::Material::Use() const {
#if defined(_OPENGL)
  shader->Use();
#endif

  unsigned int textureUnit = highestTexUnitHint + 1;
  for (auto it = properties.begin(); it != properties.end(); it++) {
    switch (it->second.type) {
      case MaterialPropertyType::BOOL:
        shader->SetBool(it->first, it->second.value._bool);
        break;
      case MaterialPropertyType::INT:
        shader->SetInt(it->first, it->second.value._int);
        break;
      case MaterialPropertyType::FLOAT:
        shader->SetFloat(it->first, it->second.value._float);
        break;
      case MaterialPropertyType::VEC2:
        shader->SetVec2(it->first, it->second.value._vec2);
        break;
      case MaterialPropertyType::VEC3:
        shader->SetVec3(it->first, it->second.value._vec3);
        break;
      case MaterialPropertyType::VEC4:
        shader->SetVec4(it->first, it->second.value._vec4);
        break;
      case MaterialPropertyType::MAT4X4:
        shader->SetMat4(it->first, it->second.value._mat4x4);
        break;
      case MaterialPropertyType::TEXTURE:
        if (it->second.texUnitHint >= 0) {
          shader->SetTexture(
              it->second.texUnitHint, it->first, it->second.texture.get());
        } else {
          shader->SetTexture(textureUnit, it->first, it->second.texture.get());
          textureUnit++;
        }
        break;
      default:
        break;
    }
  }

#if defined(_DIRECTX)
  shader->Use();
#endif
}

