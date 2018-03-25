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


dg::RasterizerState dg::RasterizerState::Flatten(const RasterizerState &parent,
                                                 const RasterizerState &child) {
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

  return merged;
}
