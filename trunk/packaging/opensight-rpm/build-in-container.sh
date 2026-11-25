#!/usr/bin/env bash
# Build both OpenSight SRS RPMs inside the pinned AlmaLinux 10.2 x86_64_v2
# builder image.  Invoked by build-rpms.sh; not intended as a host entry point.
set -euo pipefail

packaging=/packaging
inputs=/inputs
output=/out
top=/work/rpmbuild

jobs=${1:?usage: build-in-container.sh <jobs>}
[[ $jobs =~ ^[1-9][0-9]*$ ]] || { echo "error: jobs must be a positive integer" >&2; exit 1; }

# The host passes the identity of the image this container was started from.
# Refuse to build on a userspace that is not the promised appliance baseline,
# and keep the recorded build root truthful even for experimental overrides.
: "${SRS_BUILD_BASE_IMAGE:?build-rpms.sh must pass SRS_BUILD_BASE_IMAGE}"
: "${SRS_BUILD_LOCKED_IMAGE:?build-rpms.sh must pass SRS_BUILD_LOCKED_IMAGE}"
: "${SRS_BUILD_BUILDER_IMAGE_ID:?build-rpms.sh must pass SRS_BUILD_BUILDER_IMAGE_ID}"
: "${SRS_BUILD_BASE_MATCHES_LOCK:?build-rpms.sh must pass SRS_BUILD_BASE_MATCHES_LOCK}"
: "${SRS_RELEASE_SRS6:?build-rpms.sh must pass SRS_RELEASE_SRS6}"
: "${SRS_RELEASE_SRS7:?build-rpms.sh must pass SRS_RELEASE_SRS7}"
: "${SRS_RELEASE_OVERRIDE:?build-rpms.sh must pass SRS_RELEASE_OVERRIDE}"

release6=$SRS_RELEASE_SRS6
release7=$SRS_RELEASE_SRS7
[[ $SRS_RELEASE_OVERRIDE == 0 || $SRS_RELEASE_OVERRIDE == 1 ]] \
    || { echo "error: SRS_RELEASE_OVERRIDE must be 0 or 1" >&2; exit 1; }

. /etc/os-release
[[ ${ID:-} == almalinux ]] \
    || { echo "error: builder is not AlmaLinux (ID=${ID:-unset})" >&2; exit 1; }
[[ ${VERSION_ID:-} == 10.2 ]] \
    || { echo "error: builder is not AlmaLinux 10.2 (VERSION_ID=${VERSION_ID:-unset})" >&2; exit 1; }
glibc_arch=$(rpm -q --qf '%{ARCH}' glibc)
[[ $glibc_arch == x86_64_v2 ]] \
    || { echo "error: glibc arch is $glibc_arch, not x86_64_v2" >&2; exit 1; }

export SRS_BUILD_OS_ID=${ID}
export SRS_BUILD_OS_VERSION_ID=${VERSION_ID}
export SRS_BUILD_GLIBC_ARCH=$glibc_arch

echo "== verifying pinned source inputs =="
python3 -I "$packaging/verify-sources.py" \
    --lock "$packaging/sources.lock.json" --source-dir "$inputs"

# Render the release recorded by each source entry and require the explicit
# inputs to agree unless this is a deliberate override (lifecycle upgrade).
locked6=$(python3 -I "$packaging/release.py" \
    --lock "$packaging/sources.lock.json" --name srs6)
locked7=$(python3 -I "$packaging/release.py" \
    --lock "$packaging/sources.lock.json" --name srs7)
python3 -I "$packaging/release.py" --validate "$release6" >/dev/null
python3 -I "$packaging/release.py" --validate "$release7" >/dev/null
if [[ $SRS_RELEASE_OVERRIDE == 0 ]]; then
    [[ $release6 == "$locked6" ]] \
        || { echo "error: srs6 release $release6 != locked $locked6" >&2; exit 1; }
    [[ $release7 == "$locked7" ]] \
        || { echo "error: srs7 release $release7 != locked $locked7" >&2; exit 1; }
fi

rm -rf "$top"
mkdir -p "$top"/BUILD "$top"/BUILDROOT "$top"/RPMS "$top"/SOURCES "$top"/SPECS "$top"/SRPMS

