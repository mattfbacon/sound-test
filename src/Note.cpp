#include <array>
#include <iostream>
#include <math.h>

#include "Note.hpp"
#include "Waves.hpp"

#include "common.hpp"

constexpr freq_t baseline = 1.718 * (8000 / (note_duration_t)RATE);
constexpr freq_t twelfth_root_of_two = 1.05946309436;

constexpr freq_t note_freq(note_note_t const note) {
	return baseline * pow(twelfth_root_of_two, (double)note);
}

std::array<char const*, 12> const note_names{
	"A", "A♯/B♭", "B", "C", "C♯/D♭", "D", "D♯/E♭", "E", "F", "F♯/G♭", "G", "G♯/A♭",
};
char const* note_name(note_note_t const note) {
	return note_names[note % NOTES_IN_OCTAVE];
}
note_octave_t note_octave(note_note_t const note) {
	return note / 12;
}

Note::Note(note_note_t const note, note_duration_t const duration, Articulation const articulation, velocity_t velocity)
	: freq(note_freq(note)), velocity(velocity), duration(duration * DURATION_FACTOR), envelope(Envelope::from_articulation(articulation)) {
	//
}

wave_t Note::step() {
	auto const ret = Waves::sine(step_state, freq) * velocity * envelope.velocity_for_time(step_state, duration);
	step_state += time_step;
	return ret;
}
