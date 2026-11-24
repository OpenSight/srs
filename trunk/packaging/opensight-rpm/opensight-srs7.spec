# OpenSight SRS 7.0.218 runtime executable.
#
# This package owns only the versioned executable (and its license notice).
# LVR owns systemd units, configuration, logging and version selection.
#
# Build with: rpmbuild -bb --target x86_64_v2 \
#                --define 'opensight_release 1' ...
# The build must run on AlmaLinux 10.2 x86_64_v2 (see Containerfile).

%global source_root srs-77c14a5eab4849832ecc1833ddfdd70479f3d74c
%global opensight_flags -march=x86-64-v2 -mtune=generic

# The release is overridable so the lifecycle test can build a newer release
# of the same payload and exercise an independent upgrade.
%{!?opensight_release:%global opensight_release 1}

# Ship no debuginfo and no loader build-id aliases: the package has a closed
# payload allowlist and must not own paths outside it.
%global debug_package %{nil}
%global _build_id_links none
%global _unpackaged_files_terminate_build 1

Name:           opensight-srs7
Version:        7.0.218
Release:        %{opensight_release}%{?dist}
Summary:        OpenSight SRS 7 media server executable
License:        MIT
URL:            https://github.com/OpenSight/srs
Source0:        srs-77c14a5eab4849832ecc1833ddfdd70479f3d74c.zip
Source1:        sources.lock.json
Source2:        verify-sources.py
ExclusiveArch:  x86_64_v2

BuildRequires:  gcc gcc-c++ make cmake patch perl tar unzip diffutils findutils
BuildRequires:  automake pkgconf-pkg-config which python3
BuildRequires:  tcl openssl openssl-devel nasm

%description
OpenSight SRS 7.0.218 built for the LVR AlmaLinux 10.2 x86_64_v2 appliance.

The package installs exactly one versioned executable at
/usr/lib/opensight/srs/v7/srs.  It deliberately ships no systemd unit, no
configuration and no unversioned command; LVR owns the service, the
configuration and the selection between the installed SRS versions.

%prep
python3 %{SOURCE2} --lock %{SOURCE1} --source-dir %{_sourcedir} --name srs7
rm -rf srs-src
mkdir srs-src
unzip -q %{SOURCE0} -d srs-src
test -f srs-src/%{source_root}/trunk/configure
grep -q '^#define VERSION_MAJOR 7$' \
    srs-src/%{source_root}/trunk/src/core/srs_core_version7.hpp
grep -q '^#define VERSION_MINOR 0$' \
    srs-src/%{source_root}/trunk/src/core/srs_core_version7.hpp
grep -q '^#define VERSION_REVISION 218$' \
    srs-src/%{source_root}/trunk/src/core/srs_core_version7.hpp

%build
cd srs-src/%{source_root}/trunk
# Match the reviewed LVR build exactly: SRS supplies -g -O0 itself and the
# appliance passes only the x86-64-v2 baseline flags.
unset CFLAGS CXXFLAGS LDFLAGS
./configure --generic-linux=on --sanitizer=off \
    --with-stat --srt=on --rtc=on --h265=on \
    --jobs=%{_smp_build_ncpus} \
    --extra-flags='%{opensight_flags}'
make %{?_smp_mflags}

%check
test "$(srs-src/%{source_root}/trunk/objs/srs -v)" = "%{version}"

%install
install -D -m 0755 srs-src/%{source_root}/trunk/objs/srs \
    %{buildroot}%{_prefix}/lib/opensight/srs/v7/srs
install -D -m 0644 srs-src/%{source_root}/LICENSE \
    %{buildroot}%{_licensedir}/%{name}/LICENSE

%files
%defattr(-,root,root,-)
%dir %{_prefix}/lib/opensight/srs
%dir %{_prefix}/lib/opensight/srs/v7
%{_prefix}/lib/opensight/srs/v7/srs
%license %{_licensedir}/%{name}/LICENSE

%changelog
* Mon Nov 24 2026 OpenSight <opensource@opensight.cn> - 7.0.218-1
- Package the locked OpenSight SRS 7.0.218 merge as a versioned RPM.
