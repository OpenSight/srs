# OpenSight SRS v6/v7 RPM

本目录提供 IVT 双版本 SRS 的 RPM 打包定义。它只解决“把已锁定的两个 SRS 构建产物做成两个可独立安装、升级、卸载的 RPM”，不提供服务、配置或版本选择逻辑。

## 职责边界

- 本仓库负责：可复现的打包定义、固定输入校验、构建脚本和 RPM 生命周期测试。
- LVR 负责：安装两个 RPM、systemd 单元、配置校验、按需启动、启动哪个版本以及运维的版本选择。
- 两个 RPM 只包含各自的版本化可执行文件和这些组件要求的许可证/声明文件：

| 包名 | 版本 | 可执行文件 |
| --- | --- | --- |
| `opensight-srs6` | 6.0.184 | `/usr/lib/opensight/srs/v6/srs` |
| `opensight-srs7` | 7.0.218 | `/usr/lib/opensight/srs/v7/srs` |

包内没有 `/usr/bin` 命令、systemd 单元、配置、日志目录或未版本化符号链接。两个包共同拥有父目录 `/usr/lib/opensight/srs`；卸载其中一个时该目录保留给另一个包。

## 输入契约

`sources.lock.json` 是唯一的输入清单，沿用 LVR bootc 已锁定的两个来源：

| 键 | 版本 | 文件 | SHA-256 | 字节数 |
| --- | --- | --- | --- | --- |
| `srs6` | 6.0.184 | `srs-opensight-v6.0-20260222.zip` | `366cc70e…` | 34,397,478 |
| `srs7` | 7.0.218 | `srs-77c14a5eab4849832ecc1833ddfdd70479f3d74c.zip` | `60570ffd…` | 36,779,987 |

构建环境固定为 `build.image` 中的 AlmaLinux 10.2 `x86_64_v2` 镜像摘要，平台为 `linux/amd64/v2`。编译选项与当前 LVR 配方一致：

```text
./configure --generic-linux=on --sanitizer=off \
    --with-stat --srt=on --rtc=on --h265=on \
    --extra-flags='-march=x86-64-v2 -mtune=generic'
```

SRS 自身生成 `-g -O0`，因此 RPM 构建不额外注入 `-O2`。源包中的 3rdparty（ST、SRT、libsrtp、OpenSSL、FFmpeg、Opus）均已随归档提供；容器运行阶段使用 `--network=none`，不下载未锁定内容。

## 输出契约

`build-rpms.sh` 产出以下文件（`--out-dir`，默认 `dist/`）：

- `opensight-srs6-6.0.184-1.el10.x86_64_v2.rpm`
- `opensight-srs7-7.0.218-1.el10.x86_64_v2.rpm`
- 对应的 `.src.rpm`
- `SHA256SUMS`
- `build-info.json`：输入摘要、构建镜像、每个产物的 NEVRA、SHA-256、大小、文件清单和动态依赖
- `lvr-handoff.json`：可直接并入 LVR `sources.lock.json` 的 `srs6_rpm`/`srs7_rpm` 条目（发布前不含 `url`，不假定远程产物存在）

动态依赖由 RPM 自动生成，预期只需要 `libc`、`libm`、`libstdc++` 和 `libgcc_s`；两个包之间没有依赖或冲突。RPM 不注册任何 systemd 或配置路径。

## 许可证与对应源码

`/usr/lib/opensight/srs/v{6,7}/srs` 是静态链接的可执行文件。最终链接行包含 SRS 以及归档内自带的 OpenSSL、SRT、libsrtp、FFmpeg、Opus 和 State Threads，因此 RPM 的 `License` 字段和许可证载荷必须覆盖这些组件，不能只声明 SRS 的 MIT：

| 组件 | v6 版本 | v7 版本 | SPDX |
| --- | --- | --- | --- |
| SRS | 6.0.184 | 7.0.218 | `MIT` |
| json-parser（内嵌） | 归档内嵌 | 归档内嵌 | `BSD-2-Clause` |
| http-parser（内嵌） | 归档内嵌 | — | `MIT` |
| llhttp（内嵌） | — | 归档内嵌 | `MIT` |
| OpenSSL / SSLeay | 1.1.1l | 1.1.1l | `OpenSSL` |
| SRT | 1.5.3 | 1.5.6 | `MPL-2.0` |
| libsrtp | 2.3.0 | 2.3.0 | `BSD-3-Clause` |
| FFmpeg | 4.2 | 4.2 | `LGPL-2.1-or-later` |
| Opus | 1.3.1 | 1.3.1 | `BSD-3-Clause` |
| State Threads | 1.9.1 | 1.9.1 | `MPL-1.1 OR GPL-2.0-or-later` |

RPM 的 `License` 字段为：

```text
MIT AND BSD-2-Clause AND OpenSSL AND MPL-2.0 AND BSD-3-Clause AND LGPL-2.1-or-later AND (MPL-1.1 OR GPL-2.0-or-later)
```

每个包在 `/usr/share/licenses/<包名>/` 下安装以下文件：

