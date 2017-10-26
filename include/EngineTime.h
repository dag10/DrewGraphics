//
//  EngineTime.h
//
#pragma once

namespace dg {

  class Time {

    public:

      static double Elapsed;
      static double Delta;
      static double AverageFrameRate;
      static double FrameNumber;

      static void Reset();
      static void Update();

  }; // class Time

} // namespace dg
