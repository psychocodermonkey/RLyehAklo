// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RLYEH_AKLO_SCUF_HID_H
#define RLYEH_AKLO_SCUF_HID_H

/* This remains safe for consumers that need controller identity but not HID. */
#include "scuf.h"

/*
 * Ordinary Linux userspace also defines __linux__. __KERNEL__ is the required
 * boundary here: it keeps Linux kernel types out of Aklo's portable installed
 * headers while allowing kernel implementations to share the same hardware
 * definitions.
 */
#ifdef __KERNEL__

#include <linux/hid.h>

struct scufHidModel
{
  /*
   * This follows the HID input_mapping contract. A model returns zero for a
   * usage that should retain HID core's default behavior and claims only the
   * usages whose replacement mapping has been verified.
   */
  int (*inputMapping)(struct hid_device *hdev,
                      struct hid_input *hidinput,
                      struct hid_field *field,
                      struct hid_usage *usage,
                      unsigned long **bit,
                      int *max);
};

extern const struct scufHidModel scufEnvisionProV2HidModel;

#endif /* __KERNEL__ */

#endif /* RLYEH_AKLO_SCUF_HID_H */
