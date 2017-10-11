//
//  Skybox.cpp
//

#include <Skybox.h>
#include <Mesh.h>
#include <materials/StandardMaterial.h>

dg::Skybox::Skybox(std::shared_ptr<Texture> texture) {
  material = StandardMaterial::WithTexture(texture);
  material.SetLit(false);
}

dg::Skybox::Skybox(Skybox& other) {
  this->material = other.material;
}

dg::Skybox::Skybox(Skybox&& other) {
  *this = std::move(other);
}

dg::Skybox& dg::Skybox::operator=(Skybox& other) {
  *this = Skybox(other);
  return *this;
}

dg::Skybox& dg::Skybox::operator=(Skybox&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Skybox& first, Skybox& second) {
  using std::swap;
  swap(first.material, second.material);
}

void dg::Skybox::Draw(const Camera& camera, const Window& window) {
  // Move skybox position to camera position, and make it 5x5x5.
  Transform xf = Transform::TS(
      camera.transform.translation,
      glm::vec3(5));

  glm::mat4x4 view = camera.transform.Inverse().ToMat4();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window.GetWidth() / window.GetHeight());

  material.SetMatrixNormal(
      glm::mat3x3(glm::transpose(xf.Inverse().ToMat4())));
  material.SetMatrixM(xf.ToMat4());
  material.SetMatrixMVP(projection * view * xf);

  material.Use();

  glCullFace(GL_FRONT);
  glDepthMask(GL_FALSE);
  Mesh::MappedCube->Use();
  Mesh::MappedCube->Draw();
  Mesh::MappedCube->FinishUsing();
  glDepthMask(GL_TRUE);
  glCullFace(GL_BACK);
}

