#!/usr/bin/env bash
# Build the OpenSight SRS v6/v7 runtime RPMs from the pinned sources.
#
# Usage:
#   ./build-rpms.sh --source-dir DIR [--out-dir DIR] [--release N] [--jobs N]
#
# The source directory must contain the two archives pinned in
# sources.lock.json (for example the LVR bootc input cache).  The build runs
# offline in a pinned AlmaLinux 10.2 x86_64_v2 container and writes the binary
# RPMs, source RPMs, SHA256SUMS and build-info.json to the output directory.
set -euo pipefail

HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
PODMAN=${PODMAN:-podman}
BUILDER_IMAGE=${BUILDER_IMAGE:-opensight-srs-rpm-builder:el10.2}
BASE_IMAGE=${BASE_IMAGE:-}
SOURCE_DIR=${SRS_RPM_SOURCE_DIR:-}
OUT_DIR="$HERE/dist"
RELEASE=1
JOBS=${SRS_RPM_JOBS:-$(nproc 2>/dev/null || echo 2)}
BUILD_IMAGE=1

usage() {
    sed -n '2,12p' "$HERE/build-rpms.sh" | sed 's/^# \{0,1\}//'
    cat <<'EOF'

Options:
  --source-dir DIR    Directory with the pinned source archives (required)
  --out-dir DIR       Output directory (default: <this dir>/dist)
  --release N         RPM release number (default: 1)
  --jobs N            Parallel build jobs (default: host CPU count)
  --base-image IMAGE  Override the pinned AlmaLinux base image
  --no-build-image    Reuse an existing builder image tag
  --help              Show this help
EOF
}

die() {
    echo "error: $*" >&2
    exit 1
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --source-dir) SOURCE_DIR=$2; shift 2 ;;
        --out-dir) OUT_DIR=$2; shift 2 ;;
        --release) RELEASE=$2; shift 2 ;;
        --jobs) JOBS=$2; shift 2 ;;
        --base-image) BASE_IMAGE=$2; shift 2 ;;
        --no-build-image) BUILD_IMAGE=0; shift ;;
        --help|-h) usage; exit 0 ;;
        *) die "unknown option: $1 (see --help)" ;;
    esac
done

[[ $RELEASE =~ ^[1-9][0-9]*$ ]] || die "release must be a positive integer"
[[ $JOBS =~ ^[1-9][0-9]*$ ]] || die "jobs must be a positive integer"
[[ -n $SOURCE_DIR ]] || die "--source-dir is required"
[[ -d $SOURCE_DIR ]] || die "source directory not found: $SOURCE_DIR"
command -v "$PODMAN" >/dev/null || die "podman not found: $PODMAN"

lock_field() {
    python3 - "$HERE/sources.lock.json" "$1" <<'PY'
import json
import sys

lock = json.load(open(sys.argv[1], encoding="utf-8"))
print(lock["build"][sys.argv[2]])
PY
}

[[ -n $BASE_IMAGE ]] || BASE_IMAGE=$(lock_field image)
PLATFORM=$(lock_field platform)
ARCH=$(lock_field arch)

SOURCE_DIR=$(cd "$SOURCE_DIR" && pwd)
mkdir -p "$OUT_DIR"
OUT_DIR=$(cd "$OUT_DIR" && pwd)

echo "== verifying pinned source inputs on the host =="
python3 "$HERE/verify-sources.py" \
    --lock "$HERE/sources.lock.json" --source-dir "$SOURCE_DIR"

if [[ $BUILD_IMAGE == 1 ]]; then
    echo "== building builder image $BUILDER_IMAGE from $BASE_IMAGE =="
    "$PODMAN" build \
        --platform "$PLATFORM" \
        --build-arg "BASE_IMAGE=$BASE_IMAGE" \
        -t "$BUILDER_IMAGE" \
        -f "$HERE/Containerfile" \
        "$HERE"
fi

echo "== building RPMs (release $RELEASE, jobs $JOBS, arch $ARCH) =="
"$PODMAN" run --rm \
    --platform "$PLATFORM" \
    --network=none \
    --security-opt label=disable \
    --volume "$HERE:/packaging:ro" \
    --volume "$SOURCE_DIR:/inputs:ro" \
    --volume "$OUT_DIR:/out:rw" \
    "$BUILDER_IMAGE" \
    /bin/bash /packaging/build-in-container.sh "$RELEASE" "$JOBS"

# Emit the consumer-side lock entries without a URL: no remote artifact is
# assumed.  Re-run make-lvr-handoff.py with --object-prefix after publication.
python3 "$HERE/make-lvr-handoff.py" \
    --build-info "$OUT_DIR/build-info.json" --out "$OUT_DIR/lvr-handoff.json"

echo
echo "== artifacts =="
cat "$OUT_DIR/SHA256SUMS"
echo
echo "build-info:  $OUT_DIR/build-info.json"
echo "lvr-handoff: $OUT_DIR/lvr-handoff.json"
