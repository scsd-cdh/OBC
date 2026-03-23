/*
Taken straight from https://github.com/Dirbaio/rzcobs/blob/main/src/lib.rs and rewritten in C

Copyright (c) 2020 Dario Nieuwenhuis

Permission is hereby granted, free of charge, to any
person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the
Software without restriction, including without
limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

 */
// ReSharper disable CppRedundantParentheses
#include "rzcobs.h"

#include <stdint.h>


void ulog_rzcobs_encoder_write(struct ulog_rzcobs_state * self, const uint8_t byte) {
    if (self->run < 7) {
        if (byte == 0) {
            self->zeros |= 1 << self->run;
        } else {
            self->write(byte);
        }

        self->run++;
        if (self->run == 7 && self->zeros != 0x00) {
            self->write(self->zeros);
            self->run = 0;
            self->zeros = 0;
        }
    } else if (byte == 0) {
        self->write((self->run - 7) | 0x80);
        self->run = 0;
        self->zeros = 0;
    } else {
        self->write(byte);
        self->run++;
        if (self->run == 134) {
            self->write(0xFF);
            self->run = 0;
            self->zeros = 0;
        }
    }
}

void ulog_rzcobs_encoder_end(struct ulog_rzcobs_state * self) {
    if (self->run == 0) {}
    else if (self->run >= 1 && self->run <= 6)
        self->write((self->zeros | (0xFF << self->run)) & 0x7F);
    else
        self->write((self->run - 7) | 0x80);

    self->run = 0;
    self->zeros = 0;
    self->write(0x00);
}
