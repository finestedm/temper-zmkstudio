/*
 * Frames adapted from SamIAm2000/zmk, branch bongo-cat-dedicated-work-queue.
 * Copyright (c) 2021 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#define BONGO_FRAME_WIDTH 64
#define BONGO_FRAME_HEIGHT 32
#define BONGO_FRAME_BYTES (BONGO_FRAME_WIDTH * BONGO_FRAME_HEIGHT / 8)
#define BONGO_IDLE_FRAME_COUNT 5

extern const uint8_t bongo_idle_frames[BONGO_IDLE_FRAME_COUNT][BONGO_FRAME_BYTES];
extern const uint8_t bongo_tap_frames[2][BONGO_FRAME_BYTES];
