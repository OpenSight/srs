#!/usr/bin/env bash
# Exercise the OpenSight SRS RPM lifecycle in disposable, network-less
# AlmaLinux 10.2 x86_64_v2 containers.
#
# Usage:
#   ./test-rpm-lifecycle.sh --rpm-dir DIR --upgrade-rpm-dir DIR
#
# --rpm-dir         The locked release binary RPMs (from build-rpms.sh).
# --upgrade-rpm-dir Newer-release binary RPMs (build-rpms.sh --release
#                   1.2.20261125.el10).
#
# Each scenario starts from a clean container, so install order, upgrade and
# erase are independent of earlier scenarios.  SRS_PREVIOUS_RELEASE is the
# one-time pre-convention baseline for this first conversion of the three
# OpenSight RPMs (default: 1.el10); it is not a general cross-Version check.
set -euo pipefail

HERE=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
PODMAN=${PODMAN:-podman}
BASE_IMAGE=${BASE_IMAGE:-}
RPM_DIR=${RPM_DIR:-}
UPGRADE_RPM_DIR=${UPGRADE_RPM_DIR:-}
# One-time migration baseline for the first three OpenSight RPMs.
PREVIOUS_RELEASE=${SRS_PREVIOUS_RELEASE:-1.el10}
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
  --rpm-dir DIR          Locked-release binary RPM directory (required)
  --upgrade-rpm-dir DIR  Newer-release binary RPM directory (required)
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
    BASE_IMAGE=$(python3 -I - "$HERE/sources.lock.json" <<'PY'
import json
import sys

print(json.load(open(sys.argv[1], encoding="utf-8"))["build"]["image"])
PY
)
fi

lock_value() {
    python3 -I - "$HERE/sources.lock.json" "$@" <<'PY'
import json
import sys

value = json.load(open(sys.argv[1], encoding="utf-8"))
for key in sys.argv[2:]:
    value = value[key]
print(value)
PY
}

LOCKED_RELEASE_6=$(python3 -I "$HERE/release.py" \
    --lock "$HERE/sources.lock.json" --name srs6) \
    || die "cannot render the locked srs6 release"
LOCKED_RELEASE_7=$(python3 -I "$HERE/release.py" \
    --lock "$HERE/sources.lock.json" --name srs7) \
    || die "cannot render the locked srs7 release"

SRS_LICENSE_EXPR=$(lock_value license_expression)
SRS6_SOURCE_URL=$(lock_value sources srs6 url)
SRS6_SOURCE_SHA256=$(lock_value sources srs6 sha256)
SRS7_SOURCE_URL=$(lock_value sources srs7 url)
SRS7_SOURCE_SHA256=$(lock_value sources srs7 sha256)

# Both directories must hold the same two binary packages, at different
# releases for the upgrade scenarios.
for dir in "$RPM_DIR" "$UPGRADE_RPM_DIR"; do
    for pkg in opensight-srs6 opensight-srs7; do
        compgen -G "$dir/$pkg-*.rpm" | grep -qv '\.src\.rpm$' \
            || die "$dir has no binary RPM for $pkg"
    done
done

release_from_info() {
    python3 -I - "$1/build-info.json" "$2" <<'PY'
import json
import sys

info = json.load(open(sys.argv[1], encoding="utf-8"))
print(info["release_inputs"][sys.argv[2]]["release"])
PY
}

BASE_RELEASE_6=$(release_from_info "$RPM_DIR" srs6)
BASE_RELEASE_7=$(release_from_info "$RPM_DIR" srs7)
UPGRADE_RELEASE_6=$(release_from_info "$UPGRADE_RPM_DIR" srs6)
UPGRADE_RELEASE_7=$(release_from_info "$UPGRADE_RPM_DIR" srs7)

[[ $BASE_RELEASE_6 == "$LOCKED_RELEASE_6" ]] \
    || die "base srs6 release $BASE_RELEASE_6 != locked $LOCKED_RELEASE_6"
[[ $BASE_RELEASE_7 == "$LOCKED_RELEASE_7" ]] \
    || die "base srs7 release $BASE_RELEASE_7 != locked $LOCKED_RELEASE_7"
[[ $BASE_RELEASE_6 != "$UPGRADE_RELEASE_6" ]] \
    || die "upgrade srs6 release is identical to the base release"
[[ $BASE_RELEASE_7 != "$UPGRADE_RELEASE_7" ]] \
    || die "upgrade srs7 release is identical to the base release"

# The artifacts must record a build root that matches the locked base image;
# otherwise the hashes identify an unknown userspace, not the reviewed one.
# The locked directory must not be an override build; the upgrade directory
# must be one and must keep the locked source inputs.
check_build_info() {
    local dir=$1 override=$2
    python3 -I - "$dir/build-info.json" "$HERE/sources.lock.json" "$override" <<'PY' \
        || die "$dir/build-info.json does not describe the expected inputs"
import json
import sys

info = json.load(open(sys.argv[1], encoding="utf-8"))
lock = json.load(open(sys.argv[2], encoding="utf-8"))
override = sys.argv[3] == "1"
runtime = info["build"]["runtime"]
assert info["schema_version"] == 2
assert info["release_override"] is override
assert runtime["base_image"] == info["build"]["image"]
assert runtime["base_image_matches_lock"] is True
assert runtime["os_release"]["ID"] == "almalinux"
assert runtime["os_release"]["VERSION_ID"] == "10.2"
assert runtime["glibc_arch"] == "x86_64_v2"
for artifact in info["artifacts"]:
    assert artifact["is_source"] == artifact["file"].endswith(".src.rpm")
    if not artifact["is_source"]:
        assert artifact["file"] == artifact["nevra"] + ".rpm"
for key, entry in lock["sources"].items():
    got = info["release_inputs"][key]
    locked = entry["release"]
    assert got["package"] == entry["package"]
    assert got["version"] == entry["version"]
    assert got["release"] == (
        f"{got['source_revision']}.{got['packaging_revision']}."
        f"{got['date']}.{got['dist']}"
    )
    assert got["matches_lock"] is not override
    if not override:
        assert got["source_revision"] == locked["source_revision"]
        assert got["packaging_revision"] == locked["packaging_revision"]
        assert got["date"] == locked["date"]
        assert got["dist"] == locked["dist"]
PY
}

check_build_info "$RPM_DIR" 0
check_build_info "$UPGRADE_RPM_DIR" 1

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
        --env "SRS_LICENSE_EXPR=$SRS_LICENSE_EXPR" \
        --env "SRS6_SOURCE_URL=$SRS6_SOURCE_URL" \
        --env "SRS6_SOURCE_SHA256=$SRS6_SOURCE_SHA256" \
        --env "SRS7_SOURCE_URL=$SRS7_SOURCE_URL" \
        --env "SRS7_SOURCE_SHA256=$SRS7_SOURCE_SHA256" \
        --env "SRS_PREVIOUS_RELEASE=$PREVIOUS_RELEASE" \
        --env "SRS_BASE_RELEASE_6=$BASE_RELEASE_6" \
        --env "SRS_BASE_RELEASE_7=$BASE_RELEASE_7" \
        --env "SRS_UPGRADE_RELEASE_6=$UPGRADE_RELEASE_6" \
        --env "SRS_UPGRADE_RELEASE_7=$UPGRADE_RELEASE_7" \
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
