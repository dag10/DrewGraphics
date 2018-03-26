//
//  RasterizerState.cpp
//

#include "dg/RasterizerState.h"
#include <sstream>
#include <string>

dg::RasterizerState dg::RasterizerState::AdditiveBlending() {
  RasterizerState state;
  state.SetWriteDepth(false);
  state.SetBlendEnabled(true);
  state.SetRGBBlendEquation(RasterizerState::BlendEquation::ADD);
  state.SetAlphaBlendEquation(RasterizerState::BlendEquation::ADD);
  state.SetSrcRGBBlendFunc(RasterizerState::BlendFunc::SRC_ALPHA);
  state.SetDstRGBBlendFunc(RasterizerState::BlendFunc::ONE);
  state.SetSrcAlphaBlendFunc(RasterizerState::BlendFunc::ZERO);
  state.SetDstAlphaBlendFunc(RasterizerState::BlendFunc::ONE);
  return state;
}

dg::RasterizerState dg::RasterizerState::AlphaBlending() {
  RasterizerState state;
  state.SetWriteDepth(false);
  state.SetBlendEnabled(true);
  state.SetRGBBlendEquation(RasterizerState::BlendEquation::ADD);
  state.SetAlphaBlendEquation(RasterizerState::BlendEquation::ADD);
  state.SetSrcRGBBlendFunc(RasterizerState::BlendFunc::SRC_ALPHA);
  state.SetDstRGBBlendFunc(RasterizerState::BlendFunc::ONE_MINUS_SRC_ALPHA);
  state.SetSrcAlphaBlendFunc(RasterizerState::BlendFunc::ONE);
  state.SetDstAlphaBlendFunc(RasterizerState::BlendFunc::ONE);
  return state;
}

dg::RasterizerState::RasterizerState(const RasterizerState &other) {
  declaredAttributes = other.declaredAttributes;
  importantAttributes = other.importantAttributes;

#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
  member_name = other.member_name;
#include "dg/RasterizerStateAttributes.def"
}

bool dg::RasterizerState::HasDeclaredAttributes() const {
  return static_cast<std::underlying_type_t<AttrFlag>>(declaredAttributes) > 0;
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

#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
  if (static_cast<bool>(attrsFromChild & AttrFlag::public_name)) { \
    merged.member_name = child.member_name; \
  } else if (static_cast<bool>(attrsFromParent & AttrFlag::public_name)) { \
    merged.member_name = parent.member_name; \
  }
#include "dg/RasterizerStateAttributes.def"

  return merged;
}

// Automatically create Setters, Getters, and Clearers for all possible
// RasterizerState attributes.
#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
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

std::ostream &dg::operator<<(std::ostream &os, const RasterizerState &state) {
#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
  os << #public_name << ": "; \
  if (state.Declares(dg::RasterizerState::AttrFlag::public_name)) { \
    if (std::string(#attr_type) == "bool") { \
      os << (static_cast<bool>(state.member_name) ? "TRUE" : "FALSE"); \
    } else { \
      os << std::to_string(state.member_name); \
    } \
    if (state.DeclaresImportant(dg::RasterizerState::AttrFlag::public_name)) { \
      os << " (important)"; \
    } \
  } else { \
    os << "(unset)"; \
  } \
  os << std::endl;
#include "dg/RasterizerStateAttributes.def"

  return os;
}

std::string std::to_string(const dg::RasterizerState &state) {
  std::stringstream ss;
  ss << state;
  return ss.str();
}

std::string std::to_string(dg::RasterizerState::CullMode cullMode) {
  switch (cullMode) {
    case dg::RasterizerState::CullMode::OFF:
      return "OFF";
    case dg::RasterizerState::CullMode::FRONT:
      return "FRONT";
    case dg::RasterizerState::CullMode::BACK:
      return "BACK";
    default:
      return "INVALID(" +
             std::to_string(
                 static_cast<
                     std::underlying_type_t<dg::RasterizerState::CullMode>>(
                     cullMode)) +
             ")";
  }
}

std::string std::to_string(dg::RasterizerState::DepthFunc depthFunc) {
  switch (depthFunc) {
    case dg::RasterizerState::DepthFunc::ALWAYS:
      return "ALWAYS";
    case dg::RasterizerState::DepthFunc::LESS:
      return "LESS";
    case dg::RasterizerState::DepthFunc::EQUAL:
      return "EQUAL";
    case dg::RasterizerState::DepthFunc::LEQUAL:
      return "LEQUAL";
    case dg::RasterizerState::DepthFunc::GREATER:
      return "GREATER";
    case dg::RasterizerState::DepthFunc::NOTEQUAL:
      return "NOTEQUAL";
    case dg::RasterizerState::DepthFunc::GEQUAL:
      return "GEQUAL";
    default:
      return "INVALID(" +
             std::to_string(
                 static_cast<
                     std::underlying_type_t<dg::RasterizerState::DepthFunc>>(
                     depthFunc)) +
             ")";
  }
}

std::string std::to_string(dg::RasterizerState::BlendEquation blendEquation) {
  switch (blendEquation) {
    case dg::RasterizerState::BlendEquation::ADD:
      return "ADD";
    case dg::RasterizerState::BlendEquation::SUBTRACT:
      return "SUBTRACT";
    case dg::RasterizerState::BlendEquation::REVERSE_SUBTRACT:
      return "REVERSE_SUBTRACT";
    case dg::RasterizerState::BlendEquation::MIN:
      return "MIN";
    case dg::RasterizerState::BlendEquation::MAX:
      return "MAX";
    default:
      return "INVALID(" +
             std::to_string(
                 static_cast<std::underlying_type_t<
                     dg::RasterizerState::BlendEquation>>(blendEquation)) +
             ")";
  }
}

std::string std::to_string(dg::RasterizerState::BlendFunc blendFunc) {
  switch (blendFunc) {
    case dg::RasterizerState::BlendFunc::ZERO:
      return "ZERO";
    case dg::RasterizerState::BlendFunc::ONE:
      return "ONE";
    case dg::RasterizerState::BlendFunc::SRC_COLOR:
      return "SRC_COLOR";
    case dg::RasterizerState::BlendFunc::ONE_MINUS_SRC_COLOR:
      return "ONE_MINUS_SRC_COLOR";
    case dg::RasterizerState::BlendFunc::DST_COLOR:
      return "DST_COLOR";
    case dg::RasterizerState::BlendFunc::ONE_MINUS_DST_COLOR:
      return "ONE_MINUS_DST_COLOR";
    case dg::RasterizerState::BlendFunc::SRC_ALPHA:
      return "SRC_ALPHA";
    case dg::RasterizerState::BlendFunc::ONE_MINUS_SRC_ALPHA:
      return "ONE_MINUS_SRC_ALPHA";
    case dg::RasterizerState::BlendFunc::DST_ALPHA:
      return "DST_ALPHA";
    case dg::RasterizerState::BlendFunc::ONE_MINUS_DST_ALPHA:
      return "ONE_MINUS_DST_ALPHA";
    default:
      return "INVALID(" +
             std::to_string(
                 static_cast<std::underlying_type_t<
                     dg::RasterizerState::BlendFunc>>(blendFunc)) +
             ")";
  }
}
