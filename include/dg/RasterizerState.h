//
//  RasterizerState.h
//

#pragma once

#include <memory>
#include <ostream>
#include <string>

namespace dg {

  class RasterizerState {

    public:

      enum class CullMode {
        OFF,
        FRONT,
        BACK,
      };

      enum class DepthFunc {
        OFF,
        LESS,
        EQUAL,
        LEQUAL,
        GREATER,
        NOTEQUAL,
        GEQUAL,
      };

      static RasterizerState Create();
      static RasterizerState CreateDefault();

      RasterizerState() = default;
      RasterizerState(const RasterizerState &);

      bool HasDeclaredAttributes() const;

      // Automatically create Setters, Getters, and Clearers for all possible
      // RasterizerState attributes.
#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
      void Set##public_name(attr_type member_name, bool important = false); \
      void Clear##public_name(); \
      attr_type Get##public_name() const; \
      inline bool Declares##public_name() { \
        return Declares(AttrFlag::public_name); \
      }
#include "dg/RasterizerStateAttributes.def"

      static RasterizerState Flatten(const RasterizerState &parent,
                                     const RasterizerState &child);

      friend std::ostream &operator<<(std::ostream &os,
                                      const RasterizerState &state);

      // Flags to keep trach of which attributes this state has declared or
      // might wish to override on child states.
      enum class AttrFlag : uint32_t {
        None = 0,
#define STATE_ATTRIBUTE(index, attr_type, public_name, member_name) \
        public_name = 1 << index,
#include "dg/RasterizerStateAttributes.def"
      };

    private:

      using T = std::underlying_type_t<AttrFlag>;
      friend inline AttrFlag operator - (AttrFlag lhs, AttrFlag rhs) {
        return lhs & !rhs;
      };
      friend inline AttrFlag operator | (AttrFlag lhs, AttrFlag rhs) {
        return (AttrFlag)(static_cast<T>(lhs) | static_cast<T>(rhs));
      };
      friend inline AttrFlag operator |= (AttrFlag& lhs, AttrFlag rhs) {
        return lhs = lhs | rhs;
      }
      friend inline AttrFlag operator &= (AttrFlag& lhs, AttrFlag rhs) {
        return lhs = lhs & rhs;
      }
      friend inline AttrFlag operator & (AttrFlag lhs, AttrFlag rhs) {
        return (AttrFlag)(static_cast<T>(lhs) & static_cast<T>(rhs));
      };
      friend inline bool operator == (AttrFlag lhs, AttrFlag rhs) {
        return static_cast<T>(lhs) == static_cast<T>(rhs);
      };
      friend inline AttrFlag operator ! (AttrFlag flag) {
        return (AttrFlag)(~static_cast<T>(flag));
      }

      // Which attributes this state has declared.
      AttrFlag declaredAttributes = AttrFlag::None;

      // Which attributes this state declares as important, and will therefore
      // override non-important child states' attributes.
      AttrFlag importantAttributes = AttrFlag::None;

      // This state's declared attribute values.
#define STATE_ATTRIBUTE(index, attr_type, name, member_name) \
      attr_type member_name;
#include "dg/RasterizerStateAttributes.def"

      inline void SetImportant(AttrFlag attr, bool important) {
        if (important) {
          importantAttributes |= attr;
        } else {
          importantAttributes &= !attr;
        }
      }

      inline void DeclareAttribute(AttrFlag attr) {
        declaredAttributes |= attr;
      }

      inline void UndeclareAttribute(AttrFlag attr) {
        declaredAttributes &= !attr;
        importantAttributes &= !attr;
      }

      inline bool Declares(AttrFlag attr) const {
        return static_cast<bool>(declaredAttributes & attr);
      }

      inline bool DeclaresImportant(AttrFlag attr) const {
        return static_cast<bool>(importantAttributes & attr);
      }

  }; // class RasterizerState

} // namespace dg

namespace std {
  std::string to_string(const dg::RasterizerState::CullMode &cullMode);
  std::string to_string(
      const dg::RasterizerState::DepthFunc &depthFunc);

  std::string to_string(const dg::RasterizerState &state);
} // namespace std
