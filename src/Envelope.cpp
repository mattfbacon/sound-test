#include <cassert>
#include <cmath>

#include "Envelope.hpp"

Envelope Envelope::from_articulation(Articulation const articulation) {
	switch (articulation) {
		case Articulation::staccato:
			return Envelope{ 0.15, 1.0, 0.1, 0.9, 0.3 };
		case Articulation::portamento:
			return Envelope{ 0.15, 1.0, 0.1, 0.85, 0.15 };
		case Articulation::tenuto:
			return Envelope{ 0.18, 1.0, 0.13, 0.82, 0.07 };
		case Articulation::slurred:
			return Envelope{ 0.12, 0.9, 0.12, 0.85, 0.0 };
		default:
			__builtin_unreachable();
	}
}

velocity_t Envelope::velocity_for_time(wavetime_t const time, note_duration_t const total_duration) {
	assert(time >= 0.0 && time <= total_duration);
	// fallback for short notes that might mess up the more complicated algorithm
	if (__builtin_expect(total_duration < attack_time + decay_time + release_time, false)) {
		if (time > total_duration / 2) {
			return scalar_map(time, 0.0, total_duration / 2, 0.0, attack_velocity);
		} else {
			return scalar_map(time, total_duration / 2, total_duration, attack_velocity, 0.0);
		}
	} else {  // notes that would actually have sustain
		if (time < attack_time) {  // ramping up from zero to attack velocity
			return scalar_map(time, 0.0, attack_time, sustain_velocity * 0.1, attack_velocity);
		} else if (time < decay_time) {  // ramping down from attack velocity to sustain velocity
			return scalar_map(time, attack_time, attack_time + decay_time, attack_velocity, sustain_velocity);
		} else if (time < (double)total_duration - release_time) {  // sustaining
			return sustain_velocity;
		} else {  // ramping down from sustain velocity to zero
			// y=1-\cos\left(\arcsin\left(x\right)\right)
			auto const time_until_end = (double)total_duration - time;
			auto const normalized = time_until_end / release_time;  // goes from 1.0 to 0.0
			return scalar_map(1.0 - cos(asin(normalized)), 1.0, 0.0, sustain_velocity, sustain_velocity * 0.1);
		}
	}
}
