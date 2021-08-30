#include <cassert>
#include <vector>

#include "Chord.hpp"
#include "Note.hpp"
#include "SongParser.hpp"

#include "common.hpp"
#include "util.hpp"

SongParser::RememberingSubstring::RememberingSubstring(std::string const& superstring, size_t start, size_t end)
	: data(std::string_view{ superstring.data() + start, superstring.data() + end }), line_offset(start) {
	//
}
SongParser::RememberingSubstring::RememberingSubstring(RememberingSubstring const& superstring, size_t start, size_t end)
	: data(superstring.data.substr(start, end == npos ? npos : end - start)), line_offset(superstring.line_offset + start) {
	//
}

SongParser::SongParser(std::string const& filename) : input_file{ filename } {
	if (!input_file.good()) {
		throw std::runtime_error{ "Invalid input file" };
	}
}

double SongParser::parse_double(RememberingSubstring str) {
	char* endptr = nullptr;
	auto const ret = std::strtod(str.data.data(), &endptr);
	if (ret == 0.0 || endptr != str.data.end()) {
		throw SongParser::ParseException{ "Failed to parse decimal number", str };
	}
	return ret;
}
signed char SongParser::parse_signed_char(RememberingSubstring str) {
	char* endptr = nullptr;
	auto const ret = std::strtol(str.data.data(), &endptr, 10);
	if (endptr != str.data.end()) {
		throw SongParser::ParseException{ "Failed to parse number", str };
	}
	if (ret == 0 && !(str.data == "0" || str.data == "-0" || str.data == "+0")) {
		throw SongParser::ParseException{ "Failed to parse number", str };
	}
	if (ret > std::numeric_limits<signed char>::max() || ret < std::numeric_limits<signed char>::min()) {
		throw SongParser::ParseException{ "Failed to parse number", str };
	}
	return ret;
}

note_duration_t SongParser::parse_duration_string(RememberingSubstring str) {
	if (std::string{ "sixteenth" }.starts_with(str.data)) {
		return Durations::sixteenth;
	} else if (std::string{ "eighth" }.starts_with(str.data)) {
		return Durations::eighth;
	} else if (std::string{ "quarter" }.starts_with(str.data)) {
		return Durations::quarter;
	} else if (std::string{ "half" }.starts_with(str.data)) {
		return Durations::half;
	} else if (std::string{ "whole" }.starts_with(str.data)) {
		return Durations::whole;
	} else {
		throw SongParser::ParseException{ "Could not parse named duration", str };
	}
}
note_duration_t SongParser::parse_duration(RememberingSubstring str) {
	try {
		return parse_double(str);
	} catch (SongParser::ParseException const&) {
		size_t const space_pos = str.data.find_first_of(' ');
		if (space_pos != std::string_view::npos && std::string{ "dotted" }.starts_with(std::string_view{ str.data.begin(), space_pos })) {
			return 1.5 * parse_duration_string(str.substr(space_pos + 1));
		} else {
			return parse_duration_string(str);
		}
	}
}

void SongParser::parse_pitches(RememberingSubstring str, std::vector<note_note_t>& pitches) {
	size_t tokenize_start = 0, tokenize_end;
	while (true) {
		tokenize_end = str.data.find_first_of(' ', tokenize_start);
		pitches.emplace_back(parse_pitch(str.substr(tokenize_start, tokenize_end)));
		if (tokenize_end == RememberingSubstring::npos) {
			break;
		}
		tokenize_start = tokenize_end + 1;  // skip space
	}
}

std::array<signed int, 'g' - 'a' + 1> const pitches_for_offsets{
	/* ['a'] = */ 0,
	/* ['b'] = */ 2,
	/* ['c'] = */ 3,
	/* ['d'] = */ 5,
	/* ['e'] = */ 7,
	/* ['f'] = */ 8,
	/* ['g'] = */ 10,
};

note_note_t SongParser::parse_pitch(RememberingSubstring str) {
	std::clog << str.data << std::endl;
	if (str.data.size() < 2) {
		throw SongParser::ParseException{ "Could not parse pitch", str };
	}
	if (str.data[0] < 'a' || str.data[0] > 'g') {
		throw SongParser::ParseException{ "Could not parse pitch", str };
	}

	size_t number_start = 2;
	signed short pitch = pitches_for_offsets[str.data[0] - 'a'];
	if (str.data[1] == 'b') {
		pitch--;
	} else if (str.data[1] == '#') {
		pitch++;
	} else {
		number_start = 1;
	}
	signed char const octave = parse_signed_char(str.substr(number_start));
	return pitch + (signed short)octave * NOTES_IN_OCTAVE;
}

