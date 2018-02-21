//
//  scenes/DeepCloningScene.h
//

#pragma once

#include <memory>
#include <Behavior.h>
#include <Scene.h>
#include <InputCodes.h>

namespace dg {

  class Light;
  class Model;
  class Window;

  class DeepCloningScene : public Scene {

    public:

      static std::unique_ptr<DeepCloningScene> Make();

      DeepCloningScene();

      virtual void Initialize();

    private:

      static std::shared_ptr<SceneObject> BuildWidget(
          std::shared_ptr<Window> window);

  }; // class DeepCloningScene

  namespace DeepCloning {

    class WidgetBehavior : public Behavior {

      public:

        WidgetBehavior(
            std::weak_ptr<Window> window,
            std::weak_ptr<Model> button, std::weak_ptr<Light> light);

        Key key;

        virtual void Start();
        virtual void Update();

      private:

        std::weak_ptr<Window> window;
        std::weak_ptr<Model> button;
        std::weak_ptr<Light> light;

    }; // class WidgetBehavior

  } // namespace DeepCloning

} // namespace dg
