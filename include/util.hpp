#pragma once

namespace util {
// https://git.musl-libc.org/cgit/musl/tree/src/ctype
inline bool isalpha(char const c) {
	return ((unsigned char)c | 32) - 'a' < 26;
}
inline bool isdigit(char const c) {
	return (unsigned char)c - '0' < 10;
}
inline bool isspace(char const c) {
	return c == ' ' || (unsigned char)c - '\t' < 5;
}
inline bool isalnum(char const c) {
	return ::util::isalpha(c) || ::util::isdigit(c);
}
inline bool isupper(char const c) {
	return (unsigned char)c - 'A' < 26;
}
inline char tolower(char const c) {
	if (::util::isupper(c)) return c | 32;
	return c;
}
}  // namespace util
