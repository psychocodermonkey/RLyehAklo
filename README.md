# R'Lyeh Aklo

<p align="center">
  <img
    src="assets/img/vaulketh.png"
    alt="Vaul'keth"
    width="300"
  />
</p>

## Summary

R'Lyeh Aklo is a controller hardware support project focused on native
operating-system integration and reusable hardware definitions. It keeps
verified controller identities, HID usages, report layouts, and capabilities
in canonical headers that native drivers and other R'Lyeh projects can consume
without duplicating hardware knowledge.

The current build produces a native Linux HID kernel module while also exposing
the portable definitions as an installable CMake interface package. CMake and
Ninja provide the project-level workflow, with Linux kernel module compilation
delegated to Kbuild.

Platform implementations translate only behavior demonstrated to need
correction. Unknown or already-working device functions remain with the native
operating-system stack, preserving device identity and avoiding unnecessary
userspace services or device impersonation.

## Controller Documentation

### SCUF Envision Pro Controller V2

Aklo's first supported controller is the wired SCUF Envision Pro Controller V2:

```text
USB vendor ID:  1b1c
USB product ID: 3a05
```

The `rlyeh-scuf` HID driver corrects this controller's button and axis mappings
while preserving its native SCUF/Corsair identity and the standard Linux input
path:

```text
SCUF USB HID -> rlyeh-scuf.ko -> Linux input/evdev -> SDL, Steam, and games
```

The current implementation supports the primary gamepad controls, including
ABXY, D-pad, sticks, analog triggers, shoulders, system buttons, and stick
clicks. Firmware-mapped SAX buttons and rear paddles continue to emit their
configured base controls. Unrecognized usages are left to the normal HID stack
rather than being rewritten speculatively.

Current support is experimental and limited to the wired Envision Pro V2. The
wireless receiver and device configuration features are not yet supported.

The initial input milestone has been validated on two Fedora 44 systems. On a
separate machine, the module was built against the target kernel, loaded
successfully, and used to play a game through Steam with the basic button
mappings working correctly.

## Usage

Build and, when required, sign the module before loading it. Loading the module
requires administrator privileges:

```sh
sudo scripts/load-driver.py --load
```

Running the tool without an action prints its usage and the current module
state:

```sh
scripts/load-driver.py
```

Unload the development module with:

```sh
sudo scripts/load-driver.py --unload
```

The loading tool operates on
`build/debug-linux/bin/rlyeh-scuf.ko`. It does not build, sign, install, or
configure the module to load automatically. During development, keep the
unload path available and do not make the module persistent until it has been
validated on the target system.

## Build

The build uses CMake presets with Ninja as the developer-facing build system.
CMake delegates the kernel module compilation to the Linux kernel's Kbuild
system. Build on the target Linux system with the prepared headers for the
kernel against which the module will run.

For a module with debug information:

```sh
cmake --preset debug-linux
cmake --build --preset debug-linux
```

For a module with debug information stripped:

```sh
cmake --preset release-linux
cmake --build --preset release-linux
```

The finished module is written to:

```text
build/<preset>/bin/rlyeh-scuf.ko
```

To build against a prepared kernel tree other than the running kernel's tree,
set `RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR` during configuration.

The reusable headers and CMake package can be installed independently of
system-wide kernel-module installation. For example:

```sh
cmake -S . -B build/sdk -G Ninja
cmake --build build/sdk
cmake --install build/sdk --prefix /desired/prefix
```

A consuming CMake project can then use:

```cmake
find_package(RLyehAklo CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE RLyeh::Aklo)
```

The project currently does not install `rlyeh-scuf.ko` into the running
kernel's module tree. Distribution packaging, DKMS integration, and persistent
installation policy are intentionally separate from compilation.

## Signing

Secure Boot systems require the module to be signed by a certificate trusted
by the target kernel. After building, sign every kernel module in a preset's
`bin/` directory with:

```sh
sudo scripts/sign_kernel_modules.py debug-linux
```

The signing tool automatically checks the standard Fedora akmods and Debian
DKMS key locations. On Arch Linux or systems with custom key locations, provide
both paths explicitly:

```sh
sudo scripts/sign_kernel_modules.py debug-linux \
  --private-key /path/to/signing-key \
  --certificate /path/to/signing-certificate
```

Use `--kernel-release` when signing for an installed kernel other than the
currently running kernel. The certificate must already be enrolled and trusted;
the script does not create keys, enroll certificates, install modules, or load
them. Run it again after rebuilding because a new build replaces the signed
module.

See `scripts/sign_kernel_modules.py --help` and the script's documentation for
the Fedora, Debian, and Arch signing conventions.

## Future Milestones

- Investigate browser and other consumer-specific controller databases where
  correct raw inputs are exposed but the device lacks a standard presentation
  mapping.
- Add wireless Envision receiver support after its interfaces and reports have
  been independently captured and verified.
- Investigate force feedback and rumble without replacing working generic HID
  behavior or impersonating an Xbox controller.
- Research interface-4 features such as independent G-key/media handling, RGB,
  and onboard profile configuration without disrupting existing composite
  device functions.
- Add installation and distribution workflows such as DKMS and, later,
  distribution-specific packages once the driver behavior is stable.
- Extend the modular hardware-definition and driver structure to additional
  controllers and operating systems as verified hardware evidence becomes
  available.

## AI Usage

AI tools are used in this project as development support for understanding the codebase, tracking build nuances, reviewing changes, investigating issues, and improving documentation.

AI-generated output is not treated as authoritative. Contributions are expected to be reviewed, tested, and understood by the person submitting them. AI may be used by contributors, but it must be used as tooling, not as a substitute for engineering judgment.

This project allows AI-assisted work. It does not accept unchecked AI-generated code, broad automated rewrites, or changes that the contributor cannot explain and maintain.

## Legal Disclaimer

Trademarks: All product names, trademarks, and registered trademarks referenced by this project are the property of their respective owners. They are used solely for identification and compatibility purposes. RLyehAklo and R'Lyeh.Dev are independent projects and are not affiliated with, sponsored by, or endorsed by any referenced manufacturer or trademark owner.

See the repository license files and third-party notices for complete licensing information. Their contributor lists, licenses, copyright notices, and attribution files belong to those projects and should be preserved in place.