Articulation SongParser::parse_articulation(RememberingSubstring str) {
	if (std::string{ "staccato" }.starts_with(str.data)) {
		return Articulation::staccato;
	} else if (std::string{ "portamento" }.starts_with(str.data)) {
		return Articulation::portamento;
	} else if (std::string{ "tenuto" }.starts_with(str.data)) {
		return Articulation::tenuto;
	} else if (std::string{ "slurred" }.starts_with(str.data)) {
		return Articulation::slurred;
	} else {
		throw SongParser::ParseException{ "Could not parse articulation", str };
	}
}

void SongParser::parse_directive(RememberingSubstring str) {
	size_t const space_pos = str.data.find_first_of(' ');
	RememberingSubstring directive_name = str.substr(0, space_pos);
	if (std::string{ "tempo" }.starts_with(directive_name.data)) {
		assert(str.data.size() >= space_pos + 1);
		tempo = parse_double(str.substr(space_pos + 1));
	} else {
		throw SongParser::ParseException{ "Unrecognized percent-directive", directive_name };
	}
}

std::unique_ptr<Playable> SongParser::next() {
	std::string line_;
	std::getline(input_file, line_);
	if (!input_file.good()) {
		return nullptr;
	}
	file_position.line++;
	std::transform(line_.begin(), line_.end(), line_.begin(), util::tolower);
	size_t tokenize_start = 0, tokenize_end;
	RememberingSubstring line = RememberingSubstring::whole_string(line_);

	try {
		struct {
			note_duration_t duration;
			std::vector<note_note_t> pitches;
			Articulation articulation = Articulation::portamento;
			velocity_t velocity = DEFAULT_VELOCITY;
		} note_data;

		// skip empty and comment lines
		if (line.data.size() == 0 || line.data.starts_with("//")) {
			return next();
		}

		// parse duration
		tokenize_end = line.data.find_first_of(',', tokenize_start);
		if (tokenize_end == std::string::npos || line.data.size() < tokenize_end + 2) {
			throw SongParser::ParseException{ "Garbage line: ended before duration", line };
		}
		note_data.duration = parse_duration(line.substr(tokenize_start, tokenize_end));

		// parse notes or rest
		tokenize_start = tokenize_end + 2;  // skip space
		tokenize_end = line.data.find_first_of(',', tokenize_start);
		if (tokenize_end == std::string::npos) {
			tokenize_end = line.data.size();
		}
		if (auto const view = line.substr(tokenize_start, tokenize_end); view.data == "rest") {
			return std::make_unique<Rest>(note_data.duration);
		} else {
			parse_pitches(view, note_data.pitches);
		}
		assert(note_data.pitches.size() >= 1);

		// articulation
		if (line.data.size() == tokenize_end) {
			goto return_value;
		}
		tokenize_start = tokenize_end + 2;
		tokenize_end = line.data.find_first_of(',', tokenize_start);
		if (tokenize_end == std::string::npos) {
			tokenize_end = line.data.size();
		}
		note_data.articulation = parse_articulation(line.substr(tokenize_start, tokenize_end));

		// velocity
		if (line.data.size() == tokenize_end) {
			goto return_value;
		}
		tokenize_start = tokenize_end + 2;
		tokenize_end = line.data.find_first_of(',', tokenize_start);
		if (tokenize_end != std::string::npos) {
			throw SongParser::ParseException{ "Line has too many elements", line };
		}
		note_data.velocity = parse_double(line.substr(tokenize_start));

return_value:;
		if (note_data.pitches.size() > 1) {
			return std::make_unique<Chord>(note_data.pitches, note_data.duration, note_data.articulation, note_data.velocity);
		} else {
			return std::make_unique<Note>(note_data.pitches.front(), note_data.duration, note_data.articulation, note_data.velocity);
		}
	} catch (ParseException& e) {
		e.line = file_position.line;
		throw;
	}
}
