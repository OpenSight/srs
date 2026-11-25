#!/usr/bin/env bash
# RPM lifecycle assertions.  Runs inside a disposable AlmaLinux 10.2
# x86_64_v2 container with /rpms and (for upgrade scenarios) /upgrade mounted.
#
# This is deliberately the packaging seam: install order, independent upgrade,
# independent erase, ownership, dynamic dependencies and payload allowlist.
set -euo pipefail

scenario=${1:?usage: test-in-container.sh <scenario>}
rpm_dir=/rpms
upgrade_dir=/upgrade

# SPDX expression for everything statically linked into the executable:
# SRS (MIT), the embedded JSON parser (BSD-2-Clause) and HTTP parser
# (MIT), OpenSSL/SSLeay, SRT (MPL-2.0), libsrtp and Opus (BSD-3-Clause),
# FFmpeg (LGPL-2.1-or-later), State Threads (MPL-1.1 OR GPL-2.0-or-later).
LICENSE_EXPR='MIT AND BSD-2-Clause AND OpenSSL AND MPL-2.0 AND BSD-3-Clause AND LGPL-2.1-or-later AND (MPL-1.1 OR GPL-2.0-or-later)'

LICENSE_FILES=(
    LICENSE
    LICENSE.openssl
    LICENSE.srt
    LICENSE.libsrtp
    LICENSE.ffmpeg
    LICENSE.lgpl-2.1
    LICENSE.opus
    LICENSE.state-threads
    LICENSE.mpl-1.1
    LICENSE.json-parser
    CORRESPONDING-SOURCE
)

parser_license() {
    case "$1" in
        opensight-srs6) printf 'http-parser\n' ;;
        opensight-srs7) printf 'llhttp\n' ;;
        *) fail "unknown package $1" ;;
    esac
}

fail() {
    echo "FAIL[$scenario]: $*" >&2
    exit 1
}

assert_eq() {
    [[ "$1" == "$2" ]] || fail "expected '$2', got '$1'"
}

rpm_file() {
    local dir=$1 pkg=$2 file
    for file in "$dir/$pkg"-*.rpm; do
        [[ $file == *.src.rpm ]] || { printf '%s\n' "$file"; return 0; }
    done
    fail "no binary RPM for $pkg in $dir"
}

install_pkg() {
    rpm -i "$(rpm_file "$1" "$2")"
}

upgrade_pkg() {
    rpm -U "$(rpm_file "$1" "$2")"
}

binary_path() {
    case "$1" in
        opensight-srs6) printf '/usr/lib/opensight/srs/v6/srs\n' ;;
        opensight-srs7) printf '/usr/lib/opensight/srs/v7/srs\n' ;;
        *) fail "unknown package $1" ;;
    esac
}

expected_version() {
    case "$1" in
        opensight-srs6) printf '6.0.184\n' ;;
        opensight-srs7) printf '7.0.218\n' ;;
        *) fail "unknown package $1" ;;
    esac
}

is_installed() {
    rpm -q "$1" >/dev/null 2>&1
}

assert_not_installed() {
    if is_installed "$1"; then
        fail "$1 is unexpectedly installed"
    fi
}

installed_nevra() {
    rpm -q --qf '%{NAME}-%{VERSION}-%{RELEASE}.%{ARCH}' "$1"
}

installed_release() {
    rpm -q --qf '%{RELEASE}' "$1"
}

expected_release() {
    case "$1" in
        opensight-srs6) printf '%s\n' "${SRS_BASE_RELEASE_6:?}" ;;
        opensight-srs7) printf '%s\n' "${SRS_BASE_RELEASE_7:?}" ;;
        *) fail "unknown package $1" ;;
    esac
}

upgrade_release() {
    case "$1" in
        opensight-srs6) printf '%s\n' "${SRS_UPGRADE_RELEASE_6:?}" ;;
        opensight-srs7) printf '%s\n' "${SRS_UPGRADE_RELEASE_7:?}" ;;
        *) fail "unknown package $1" ;;
    esac
}

