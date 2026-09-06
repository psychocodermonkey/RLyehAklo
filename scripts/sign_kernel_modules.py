#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 A.D. (PsychoCoderMonkey) <andrew.dixon@rlyeh.dev>
# SPDX-License-Identifier: GPL-3.0-only

"""Sign every kernel module produced by a CMake build preset.

The script signs every ``.ko`` file under ``build/<preset>/bin/`` using the
``sign-file`` utility supplied by the target kernel. The certificate must
already be trusted by that kernel. This tool does not create keys, enroll
certificates, install modules, or load modules.

Rebuilding a preset replaces its signed modules. Run this tool after the final
build whenever a new module is produced.

Fedora
------
Fedora systems using akmods normally keep their signing material at:

  /etc/pki/akmods/private/private_key.priv
  /etc/pki/akmods/certs/public_key.der

Those paths are detected automatically. After confirming the akmods
certificate is enrolled, build and sign with:

  cmake --build --preset debug-linux
  sudo scripts/sign_kernel_modules.py debug-linux

Debian
------
Current DKMS installations normally keep their Machine Owner Key at:

  /var/lib/dkms/mok.key
  /var/lib/dkms/mok.pub

Those paths are also detected automatically. DKMS 3.1 and newer can generate
the pair with ``sudo dkms generate_mok``. The public certificate must then be
enrolled according to the distribution's Secure Boot procedure. Once it is
enrolled, build and sign with:

  cmake --build --preset debug-linux
  sudo scripts/sign_kernel_modules.py debug-linux

If DKMS is configured with different ``mok_signing_key`` or ``mok_certificate``
paths, pass those paths explicitly:

  sudo scripts/sign_kernel_modules.py debug-linux \
    --private-key /path/to/mok.key \
    --certificate /path/to/mok.der

Arch Linux
----------
Arch does not define one universal out-of-tree module-signing key location.
The installed kernel headers must provide ``scripts/sign-file``, and the key's
certificate must be trusted by the running kernel. Pass the configured paths:

  cmake --build --preset debug-linux
  sudo scripts/sign_kernel_modules.py debug-linux \
    --private-key /path/to/signing_key.pem \
    --certificate /path/to/signing_key.x509

If Arch is configured to use DKMS's standard ``/var/lib/dkms/mok.*`` files,
the explicit path arguments may be omitted.

Use ``--kernel-release`` when signing a module for an installed kernel other
than the currently running kernel. Use ``--help`` to see all options.
"""

import argparse
import platform
import shutil
import subprocess
import sys
from pathlib import Path

SIGNING_KEY_CANDIDATES = (
  (
    "akmods",
    Path("/etc/pki/akmods/private/private_key.priv"),
    Path("/etc/pki/akmods/certs/public_key.der"),
  ),
  (
    "DKMS",
    Path("/var/lib/dkms/mok.key"),
    Path("/var/lib/dkms/mok.pub"),
  ),
)


def parse_arguments() -> argparse.Namespace:
  parser = argparse.ArgumentParser(
    description="Sign every .ko file under build/<preset>/bin/."
  )
  parser.add_argument("preset", help="CMake preset whose kernel modules should be signed")
  parser.add_argument(
    "--private-key",
    type=Path,
    help="module-signing private key (default: auto-detect akmods or DKMS)",
  )
  parser.add_argument(
    "--certificate",
    type=Path,
    help="module-signing certificate (default: auto-detect akmods or DKMS)",
  )
  parser.add_argument(
    "--kernel-release",
    default=platform.uname().release,
    help="kernel release containing scripts/sign-file (default: running kernel)",
  )
  parser.add_argument(
    "--hash-algorithm",
    default="sha256",
    help="hash algorithm passed to sign-file (default: sha256)",
  )
  return parser.parse_args()


def require_file(path: Path, description: str) -> None:
  if not path.is_file():
    raise RuntimeError(f"{description} not found: {path}")


def find_signing_material(arguments: argparse.Namespace) -> tuple[Path, Path]:
  if (arguments.private_key is None) != (arguments.certificate is None):
    raise RuntimeError("--private-key and --certificate must be provided together")

  if arguments.private_key is not None:
    return arguments.private_key, arguments.certificate

  for provider, private_key, certificate in SIGNING_KEY_CANDIDATES:
    if private_key.is_file() and certificate.is_file():
      print(f"Using {provider} signing material")
      return private_key, certificate

  locations = ", ".join(
    f"{private_key} and {certificate}"
    for _, private_key, certificate in SIGNING_KEY_CANDIDATES
  )
  raise RuntimeError(
    f"no signing material found; checked {locations}; "
    "use --private-key and --certificate to specify custom paths"
  )


def main() -> int:
  arguments = parse_arguments()

  if Path(arguments.preset).name != arguments.preset or arguments.preset in {".", ".."}:
    raise RuntimeError("preset must be a single directory name")

  project_root = Path(__file__).resolve().parent.parent
  module_directory = project_root / "build" / arguments.preset / "bin"
  sign_file = (
    Path("/lib/modules") / arguments.kernel_release / "build/scripts/sign-file"
  ).resolve()

  if not module_directory.is_dir():
    raise RuntimeError(f"preset output directory not found: {module_directory}")

  private_key, certificate = find_signing_material(arguments)

  require_file(sign_file, "kernel sign-file utility")
  require_file(private_key, "private key")
  require_file(certificate, "certificate")

  modules = sorted(module_directory.rglob("*.ko"))
  if not modules:
    raise RuntimeError(f"no kernel modules found under: {module_directory}")

  modinfo = shutil.which("modinfo")
  if modinfo is None:
    raise RuntimeError("modinfo was not found in PATH")

  for module in modules:
    print(f"Signing {module.relative_to(project_root)}")
    subprocess.run(
      [
        str(sign_file),
        arguments.hash_algorithm,
        str(private_key),
        str(certificate),
        str(module),
      ],
      check=True,
    )

    result = subprocess.run(
      [modinfo, "-F", "signer", str(module)],
      check=True,
      capture_output=True,
      text=True,
    )
    signer = result.stdout.strip()
    if not signer:
      raise RuntimeError(f"module has no signer after signing: {module}")
    print(f"Signed by {signer}")

  return 0


if __name__ == "__main__":
  try:
    sys.exit(main())
  except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
    print(f"error: {error}", file=sys.stderr)
    sys.exit(1)
