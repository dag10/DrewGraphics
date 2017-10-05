//
//  Scene.h
//

#pragma once

#include <Window.h>
#include <SceneObject.h>

namespace dg {

  class Scene : public SceneObject {

    public:

      Scene();

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
