#!/usr/bin/env python3
"""Emit LVR ``srs6_rpm`` / ``srs7_rpm`` lock entries from build-info.json.

The LVR build consumes the two binary RPMs through the same input contract as
``ffmpeg_rpm``.  This helper reads the provider build metadata and prints the
exact entries, so the consumer does not have to retype hashes or NEVRAs.

Without ``--object-prefix`` the ``url`` field is omitted: no remote artifact is
assumed to exist.  Once the signed-off RPMs are uploaded to an immutable,
content-addressed location, pass the prefix and the emitted entries gain the
``url`` required by LVR's download path.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys

PACKAGES = {"opensight-srs6": "srs6_rpm", "opensight-srs7": "srs7_rpm"}


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-info", required=True, type=Path)
    parser.add_argument(
        "--object-prefix",
        help="immutable object prefix, e.g. https://host/software/srs-rpm",
    )
    parser.add_argument("--out", type=Path, help="write here instead of stdout")
    args = parser.parse_args(argv)

    info = json.loads(args.build_info.read_text(encoding="utf-8"))
    entries: dict[str, dict] = {}
    for artifact in info.get("artifacts", []):
        name = artifact.get("name")
        if name not in PACKAGES:
            continue
        if artifact.get("arch") == "src":
            continue
        entry = {
            "file": artifact["file"],
            "sha256": artifact["sha256"],
            "size_bytes": artifact["size_bytes"],
            "rpm": {
                "name": name,
                "epoch": 0,
                "version": artifact["version"],
                "release": artifact["release"],
                "arch": artifact["arch"],
                "nevra": artifact["nevra"],
            },
        }
        if args.object_prefix:
            entry["url"] = (
                args.object_prefix.rstrip("/")
                + "/"
                + artifact["sha256"]
                + "/"
                + artifact["file"]
            )
        entries[PACKAGES[name]] = entry

    missing = sorted(set(PACKAGES.values()) - set(entries))
    if missing:
        raise SystemExit(f"build-info is missing {', '.join(missing)}")

    text = json.dumps(entries, indent=2, sort_keys=True) + "\n"
    if args.out:
        args.out.write_text(text, encoding="utf-8")
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
