//
//  CavrEngine.cpp
//

#include "cavr/CavrEngine.h"
#include "dg/Graphics.h"
#include "dg/Scene.h"
#include "dg/Window.h"

cavr::CavrEngine::CavrEngine(std::shared_ptr<dg::Window> window)
    : dg::Engine(window) {}

void cavr::CavrEngine::UpdateWindowTitle() {
  // Update window title every 0.1 seconds.
  const float titleUpdateFreq = 0.1f;
  if (dg::Time::Elapsed < lastWindowUpdateTime + titleUpdateFreq) {
    return;
  }

  if (scene->AutomaticWindowTitle()) {
    window->SetTitle(
        ((std::ostringstream &)(std::ostringstream()
                                << "CaVR | " << dg::Graphics::GetAPIName()
                                << " | " << (int)(1.0 / dg::Time::Delta)
                                << " FPS | " << dg::Time::AverageFrameRate
                                << " average FPS"))
            .str());
  }
  lastWindowUpdateTime = dg::Time::Elapsed;
}
