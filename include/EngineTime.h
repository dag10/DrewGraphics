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

  private:

#if defined(_DIRECTX)
    static double perfCounterSeconds;
    static __int64 startTime;
    static __int64 currentTime;
    static __int64 previousTime;
#endif

  }; // class Time

} // namespace dg
