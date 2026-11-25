# OpenSight SRS 6.0.184 runtime executable.
#
# This package owns only the versioned executable (and its license notice).
# LVR owns systemd units, configuration, logging and version selection.
#
# Build with: rpmbuild -bb --target x86_64_v2 \
#                --define 'opensight_release 1.1.20261125.el10' ...
# The build must run on AlmaLinux 10.2 x86_64_v2 (see Containerfile).

%global source_root srs-opensight-v6.0
%global opensight_flags -march=x86-64-v2 -mtune=generic

# Everything statically linked into the executable: SRS itself (MIT), the
# embedded JSON parser (BSD-2-Clause) and HTTP parser (MIT, Joyent), the
# vendored OpenSSL/SSLeay toolkit, SRT (MPL-2.0), libsrtp and Opus
# (BSD-3-Clause), FFmpeg (LGPL-2.1-or-later) and State Threads
# (MPL-1.1 OR GPL-2.0-or-later).
%global opensight_license MIT AND BSD-2-Clause AND OpenSSL AND MPL-2.0 AND BSD-3-Clause AND LGPL-2.1-or-later AND (MPL-1.1 OR GPL-2.0-or-later)

# Release is <OpenSight source revision>.<packaging revision>.<UTC release
# date>.<distribution>.  This default matches sources.lock.json so a direct
# rebuild of this spec or its source RPM reproduces the verified release.  The
# provider build passes an explicit define (see release.py); the lifecycle test
# overrides it with a newer packaging revision to exercise an upgrade.
%{!?opensight_release:%global opensight_release 1.1.20261125.el10}

# Ship no debuginfo and no loader build-id aliases: the package has a closed
# payload allowlist and must not own paths outside it.
%global debug_package %{nil}
%global _build_id_links none
%global _unpackaged_files_terminate_build 1

Name:           opensight-srs6
Version:        6.0.184
Release:        %{opensight_release}
Summary:        OpenSight SRS 6 media server executable
License:        %{opensight_license}
URL:            https://github.com/OpenSight/srs
Source0:        srs-opensight-v6.0-20260222.zip
Source1:        sources.lock.json
Source2:        verify-sources.py
Source3:        LGPL-2.1.txt
Source4:        MPL-1.1.txt
Source5:        json-parser-BSD-2-Clause.txt
Source6:        http-parser-MIT.txt
Source7:        render-notice.py
ExclusiveArch:  x86_64_v2

BuildRequires:  gcc gcc-c++ make cmake patch perl tar unzip diffutils findutils
BuildRequires:  automake pkgconf-pkg-config which python3
BuildRequires:  tcl openssl openssl-devel nasm

%description
OpenSight SRS 6.0.184 built for the LVR AlmaLinux 10.2 x86_64_v2 appliance.

The package installs exactly one versioned executable at
/usr/lib/opensight/srs/v6/srs.  It deliberately ships no systemd unit, no
configuration and no unversioned command; LVR owns the service, the
configuration and the selection between the installed SRS versions.

The executable statically links the vendored components recorded in
sources.lock.json.  Corresponding source is that pinned archive, and the
accompanying source RPM contains it together with this spec and the build
recipe, so the binary can be rebuilt or relinked.  The applicable license and
notice texts are installed under /usr/share/licenses/opensight-srs6.

%prep
# The archive that gets extracted must be exactly the archive the lock file
# names and verifies; never trust an independently spelled Source0.
source_file=$(python3 -I %{SOURCE2} --lock %{SOURCE1} --source-dir %{_sourcedir} --name srs6 --print-files)
test "$source_file" = "$(basename %{SOURCE0})" || {
    echo "locked srs6 archive '$source_file' does not match Source0 '%{SOURCE0}'" >&2
    exit 1
}
python3 -I %{SOURCE2} --lock %{SOURCE1} --source-dir %{_sourcedir} --name srs6
rm -rf srs-src
mkdir srs-src
unzip -q "%{_sourcedir}/$source_file" -d srs-src
test "$(ls -A srs-src)" = "%{source_root}"
test -f srs-src/%{source_root}/trunk/configure
# Opus is only shipped as a tarball inside the archive; take its license text
# from that exact archive.
tar -xzf srs-src/%{source_root}/trunk/3rdparty/opus-1.3.1.tar.gz \
    -C srs-src opus-1.3.1/COPYING
