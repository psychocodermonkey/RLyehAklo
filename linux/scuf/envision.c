// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-3.0-only

#include <linux/input.h>

#include <scuf/envision.h>
#include <scuf/hid.h>

static int scufEnvisionProV2InputMapping(struct hid_device *hdev,
                                         struct hid_input *hidinput,
                                         struct hid_field *field,
                                         struct hid_usage *usage,
                                         unsigned long **bit,
                                         int *max) {
  (void)hdev;

  if (field->report->id != SCUF_ENVISION_PRO_V2_GAMEPAD_REPORT_ID)
    return 0;

  if ((usage->hid & HID_USAGE_PAGE) != HID_UP_GENDESK)
    return 0;

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

const struct scufHidModel scufEnvisionProV2HidModel = {
  .inputMapping = scufEnvisionProV2InputMapping,
};