| 文件 | 来源 |
| --- | --- |
| `LICENSE` | 归档根目录的 SRS `LICENSE`（MIT） |
| `LICENSE.openssl` | `trunk/3rdparty/openssl-1.1-fit/LICENSE`（OpenSSL + SSLeay 双许可全文） |
| `LICENSE.srt` | `trunk/3rdparty/srt-1-fit/LICENSE`（MPL-2.0 全文） |
| `LICENSE.libsrtp` | `trunk/3rdparty/libsrtp-2-fit/LICENSE` |
| `LICENSE.ffmpeg` | `trunk/3rdparty/ffmpeg-4-fit/LICENSE.md` |
| `LICENSE.lgpl-2.1` | 标准 GNU LGPL 2.1 全文（FFmpeg 声明所引用的许可证） |
| `LICENSE.opus` | `trunk/3rdparty/opus-1.3.1.tar.gz` 内的 `opus-1.3.1/COPYING` |
| `LICENSE.state-threads` | `trunk/3rdparty/st-srs/LICENSE` |
| `LICENSE.mpl-1.1` | 标准 Mozilla Public License 1.1 全文（State Threads 声明所引用的许可证） |
| `LICENSE.json-parser` | `trunk/src/protocol/srs_protocol_json.cpp` 内嵌的 BSD-2-Clause 声明 |
| `LICENSE.http-parser` | v6 `trunk/src/protocol/srs_protocol_http_stack.cpp` 内嵌的 Joyent MIT 声明 |
| `LICENSE.llhttp` | v7 内嵌 llhttp 的 MIT 声明（`licenses/llhttp-MIT.txt`） |
| `CORRESPONDING-SOURCE` | 对应源码与重链接说明，构建时由 `render-notice.py` 从锁文件生成 |

对应源码与重链接方式（完整说明见 `CORRESPONDING-SOURCE.md`，每个包也在
`/usr/share/licenses/<包名>/CORRESPONDING-SOURCE` 安装同一说明）：

- 两个固定归档本身已经包含上述全部第三方源码；`sources.lock.json` 记录了公开 URL、SHA-256 和大小。
- 每个二进制 RPM 都配套一个 `.src.rpm`，其中包含完整源码归档、spec、锁文件、校验器、许可证文本和构建脚本。
- 提交的锁固定的是**原始字节**：`build-rpms.sh` 会先校验 SHA-256 和大小，修改过的归档会被拒绝。不要指望提交的 pin 接受新字节。
- 未修改的重链接：下载并校验归档后运行本目录的 `build-rpms.sh --source-dir <目录>`；它使用原始 configure 选项和功能开关。
- 修改第三方组件后的重链接有两种准确做法：
  1. 直接解压归档，在 `trunk/3rdparty/` 中修改组件，然后在 `trunk/` 运行原始
     `./configure` 选项并 `make`；
  2. 复制整个打包目录到本地工作区，制作替换归档；归档必须保持与原固定归档
     相同的**文件名**（spec 的 `Source0` 是字面值，`%prep` 要求锁定文件名与
     它一致）和相同的顶层根目录，然后在**副本**的 `sources.lock.json` 中更新
     URL、SHA-256 和大小（如确实要改名，必须同时改副本 spec 的 `Source0`），
     再运行副本的 `build-rpms.sh`。提交的锁和已发布说明仍固定原始字节。
- 构建与生命周期测试不下载任何未锁定内容；发布前也不假定任何远程 RPM 产物存在。SRPM 仅本地保留，不发布到 OSS。

## 构建

准备一个包含两个固定归档的目录（LVR 的输入缓存即可），然后：

```bash
./build-rpms.sh --source-dir /path/to/pinned/sources --out-dir dist
```

脚本会：

1. 在宿主机校验两个归档的 SHA-256 和大小；
2. 用 `Containerfile` 从固定摘要构建 `opensight-srs-rpm-builder:el10.2`，镜像带有 `org.opensight.srs.base-image` 标签；
3. 一次性把 builder 标签解析为不可变镜像 ID，从同一次读取取得标签，要求它与 `sources.lock.json` 的 `build.image` 一致；随后记录该 ID 并用同一个 ID 启动构建容器，避免标签在两次查询之间被替换；不一致时拒绝发布构建（实验可用 `--allow-unlocked-base`）；
4. 在 `--network=none` 的 AlmaLinux 10.2 `x86_64_v2` 容器中运行 `rpmbuild -ba --target x86_64_v2`，并在容器内再次校验 os-release 和 glibc 架构；
5. 在 `build-info.json` 中记录声明镜像、实际镜像标签、builder 镜像 ID/摘要、os-release 和 glibc 架构，并生成 `SHA256SUMS`。

`--release N` 用于构建同一载荷的新 release（例如升级测试的 `2.el10`）。首次发布使用默认值 `1.el10`。

