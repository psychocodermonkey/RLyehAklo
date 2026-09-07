// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/module.h>

#include <scuf/envision.h>
#include <scuf/hid.h>

static int scufInputMapping(struct hid_device *hdev,
                            struct hid_input *hidinput,
                            struct hid_field *field,
                            struct hid_usage *usage,
                            unsigned long **bit,
                            int *max) {
  const struct scufHidModel *model;

  /*
   * Device-specific behavior is selected from the matched ID entry. This
   * keeps the common driver responsible for HID lifecycle only; each model
   * decides which verified usages require intervention.
   */
  model = hid_get_drvdata(hdev);
  if (!model || !model->inputMapping)
    return 0;

  return model->inputMapping(hdev, hidinput, field, usage, bit, max);
}

static int scufProbe(struct hid_device *hdev, const struct hid_device_id *id) {
  const struct scufHidModel *model;
  int ret;

  model = (const struct scufHidModel *)id->driver_data;
  hid_set_drvdata(hdev, (void *)model);

  /*
   * The Envision descriptor contains separate application collections. Keep
   * their input devices separate so vendor usages cannot leak into the
   * gamepad evdev device. This must be set before parsing, when HID creates
   * those input devices.
   */
  hdev->quirks |= HID_QUIRK_INPUT_PER_APP;

  ret = hid_parse(hdev);
  if (ret)
    return ret;

  ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
  if (ret)
    return ret;

  return 0;
}

static void scufRemove(struct hid_device *hdev) {
  hid_hw_stop(hdev);
}

static const struct hid_device_id scufDevices[] = {
  /*
   * The model pointer is deliberately table data, not probe-time identity
   * logic. A future SCUF controller can share this driver's lifecycle while
   * supplying only its own verified input policy.
   */
  {HID_USB_DEVICE(SCUF_USB_VENDOR_ID, SCUF_ENVISION_PRO_V2_PRODUCT_ID),
   .driver_data = (kernel_ulong_t)&scufEnvisionProV2HidModel},
  {}
};

MODULE_DEVICE_TABLE(hid, scufDevices);

static struct hid_driver scufDriver = {
  .name = "rlyeh-scuf",
  .id_table = scufDevices,
  .probe = scufProbe,
  .remove = scufRemove,
  .input_mapping = scufInputMapping,
};

module_hid_driver(scufDriver);

MODULE_AUTHOR("R'Lyeh.Dev");
MODULE_DESCRIPTION("SCUF controller HID driver");
MODULE_LICENSE("GPL");
