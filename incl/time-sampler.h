#ifdef __linux
#include <time.h>
#include <sys/time.h>
#else

#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

class tsamplr_t {
public:
  typedef uint64_t storage_t;
  typedef int unit_t;

private:
  storage_t t0, t1;
  storage_t *_ext;

public:
  enum units {
    _s_ = 0,  // seconds
    _ms_ = 1, // milliseconds
    _us_ = 2, // microseconds
    _ns_ = 3, // nanoseconds
  };

  tsamplr_t(storage_t *lifetime) : t0(0U), t1(0U), _ext(lifetime) {
    if (_ext)
      sample();
  }

  ~tsamplr_t(void) {
    if (_ext) {
      sample();
      (*_ext) = t1 - t0;
    }
  }

  inline void sample(void) {
    t0 = t1;
#ifdef __linux__
    timespec time_spec;
    int err = clock_gettime(CLOCK_MONOTONIC, &time_spec);
    if (err) {
      perror("clock_gettime(...)");
      exit(1);
    }

    t1 = (time_spec.tv_sec * 1000000000) + time_spec.tv_nsec;
#else
#if _WIN32
    LARGE_INTEGER time_spec;
    BOOL rt = QueryPerformanceCounter(&time_spec);
    if (!rt) {
      perror("QueryPerformanceCounter(&time_spec)");
      exit(1);
    }
    t1 = time_spec.QuadPart;
#endif // _WIN32
#endif
  }

  static double convert(storage_t v, unit_t units) {
#ifdef _WIN32
    static LARGE_INTEGER frequency;
    if (!t0) {
      BOOL rt = QueryPerformanceFrequency(&frequency);
      if (!rt) {
        perror("QueryPerformanceFrequency(&frequency)");
        exit(1);
      }
    }

#endif
    switch (units) {

#ifdef __linux__
    case _s_:
      return static_cast<double>(v / 1000000000.0);
      break;
    case _ms_:
      return static_cast<double>(v / 1000000.0);
      break;
    case _us_:
      return static_cast<double>(v / 1000.0);
      break;
    case _ns_:
      return static_cast<double>(v);
      break;
#else
#ifdef _WIN32
    case _s_:
      return static_cast<double>(v / frequency.QuadPart);
      break;
    case _ms_:
      return static_cast<double>(v * 1000) / frequency.QuadPart;
      break;
    case _us_:
      return static_cast<double>(v * 1000000) / frequency.QuadPart;
      break;
    case _ns_:
      return static_cast<double>(v * 1000000000) / frequency.QuadPart;
      break;
#else
#error Unsupported Platform
#endif //_WIN32
#endif
    default:
      assert(0 && "Invalid time unit!");
    }
  }

  inline double get_dt(unit_t u) { return convert(t1 - t0, u); }
};

/*
int main() {
  tsamplr_t ts(NULL);

  ts.sample();
  int i = 0, n;
  for (; i < 100000000; ++i)
    n = i;
  ts.sample();

  double elapsed = ts.get_dt(tsamplr_t::_ms_);
  printf("elapsed: %f\n", elapsed);
  elapsed = ts.get_dt(tsamplr_t::_s_);
  printf("elapsed: %f\n", elapsed);

  tsamplr_t::storage_t lifetime;
  {
    tsamplr_t ts(&lifetime);
    int i = 0, n;
    for (; i < 200000000; ++i)
      n = i;
  }
  double lt = tsamplr_t::convert(lifetime, tsamplr_t::_ms_);
  printf("lifetime ms: %f\n", lt);

  return 0;
}*/
