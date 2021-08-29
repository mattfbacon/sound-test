#pragma once

#include <iostream>

#include "Waves.hpp"

#include "common.hpp"

struct Playable {
	virtual ~Playable() {}
	void play() {
		while (!done()) {
			out_t const storage = out_wave(step());
			std::cout.write((char const*)&storage, sizeof(out_t));
		}
	}
	virtual void reset() = 0;
	virtual bool done() const = 0;
	virtual wave_t step() = 0;
};