cp "$packaging/sources.lock.json" "$packaging/verify-sources.py" "$packaging/render-notice.py" "$top/SOURCES/"
cp "$packaging"/licenses/*.txt "$top/SOURCES/"
mapfile -t source_files < <(
    python3 -I "$packaging/verify-sources.py" \
        --lock "$packaging/sources.lock.json" --source-dir "$inputs" --print-files
)
for file in "${source_files[@]}"; do
    cp "$inputs/$file" "$top/SOURCES/"
done
cp "$packaging"/opensight-srs6.spec "$packaging"/opensight-srs7.spec "$top/SPECS/"

# A spec carries the verified release as its default so a direct source-RPM
# rebuild reproduces it.  Fail the provider build if that default drifts from
# the lock; an intentional lifecycle override only warns.
spec_release_default() {
    sed -n 's/^%{!?opensight_release:%global opensight_release \(.*\)}$/\1/p' "$1"
}

common_args=(
    --target x86_64_v2
    --define "_topdir $top"
    --define "_buildhost opensight-srs-rpm"
    --define "_smp_build_ncpus $jobs"
    --define "debug_package %{nil}"
)

for spec in opensight-srs6 opensight-srs7; do
    case $spec in
        opensight-srs6) release=$release6 ;;
        opensight-srs7) release=$release7 ;;
    esac
    default=$(spec_release_default "$top/SPECS/$spec.spec")
    [[ -n $default ]] \
        || { echo "error: $spec.spec has no default opensight_release" >&2; exit 1; }
    if [[ $default != "$release" ]]; then
        if [[ $SRS_RELEASE_OVERRIDE == 1 ]]; then
            echo "warning: $spec.spec default release $default overridden with $release" >&2
        else
            echo "error: $spec.spec default release $default != locked $release" >&2
            exit 1
        fi
    fi
    echo "== building $spec (release $release, jobs $jobs) =="
    rpmbuild -ba "${common_args[@]}" \
        --define "opensight_release $release" \
        "$top/SPECS/$spec.spec"
done

# Stage every generated file first.  The mounted output directory belongs to
# the caller and may contain unrelated files; only the exact artifact names
# below are ever written there, and nothing is deleted.
stage=/work/stage
rm -rf "$stage"
mkdir -p "$stage"

find "$top/RPMS" "$top/SRPMS" -type f -name '*.rpm' -exec cp '{}' "$stage"/ \;
( cd "$stage" && sha256sum *.rpm > SHA256SUMS )

python3 - "$packaging/sources.lock.json" "$stage" \
    "$release6" "$release7" "$locked6" "$locked7" "$SRS_RELEASE_OVERRIDE" <<'PY'
import hashlib
import json
import os
import pathlib
import subprocess
import sys

lock_path, stage = sys.argv[1], pathlib.Path(sys.argv[2])
release6, release7, locked6, locked7, override = sys.argv[3:8]
lock = json.loads(pathlib.Path(lock_path).read_text(encoding="utf-8"))
sources = lock["sources"]
key_by_package = {entry["package"]: key for key, entry in sources.items()}
expected = {entry["package"]: entry for entry in sources.values()}
actual_release = {"srs6": release6, "srs7": release7}
locked_release = {"srs6": locked6, "srs7": locked7}

def sha256(path):
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()

def query(path, fmt):
    return subprocess.check_output(
        ["rpm", "-qp", "--qf", fmt, str(path)], text=True
    )

records = []
for path in sorted(stage.glob("*.rpm")):
    name, version, rel, arch, size = query(
        path, "%{NAME}\t%{VERSION}\t%{RELEASE}\t%{ARCH}\t%{SIZE}"
    ).split("\t")
    source_rpm = query(path, "%{SOURCERPM}").strip()
    # rpmbuild --target also stamps the target arch into the source RPM header,
    # so the file suffix, not %{ARCH}, identifies a source package.
    is_source = path.name.endswith(".src.rpm")
    records.append(
        {
            "name": name,
            "version": version,
            "release": rel,
            "arch": arch,
            "is_source": is_source,
            "nevra": f"{name}-{version}-{rel}.{'src' if is_source else arch}",
            "source_rpm": source_rpm,
            "file": path.name,
            "sha256": sha256(path),
            "size_bytes": path.stat().st_size,
            "installed_size_bytes": int(size),
            "payload": query(path, "[%{FILENAMES}\n]").splitlines(),
            "requires": query(path, "[%{REQUIRENEVRS}\n]").splitlines(),
            "license": query(path, "%{LICENSE}").strip(),
        }
    )

for record in records:
    entry = expected.get(record["name"])
    if entry is None:
        raise SystemExit(f"unexpected package {record['name']}")
    if record["version"] != entry["version"]:
        raise SystemExit(
            f"{record['name']} version {record['version']} != {entry['version']}"
        )
    key = key_by_package[record["name"]]
    if record["release"] != actual_release[key]:
        raise SystemExit(
            f"{record['name']} release {record['release']} != {actual_release[key]}"
        )

release_inputs = {}
for key, entry in sources.items():
    actual = actual_release[key]
    source_revision, packaging_revision, date, dist = actual.split(".", 3)
    release_inputs[key] = {
        "package": entry["package"],
        "version": entry["version"],
        "source_revision": int(source_revision),
        "packaging_revision": int(packaging_revision),
        "date": date,
        "dist": dist,
        "release": actual,
        "matches_lock": actual == locked_release[key],
    }

runtime = {
    "base_image": os.environ["SRS_BUILD_BASE_IMAGE"],
    "locked_base_image": os.environ["SRS_BUILD_LOCKED_IMAGE"],
    "base_image_matches_lock": os.environ["SRS_BUILD_BASE_MATCHES_LOCK"] == "true",
    "builder_image_id": os.environ["SRS_BUILD_BUILDER_IMAGE_ID"],
    "builder_image_digest": os.environ.get("SRS_BUILD_BUILDER_IMAGE_DIGEST", ""),
    "os_release": {
        "ID": os.environ["SRS_BUILD_OS_ID"],
        "VERSION_ID": os.environ["SRS_BUILD_OS_VERSION_ID"],
    },
    "glibc_arch": os.environ["SRS_BUILD_GLIBC_ARCH"],
}

info = {
    "schema_version": 2,
    "build": {**lock["build"], "runtime": runtime},
    "release_inputs": release_inputs,
    "release_override": override == "1",
    "sources": lock["sources"],
    "artifacts": records,
}
(stage / "build-info.json").write_text(
    json.dumps(info, indent=2, sort_keys=True) + "\n", encoding="utf-8"
)
print(json.dumps(info, indent=2, sort_keys=True))
PY

mkdir -p "$output"
install -m 0644 "$stage"/*.rpm "$stage/SHA256SUMS" "$stage/build-info.json" "$output"/
( cd "$output" && sha256sum --check SHA256SUMS )
echo "== built artifacts =="
ls -l "$stage"