assert_rpm_identity() {
    local dir=$1 pkg=$2 file expected actual
    file=$(rpm_file "$dir" "$pkg")
    expected=$(basename "$file" .rpm)
    actual=$(rpm -qp --qf '%{NAME}-%{VERSION}-%{RELEASE}.%{ARCH}' "$file")
    assert_eq "$actual" "$expected"
}

# rpmvercmp returns 1 when the first release is newer.  This covers both the
# previous published release (1.el10) and an explicit upgrade release.
assert_vercmp() {
    local left=$1 right=$2 expected=$3 result
    result=$(rpm --eval "%{lua:print(rpm.vercmp('$left', '$right'))}") \
        || fail "cannot compare releases $left and $right"
    assert_eq "$result" "$expected"
}

assert_release_contract() {
    local pkg=$1 current previous=${SRS_PREVIOUS_RELEASE:?}
    current=$(installed_release "$pkg")
    assert_eq "$current" "$(expected_release "$pkg")"
    assert_vercmp "$current" "$previous" 1
}

assert_binary_runs() {
    local pkg=$1 binary version
    binary=$(binary_path "$pkg")
    [[ -x $binary ]] || fail "$binary is not executable"
    version=$("$binary" -v)
    assert_eq "$version" "$(expected_version "$pkg")"
}

assert_arch() {
    assert_eq "$(rpm -q --qf '%{ARCH}' "$1")" "x86_64_v2"
}

assert_owned_by() {
    assert_eq "$(rpm -qf --qf '%{NAME}' "$2")" "$1"
}

