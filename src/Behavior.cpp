//
//  Behavior.cpp
//

#include <Behavior.h>

void dg::Behavior::Update() {
  if (!started) {
    Start();
    started = true;
  }
}

