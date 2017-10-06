//
//  Scene.h
//

#pragma once

#include <memory>
#include <Window.h>
#include <Behavior.h>
#include <SceneObject.h>

namespace dg {

  class Scene : public SceneObject {

    public:

      Scene();

      virtual void Initialize() {};
      virtual void Update();
      virtual void Render();

      void SetWindow(std::shared_ptr<Window> window) {
        this->window = window;
      }

    protected:

      std::vector<std::unique_ptr<Behavior>> behaviors;
      std::shared_ptr<Window> window = nullptr;

  }; // class Scene

} // namespace dg
