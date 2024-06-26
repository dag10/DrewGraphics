//
//  Lights.cpp
//

#include "dg/Lights.h"
#include "dg/Material.h"
#include "dg/Texture.h"

// NOTE: Keep these consistent with MAX_LIGHTS in
//       -> assets/shaders/includes/fragment_head.glsl.
//       -> assets/shaders/StandardPixelShader.hlsl.
#if defined(_OPENGL)
const char *dg::Light::LIGHTS_ARRAY_NAME = "_Lights";
#elif defined(_DIRECTX)
const char *dg::Light::LIGHTS_ARRAY_NAME = "lights";
#endif
const int dg::Light::MAX_LIGHTS;

#pragma region Light

dg::Light::Light(
  glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {}

dg::Light::Light(
  glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
  SetAmbient(ambient);
  SetDiffuse(diffuse);
  SetSpecular(specular);
}

void dg::Light::SetAmbient(const glm::vec3& ambient) {
  data.ambient = ambient;
}

void dg::Light::SetDiffuse(const glm::vec3& diffuse) {
  data.diffuse = diffuse;
}

void dg::Light::SetSpecular(const glm::vec3& specular) {
  data.specular = specular;
}

void dg::Light::SetShadowMap(std::shared_ptr<Texture> shadowMap) {
  this->shadowMap = shadowMap;
  data.hasShadow = (shadowMap != nullptr) ? 1 : 0;
}

void dg::Light::SetCastShadows(bool castShadows) {
  this->castShadows = castShadows;
}

void dg::Light::SetLightTransform(const glm::mat4x4 &xf) {
  data.lightTransform = xf;
}

glm::vec3 dg::Light::GetAmbient() const {
  return data.ambient;
}

glm::vec3 dg::Light::GetDiffuse() const {
  return data.diffuse;
}

glm::vec3 dg::Light::GetSpecular() const {
  return data.specular;
}

std::shared_ptr<dg::Texture> dg::Light::GetShadowMap() const {
  return shadowMap;
}

bool dg::Light::GetCastShadows() const {
  return castShadows;
}

const glm::mat4x4 &dg::Light::GetLightTransform() const {
  return data.lightTransform;
}

dg::Light::ShaderData dg::Light::GetShaderData() const {
  return data;
}

#pragma endregion
#pragma region Directional Light

dg::DirectionalLight::DirectionalLight() : Light() {};

dg::DirectionalLight::DirectionalLight(
  glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {
  data.type = LightType::DIRECTIONAL;
}

dg::DirectionalLight::DirectionalLight(
  glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : Light(ambient, diffuse, specular) {
  data.type = LightType::DIRECTIONAL;
}

dg::Light::ShaderData dg::DirectionalLight::GetShaderData() const {
  ShaderData ret = data;
  ret.direction = (glm::vec3)CachedSceneSpace().Forward();
  return ret;
}

#pragma endregion
#pragma region Point Light

void dg::PointLight::SetConstant(const float constant) {
  data.constantCoeff = constant;
}

void dg::PointLight::SetLinear(const float linear) {
  data.linearCoeff = linear;
}

void dg::PointLight::SetQuadratic(const float quadratic) {
  data.quadraticCoeff = quadratic;
}

float dg::PointLight::GetConstant() const {
  return data.constantCoeff;
}

float dg::PointLight::GetLinear() const {
  return data.linearCoeff;
}

float dg::PointLight::GetQuadratic() const {
  return data.quadraticCoeff;
}

dg::PointLight::PointLight() {
  data.type = LightType::POINT;
}

dg::PointLight::PointLight(
  glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {
  data.type = LightType::POINT;
}

dg::PointLight::PointLight(
  glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : Light(ambient, diffuse, specular) {
  data.type = LightType::POINT;
}

dg::Light::ShaderData dg::PointLight::GetShaderData() const {
  ShaderData ret = data;
  ret.position = (glm::vec3)CachedSceneSpace().translation;
  return ret;
}

#pragma endregion
#pragma region Spot Light

void dg::SpotLight::SetCutoff(float cutoff) {
  this->cutoff = cutoff;
  data.innerCutoff = cutoff - (feather / 2.f);
  data.outerCutoff = cutoff + (feather / 2.f);
}

void dg::SpotLight::SetFeather(float feather) {
  this->feather = feather;
  data.innerCutoff = cutoff - (feather / 2.f);
  data.outerCutoff = cutoff + (feather / 2.f);
}

float dg::SpotLight::GetCutoff() const {
  return cutoff;
}

float dg::SpotLight::GetFeather() const {
  return feather;
}

dg::Light::ShaderData dg::SpotLight::GetShaderData() const {
  ShaderData ret = data;
  ret.direction = (glm::vec3)CachedSceneSpace().Forward();
  ret.position = (glm::vec3)CachedSceneSpace().translation;
  return ret;
}

dg::SpotLight::SpotLight() : Light() {
  data.type = LightType::SPOT;
  SetDefaultProperties();
}

dg::SpotLight::SpotLight(
  glm::vec3 color, float ambient, float diffuse, float specular)
  : Light(color * ambient, color * diffuse, color * specular) {
  data.type = LightType::SPOT;
  SetDefaultProperties();
}

dg::SpotLight::SpotLight(
  glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
  : Light(ambient, diffuse, specular) {
  data.type = LightType::SPOT;
  SetDefaultProperties();
}

// Set default virtual properties in underlying data struct.
void dg::SpotLight::SetDefaultProperties() {
  SetFeather(feather);
  SetCutoff(cutoff);
}

#pragma endregion
