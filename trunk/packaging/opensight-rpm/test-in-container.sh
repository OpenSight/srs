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
    local pkg=$1 version_dir=$2 actual expected
    expected=$(printf '%s\n' \
        /usr/lib/opensight/srs \
        "/usr/lib/opensight/srs/$version_dir" \
        "/usr/lib/opensight/srs/$version_dir/srs" \
        "/usr/share/licenses/$pkg/LICENSE" | sort)
    actual=$(rpm -ql "$pkg" | sort)
    if [[ $actual != "$expected" ]]; then
        diff -u <(printf '%s\n' "$expected") <(printf '%s\n' "$actual") >&2 || true
        fail "$pkg payload is not the expected allowlist"
    fi
    while IFS= read -r path; do
        case $path in
            /usr/lib/opensight/srs|/usr/lib/opensight/srs/v[67]|/usr/lib/opensight/srs/v[67]/srs) ;;
            "/usr/share/licenses/$pkg/LICENSE") ;;
            *) fail "$pkg owns unexpected path: $path" ;;
        esac
    done <<<"$actual"
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
        assert_payload_allowlist "$pkg" "$version_dir"
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
        upgrade_pkg "$upgrade_dir" opensight-srs6
        assert_eq "$(installed_release opensight-srs6)" "2.el10"
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
        upgrade_pkg "$upgrade_dir" opensight-srs7
        assert_eq "$(installed_release opensight-srs7)" "2.el10"
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
        for pkg in opensight-srs6 opensight-srs7; do
            [[ -s "/usr/share/licenses/$pkg/LICENSE" ]] \
                || fail "$pkg does not install its license notice"
        done
        ;;

    *)
        fail "unknown scenario: $scenario"
        ;;
esac

echo "PASS[$scenario]"
