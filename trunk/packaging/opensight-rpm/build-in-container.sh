#!/usr/bin/env bash
# Build both OpenSight SRS RPMs inside the pinned AlmaLinux 10.2 x86_64_v2
# builder image.  Invoked by build-rpms.sh; not intended as a host entry point.
set -euo pipefail

release=${1:?usage: build-in-container.sh <release> <jobs>}
jobs=${2:?usage: build-in-container.sh <release> <jobs>}
[[ $release =~ ^[1-9][0-9]*$ ]]
[[ $jobs =~ ^[1-9][0-9]*$ ]]

# The host passes the identity of the image this container was started from.
# Refuse to build on a userspace that is not the promised appliance baseline,
# and keep the recorded build root truthful even for experimental overrides.
: "${SRS_BUILD_BASE_IMAGE:?build-rpms.sh must pass SRS_BUILD_BASE_IMAGE}"
: "${SRS_BUILD_LOCKED_IMAGE:?build-rpms.sh must pass SRS_BUILD_LOCKED_IMAGE}"
: "${SRS_BUILD_BUILDER_IMAGE_ID:?build-rpms.sh must pass SRS_BUILD_BUILDER_IMAGE_ID}"
: "${SRS_BUILD_BASE_MATCHES_LOCK:?build-rpms.sh must pass SRS_BUILD_BASE_MATCHES_LOCK}"

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

packaging=/packaging
inputs=/inputs
output=/out
top=/work/rpmbuild

echo "== verifying pinned source inputs =="
python3 "$packaging/verify-sources.py" \
    --lock "$packaging/sources.lock.json" --source-dir "$inputs"

rm -rf "$top"
mkdir -p "$top"/BUILD "$top"/BUILDROOT "$top"/RPMS "$top"/SOURCES "$top"/SPECS "$top"/SRPMS

cp "$packaging/sources.lock.json" "$packaging/verify-sources.py" "$top/SOURCES/"
cp "$packaging"/licenses/*.txt "$top/SOURCES/"
mapfile -t source_files < <(
    python3 "$packaging/verify-sources.py" \
        --lock "$packaging/sources.lock.json" --source-dir "$inputs" --print-files
)
for file in "${source_files[@]}"; do
    cp "$inputs/$file" "$top/SOURCES/"
done
cp "$packaging"/opensight-srs6.spec "$packaging"/opensight-srs7.spec "$top/SPECS/"

common_args=(
    --target x86_64_v2
    --define "_topdir $top"
    --define "_buildhost opensight-srs-rpm"
    --define "opensight_release $release"
    --define "_smp_build_ncpus $jobs"
    --define "debug_package %{nil}"
)

for spec in opensight-srs6 opensight-srs7; do
    echo "== building $spec (release $release, jobs $jobs) =="
    rpmbuild -ba "${common_args[@]}" "$top/SPECS/$spec.spec"
done

# Stage every generated file first.  The mounted output directory belongs to
# the caller and may contain unrelated files; only the exact artifact names
# below are ever written there, and nothing is deleted.
stage=/work/stage
rm -rf "$stage"
mkdir -p "$stage"

find "$top/RPMS" "$top/SRPMS" -type f -name '*.rpm' -exec cp '{}' "$stage"/ \;
( cd "$stage" && sha256sum *.rpm > SHA256SUMS )

python3 - "$packaging/sources.lock.json" "$stage" "$release" <<'PY'
import hashlib
import json
import os
import pathlib
import subprocess
import sys

lock_path, stage, release = sys.argv[1], pathlib.Path(sys.argv[2]), sys.argv[3]
lock = json.loads(pathlib.Path(lock_path).read_text(encoding="utf-8"))
expected = {entry["package"]: entry for entry in lock["sources"].values()}

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
    records.append(
        {
            "name": name,
            "version": version,
            "release": rel,
            "arch": arch,
            "nevra": f"{name}-{version}-{rel}.{arch}",
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
    if record["arch"] == "src":
        continue
    entry = expected.get(record["name"])
    if entry is None:
        raise SystemExit(f"unexpected package {record['name']}")
    if record["version"] != entry["version"]:
        raise SystemExit(
            f"{record['name']} version {record['version']} != {entry['version']}"
        )

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
    "schema_version": 1,
    "build": {**lock["build"], "runtime": runtime},
    "release": int(release),
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
