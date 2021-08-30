# Test of Sound Synthesis

This is a demonstration/test/learning experiment relating to digital sound encoding and synthesis, as well as a bit of grammar parsing (though that code is spaghetti and I'm not proud of it).

## Building and Running

```sh
make release
./dist/Release/soundtest path/to/song.sng
# or
./run.sh path/to/song.sng
```

## Program Structure

The sound synthesis begins with the Waves. I have implemented sine, square, and (forward) sawtooth. They take a time step from 0.0 to 1.0 (though it can exceed 1.0 in which case it wraps around) as well as a multiplier that can also be thought of as the frequency, though it's relative and doesn't refer to actual frequencies.

More on that: since this is ad-hoc, the baseline is arbitrary, so I tuned it using a tuner, finding the baseline for this specific implementation. The baseline also depends on the sample rate, as can be seen in its definition (it is in terms of the sample rate).

One layer of abstraction above the waves is the Playables. There are a few Playables: Notes, Chords, and Rests, which are all pretty self-explanatory. The interface to a Playable is reset, step, and done. Since Playables maintain state (in order to generate waves over time), the reset method resets that state. The step method generates a wave at a point in time, and advances forward in time by one step. The done method checks if the Playable is done. Additionally, as a convenience, Playable implements a non-abstract play method, which effectively drains the Playable: it reads until it's done, writing the data to `std::cout`.

Since the Playables are stepped, the Chord is just a collection of Playables, where each Playable is stepped in parallel and mixed together. The Chord is done when all the sub-Playables are done.

Notes include envelopes that are chosen based on the articulation. They are typical ADSR envelopes, where the durations are absolute, so notes are sustained properly. Envelopes are stored with Notes. For very short notes (less than the attack plus the decay plus the release times), the note is just a spike up to the attack velocity, since the more complicated algorithm would not work properly.

The next layer on top of Playables is the SongParser. The parser is abe to read from a `.sng` file (an ad-hoc format that will be explained later, including a formal specification) and convert it to Playables, utilizing a generator-style interface with state and a next method.

The main entry-point gets a filename from the arguments, creates a SongParser for that file, and drains the Parser, playing each note until there are no more notes remaining.

Additionally, for the Parser, I implemented a slightly smarter wrapper around substrings of `std::string_view` called RememberingSubstring, that keeps track of its start's distance from the start of the original string. This allows error messages to give a column. Line numbers are added by catching, modifying, and rethrowing ParserErrors, which have a field for the line, that is initialized to zero (which is considered as "unknown line" if it's not filled in before it's printed).

## `.sng` Files

### Explanation

Each line consists of a duration, either "rest" or a series of notes, the latter optionally including an articulation and velocity. Sections are separated by commas. There are also directives and comments. Some examples:

```c
// first set the tempo with a tempo directive (bpm in 4/4)
%tempo 80
// whole note, c3
whole, c3
// whole note, c major chord
whole, c3 e3 g3
// articulation + velocity
whole, c3, tenuto
whole, g3, staccato, 1.0
// slurs
quarter, c3, slur
quarter, d3, slur
quarter, e3
// rests
quarter, rest
// abbreviations (can be as short as you want, and are checked for prefix)
qua, c3, stac
// numbers work for durations
// 1 = sixteenth
1.0, c3
// fractional notes (32nd in this case)
0.5, e3, tenuto
// dotted
dotted half, e3
dot qua, g3, portamento
```

Some notes:

- The baseline is sixteenths, so 1.0 is a sixteenth note. However, fractions and dotted notes are possible.
- Comments are useful for measure numbers, song names, key signature, time signature, etc., as none of these exist natively in the format.
- Staccato articulation plays for 80% of the duration, portamento (default) plays for 90%, tenuto plays for 95%, and slurred plays for 100%.
- In general, "invalid" values are permitted, but behavior is undefined.
- `%tempo` is currently the only directive.
- The `.sng` extension is an abbreviation for song.

For more examples, see the `music` directory.

### Formal Specification

Naming Scheme:
- `ALLCAPS`: lexical names for characters or strings
- `PascalCase`: entities or functions too commonly understood to necessitate being formally specified
- `snake_case`: generic entities

The entry point is `main`.

```
COMMA ::= ','
SPACE ::= ' '
REST ::= "rest"
NEWLINE ::= '\n'
COMMENT_START ::= "//"
DIRECTIVE_START ::= '%'

main ::= line NEWLINE | main line NEWLINE

line ::= Empty | comment | directive | duration COMMA SPACE REST | duration COMMA SPACE notespec

comment ::= COMMENT_START /[^\n]*/

directive ::= DIRECTIVE_START directive_content
directive_content ::= tempo_directive
tempo_directive ::= Prefix("tempo") SPACE RealNumber

duration ::= RealNumber | named_duration
named_duration ::= Prefix(named_duration_name) | Prefix("dotted") SPACE named_duration_name
named_duration_name ::= "sixteenth" | "eighth" | "quarter" | "half" | "whole"

notespec ::= notes | notes COMMA SPACE articulation | notes COMMA SPACE articulation COMMA SPACE velocity

notes ::= note | notes SPACE note
note ::= [a-gA-G][#bB]? WholeNumber

articulation ::= Prefix(named_articulation)
named_articulation ::= "staccato" | "portamento" | "tenuto" | "slurred"

velocity ::= RealNumber
```
