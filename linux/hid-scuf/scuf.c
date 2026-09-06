// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-3.0-only

#include <linux/hid.h>
#include <linux/module.h>

#define USB_VENDOR_ID_CORSAIR            0x1b1c
#define USB_DEVICE_ID_SCUF_ENVISION_V2   0x3a05

static int scufProbe(struct hid_device *hdev,
                     const struct hid_device_id *id)
{
  int ret;

  ret = hid_parse(hdev);
  if (ret)
    return ret;

  ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
  if (ret)
    return ret;

  return 0;
}

static void scufRemove(struct hid_device *hdev)
{
  hid_hw_stop(hdev);
}

static const struct hid_device_id scufDevices[] = {
  { HID_USB_DEVICE(USB_VENDOR_ID_CORSAIR,
                   USB_DEVICE_ID_SCUF_ENVISION_V2) },
  { }
};
MODULE_DEVICE_TABLE(hid, scufDevices);

static struct hid_driver scufDriver = {
  .name = "rlyeh-scuf",
  .id_table = scufDevices,
  .probe = scufProbe,
  .remove = scufRemove,
};

module_hid_driver(scufDriver);

MODULE_AUTHOR("R'Lyeh.Dev");
MODULE_DESCRIPTION("SCUF controller HID driver");
MODULE_LICENSE("GPL");
