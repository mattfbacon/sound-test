#include <cstdlib>

#include "SongParser.hpp"

#include "tclap/CmdLine.h"
#include "tclap/UnlabeledMultiArg.h"

int main(int const argc, char const* const argv[]) {
	TCLAP::CmdLine cmdline{ "Sound Test", ' ', "0.1", true };
	TCLAP::UnlabeledMultiArg<std::string> songs{ "song", "Song to play", true, "path/to/song.sng", cmdline, false };
	cmdline.parse(argc, argv);

	if (isatty(STDOUT_FILENO)) {
		std::clog << "This program outputs binary data to stdout. Pipe it into `aplay -f S32_LE -r " << RATE << "` or equivalent, or use the bundled `run.sh`." << std::endl;
		return 1;
	}

	for (auto const& song_path : songs.getValue()) {
		SongParser parser{ song_path };
		try {
			for (std::unique_ptr<Playable> parser_note = parser.next(); parser_note; parser_note = parser.next()) {
				parser_note->play();
			}
		} catch (SongParser::ParseException const& e) {
			std::clog << "Parsing error at line " << (e.line == 0 ? "(unknown)" : std::to_string(e.line)) << ':' << e.line_offset << '\n';
			std::clog << e.message << ": " << e.string_in_question << std::endl;
			return EXIT_FAILURE;
		} catch (std::runtime_error const& e) {
			std::clog << "Error: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
