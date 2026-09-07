// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RLYEH_AKLO_SCUF_ENVISION_H
#define RLYEH_AKLO_SCUF_ENVISION_H

#include "scuf.h"

/* Identity and report selection observed on the wired Envision Pro V2 only. */
#define SCUF_ENVISION_PRO_V2_PRODUCT_ID 0x3a05
#define SCUF_ENVISION_PRO_V2_GAMEPAD_REPORT_ID 0x06

/*
 * These are HID Generic Desktop usage values, not Linux input codes. Keeping
 * the descriptor vocabulary portable lets each platform map the same verified
 * physical controls according to its own input API.
 */
enum scufEnvisionProV2GenericDesktopUsage
{
  SCUF_ENVISION_PRO_V2_LEFT_STICK_X = 0x30,
  SCUF_ENVISION_PRO_V2_LEFT_STICK_Y = 0x31,
  SCUF_ENVISION_PRO_V2_RIGHT_STICK_X = 0x32,
  SCUF_ENVISION_PRO_V2_LEFT_TRIGGER = 0x33,
  SCUF_ENVISION_PRO_V2_RIGHT_TRIGGER = 0x34,
  SCUF_ENVISION_PRO_V2_RIGHT_STICK_Y = 0x35,
  SCUF_ENVISION_PRO_V2_HAT_SWITCH = 0x39,
};

/* Descriptor logical ranges; revise only from captured hardware evidence. */
#define SCUF_ENVISION_PRO_V2_STICK_AXIS_MIN (-32768)
#define SCUF_ENVISION_PRO_V2_STICK_AXIS_MAX 32767

#define SCUF_ENVISION_PRO_V2_TRIGGER_MIN 0
#define SCUF_ENVISION_PRO_V2_TRIGGER_MAX 1023

#define SCUF_ENVISION_PRO_V2_HAT_SWITCH_MIN 0
#define SCUF_ENVISION_PRO_V2_HAT_SWITCH_MAX 7
#define SCUF_ENVISION_PRO_V2_HAT_SWITCH_HAS_NULL_STATE 1

/*
 * Only buttons 1 through 11 have verified, stable physical identities. The
 * descriptor exposes additional button usages, but they intentionally remain
 * unnamed until their independent behavior is established rather than being
 * inferred from configurable onboard mappings.
 */
enum scufEnvisionProV2ButtonUsage
{
  SCUF_ENVISION_PRO_V2_BUTTON_A = 1,
  SCUF_ENVISION_PRO_V2_BUTTON_B = 2,
  SCUF_ENVISION_PRO_V2_BUTTON_X = 3,
  SCUF_ENVISION_PRO_V2_BUTTON_Y = 4,
  SCUF_ENVISION_PRO_V2_BUTTON_LB = 5,
  SCUF_ENVISION_PRO_V2_BUTTON_RB = 6,
  SCUF_ENVISION_PRO_V2_BUTTON_SELECT = 7,
  SCUF_ENVISION_PRO_V2_BUTTON_START = 8,
  SCUF_ENVISION_PRO_V2_BUTTON_L3 = 9,
  SCUF_ENVISION_PRO_V2_BUTTON_R3 = 10,
  SCUF_ENVISION_PRO_V2_BUTTON_GUIDE = 11,
};

#endif /* RLYEH_AKLO_SCUF_ENVISION_H */
