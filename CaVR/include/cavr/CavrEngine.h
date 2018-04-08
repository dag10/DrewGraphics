//
//  CavrEngine.h
//

#pragma once

#include "dg/Engine.h"

namespace cavr {

  class CavrEngine : public dg::Engine {

    public:

      CavrEngine(std::shared_ptr<dg::Window> window);

    protected:

      virtual void UpdateWindowTitle();

  }; // class CavrEngine

} // namespace cavr
