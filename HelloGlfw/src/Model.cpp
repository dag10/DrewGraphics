//
//  Model.cpp
//

#include "Model.h"

dg::Model::Model(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader,
    Transform transform) {
  this->mesh = mesh;
  this->shader = shader;
  this->transform = transform;
}

dg::Model::Model(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture, glm::vec2 uvScale,
    Transform transform) {
  this->mesh = mesh;
  this->shader = shader;
  this->transform = transform;

  this->texture = texture;
  this->uvScale = uvScale;
}

dg::Model::Model(Model& other) {
  this->mesh = other.mesh;
  this->shader = other.shader;
  this->transform = other.transform;

  this->texture = other.texture;
  this->uvScale = other.uvScale;
  this->invPortal = other.invPortal;

  this->lit = other.lit;
  this->albedo = other.albedo;
  this->lightColor = other.lightColor;
  this->ambientStrength = other.ambientStrength;
  this->diffuseStrength = other.diffuseStrength;
  this->specularStrength = other.specularStrength;
}

dg::Model::Model(Model&& other) {
  *this = std::move(other);
}

dg::Model& dg::Model::operator=(Model& other) {
  *this = Model(other);
  return *this;
}

dg::Model& dg::Model::operator=(Model&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Model& first, Model& second) {
  using std::swap;
  swap(first.mesh, second.mesh);
  swap(first.shader, second.shader);
  swap(first.transform, second.transform);

  swap(first.texture, second.texture);
  swap(first.uvScale, second.uvScale);
  swap(first.invPortal, second.invPortal);

  swap(first.lit, second.lit);
  swap(first.albedo, second.albedo);
  swap(first.lightColor, second.lightColor);
  swap(first.ambientStrength, second.ambientStrength);
  swap(first.diffuseStrength, second.diffuseStrength);
  swap(first.specularStrength, second.specularStrength);
}

void dg::Model::Draw(glm::mat4x4 view, glm::mat4x4 projection) const {
  shader->Use();
  shader->SetMat3(
      "MATRIX_NORMAL",
      glm::mat3x3(glm::transpose(transform.Inverse().ToMat4())));
  shader->SetMat4("MATRIX_M", transform);
  shader->SetMat4("MATRIX_MVP", projection * view * transform);
  shader->SetVec2("UVScale", uvScale);
  shader->SetMat4("InvPortal", invPortal);
  shader->SetBool("Lit", lit);
  shader->SetVec3("Albedo", albedo);
  shader->SetVec3("LightColor", lightColor);
  shader->SetFloat("AmbientStrength", ambientStrength);
  shader->SetFloat("DiffuseStrength", diffuseStrength);
  shader->SetFloat("SpecularStrength", specularStrength);

  if (texture != nullptr) {
    shader->SetTexture(0, "MainTex", *texture);
  }
  mesh->Use();
  mesh->Draw();
  mesh->FinishUsing();
}

