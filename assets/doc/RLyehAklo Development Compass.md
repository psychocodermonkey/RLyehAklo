# RLyehAklo Development Compass

## Purpose

RLyehAklo provides native operating-system support for controllers that do not behave correctly through the platform's default input stack.

Its first supported device is the wired **SCUF Envision Pro Controller V2** on Linux.

Aklo should make supported hardware behave like a sane native controller without requiring applications to know Aklo exists. Where useful, it also exposes reusable hardware and protocol knowledge for other R'Lyeh projects.

The ownership model is:

- **Aklo owns knowledge about the hardware.**
- Native OS drivers consume that knowledge.
- Applications may also consume that knowledge directly when appropriate.
- Applications own application-specific semantics.
- Aklo remains useful as a standalone project independent of other R'Lyeh software.

The preferred path is always:

```text
controller
    ↓
native OS driver
    ↓
standard OS input stack
    ↓
applications
```

Avoid userspace translation layers, virtual-controller impersonation, or application-specific workarounds when the operating system can represent the hardware correctly itself.

---

## Development Principles

Prefer the smallest intervention that makes the hardware behave correctly.

Do not replace working platform behavior merely because Aklo could implement it itself. Preserve generic parsing, mappings, interfaces, and functionality whenever they are already correct.

In particular:

- correct quirks rather than replacing entire input paths;
- preserve hardware identity unless compatibility requires otherwise;
- avoid raw report parsing when the platform HID parser already understands the descriptor;
- do not claim semantics for controls whose behavior has not been independently verified;
- distinguish observed hardware behavior from assumptions or inferred behavior;
- keep experimental discoveries that constrain future implementation documented near the relevant hardware definitions or implementation.

Code that appears unusual may encode a verified hardware quirk. Preserve comments explaining behavior that could otherwise look unnecessary or safe to simplify.

When runtime or hardware verification is required, stop at that boundary and verify before building further assumptions on top of it.

---

## Architecture

Portable controller and protocol knowledge belongs in the canonical Aklo include hierarchy.

Platform implementations consume that knowledge rather than maintaining independent copies.

General rule:

- If something describes **the controller, hardware, or protocol**, it belongs in reusable Aklo definitions.
- If something describes **what an operating system does with that hardware**, it belongs in that platform implementation.
- Application-specific semantics belong to the application.
- Avoid duplicating portable definitions between Linux, macOS, R'LyehPSX, RLyehEcho, or future consumers.

Platform-dependent declarations may exist behind appropriate compile-time guards when that produces a cleaner shared interface.

On Linux, `__linux__` identifies Linux userspace as well as kernel builds. Kernel-only declarations therefore use `__KERNEL__` where appropriate.

### Linux driver organization

The Linux SCUF driver uses a common HID lifecycle and dispatch layer with model-specific behavior supplied separately.

The intended pattern is:

```text
common SCUF HID driver
        ↓
matched device/model
        ↓
model-specific verified input policy
```

Adding another controller should normally require adding its hardware definitions, model implementation, and device-table entry rather than duplicating the HID driver lifecycle or creating PID-based conditional chains.

The design should support multiple controller models and multiple simultaneously attached devices.

---

## Naming and Identity

The Linux kernel module and HID driver use the `rlyeh-scuf` name.

Do not occupy an obvious manufacturer or potential upstream namespace such as `hid-scuf`. An eventual official or upstream driver should be able to coexist conceptually and nominally with Aklo.

Manufacturer and product names are used only to identify compatible hardware. The project must not imply manufacturer affiliation, sponsorship, or endorsement.

The controller should retain its real USB identity. Do not advertise or emulate another controller family merely to satisfy applications unless compatibility testing eventually demonstrates that doing so is unavoidable.

---

## SCUF Envision Pro Controller V2

### USB Identity

Wired controller:

```text
VID: 0x1b1c
PID: 0x3a05
USB: Full Speed, USB 2.00
```

The wireless receiver is a distinct device and should not be assumed to behave identically to the wired controller.

```text
VID: 0x1b1c
PID: 0x3a09
```

Support for one does not imply support for the other.

---

### USB Interfaces

The wired controller exposes five USB interfaces:

```text
0  Audio
1  Audio
2  Audio
3  HID
4  HID
```

The audio interfaces are outside the controller-input driver's responsibility.

The HID interfaces have distinct roles:

```text
interface 3  primary gamepad + vendor channel
interface 4  miscellaneous/control/vendor functionality
```

A device match based solely on VID/PID may encounter both HID interfaces. Model behavior must therefore be narrow enough that unrelated reports and functionality continue through normal HID handling.

Do not assume every HID interface belonging to a supported USB device requires Aklo intervention.

---

### Interface 3

Interface 3 contains the standard gamepad report as well as vendor-defined communication.

#### Standard gamepad report

Report ID:

```text
0x06
```

The descriptor exposes a Generic Desktop / Game Pad collection.

Observed axis data:

```text
X   signed 16-bit  -32768..32767
Y   signed 16-bit  -32768..32767
Z   signed 16-bit  -32768..32767
Rz  signed 16-bit  -32768..32767

Rx  10-bit  0..1023
Ry  10-bit  0..1023

Hat switch  0..7 with null state
```

Generic HID parses these fields correctly. Aklo should remap only usages whose resulting Linux axis identities are wrong rather than assuming responsibility for the entire report.

The verified Linux interpretation is:

```text
Left stick X   → ABS_X
Left stick Y   → ABS_Y
Right stick X  → ABS_RX
Right stick Y  → ABS_RY
Left trigger   → ABS_Z
Right trigger  → ABS_RZ
D-pad          → ABS_HAT0X / ABS_HAT0Y
```

