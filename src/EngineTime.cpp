//
//  EngineTime.cpp
//

#include "dg/EngineTime.h"

#include <Windows.h>

double dg::Time::Elapsed = 0;
double dg::Time::Delta = 0;
double dg::Time::FrameNumber = 1;
double dg::Time::AverageFrameRate = -1;
double dg::Time::perfCounterSeconds;
__int64 dg::Time::startTime;
__int64 dg::Time::currentTime;
__int64 dg::Time::previousTime;

void dg::Time::Reset() {
  __int64 perfFreq;
  QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
  perfCounterSeconds = 1.0 / (double)perfFreq;

  __int64 now;
  QueryPerformanceCounter((LARGE_INTEGER*)&now);
  startTime = now;
  currentTime = now;
  previousTime = now;
}

void dg::Time::Update() {

  QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
  Delta = max(((double)(currentTime - previousTime) * perfCounterSeconds), 0.0);
  Elapsed = ((double)(currentTime - startTime) * perfCounterSeconds);
  previousTime = currentTime;

  double fps = 1.0 / Delta;
  if (FrameNumber < 60) { // Don't start averaging framerate immediately.
    AverageFrameRate = fps;
  } else {
    AverageFrameRate = (AverageFrameRate * (FrameNumber - 1) / FrameNumber)
                     + (fps / FrameNumber);
  }
  FrameNumber++;
}

