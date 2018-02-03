//
//  Behavior.h
//

#pragma once

#include <memory>

namespace dg {

  class SceneObject;

  class Behavior {

    public:

      // Attach a behavior to a scene object. This initializes the behavior.
      static void Attach(
          std::shared_ptr<SceneObject> object,
          std::shared_ptr<Behavior> behavior);

      Behavior() = default;
      virtual ~Behavior() {};

      // Called when the behavior is first created or copied, even if disabled.
      // Private initial values should be set in this method, not in the
      // constructor. This way, if copied, internal state values can be
      // reset, but the public fields will retain their values.
      virtual void Initialize() {};

      // Called before the very first Update(), only if/once it's enabled.
      virtual void Start() {};

      // Called every frame, only if it's enabled.
      virtual void Update();

      // Gets the SceneObject the behavior is attached to.
      std::shared_ptr<SceneObject> SceneObject() const;

      // Whether Start() or Update() will be called.
      bool enabled = true;

    protected:

      // The SceneObject the behavior is attached to.
      std::weak_ptr<dg::SceneObject> sceneObject;

    private:

      // Whether Start() has been called on the behavior yet.
      bool started = false;

  }; // class Behavior

} // namespace dg
