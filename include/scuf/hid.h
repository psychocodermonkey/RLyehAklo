// SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef RLYEH_AKLO_SCUF_HID_H
#define RLYEH_AKLO_SCUF_HID_H

#include "scuf.h"

#ifdef __KERNEL__

#include <linux/hid.h>

struct scufHidModel
{
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
