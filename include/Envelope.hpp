#pragma once

#include "Articulation.hpp"
#include "Playable.hpp"

#include "common.hpp"

class Envelope {
public:
	static Envelope from_articulation(Articulation const articulation);
	Envelope(wavetime_t attack_time, velocity_t attack_velocity, wavetime_t decay_time, velocity_t sustain_velocity, wavetime_t release_time)
		: attack_time(attack_time), attack_velocity(attack_velocity), decay_time(attack_time + decay_time), sustain_velocity(sustain_velocity), release_time(release_time) {}
	velocity_t velocity_for_time(wavetime_t time, note_duration_t const total_duration);
protected:
	// these are all proportional:
	// durations are absolute
	// velocities are relative to the velocity of the note
	wavetime_t attack_time;
	velocity_t attack_velocity;
	wavetime_t decay_time;
	velocity_t sustain_velocity;
	// release is relative to the end
	wavetime_t release_time;
};
