#pragma once

#include <limits>

using out_t = signed int;
using wavetime_t = double;
using freq_t = double;
using velocity_t = double;

#define NOTES_IN_OCTAVE 12

template <typename T>
T scalar_map(T const x, T const in_min, T const in_max, T const out_min, T const out_max) {
	return (x - in_min) * ((out_max - out_min) / (in_max - in_min)) + out_min;
}

using note_note_t = signed short;
using note_duration_t = double;
using note_octave_t = signed char;

constexpr velocity_t DEFAULT_VELOCITY = 0.5;

constexpr note_duration_t DURATION_FACTOR = 1.7 * ((note_duration_t)RATE / 8000);
