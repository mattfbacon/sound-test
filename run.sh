#!/bin/sh
make release && ./dist/Release/soundtest $1 | aplay -f S32_LE -r $(cat rate)
