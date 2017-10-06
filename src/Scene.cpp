//
//  Scene.cpp
//

#include <Scene.h>

dg::Scene::Scene() : SceneObject() {}

void dg::Scene::Update() {
  // Execute Update() for all behaviors.
  for (
      auto behavior = behaviors.begin();
      behavior != behaviors.end();
      behavior++) {
    (*behavior)->Update();
  }
}