The descriptor also exposes HID Buttons 1 through 19. Only controls whose independent physical identity has been established should receive controller-specific names.

#### Vendor report 0x58

Interface 3 also contains vendor page `0xFF58` with 63-byte input and output reports.

Its purpose is not required for normal gamepad mapping and should remain under generic HID handling until its semantics are established.

#### Output report 0x09

The standard gamepad collection also exposes a 12-byte output report.

Its existence does not by itself establish rumble or other output semantics. Those should be implemented only after their behavior is verified.

---

### Interface 4

Interface 4 contains functionality separate from the standard gamepad path, including:

```text
Report 0x11  Consumer Control
             volume up/down/mute

Report 0x12  Telephony usages

Report 0x13  LED output

Vendor page 0xFF42:
  Report 0x01  63-byte input
  Report 0x02  63-byte output
  Report 0x0c  63-byte feature
  Report 0x03  63-byte input
```

These reports may eventually relate to media controls, G-keys, lighting, profiles, configuration, or other controller features.

Until their behavior is independently understood, preserve generic HID handling rather than assigning speculative semantics.

---

## Verified Input Behavior

### Buttons

Physical testing using HID scan usages established:

```text
HID Button 1   A
HID Button 2   B
HID Button 3   X
HID Button 4   Y
HID Button 5   LB
HID Button 6   RB
HID Button 7   Select/Back
HID Button 8   Start/Menu
HID Button 9   L3
HID Button 10  R3
HID Button 11  Guide/Home
```

The corresponding Linux identities are:

```text
1   → BTN_SOUTH
2   → BTN_EAST
3   → BTN_NORTH
4   → BTN_WEST
5   → BTN_TL
6   → BTN_TR
7   → BTN_SELECT
8   → BTN_START
9   → BTN_THUMBL
10  → BTN_THUMBR
11  → BTN_MODE
```

Linux's historical button aliases and the controller's observed layout produce an unusual X/Y relationship. Preserve the verified mappings rather than attempting to make the numerical HID usages appear sequential or aesthetically consistent.

Buttons 12 through 19 must remain unnamed until their independent behavior is established.

---

### Extra Controls and Onboard Profiles

The controller can store mappings for rear paddles and SAX controls in onboard firmware.

Testing established that firmware-mapped controls emit the same underlying HID usages as their configured target buttons.

For example:

```text
paddle mapped to X   → HID Button 3
paddle mapped to Y   → HID Button 4
paddle mapped to L3  → HID Button 9
paddle mapped to R3  → HID Button 10
SAX mapped to LB     → HID Button 5
SAX mapped to RB     → HID Button 6
```

Therefore a firmware-mapped extra control must not be mistaken for evidence that its HID usage represents that physical control independently.

Correcting the base button mappings naturally corrects controls mapped to those buttons in firmware.

Independent paddle or SAX semantics require separate evidence.

---

## Linux Implementation Strategy

Generic Linux HID already performs most of the necessary work.

Aklo should therefore use the narrowest appropriate HID hooks:

```text
input_mapping  preferred for correcting verified input identities
report_fixup   only when evidence demonstrates a descriptor correction is needed
raw_event      only when evidence demonstrates packet processing is needed
```

Do not introduce a raw parser merely because the controller has vendor-defined reports.

For reports and usages Aklo does not explicitly handle, return control to the HID core so normal generic behavior is preserved.

This is particularly important because the controller exposes functionality beyond the standard gamepad report.

---

## Reverse-Engineering Discipline

Hardware observations are authoritative for Aklo.

External implementations and documentation may provide useful leads or corroboration, but should not silently become the source of truth for controller behavior.

When investigating a control or protocol:

1. Record what the hardware actually exposes.
2. Separate descriptor facts from runtime observations.
3. Verify physical identities independently where possible.
4. Record uncertainty rather than assigning convenient semantics.
5. Implement only the behavior necessary to correct a demonstrated problem.
6. Preserve unexplained but reproducible behavior as such until evidence explains it.

A strange result is not automatically an error. If testing repeatedly produces an asymmetric or unexpected mapping, document the observation rather than “correcting” the evidence to match expectations.

---

## Scope Direction

The initial focus is a correctly functioning wired controller through the native Linux input stack.

Core controller functionality includes:

```text
face buttons
D-pad
both sticks
shoulder buttons
analog triggers
Select/Back
Start/Menu
L3/R3
Guide/Home
firmware-mapped extra controls where they naturally follow base mappings
```

Separate capabilities should be developed independently as their protocols and behavior become understood:

```text
wireless support
rumble / force feedback
lighting
configuration
profiles
firmware interaction
independent extra-button semantics
additional controller models
additional operating systems
```

Do not make unfinished optional capabilities prerequisites for correct basic controller operation.

---

## Development Compass

When choosing between implementations, prefer the one that:

1. preserves native hardware identity;
2. uses the normal operating-system input stack;
3. changes only behavior demonstrated to be wrong;
4. leaves already-correct generic behavior alone;
5. centralizes portable hardware knowledge;
6. keeps platform policy in the platform implementation;
7. records verified quirks without inventing explanations;
8. allows future controller models and platforms without duplicating knowledge;
9. remains independently useful outside the rest of the R'Lyeh ecosystem.

The desired result is deliberately boring from an application's perspective:

```text
plug controller in
      ↓
Aklo handles known platform quirks
      ↓
standard native input device
      ↓
applications simply work
```

That boring result is the point.