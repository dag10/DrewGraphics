//
//  Behavior.h
//

#pragma once

namespace dg {

  class Behavior {

    public:

      virtual void Start() {};
      virtual void Update() = 0;

  }; // class Behavior

} // namespace dg
