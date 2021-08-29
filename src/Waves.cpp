#include <math.h>

#include "Waves.hpp"

#include "common.hpp"

namespace Waves {
// uses recursion to implement harmonics
wave_t sine(wavetime_t const time, freq_t const freq, unsigned int const nth_harmonic) {
	if (nth_harmonic > 13) {
		return 0.0;
	} else {
		return (sin(time * 2 * M_PI * freq) * 0.8) + (sine(time, freq * ((freq_t)(nth_harmonic + 1) / (freq_t)nth_harmonic), nth_harmonic + 1) * 0.2);
	}
}
wave_t square(wavetime_t const time, freq_t const multiplier) {
	return fmod(time * multiplier, 1.0) > 0.5 ? 1.0 : -1.0;
}
wave_t sawtooth(wavetime_t const time, freq_t const multiplier) {
	return scalar_map(fmod(time * multiplier, 1.0), 0.0, 1.0, -1.0, 1.0);
}
}  // namespace Waves