`--base-image` 和 `--no-build-image` 只改变实际构建根，不会改变记录：`build-info.json` 会如实记录实际标签和不可变镜像 ID，并在 `build.runtime.base_image_matches_lock` 标出是否与锁定镜像一致。发布构建必须为 `true`；生命周期测试也会校验这一点。

输出目录不会被清空或删除任何文件。构建先在容器内暂存，然后只覆盖与本 release 对应的精确文件名（两个二进制 RPM、两个 SRPM、`SHA256SUMS`、`build-info.json`）。请为每次发布使用新的输出目录，避免旧 release 文件与新文件混在一起。

> 若宿主机 podman 使用非默认存储配置，先设置 `CONTAINERS_STORAGE_CONF`。脚本本身不修改宿主机存储或镜像。

## 生命周期测试

先构建 release 1 和用于升级测试的 release 2：

```bash
./build-rpms.sh --source-dir /path/to/pinned/sources --out-dir dist
./build-rpms.sh --source-dir /path/to/pinned/sources --out-dir dist-r2 --release 2
```

然后从干净容器逐个执行场景：

```bash
./test-rpm-lifecycle.sh --rpm-dir dist --upgrade-rpm-dir dist-r2
```

覆盖的场景：

- `install-order-6-then-7` 与 `install-order-7-then-6`：两种安装顺序。
- `upgrade-6` 与 `upgrade-7`：只升级一个包，另一个包的 NEVRA、可执行文件哈希和 `rpm -V` 保持不变。
- `uninstall-6` 与 `uninstall-7`：只卸载一个包，另一个包仍可执行、归属正确，共享父目录保留。
- `payload-contract`：包内容白名单、`x86_64_v2` 架构、动态依赖、无跨包依赖、无 systemd/配置/命令路径、许可证声明存在。

每个场景都在 `--network=none` 的全新容器中运行，不接触部署设备。

## 供 LVR 集成的建议

LVR 不需要在本仓库之外重新编译 SRS。按现有 `ffmpeg_rpm` 的模式接入：

1. 在 LVR `deploy/bootc/sources.lock.json` 增加 `srs6_rpm`、`srs7_rpm`。`build.py` 的 `rpm_identity` 会校验名称、版本、release、架构、NEVRA、文件名、大小和 SHA-256：

```json
{
  "srs6_rpm": {
    "url": "https://<immutable-object-store>/<sha256>/opensight-srs6-6.0.184-1.el10.x86_64_v2.rpm",
    "file": "opensight-srs6-6.0.184-1.el10.x86_64_v2.rpm",
    "sha256": "<build-info.json 中的 sha256>",
    "size_bytes": 12345678,
    "rpm": {
      "name": "opensight-srs6",
      "epoch": 0,
      "version": "6.0.184",
      "release": "1.el10",
      "arch": "x86_64_v2",
      "nevra": "opensight-srs6-6.0.184-1.el10.x86_64_v2"
    }
  }
}
```

   `srs7_rpm` 同理，版本为 `7.0.218`。仓库原有的 `srs`/`srs_v7` 源码锁定可保留作为来源追溯，但不再参与镜像构建。

2. 在 `build.py` 的 `TARGET_INPUTS` 中用 `srs6_rpm`/`srs7_rpm` 替代 `srs`/`srs_v7`，并在 `IMAGE_SCRIPTS` 中用 `install-srs-rpms.sh` 替代 `build-srs.sh`。
3. 在 Containerfile 中校验 SHA-256、大小和 NEVRA 后用 `dnf` 从本地 RPM 安装；移除 `srs-build` 阶段。
4. `verify-image.sh`/`verify-booted.sh` 增加 `rpm -qf /usr/lib/opensight/srs/v6/srs` 与 `v7/srs` 的归属断言，并用 `rpm -q` 记录两个 NEVRA。
5. LVR 的服务、配置和选择逻辑通过 `run-srs`/`srs_version.py` 指向所选版本的 `/usr/lib/opensight/srs/v{6,7}/srs`；不要恢复 `/usr/lib/opensight/srs/srs` 或 `srs-6.0.184` 旧路径。

`lvr-handoff.json` 默认不含 `url`。在上传并远程校验 RPM 之后，用对象前缀重新生成带 URL 的条目：

```bash
./make-lvr-handoff.py --build-info dist/build-info.json \
    --object-prefix https://opensight-public.oss-cn-hangzhou.aliyuncs.com/software/srs-rpm \
    --out dist/lvr-handoff.published.json
```

在对象上传并远程校验之前，不应假定任何远程产物存在；`lvr-handoff.proposed-url.json` 一类的文件只表示建议的不可变键。

## 可复现性说明

- 构建配方、输入归档和容器摘要是固定的；同一输入可重复产出功能一致的 RPM。
- SRS 的链接输出会嵌入构建元数据，历史上不是逐字节可复现的；RPM 头也记录构建时间。因此以 `build-info.json` 中的 SHA-256 标识本次产物，而不是承诺跨次构建字节相同。
- 构建依赖来自 AlmaLinux 10.2 仓库，未逐包锁定 NEVRA；发布时应连同 `build-info.json` 一起归档。
