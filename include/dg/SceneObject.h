//
//  SceneObject.h
//
#pragma once

#include "dg/Transform.h"
#include "dg/Behavior.h"
#include <set>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dg {

  // Represents an object within the scene tree.
  //
  // NOTE: SceneObjects cannot be moved or swapped, because they are
  //       intended to always be owned by a std::shared_ptr.
  //
  //       They can be copied, but this does not preserve the parent
  //        or children.
  class SceneObject {

    public:
      Transform transform = Transform();
      bool enabled = true;

      SceneObject() = default;
      SceneObject(Transform transform);
      SceneObject(SceneObject& other);
      SceneObject(SceneObject&& other) = delete;
      SceneObject& operator=(SceneObject& other) = delete;
      SceneObject& operator=(SceneObject&& other) = delete;
      virtual ~SceneObject() = default;

      void AddBehavior(std::shared_ptr<Behavior> behavior);
      void UpdateBehaviors();

      template<class T>
      std::shared_ptr<T> GetBehavior() {
        for (auto iter = behaviors.begin(); iter != behaviors.end(); iter++) {
          auto cast = std::dynamic_pointer_cast<T>(*iter);
          if (cast != nullptr) {
            return cast;
          }
        }
        return nullptr;
      }

      Transform SceneSpace() const;
      void SetSceneSpace(Transform transform);

      void AddChild(std::shared_ptr<SceneObject> child);
      void AddChild(
          std::shared_ptr<SceneObject> child, bool preserveSceneSpace);
      void RemoveChild(std::shared_ptr<SceneObject> child);

      SceneObject *Parent() const;
      std::set<std::shared_ptr<SceneObject>> &Children();

      void LookAt(const SceneObject& object);
      void LookAtDirection(glm::vec3 direction);
      void LookAtPoint(glm::vec3 target);
      void OrientUpwards();

    private:
      std::vector<std::shared_ptr<Behavior>> behaviors;
      SceneObject *parent = nullptr;
      std::set<std::shared_ptr<SceneObject>> children;

      void SetParent(SceneObject *parent, bool preserveSceneSpace);

  }; // class SceneObject

} // namespace dg
