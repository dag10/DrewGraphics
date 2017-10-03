//
//  Scene.h
//

#pragma once

#include <Window.h>

namespace dg {

  class Scene {

    public:

      virtual void Initialize() {};
      virtual void Update() {};
      virtual void Render() = 0;

      void SetWindow(std::shared_ptr<Window> window) {
        this->window = window;
      }

    protected:

      std::shared_ptr<Window> window = nullptr;

  }; // class Scene

} // namespace dg
