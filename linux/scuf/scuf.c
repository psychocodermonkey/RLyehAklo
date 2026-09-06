// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-3.0-only

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
