#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
# SPDX-License-Identifier: GPL-3.0-only

"""Load, unload, or report the state of the development kernel module.

The script operates on ``build/debug-linux/bin/rlyeh-scuf.ko``. It does not
build or sign the module and does not invoke ``sudo`` internally.

Usage:

  scripts/load-driver.py
  sudo scripts/load-driver.py --load
  sudo scripts/load-driver.py --unload
"""

import argparse
import shutil
import subprocess
import sys
from pathlib import Path


MODULE_NAME = "rlyeh_scuf"
# This intentionally targets the debug preset's direct build artifact. Using
# insmod keeps bring-up isolated from the system module tree, depmod state, and
# persistent boot configuration; packaging/install workflows are separate.
MODULE_RELATIVE_PATH = Path("build/debug-linux/bin/rlyeh-scuf.ko")
MODULE_SYSFS_PATH = Path("/sys/module") / MODULE_NAME


def parse_arguments() -> tuple[argparse.ArgumentParser, argparse.Namespace]:
  parser = argparse.ArgumentParser(
    description="Load or unload the rlyeh-scuf development kernel module."
  )
  action = parser.add_mutually_exclusive_group()
  action.add_argument("--load", action="store_true", help="load the kernel module")
  action.add_argument("--unload", action="store_true", help="unload the kernel module")
  return parser, parser.parse_args()


def is_loaded() -> bool:
  # sysfs reflects the kernel's authoritative module state and avoids parsing
  # presentation-oriented command output such as lsmod.
  return MODULE_SYSFS_PATH.is_dir()


def print_state() -> None:
  state = "loaded" if is_loaded() else "unloaded"
  print(f"{MODULE_NAME}: {state}")


def require_command(command: str) -> str:
  executable = shutil.which(command)
  if executable is None:
    raise RuntimeError(f"{command} was not found in PATH")
  return executable


def load_module(project_root: Path) -> None:
  # Repeated development commands should preserve the currently tested module
  # rather than replacing it implicitly. Rebuild, sign, unload, then load to
  # make an intentional transition to a new artifact.
  if is_loaded():
    print(f"{MODULE_NAME} is already loaded")
    return

  module = project_root / MODULE_RELATIVE_PATH
  if not module.is_file():
    raise RuntimeError(f"kernel module not found: {module}")

  subprocess.run([require_command("insmod"), str(module)], check=True)
  if not is_loaded():
    raise RuntimeError(f"{MODULE_NAME} did not appear in sysfs after loading")

  print(f"Loaded {module.relative_to(project_root)}")


def unload_module() -> None:
  # An unloaded module restores the normal HID-driver binding path, which is
  # the recovery mechanism during hardware bring-up.
  if not is_loaded():
    print(f"{MODULE_NAME} is already unloaded")
    return

  subprocess.run([require_command("rmmod"), MODULE_NAME], check=True)
  if is_loaded():
    raise RuntimeError(f"{MODULE_NAME} remains present in sysfs after unloading")

  print(f"Unloaded {MODULE_NAME}")


def main() -> int:
  parser, arguments = parse_arguments()
  project_root = Path(__file__).resolve().parent.parent

  if not arguments.load and not arguments.unload:
    parser.print_usage()
    print_state()
    return 0

  if arguments.load:
    load_module(project_root)
  else:
    unload_module()

  print_state()
  return 0


if __name__ == "__main__":
  try:
    sys.exit(main())
  except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
    print(f"error: {error}", file=sys.stderr)
    sys.exit(1)
