//
//  EngineTime.cpp
//

#include "dg/EngineTime.h"

#if defined(_OPENGL)
#include "dg/opengl/glad/glad.h"

#include <GLFW/glfw3.h>
#elif defined(_DIRECTX)
#include <Windows.h>
#endif

double dg::Time::Elapsed = 0;
double dg::Time::Delta = 0;
double dg::Time::FrameNumber = 1;
double dg::Time::AverageFrameRate = -1;
#if defined(_DIRECTX)
double dg::Time::perfCounterSeconds;
__int64 dg::Time::startTime;
__int64 dg::Time::currentTime;
__int64 dg::Time::previousTime;
#endif

void dg::Time::Reset() {
#if defined(_OPENGL)
  glfwSetTime(0);
#elif defined(_DIRECTX)
  __int64 perfFreq;
  QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
  perfCounterSeconds = 1.0 / (double)perfFreq;

  __int64 now;
  QueryPerformanceCounter((LARGE_INTEGER*)&now);
  startTime = now;
  currentTime = now;
  previousTime = now;
#endif
}

void dg::Time::Update() {

#if defined(_OPENGL)
  double oldElapsed = Elapsed;
  Elapsed = glfwGetTime();
  Delta = Elapsed - oldElapsed;
#elif defined(_DIRECTX)
  QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
  Delta = max(((double)(currentTime - previousTime) * perfCounterSeconds), 0.0);
  Elapsed = ((double)(currentTime - startTime) * perfCounterSeconds);
  previousTime = currentTime;
#endif

  double fps = 1.0 / Delta;
  if (FrameNumber < 60) { // Don't start averaging framerate immediately.
    AverageFrameRate = fps;
  } else {
    AverageFrameRate = (AverageFrameRate * (FrameNumber - 1) / FrameNumber)
                     + (fps / FrameNumber);
  }
  FrameNumber++;
}

