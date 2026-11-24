#!/usr/bin/env bash
# Exercise the OpenSight SRS RPM lifecycle in disposable, network-less
# AlmaLinux 10.2 x86_64_v2 containers.
#
# Usage:
#   ./test-rpm-lifecycle.sh --rpm-dir DIR --upgrade-rpm-dir DIR
#
# --rpm-dir         Release 1 binary RPMs (from build-rpms.sh).
# --upgrade-rpm-dir Release 2 binary RPMs (build-rpms.sh --release 2).
#
# Each scenario starts from a clean container, so install order, upgrade and
# erase are independent of earlier scenarios.
set -euo pipefail

HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
PODMAN=${PODMAN:-podman}
BASE_IMAGE=${BASE_IMAGE:-}
RPM_DIR=${RPM_DIR:-}
UPGRADE_RPM_DIR=${UPGRADE_RPM_DIR:-}
SCENARIOS=()

ALL_SCENARIOS=(
    install-order-6-then-7
    install-order-7-then-6
    upgrade-6
    upgrade-7
    uninstall-6
    uninstall-7
    payload-contract
)

usage() {
    sed -n '2,14p' "$HERE/test-rpm-lifecycle.sh" | sed 's/^# \{0,1\}//'
    cat <<'EOF'

Options:
  --rpm-dir DIR          Release 1 binary RPM directory (required)
  --upgrade-rpm-dir DIR  Release 2 binary RPM directory (required)
  --scenario NAME        Run only this scenario (repeatable)
  --base-image IMAGE     Override the pinned AlmaLinux base image
  --help                 Show this help
EOF
}

die() {
    echo "error: $*" >&2
    exit 1
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --rpm-dir) RPM_DIR=$2; shift 2 ;;
        --upgrade-rpm-dir) UPGRADE_RPM_DIR=$2; shift 2 ;;
        --scenario) SCENARIOS+=("$2"); shift 2 ;;
        --base-image) BASE_IMAGE=$2; shift 2 ;;
        --help|-h) usage; exit 0 ;;
        *) die "unknown option: $1 (see --help)" ;;
    esac
done

[[ -n $RPM_DIR ]] || die "--rpm-dir is required"
[[ -n $UPGRADE_RPM_DIR ]] || die "--upgrade-rpm-dir is required"
[[ -d $RPM_DIR ]] || die "RPM directory not found: $RPM_DIR"
[[ -d $UPGRADE_RPM_DIR ]] || die "upgrade RPM directory not found: $UPGRADE_RPM_DIR"
command -v "$PODMAN" >/dev/null || die "podman not found: $PODMAN"

RPM_DIR=$(cd "$RPM_DIR" && pwd)
UPGRADE_RPM_DIR=$(cd "$UPGRADE_RPM_DIR" && pwd)

if [[ -z $BASE_IMAGE ]]; then
    BASE_IMAGE=$(python3 - "$HERE/sources.lock.json" <<'PY'
import json
import sys

print(json.load(open(sys.argv[1], encoding="utf-8"))["build"]["image"])
PY
)
fi

# Both directories must hold the same two binary packages, at different
# releases for the upgrade scenarios.
for dir in "$RPM_DIR" "$UPGRADE_RPM_DIR"; do
    for pkg in opensight-srs6 opensight-srs7; do
        compgen -G "$dir/$pkg-*.rpm" | grep -qv '\.src\.rpm$' \
            || die "$dir has no binary RPM for $pkg"
    done
done

# The artifacts must record a build root that matches the locked base image;
# otherwise the hashes identify an unknown userspace, not the reviewed one.
for dir in "$RPM_DIR" "$UPGRADE_RPM_DIR"; do
    python3 -I - "$dir/build-info.json" <<'PY' \
        || die "$dir/build-info.json does not describe the locked build root"
import json
import sys

info = json.load(open(sys.argv[1], encoding="utf-8"))
runtime = info["build"]["runtime"]
assert runtime["base_image"] == info["build"]["image"]
assert runtime["base_image_matches_lock"] is True
assert runtime["os_release"]["ID"] == "almalinux"
assert runtime["os_release"]["VERSION_ID"] == "10.2"
assert runtime["glibc_arch"] == "x86_64_v2"
PY
done

if [[ ${#SCENARIOS[@]} -eq 0 ]]; then
    SCENARIOS=("${ALL_SCENARIOS[@]}")
fi

results=()
failed=0
for scenario in "${SCENARIOS[@]}"; do
    echo "== $scenario =="
    if "$PODMAN" run --rm \
        --platform linux/amd64/v2 \
        --network=none \
        --security-opt label=disable \
        --volume "$HERE:/tests:ro" \
        --volume "$RPM_DIR:/rpms:ro" \
        --volume "$UPGRADE_RPM_DIR:/upgrade:ro" \
        "$BASE_IMAGE" \
        /bin/bash /tests/test-in-container.sh "$scenario"; then
        results+=("PASS $scenario")
    else
        results+=("FAIL $scenario")
        failed=1
    fi
done

echo
echo "== summary =="
printf '%s\n' "${results[@]}"
exit "$failed"