grep -q '^#define VERSION_MAJOR 6$' \
    srs-src/%{source_root}/trunk/src/core/srs_core_version6.hpp
grep -q '^#define VERSION_MINOR 0$' \
    srs-src/%{source_root}/trunk/src/core/srs_core_version6.hpp
grep -q '^#define VERSION_REVISION 184$' \
    srs-src/%{source_root}/trunk/src/core/srs_core_version6.hpp

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
    %{buildroot}%{_prefix}/lib/opensight/srs/v6/srs
install -D -m 0644 srs-src/%{source_root}/LICENSE \
    %{buildroot}%{_licensedir}/%{name}/LICENSE
install -D -m 0644 srs-src/%{source_root}/trunk/3rdparty/openssl-1.1-fit/LICENSE \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.openssl
install -D -m 0644 srs-src/%{source_root}/trunk/3rdparty/srt-1-fit/LICENSE \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.srt
install -D -m 0644 srs-src/%{source_root}/trunk/3rdparty/libsrtp-2-fit/LICENSE \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.libsrtp
install -D -m 0644 srs-src/%{source_root}/trunk/3rdparty/ffmpeg-4-fit/LICENSE.md \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.ffmpeg
install -D -m 0644 %{SOURCE3} \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.lgpl-2.1
install -D -m 0644 srs-src/opus-1.3.1/COPYING \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.opus
install -D -m 0644 srs-src/%{source_root}/trunk/3rdparty/st-srs/LICENSE \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.state-threads
install -D -m 0644 %{SOURCE4} \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.mpl-1.1
install -D -m 0644 %{SOURCE5} \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.json-parser
install -D -m 0644 %{SOURCE6} \
    %{buildroot}%{_licensedir}/%{name}/LICENSE.http-parser
install -d %{buildroot}%{_licensedir}/%{name}
python3 -I %{SOURCE7} --lock %{SOURCE1} --name srs6 \
    --out %{buildroot}%{_licensedir}/%{name}/CORRESPONDING-SOURCE

%files
%defattr(-,root,root,-)
%dir %{_prefix}/lib/opensight/srs
%dir %{_prefix}/lib/opensight/srs/v6
%{_prefix}/lib/opensight/srs/v6/srs
%license %{_licensedir}/%{name}/LICENSE
%license %{_licensedir}/%{name}/LICENSE.openssl
%license %{_licensedir}/%{name}/LICENSE.srt
%license %{_licensedir}/%{name}/LICENSE.libsrtp
%license %{_licensedir}/%{name}/LICENSE.ffmpeg
%license %{_licensedir}/%{name}/LICENSE.lgpl-2.1
%license %{_licensedir}/%{name}/LICENSE.opus
%license %{_licensedir}/%{name}/LICENSE.state-threads
%license %{_licensedir}/%{name}/LICENSE.mpl-1.1
%license %{_licensedir}/%{name}/LICENSE.json-parser
%license %{_licensedir}/%{name}/LICENSE.http-parser
%license %{_licensedir}/%{name}/CORRESPONDING-SOURCE

%changelog
* Wed Nov 25 2026 OpenSight <opensource@opensight.cn> - 6.0.184-1.1.20261125.el10
- Publish the verified runtime RPM under the OpenSight release convention.
* Tue Nov 24 2026 OpenSight <opensource@opensight.cn> - 6.0.184-1
- Package the locked OpenSight SRS 6.0.184 source as a versioned RPM.
