//
//  scenes/WidgetScene.h
//

#pragma once

#include <memory>
#include "dg/Behavior.h"
#include "dg/Scene.h"
#include "dg/InputCodes.h"

namespace dg {

  class Light;
  class Model;
  class Window;

  class WidgetScene : public Scene {

    public:

      static std::unique_ptr<WidgetScene> Make();
      static std::unique_ptr<WidgetScene> MakeVR();


      virtual void Initialize();

    private:

      WidgetScene(bool enableVR);

      static std::shared_ptr<SceneObject> BuildWidget(
          std::shared_ptr<Window> window, Key key, glm::vec3 color,
          bool momentary = false);

  }; // class WidgetScene

  namespace Widget {

    class WidgetBehavior : public Behavior {

      public:

        WidgetBehavior(
            std::weak_ptr<Window> window,
            std::weak_ptr<Model> button, std::weak_ptr<Light> light);

        Key key;
        glm::vec3 color;
        bool momentary = false;

        virtual void Start();
        virtual void Update();

        void SetLight(bool enabled);

      private:

        glm::vec3 originalButtonPos;

        std::weak_ptr<Window> window;
        std::weak_ptr<Model> button;
        std::weak_ptr<Light> light;

    }; // class WidgetBehavior

  } // namespace Widget

} // namespace dg
