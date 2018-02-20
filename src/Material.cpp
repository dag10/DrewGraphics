//
//  Material.cpp
//

#include <Material.h>

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
  prop.type = PROPERTY_BOOL;
  prop.value._bool = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, int value) {
  MaterialProperty prop;
  prop.type = PROPERTY_INT;
  prop.value._int = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, float value) {
  MaterialProperty prop;
  prop.type = PROPERTY_FLOAT;
  prop.value._float = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::vec2 value) {
  MaterialProperty prop;
  prop.type = PROPERTY_VEC2;
  prop.value._vec2 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::vec3 value) {
  MaterialProperty prop;
  prop.type = PROPERTY_VEC3;
  prop.value._vec3 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::vec4 value) {
  MaterialProperty prop;
  prop.type = PROPERTY_VEC4;
  prop.value._vec4 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::mat2x2 value) {
  MaterialProperty prop;
  prop.type = PROPERTY_MAT2X2;
  prop.value._mat2x2 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::mat3x3 value) {
  MaterialProperty prop;
  prop.type = PROPERTY_MAT3X3;
  prop.value._mat3x3 = value;
  properties.insert_or_assign(name, prop);
}

void dg::Material::SetProperty(const std::string& name, glm::mat4x4 value) {
  MaterialProperty prop;
  prop.type = PROPERTY_MAT4X4;
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
  prop.type = PROPERTY_TEXTURE;
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

void dg::Material::SetLight(int index, const Light::ShaderData& data) {
  SetProperty(LightProperty(index, "diffuse"), data.diffuse);
  SetProperty(LightProperty(index, "type"), (int)data.type);
  SetProperty(LightProperty(index, "ambient"), data.ambient);
  SetProperty(LightProperty(index, "innerCutoff"), data.innerCutoff);
  SetProperty(LightProperty(index, "specular"), data.specular);
  SetProperty(LightProperty(index, "outerCutoff"), data.outerCutoff);
  SetProperty(LightProperty(index, "position"), data.position);
  SetProperty(LightProperty(index, "constantCoeff"), data.constantCoeff);
  SetProperty(LightProperty(index, "direction"), data.direction);
  SetProperty(LightProperty(index, "linearCoeff"), data.linearCoeff);
  SetProperty(LightProperty(index, "quadraticCoeff"), data.quadraticCoeff);
}

void dg::Material::ClearLights() {
  for (int i = 0; i < Light::MAX_LIGHTS; i++) {
    ClearLight(i);
  }
}

void dg::Material::ClearLight(int index) {
  SetProperty(LightProperty(index, "type"), (int)Light::LightType::NONE);
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
  shader->Use();

  unsigned int textureUnit = highestTexUnitHint + 1;
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
}

