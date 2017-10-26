//
//  EngineTime.cpp
//

#include <EngineTime.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

double dg::Time::Elapsed = 0;
double dg::Time::Delta = 0;
double dg::Time::FrameNumber = 1;
double dg::Time::AverageFrameRate = -1;

void dg::Time::Reset() {
  glfwSetTime(0);
}

void dg::Time::Update() {
  Delta = glfwGetTime();
  Elapsed += Delta;
  glfwSetTime(0);

  double fps = 1.0 / Delta;
  if (FrameNumber < 60) { // Don't start averaging framerate immediately.
    AverageFrameRate = fps;
  } else {
    AverageFrameRate = (AverageFrameRate * (FrameNumber - 1) / FrameNumber)
                     + (fps / FrameNumber);
  }
  FrameNumber++;
}

