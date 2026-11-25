#!/usr/bin/env python3
"""Render a package's corresponding-source and relink notice.

The notice is generated from the same lock file the build verifies, so the
public source URL, SHA-256 and size in the installed notice cannot drift from
the bytes the package was built from.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lock", required=True, type=Path)
    parser.add_argument("--name", required=True, choices=("srs6", "srs7"))
    parser.add_argument("--out", type=Path, help="write here instead of stdout")
    args = parser.parse_args(argv)

    lock = json.loads(args.lock.read_text(encoding="utf-8"))
    source = lock["sources"][args.name]
    build = lock["build"]
    license_expression = lock["license_expression"]

    components = []
    for component, metadata in source["vendored"].items():
        version = metadata.get("version")
        components.append(
            f"{component} {version}" if version else f"{component} (embedded)"
        )

    text = f"""OpenSight SRS {source['version']} corresponding source and relink notice
=====================================================================

Package:  {source['package']}
Binary:   {source['binary']}
License:  {license_expression}
          The other LICENSE.* files in this directory contain the full texts.

Corresponding source
--------------------
This executable statically links: {', '.join(components)}.

The complete corresponding source is the exact public archive this binary
was built from:

  URL:     {source['url']}
  File:    {source['file']}
  SHA-256: {source['sha256']}
  Size:    {source['size_bytes']} bytes

The committed sources.lock.json pins these bytes. The build verifies the
SHA-256 and size before unpacking, so an archive with different bytes is
rejected by that pin. Do not expect the unchanged pin to accept modified
component sources.

Rebuild / relink
----------------
Prerequisites: an x86-64-v2 Linux userspace compatible with the appliance
({build['baseline']}), plus gcc, gcc-c++, make, cmake, patch, perl, tar,
unzip, diffutils, findutils, automake, pkgconf-pkg-config, which, python3,
tcl, openssl, openssl-devel, nasm (CRB), rpm-build and cpio.
Base image:
  {build['image']}

A. Unmodified relink with the reviewed recipe:
   1. Download the archive above and verify its SHA-256 and size.
   2. Run trunk/packaging/opensight-rpm/build-rpms.sh with
      --source-dir <directory containing the archive>.
   It unpacks the pinned archive and runs the original SRS configure flags:
      {build['configure']}
      --extra-flags='{build['extra_flags']}'
   SRS supplies -g -O0 itself; do not add -O2.

B. Modify a vendored component and relink:
   The checked-in pin cannot accept changed bytes. Use either:
   B1. Relink directly from an extracted tree:
         unzip {source['file']}
         cd {source['root']}/trunk
         # edit the component under 3rdparty/ ...
         ./configure {build['configure']} \\
             --extra-flags='{build['extra_flags']}'
         make -j"$(nproc)"
         ./objs/srs -v
   B2. Or copy this packaging directory to a local work area, create your own
       replacement archive, and update the COPY's sources.lock.json with the
       new file name, URL, SHA-256 and size. Run the COPY's build-rpms.sh.
       The committed lock and this notice remain pinned to the original
       bytes.

The matching source RPM contains the original archive, the spec, the lock,
the verifier and these build scripts. No registration or fee is required to
obtain the source.

Generated from sources.lock.json; do not edit by hand.
"""

    if args.out:
        args.out.write_text(text, encoding="utf-8")
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
