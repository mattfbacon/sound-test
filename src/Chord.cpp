#include "Chord.hpp"

wave_t Chord::step() {
	wave_t result = 0;
	for (Note& note : notes) {
		if (!note.done()) {
			result += note.step() / notes.size();
		}
	}
	return result;
}

bool Chord::done() const {
	for (Note const& note : notes) {
		if (!note.done()) {
			return false;
		}
	}
	return true;
}
