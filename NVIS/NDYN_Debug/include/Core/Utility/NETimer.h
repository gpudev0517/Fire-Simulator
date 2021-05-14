#pragma once

// forward declaration
struct TimeMeasurement
{
  QElapsedTimer qeTimer;
};

class NECEXP_CORE NETimer
{
  enum {
    MAX_COUNT = 500
  };


public:
  static void start(const std::string& identifier);
  static double stop(const std::string& identifier);
  
  static bool s_bTiming;

private:
  //static long long m_frequency;
  static std::map<std::string,TimeMeasurement> m_counters;
};
