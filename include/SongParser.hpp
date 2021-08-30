#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Articulation.hpp"
#include "Playable.hpp"

class SongParser {
public:
	class RememberingSubstring {
	public:
		std::string_view const data;
		size_t const line_offset;
		static constexpr auto npos = std::string_view::npos;
		static RememberingSubstring whole_string(std::string const& superstring) {
			return RememberingSubstring{ superstring, 0, superstring.size() };
		}
		// NB this does not have the same signature as the homonymous string_view method
		RememberingSubstring substr(size_t start, size_t end = npos) {
			return RememberingSubstring{ *this, start, end };
		}
		RememberingSubstring(std::string const& superstring, size_t start, size_t end = npos);
		RememberingSubstring(RememberingSubstring const& superstring, size_t start, size_t end = npos);
	};
	struct ParseException : public std::exception {
		ParseException(std::string message, RememberingSubstring string_in_question)
			: message{ std::move(message) }, string_in_question{ string_in_question.data }, line_offset{ string_in_question.line_offset } {
			//
		}
		std::string const message;
		std::string const string_in_question;
		size_t line_offset;
		size_t line = 0;
	};
public:
	SongParser(std::string const& filename);
	std::unique_ptr<Playable> next() noexcept(false);
protected:
	struct {
		size_t line = 0;
	} file_position;
	std::ifstream input_file;
	double tempo = 60.0;
protected:
	double parse_double(RememberingSubstring str) noexcept(false);
	signed char parse_signed_char(RememberingSubstring str) noexcept(false);
	note_duration_t parse_duration_string(RememberingSubstring str) noexcept(false);
	note_duration_t parse_duration(RememberingSubstring str) noexcept(false);
	void parse_pitches(RememberingSubstring str, std::vector<note_note_t>& pitches) noexcept(false);
	note_note_t parse_pitch(RememberingSubstring str) noexcept(false);
	Articulation parse_articulation(RememberingSubstring str) noexcept(false);
	void parse_directive(RememberingSubstring str) noexcept(false);
};