assert_payload_allowlist() {
    local pkg=$1 version_dir=$2 actual expected license_dir=/usr/share/licenses/$pkg path
    expected=$(printf '%s\n' \
        /usr/lib/opensight/srs \
        "/usr/lib/opensight/srs/$version_dir" \
        "/usr/lib/opensight/srs/$version_dir/srs" \
        "$license_dir/LICENSE" \
        "$license_dir/LICENSE.openssl" \
        "$license_dir/LICENSE.srt" \
        "$license_dir/LICENSE.libsrtp" \
        "$license_dir/LICENSE.ffmpeg" \
        "$license_dir/LICENSE.lgpl-2.1" \
        "$license_dir/LICENSE.opus" \
        "$license_dir/LICENSE.state-threads" \
        "$license_dir/LICENSE.mpl-1.1" \
        "$license_dir/LICENSE.json-parser" \
        "$license_dir/LICENSE.$(parser_license "$pkg")" \
        "$license_dir/CORRESPONDING-SOURCE" | sort)
    actual=$(rpm -ql "$pkg" | sort)
    if [[ $actual != "$expected" ]]; then
        {
            echo "expected payload:"
            printf '%s\n' "$expected"
            echo "actual payload:"
            printf '%s\n' "$actual"
        } >&2
        fail "$pkg payload is not the expected allowlist"
    fi
    while IFS= read -r path; do
        case $path in
            /usr/lib/opensight/srs|/usr/lib/opensight/srs/v[67]|/usr/lib/opensight/srs/v[67]/srs) ;;
            /usr/share/licenses/"$pkg"/*) ;;
            *) fail "$pkg owns unexpected path: $path" ;;
        esac
    done <<<"$actual"
}

assert_license_materials() {
    local pkg=$1 license_dir=/usr/share/licenses/$pkg file url sha
    local files=("${LICENSE_FILES[@]}" "LICENSE.$(parser_license "$pkg")")
    assert_eq "$(rpm -q --qf '%{LICENSE}' "$pkg")" \
        "${SRS_LICENSE_EXPR:-$LICENSE_EXPR}"
    for file in "${files[@]}"; do
        [[ -s "$license_dir/$file" ]] \
            || fail "$pkg is missing non-empty license material $file"
    done
    case $pkg in
        opensight-srs6) url=${SRS6_SOURCE_URL:-}; sha=${SRS6_SOURCE_SHA256:-} ;;
        opensight-srs7) url=${SRS7_SOURCE_URL:-}; sha=${SRS7_SOURCE_SHA256:-} ;;
        *) fail "unknown package $pkg" ;;
    esac
    [[ -n $url && -n $sha ]] \
        || fail "test driver did not pass the locked source URL/SHA for $pkg"
    assert_owned_by "$pkg" "$license_dir/CORRESPONDING-SOURCE"
    grep -Fq "$url" "$license_dir/CORRESPONDING-SOURCE" \
        || fail "$pkg corresponding-source notice lacks the locked URL"
    grep -Fq "$sha" "$license_dir/CORRESPONDING-SOURCE" \
        || fail "$pkg corresponding-source notice lacks the locked SHA-256"
    grep -Fq 'rejected by that pin' "$license_dir/CORRESPONDING-SOURCE" \
        || fail "$pkg corresponding-source notice does not warn that the pin rejects modified bytes"
}

assert_no_service_or_config() {
    local pkg=$1 paths
    paths=$(rpm -ql "$pkg")
    if grep -qE '^/(etc|usr/bin|usr/sbin|usr/lib/systemd|usr/libexec|var)/' <<<"$paths"; then
        fail "$pkg ships service, configuration or command paths"
    fi
    if grep -qiE '\.(service|socket|target|conf|cfg|ini)$' <<<"$paths"; then
        fail "$pkg ships service or configuration files"
    fi
}

assert_dependencies() {
    local pkg=$1 requires
    requires=$(rpm -q --requires "$pkg")
    for lib in libc.so.6 libm.so.6 libstdc++.so.6 libgcc_s.so.1; do
        grep -q "^$lib" <<<"$requires" || fail "$pkg does not require $lib"
    done
    if grep -qE '^opensight-srs[67]' <<<"$requires"; then
        fail "$pkg depends on the other SRS package"
    fi
    if grep -qiE '^systemd' <<<"$requires"; then
        fail "$pkg depends on systemd"
    fi
}

assert_clean_verify() {
    local output
    output=$(rpm -V "$1")
    [[ -z $output ]] || fail "rpm -V $1 reported: $output"
}

assert_both_installed() {
    local pkg version_dir
    for pkg in opensight-srs6 opensight-srs7; do
        is_installed "$pkg" || fail "$pkg is not installed"
        case $pkg in
            opensight-srs6) version_dir=v6 ;;
            opensight-srs7) version_dir=v7 ;;
        esac
        assert_arch "$pkg"
        assert_binary_runs "$pkg"
        assert_owned_by "$pkg" "$(binary_path "$pkg")"
        assert_rpm_identity "$rpm_dir" "$pkg"
        assert_release_contract "$pkg"
        assert_payload_allowlist "$pkg" "$version_dir"
        assert_license_materials "$pkg"
        assert_no_service_or_config "$pkg"
        assert_dependencies "$pkg"
        assert_clean_verify "$pkg"
    done
    # Both packages co-own the shared parent directory.
    assert_eq \
        "$(rpm -qf --qf '%{NAME}\n' /usr/lib/opensight/srs | sort | tr '\n' ' ')" \
        "opensight-srs6 opensight-srs7 "
}

assert_only_remaining() {
    local keep=$1 removed=$2 removed_dir=$3
    is_installed "$keep" || fail "$keep was removed with $removed"
    assert_not_installed "$removed"
    [[ -x $(binary_path "$keep") ]] || fail "$keep executable disappeared"
    [[ ! -e $(binary_path "$removed") ]] || fail "$removed executable still exists"
    [[ ! -e /usr/lib/opensight/srs/$removed_dir ]] \
        || fail "$removed version directory still exists"
    assert_owned_by "$keep" "$(binary_path "$keep")"
    assert_owned_by "$keep" /usr/lib/opensight/srs
    assert_clean_verify "$keep"
    assert_binary_runs "$keep"
}

case $scenario in
    install-order-6-then-7)
        install_pkg "$rpm_dir" opensight-srs6
        is_installed opensight-srs6 || fail "opensight-srs6 did not install"
        assert_not_installed opensight-srs7
        assert_binary_runs opensight-srs6
        install_pkg "$rpm_dir" opensight-srs7
        assert_both_installed
        ;;

    install-order-7-then-6)
        install_pkg "$rpm_dir" opensight-srs7
        is_installed opensight-srs7 || fail "opensight-srs7 did not install"
        assert_not_installed opensight-srs6
        assert_binary_runs opensight-srs7
        install_pkg "$rpm_dir" opensight-srs6
        assert_both_installed
        ;;

    upgrade-6)
        install_pkg "$rpm_dir" opensight-srs6
        install_pkg "$rpm_dir" opensight-srs7
        kept_nevra=$(installed_nevra opensight-srs7)
        kept_hash=$(sha256sum "$(binary_path opensight-srs7)" | cut -d' ' -f1)
        assert_rpm_identity "$upgrade_dir" opensight-srs6
        upgrade_pkg "$upgrade_dir" opensight-srs6
        assert_eq "$(installed_release opensight-srs6)" \
            "$(upgrade_release opensight-srs6)"
        assert_vercmp "$(installed_release opensight-srs6)" \
            "${SRS_BASE_RELEASE_6:?}" 1
        assert_vercmp "$(installed_release opensight-srs6)" \
            "${SRS_PREVIOUS_RELEASE:?}" 1
        assert_eq "$(installed_nevra opensight-srs7)" "$kept_nevra"
        assert_eq "$(sha256sum "$(binary_path opensight-srs7)" | cut -d' ' -f1)" "$kept_hash"
        assert_clean_verify opensight-srs7
        assert_binary_runs opensight-srs7
        assert_binary_runs opensight-srs6
        assert_owned_by opensight-srs6 "$(binary_path opensight-srs6)"
        ;;

    upgrade-7)
        install_pkg "$rpm_dir" opensight-srs6
        install_pkg "$rpm_dir" opensight-srs7
        kept_nevra=$(installed_nevra opensight-srs6)
        kept_hash=$(sha256sum "$(binary_path opensight-srs6)" | cut -d' ' -f1)
        assert_rpm_identity "$upgrade_dir" opensight-srs7
        upgrade_pkg "$upgrade_dir" opensight-srs7
        assert_eq "$(installed_release opensight-srs7)" \
            "$(upgrade_release opensight-srs7)"
        assert_vercmp "$(installed_release opensight-srs7)" \
            "${SRS_BASE_RELEASE_7:?}" 1
        assert_vercmp "$(installed_release opensight-srs7)" \
            "${SRS_PREVIOUS_RELEASE:?}" 1
        assert_eq "$(installed_nevra opensight-srs6)" "$kept_nevra"
        assert_eq "$(sha256sum "$(binary_path opensight-srs6)" | cut -d' ' -f1)" "$kept_hash"
        assert_clean_verify opensight-srs6
        assert_binary_runs opensight-srs6
        assert_binary_runs opensight-srs7
        assert_owned_by opensight-srs7 "$(binary_path opensight-srs7)"
        ;;

    uninstall-6)
        install_pkg "$rpm_dir" opensight-srs6
        install_pkg "$rpm_dir" opensight-srs7
        rpm -e opensight-srs6
        assert_only_remaining opensight-srs7 opensight-srs6 v6
        ;;

    uninstall-7)
        install_pkg "$rpm_dir" opensight-srs6
        install_pkg "$rpm_dir" opensight-srs7
        rpm -e opensight-srs7
        assert_only_remaining opensight-srs6 opensight-srs7 v7
        ;;

    payload-contract)
        install_pkg "$rpm_dir" opensight-srs6
        install_pkg "$rpm_dir" opensight-srs7
        assert_both_installed
        # The upgrade artifacts use the real RPM basename and a newer Release.
        assert_rpm_identity "$upgrade_dir" opensight-srs6
        assert_rpm_identity "$upgrade_dir" opensight-srs7
        assert_vercmp "$(upgrade_release opensight-srs6)" \
            "${SRS_BASE_RELEASE_6:?}" 1
        assert_vercmp "$(upgrade_release opensight-srs7)" \
            "${SRS_BASE_RELEASE_7:?}" 1
        ;;

    *)
        fail "unknown scenario: $scenario"
        ;;
esac

echo "PASS[$scenario]"
