//
//  Scene.h
//

#pragma once

#include "Window.h"

namespace dg {

  class Scene {

    public:

      virtual void Initialize() {};
      virtual void Update() {};
      virtual void Render(dg::Window& window) = 0;

  }; // class Scene

} // namespace dg
