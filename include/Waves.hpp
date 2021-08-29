#pragma once

#include "common.hpp"

// with range -1.0 to 1.0
using wave_t = double;

inline out_t out_wave(wave_t const wave) {
	if constexpr (std::numeric_limits<out_t>::is_signed) {
		return (out_t)(wave * std::numeric_limits<out_t>::max());
	} else {
		return (out_t)((wave + 1.0) * (wave_t)(std::numeric_limits<out_t>::max() / 2));
	}
}

namespace Waves {
using Wave = wave_t(wavetime_t, freq_t);

wave_t sine(wavetime_t, freq_t, unsigned int const nth_harmonic = 1);
extern Wave square;
extern Wave sawtooth;
}  // namespace Waves
