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

  if (static_cast<bool>(attrsFromChild & AttrFlag::CullMode)) {
    merged.cullMode = child.cullMode;
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::CullMode)) {
    merged.cullMode = parent.cullMode;
  }

  if (static_cast<bool>(attrsFromChild & AttrFlag::WriteDepth)) {
    merged.writeDepth = child.writeDepth;
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::WriteDepth)) {
    merged.writeDepth = parent.writeDepth;
  }

  if (static_cast<bool>(attrsFromChild & AttrFlag::DepthFunc)) {
    merged.depthFunc = child.depthFunc;
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::DepthFunc)) {
    merged.depthFunc = parent.depthFunc;
  }

  return merged;
}

// Automatically create Setters, Getters, and Clearers for all possible
// RasterizerState attributes.
#define STATE_ATTRIBUTE(bit_index, attr_type, public_name, member_name) \
void dg::RasterizerState::Set##public_name(attr_type member_name, bool important) { \
  this->member_name = member_name; \
  DeclareAttribute(AttrFlag::public_name); \
  SetImportant(AttrFlag::public_name, important); \
} \
void dg::RasterizerState::Clear##public_name() { \
  UndeclareAttribute(AttrFlag::public_name); \
} \
attr_type dg::RasterizerState::Get##public_name() const { \
  return member_name; \
}
#include "dg/RasterizerStateAttributes.def"
