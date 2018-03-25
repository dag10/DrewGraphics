//
//  RasterizerState.h
//

#pragma once

#include <memory>

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

      static RasterizerState CloneFlat(
          const RasterizerState &other);
      static RasterizerState CloneAsSibling(
          const RasterizerState other);

      void SetCullMode(CullMode mode, bool important = false);
      void ClearCullMode();
      CullMode GetCullMode() const;

      void SetWriteDepth(bool writeDepth, bool important = false);
      void ClearWriteDepth();
      bool GetWriteDepth() const;

      void SetDepthFunc(DepthFunc func, bool important = false);
      void ClearDepthFunc();
      DepthFunc GetDepthFunc() const;

      static RasterizerState Flatten(const RasterizerState &parent,
                                     const RasterizerState &child);

    private:

      // Flags to keep trach of which attributes this state has declared or
      // might wish to override on child states.
      enum class AttrFlag : uint32_t {
        NONE        = 0,
        CULL        = 1,
        WRITE_DEPTH = 2,
        DEPTH_FUNC  = 4,
      };

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
      AttrFlag declaredAttributes = AttrFlag::NONE;

      // Which attributes this state declares as important, and will therefore
      // override non-important child states' attributes.
      AttrFlag importantAttributes = AttrFlag::NONE;

      // This state's declared attribute values.
      CullMode cullMode;
      bool writeDepth;
      DepthFunc depthFunc;

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
