//
//  Material.cpp
//

#include "dg/Material.h"

dg::Material::Material(Material& other) {
  this->shader = other.shader;
  this->properties = other.properties;
  this->highestTexUnitHint = other.highestTexUnitHint;
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


void dg::Material::SendLights(
    const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) {
  shader->SetData(Light::LIGHTS_ARRAY_NAME, lights);
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

  shader->Use();
}

