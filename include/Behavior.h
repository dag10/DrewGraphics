//
//  Behavior.h
//

#pragma once

namespace dg {

  class Behavior {

    public:

      Behavior() = default;

      // Called when the behavior is first created or copied, even if disabled.
      virtual void Initialize() {};

      // Called before the very first Update(), only if/once it's enabled.
      virtual void Start() {};

      // Called every frame, only if it's enabled.
      virtual void Update();

      // Whether Start() or Update() will be called.
      bool enabled = true;

    private:

      bool started = false;

  }; // class Behavior

} // namespace dg
