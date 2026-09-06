// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-3.0-only

#include <linux/input.h>

#include <scuf/envision.h>
#include <scuf/hid.h>

static int scufEnvisionProV2AxisMapping(struct hid_input *hidinput,
                                        struct hid_usage *usage,
                                        unsigned long **bit,
                                        int *max) {
  switch (usage->hid & HID_USAGE)
  {
    case SCUF_ENVISION_PRO_V2_RIGHT_STICK_X:
      hid_map_usage_clear(hidinput, usage, bit, max, EV_ABS, ABS_RX);
      return 1;

    case SCUF_ENVISION_PRO_V2_LEFT_TRIGGER:
      hid_map_usage_clear(hidinput, usage, bit, max, EV_ABS, ABS_Z);
      return 1;

    case SCUF_ENVISION_PRO_V2_RIGHT_TRIGGER:
      hid_map_usage_clear(hidinput, usage, bit, max, EV_ABS, ABS_RZ);
      return 1;

    case SCUF_ENVISION_PRO_V2_RIGHT_STICK_Y:
      hid_map_usage_clear(hidinput, usage, bit, max, EV_ABS, ABS_RY);
      return 1;

    default:
      return 0;
  }
}

static int scufEnvisionProV2ButtonMapping(struct hid_input *hidinput,
                                          struct hid_usage *usage,
                                          unsigned long **bit,
                                          int *max) {
  int code;

  switch (usage->hid & HID_USAGE)
  {
    case SCUF_ENVISION_PRO_V2_BUTTON_A:
      code = BTN_SOUTH;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_B:
      code = BTN_EAST;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_X:
      code = BTN_NORTH;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_Y:
      code = BTN_WEST;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_LB:
      code = BTN_TL;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_RB:
      code = BTN_TR;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_SELECT:
      code = BTN_SELECT;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_START:
      code = BTN_START;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_L3:
      code = BTN_THUMBL;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_R3:
      code = BTN_THUMBR;
      break;

    case SCUF_ENVISION_PRO_V2_BUTTON_GUIDE:
      code = BTN_MODE;
      break;

    default:
      return 0;
  }

  hid_map_usage_clear(hidinput, usage, bit, max, EV_KEY, code);
  return 1;
}

static int scufEnvisionProV2InputMapping(struct hid_device *hdev,
                                         struct hid_input *hidinput,
                                         struct hid_field *field,
                                         struct hid_usage *usage,
                                         unsigned long **bit,
                                         int *max) {
  (void)hdev;

  if (field->report->id != SCUF_ENVISION_PRO_V2_GAMEPAD_REPORT_ID)
    return 0;

  switch (usage->hid & HID_USAGE_PAGE)
  {
    case HID_UP_GENDESK:
      return scufEnvisionProV2AxisMapping(hidinput, usage, bit, max);

    case HID_UP_BUTTON:
      return scufEnvisionProV2ButtonMapping(hidinput, usage, bit, max);

    default:
      return 0;
  }
}

const struct scufHidModel scufEnvisionProV2HidModel = {
  .inputMapping = scufEnvisionProV2InputMapping,
};
