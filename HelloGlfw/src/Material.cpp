//
//  Material.cpp
//

#include "Material.h"

dg::Material::Material(Material& other) {
  this->shader = other.shader;
  this->properties = other.properties;
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

}

void dg::Material::SetProperty(const std::string& name, bool value) {
  ShaderProperty prop;
  prop.type = PROPERTY_BOOL;
  prop.value._bool = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, int value) {
  ShaderProperty prop;
  prop.type = PROPERTY_INT;
  prop.value._int = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, float value) {
  ShaderProperty prop;
  prop.type = PROPERTY_FLOAT;
  prop.value._float = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, glm::vec2 value) {
  ShaderProperty prop;
  prop.type = PROPERTY_VEC2;
  prop.value._vec2 = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, glm::vec3 value) {
  ShaderProperty prop;
  prop.type = PROPERTY_VEC3;
  prop.value._vec3 = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, glm::vec4 value) {
  ShaderProperty prop;
  prop.type = PROPERTY_VEC4;
  prop.value._vec4 = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, glm::mat2x2 value) {
  ShaderProperty prop;
  prop.type = PROPERTY_MAT2X2;
  prop.value._mat2x2 = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, glm::mat3x3 value) {
  ShaderProperty prop;
  prop.type = PROPERTY_MAT3X3;
  prop.value._mat3x3 = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(const std::string& name, glm::mat4x4 value) {
  ShaderProperty prop;
  prop.type = PROPERTY_MAT4X4;
  prop.value._mat4x4 = value;
  properties[name] = prop;
}

void dg::Material::SetProperty(
    const std::string& name, std::shared_ptr<Texture> value) {
  ShaderProperty prop;
  prop.type = PROPERTY_TEXTURE;
  prop.texture = value;
  properties[name] = prop;
}

void dg::Material::SetCameraPosition(glm::vec3 position) {
  SetProperty("_CameraPosition", position);
}

void dg::Material::SetMatrixMVP(glm::mat4x4 mvp) {
  SetProperty("_Matrix_MVP", mvp);
}

void dg::Material::SetMatrixM(glm::mat4x4 m) {
  SetProperty("_Matrix_M", m);
}

void dg::Material::SetMatrixNormal(glm::mat3x3 normal) {
  SetProperty("_Matrix_Normal", normal);
}

void dg::Material::SetLightPosition(glm::vec3 position) {
  SetProperty("_LightPosition", position);
}

void dg::Material::SetLightColor(glm::vec3 lightColor) {
  SetProperty("_LightColor", lightColor);
}

void dg::Material::SetInvPortal(glm::mat4x4 invPortal) {
  SetProperty("_InvPortal", invPortal);
}

void dg::Material::Use() const {
  shader->Use();
  shader->ResetProperties();

  unsigned int textureUnit = 0;
  for (auto it = properties.begin(); it != properties.end(); it++) {
    switch (it->second.type) {
      case PROPERTY_BOOL:
        shader->SetBool(it->first, it->second.value._bool);
        break;
      case PROPERTY_INT:
        shader->SetInt(it->first, it->second.value._int);
        break;
      case PROPERTY_FLOAT:
        shader->SetFloat(it->first, it->second.value._float);
        break;
      case PROPERTY_VEC2:
        shader->SetVec2(it->first, it->second.value._vec2);
        break;
      case PROPERTY_VEC3:
        shader->SetVec3(it->first, it->second.value._vec3);
        break;
      case PROPERTY_VEC4:
        shader->SetVec4(it->first, it->second.value._vec4);
        break;
      case PROPERTY_MAT2X2:
        shader->SetMat2(it->first, it->second.value._mat2x2);
        break;
      case PROPERTY_MAT3X3:
        shader->SetMat3(it->first, it->second.value._mat3x3);
        break;
      case PROPERTY_MAT4X4:
        shader->SetMat4(it->first, it->second.value._mat4x4);
        break;
      case PROPERTY_TEXTURE:
        shader->SetTexture(textureUnit, it->first, *it->second.texture);
        textureUnit++;
        break;
      default:
        break;
    }
  }
}

