#pragma once

#include "Articulation.hpp"
#include "Envelope.hpp"
#include "Playable.hpp"

#include "common.hpp"

constexpr wavetime_t time_step = 0.001;

enum NamedNote : unsigned char {
	NOTE_A,
	NOTE_A_S,
	NOTE_B_B = NOTE_A_S,
	NOTE_B,
	NOTE_C,
	NOTE_C_S,
	NOTE_D_B = NOTE_C_S,
	NOTE_D,
	NOTE_D_S,
	NOTE_E_B = NOTE_D_S,
	NOTE_E,
	NOTE_F,
	NOTE_F_S,
	NOTE_G_B = NOTE_F_S,
	NOTE_G,
	NOTE_G_S,
	NOTE_A_B = NOTE_G_S,
};

inline note_note_t make_note(note_octave_t const octave, NamedNote const note_name) {
	return octave * NOTES_IN_OCTAVE + note_name;
}

namespace Durations {
constexpr note_duration_t sixteenth = 1;
constexpr note_duration_t eighth = 2;
constexpr note_duration_t quarter = 4;
constexpr note_duration_t half = 8;
constexpr note_duration_t whole = 16;
}  // namespace Durations

class Note : public Playable {
public:
	Note(note_note_t const note, note_duration_t const duration, Articulation const articulation = Articulation::portamento, velocity_t const velocity = DEFAULT_VELOCITY);
	Note(unsigned char const octave, NamedNote const note_name, note_duration_t const duration, Articulation const articulation = Articulation::portamento, velocity_t const velocity = DEFAULT_VELOCITY)
		: Note(make_note(octave, note_name), duration, articulation, velocity) {}
	void reset() override {
		step_state = 0;
	}
	wave_t step() override;
	bool done() const override {
		return step_state >= duration;
	}
protected:
	wavetime_t step_state = 0;
protected:
	freq_t freq;
	velocity_t velocity;
	note_duration_t duration;
	Envelope envelope;
};

class Rest : public Playable {
public:
	Rest(note_duration_t const duration) : duration(duration * DURATION_FACTOR) {}
	void reset() override {
		step_state = 0;
	}
	wave_t step() override {
		step_state += time_step;
		return 0.0;
	}
	bool done() const override {
		return step_state >= duration;
	}
protected:
	wavetime_t step_state = 0;
	note_duration_t duration;
};
