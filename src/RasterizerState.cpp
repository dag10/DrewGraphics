//
//  RasterizerState.cpp
//

#include "dg/RasterizerState.h"
#include <string>

dg::RasterizerState::RasterizerState(const RasterizerState &other) {
  declaredAttributes = other.declaredAttributes;
  importantAttributes = other.importantAttributes;

  cullMode = other.cullMode;
  writeDepth = other.writeDepth;
  depthFunc = other.depthFunc;
}

bool dg::RasterizerState::HasDeclaredAttributes() const {
  return static_cast<bool>(declaredAttributes);
}

void dg::RasterizerState::SetCullMode(CullMode mode, bool important) {
  cullMode = mode;
  DeclareAttribute(AttrFlag::CULL);
  SetImportant(AttrFlag::CULL, important);
}

void dg::RasterizerState::ClearCullMode() {
  UndeclareAttribute(AttrFlag::CULL);
}

dg::RasterizerState::CullMode dg::RasterizerState::GetCullMode() const {
  return cullMode;
}

void dg::RasterizerState::SetWriteDepth(bool writeDepth, bool important) {
  this->writeDepth = writeDepth;
  DeclareAttribute(AttrFlag::WRITE_DEPTH);
  SetImportant(AttrFlag::WRITE_DEPTH, important);
}

void dg::RasterizerState::ClearWriteDepth() {
  UndeclareAttribute(AttrFlag::WRITE_DEPTH);
}

bool dg::RasterizerState::GetWriteDepth() const {
  return writeDepth;
}

void dg::RasterizerState::SetDepthFunc(DepthFunc func, bool important) {
  depthFunc = func;
  DeclareAttribute(AttrFlag::DEPTH_FUNC);
  SetImportant(AttrFlag::DEPTH_FUNC, important);
}

void dg::RasterizerState::ClearDepthFunc() {
  UndeclareAttribute(AttrFlag::DEPTH_FUNC);
}

dg::RasterizerState::DepthFunc dg::RasterizerState::GetDepthFunc() const {
  return depthFunc;
}

dg::RasterizerState dg::RasterizerState::Flatten(const RasterizerState &parent,
                                                 const RasterizerState &child) {
  // Early out if child doesn't declare anything.
  if (!child.HasDeclaredAttributes()) {
    return RasterizerState(parent);
  }

  RasterizerState merged;

  merged.declaredAttributes =
      parent.declaredAttributes | child.declaredAttributes;
  merged.importantAttributes =
      parent.importantAttributes | child.importantAttributes;

  AttrFlag attrsFromParent =
      parent.importantAttributes |
      (parent.declaredAttributes & !child.declaredAttributes);
  AttrFlag attrsFromChild = merged.declaredAttributes - attrsFromParent;

  if (static_cast<bool>(attrsFromChild & AttrFlag::CULL)) {
    merged.cullMode = child.cullMode;
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::CULL)) {
    merged.cullMode = parent.cullMode;
  }

  if (static_cast<bool>(attrsFromChild & AttrFlag::WRITE_DEPTH)) {
    merged.writeDepth = child.writeDepth;
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::WRITE_DEPTH)) {
    merged.writeDepth = parent.writeDepth;
  }

  if (static_cast<bool>(attrsFromChild & AttrFlag::DEPTH_FUNC)) {
    merged.depthFunc = child.depthFunc;
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::DEPTH_FUNC)) {
    merged.depthFunc = parent.depthFunc;
  }

  return merged;
}
