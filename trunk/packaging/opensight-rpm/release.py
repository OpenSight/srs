#!/usr/bin/env python3
"""Render and validate the OpenSight RPM release from sources.lock.json.

Each source entry records its own release inputs, so the two SRS majors can
carry independent source revisions, packaging revisions and dates:

    "release": {
      "source_revision": 1,
      "packaging_revision": 1,
      "date": "20261125",
      "dist": "el10"
    }

The rendered Release is ``<source>.<packaging>.<date>.<dist>``.  The date is a
controlled UTC input; rebuilding the same release reuses it.
"""

from __future__ import annotations

import argparse
from datetime import datetime
import json
from pathlib import Path
import re
import sys

RELEASE_RE = re.compile(
    r"^(?P<source_revision>[1-9][0-9]*)\."
    r"(?P<packaging_revision>[1-9][0-9]*)\."
    r"(?P<date>[0-9]{8})\."
    r"(?P<dist>[a-z0-9][a-z0-9._]*)$"
)
DIST_RE = re.compile(r"^[a-z0-9][a-z0-9._]*$")
FIELDS = ("source_revision", "packaging_revision", "date", "dist")


class ReleaseError(Exception):
    pass


def load_sources(path: Path) -> dict:
    try:
        lock = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ReleaseError(f"cannot read source lock {path}: {exc}") from exc
    sources = lock.get("sources")
    if not isinstance(sources, dict) or not sources:
        raise ReleaseError(f"source lock {path} has no sources")
    return sources


def render_release(name: str, entry: dict) -> str:
    inputs = entry.get("release")
    if not isinstance(inputs, dict):
        raise ReleaseError(f"source {name} has no release inputs")
    for field in FIELDS:
        if field not in inputs:
            raise ReleaseError(f"source {name} release is missing {field}")
    for field in ("source_revision", "packaging_revision"):
        value = inputs[field]
        if isinstance(value, bool) or not isinstance(value, int) or value < 1:
            raise ReleaseError(
                f"source {name} release {field} must be a positive integer"
            )
    date = inputs["date"]
    if not isinstance(date, str) or not re.fullmatch(r"[0-9]{8}", date):
        raise ReleaseError(f"source {name} release date must be YYYYMMDD")
    try:
        datetime.strptime(date, "%Y%m%d")
    except ValueError as exc:
        raise ReleaseError(f"source {name} release date is invalid: {date}") from exc
    dist = inputs["dist"]
    if not isinstance(dist, str) or not DIST_RE.fullmatch(dist):
        raise ReleaseError(
            f"source {name} release dist must match {DIST_RE.pattern}"
        )
    release = (
        f"{inputs['source_revision']}.{inputs['packaging_revision']}."
        f"{inputs['date']}.{inputs['dist']}"
    )
    if not RELEASE_RE.fullmatch(release):
        raise ReleaseError(f"source {name} release is invalid: {release}")
    return release


def validate_release(release: str) -> str:
    match = RELEASE_RE.fullmatch(release)
    if not match:
        raise ReleaseError(
            "release must be <source>.<packaging>.<YYYYMMDD>.<dist>, "
            "with positive revisions and a lowercase dist"
        )
    try:
        datetime.strptime(match.group("date"), "%Y%m%d")
    except ValueError as exc:
        raise ReleaseError(f"release date is invalid: {match.group('date')}") from exc
    return release


def describe(name: str, entry: dict) -> dict:
    release = render_release(name, entry)
    inputs = entry["release"]
    return {
        "package": entry.get("package"),
        "version": entry.get("version"),
        "source_revision": inputs["source_revision"],
        "packaging_revision": inputs["packaging_revision"],
        "date": inputs["date"],
        "dist": inputs["dist"],
        "release": release,
    }


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--lock", type=Path, help="sources.lock.json (required for rendering)"
    )
    parser.add_argument("--name", help="source key, for example srs6")
    parser.add_argument("--field", choices=FIELDS, help="print one release input")
    parser.add_argument(
        "--all", action="store_true", help="print every source's release as JSON"
    )
    parser.add_argument(
        "--validate", metavar="RELEASE", help="validate an explicit release string"
    )
    args = parser.parse_args(argv)

    try:
        if args.validate:
            print(validate_release(args.validate))
            return 0
        if args.lock is None:
            raise ReleaseError("--lock is required unless --validate is used")
        sources = load_sources(args.lock)
        if args.all:
            print(
                json.dumps(
                    {name: describe(name, entry) for name, entry in sources.items()},
                    indent=2,
                    sort_keys=True,
                )
            )
            return 0
        if not args.name:
            raise ReleaseError("--name or --all is required")
        if args.name not in sources:
            raise ReleaseError(f"source lock has no entry for {args.name}")
        entry = sources[args.name]
        render_release(args.name, entry)
        if args.field:
            print(entry["release"][args.field])
        else:
            print(render_release(args.name, entry))
    except ReleaseError as exc:
        print(f"release error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
