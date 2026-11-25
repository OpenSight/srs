#!/usr/bin/env bash
# Build the OpenSight SRS v6/v7 runtime RPMs from the pinned sources.
#
# Usage:
#   ./build-rpms.sh --source-dir DIR [--out-dir DIR] [--release RELEASE] [--jobs N]
#
# The source directory must contain the two archives pinned in
# sources.lock.json (for example the LVR bootc input cache).  The build runs
# offline in a pinned AlmaLinux 10.2 x86_64_v2 container and writes the binary
# RPMs, source RPMs, SHA256SUMS and build-info.json to the output directory.
# Each package's Release comes from its own release inputs in sources.lock.json.
set -euo pipefail

HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
PODMAN=${PODMAN:-podman}
BUILDER_IMAGE=${BUILDER_IMAGE:-opensight-srs-rpm-builder:el10.2}
BASE_IMAGE=${BASE_IMAGE:-}
SOURCE_DIR=${SRS_RPM_SOURCE_DIR:-}
OUT_DIR="$HERE/dist"
RELEASE_OVERRIDE_VALUE=
JOBS=${SRS_RPM_JOBS:-$(nproc 2>/dev/null || echo 2)}
BUILD_IMAGE=1
ALLOW_UNLOCKED_BASE=0

usage() {
    sed -n '2,11p' "$HERE/build-rpms.sh" | sed 's/^# \{0,1\}//'
    cat <<'EOF'

Options:
  --source-dir DIR    Directory with the pinned source archives (required)
  --out-dir DIR       Output directory (default: <this dir>/dist)
  --release RELEASE   Override both packages with an exact Release string,
                      for example 1.2.20261125.el10 (default: sources.lock.json)
  --jobs N            Parallel build jobs (default: host CPU count)
  --base-image IMAGE  Override the pinned AlmaLinux base image
  --no-build-image    Reuse an existing builder image tag
  --allow-unlocked-base
                      Permit an experimental build whose recorded base does
                      not match the lock (never for a release)
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
        --release) RELEASE_OVERRIDE_VALUE=$2; shift 2 ;;
        --jobs) JOBS=$2; shift 2 ;;
        --base-image) BASE_IMAGE=$2; shift 2 ;;
        --no-build-image) BUILD_IMAGE=0; shift ;;
        --allow-unlocked-base) ALLOW_UNLOCKED_BASE=1; shift ;;
        --help|-h) usage; exit 0 ;;
        *) die "unknown option: $1 (see --help)" ;;
    esac
done

[[ $JOBS =~ ^[1-9][0-9]*$ ]] || die "jobs must be a positive integer"
[[ -n $SOURCE_DIR ]] || die "--source-dir is required"
[[ -d $SOURCE_DIR ]] || die "source directory not found: $SOURCE_DIR"
command -v "$PODMAN" >/dev/null || die "podman not found: $PODMAN"

lock_field() {
    python3 -I - "$HERE/sources.lock.json" "$1" <<'PY'
import json
import sys

lock = json.load(open(sys.argv[1], encoding="utf-8"))
print(lock["build"][sys.argv[2]])
PY
}

release_for() {
    python3 -I "$HERE/release.py" \
        --lock "$HERE/sources.lock.json" --name "$1"
}

RELEASE_SRS6=$(release_for srs6) \
    || die "cannot render the srs6 release from sources.lock.json"
RELEASE_SRS7=$(release_for srs7) \
    || die "cannot render the srs7 release from sources.lock.json"
RELEASE_OVERRIDE=0
if [[ -n $RELEASE_OVERRIDE_VALUE ]]; then
    python3 -I "$HERE/release.py" --validate "$RELEASE_OVERRIDE_VALUE" >/dev/null \
        || die "invalid --release value: $RELEASE_OVERRIDE_VALUE"
    RELEASE_SRS6=$RELEASE_OVERRIDE_VALUE
    RELEASE_SRS7=$RELEASE_OVERRIDE_VALUE
    RELEASE_OVERRIDE=1
    echo "warning: overriding the locked releases with $RELEASE_OVERRIDE_VALUE" >&2
fi

LOCKED_BASE_IMAGE=$(lock_field image)
[[ -n $BASE_IMAGE ]] || BASE_IMAGE=$LOCKED_BASE_IMAGE
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

# A tag is mutable: it can be retagged or rebuilt between two lookups.  Resolve
# it exactly once here, then record that immutable image ID as provenance and
# run that same ID below, so the container cannot differ from what build-info
# claims.  Never let build-info name the locked root without this proof.
image_info=$("$PODMAN" image inspect \
    --format '{{.Id}}|{{ index .Config.Labels "org.opensight.srs.base-image" }}|{{.Digest}}' \
    "$BUILDER_IMAGE" 2>/dev/null) \
    || die "cannot inspect builder image $BUILDER_IMAGE"
IFS='|' read -r BUILDER_IMAGE_ID ACTUAL_BASE_IMAGE BUILDER_IMAGE_DIGEST <<<"$image_info"
[[ -n $BUILDER_IMAGE_ID ]] \
    || die "cannot resolve builder image $BUILDER_IMAGE to an immutable ID"
[[ -n $ACTUAL_BASE_IMAGE && $ACTUAL_BASE_IMAGE != '<no value>' ]] \
    || die "builder image $BUILDER_IMAGE has no org.opensight.srs.base-image label; rebuild it"

BASE_MATCHES_LOCK=false
if [[ $ACTUAL_BASE_IMAGE == "$LOCKED_BASE_IMAGE" ]]; then
    BASE_MATCHES_LOCK=true
fi
if [[ $BASE_MATCHES_LOCK == false && $ALLOW_UNLOCKED_BASE == 0 ]]; then
    die "builder base '$ACTUAL_BASE_IMAGE' does not match the locked base '$LOCKED_BASE_IMAGE'; rebuild from the pinned base or pass --allow-unlocked-base for a non-release experiment"
fi
if [[ $BASE_MATCHES_LOCK == false ]]; then
    echo "warning: building from unlocked base '$ACTUAL_BASE_IMAGE'" >&2
fi
echo "== build root: $ACTUAL_BASE_IMAGE (matches lock: $BASE_MATCHES_LOCK) =="

echo "== building RPMs (srs6 $RELEASE_SRS6, srs7 $RELEASE_SRS7, jobs $JOBS, arch $ARCH) =="
"$PODMAN" run --rm \
    --platform "$PLATFORM" \
    --network=none \
    --security-opt label=disable \
    --env "SRS_BUILD_BASE_IMAGE=$ACTUAL_BASE_IMAGE" \
    --env "SRS_BUILD_LOCKED_IMAGE=$LOCKED_BASE_IMAGE" \
    --env "SRS_BUILD_BASE_MATCHES_LOCK=$BASE_MATCHES_LOCK" \
    --env "SRS_BUILD_BUILDER_IMAGE_ID=$BUILDER_IMAGE_ID" \
    --env "SRS_BUILD_BUILDER_IMAGE_DIGEST=$BUILDER_IMAGE_DIGEST" \
    --env "SRS_RELEASE_SRS6=$RELEASE_SRS6" \
    --env "SRS_RELEASE_SRS7=$RELEASE_SRS7" \
    --env "SRS_RELEASE_OVERRIDE=$RELEASE_OVERRIDE" \
    --volume "$HERE:/packaging:ro" \
    --volume "$SOURCE_DIR:/inputs:ro" \
    --volume "$OUT_DIR:/out:rw" \
    "$BUILDER_IMAGE_ID" \
    /bin/bash /packaging/build-in-container.sh "$JOBS"

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
