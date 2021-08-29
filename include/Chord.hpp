#pragma once

#include <vector>

#include "Note.hpp"
#include "Playable.hpp"

class Chord : public Playable {
protected:
	std::vector<Note> notes;
public:
	Chord(std::initializer_list<Note> notes) : notes(notes) {}
	Chord(std::vector<note_note_t> const& pitches, note_duration_t const duration, Articulation const articulation, velocity_t const velocity) {
		for (auto const& pitch : pitches) {
			notes.emplace_back(pitch, duration, articulation, velocity);
		}
	}
	void reset() override {
		for (Note& note : notes) {
			note.reset();
		}
	}
	wave_t step() override;
	bool done() const override;
};
