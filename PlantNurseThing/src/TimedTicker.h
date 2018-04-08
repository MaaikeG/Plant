#ifndef _TIMED_TICKER_h
#define _TIMED_TICKER_h

#include <Ticker.h>

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

class TimedTicker : public Ticker {
 private:
  unsigned long duration;
  static void detachCallback(TimedTicker* ticker) { ticker->detach(); };
  Ticker detachTicker;

 public:
  TimedTicker();
  template <typename TArg>
  void attach_ms(uint32_t millis, void (*callback)(TArg), TArg arg,
                 uint32_t duration) {
    Ticker::attach_ms(millis, callback, arg);
    detachTicker.once_ms(duration, detachCallback, this);
  }
};

#endif