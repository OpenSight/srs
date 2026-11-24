# OpenSight SRS v6/v7 RPM

本目录提供 IVT 双版本 SRS 的 RPM 打包定义。它只解决“把已锁定的两个 SRS 构建产物做成两个可独立安装、升级、卸载的 RPM”，不提供服务、配置或版本选择逻辑。

## 职责边界

- 本仓库负责：可复现的打包定义、固定输入校验、构建脚本和 RPM 生命周期测试。
- LVR 负责：安装两个 RPM、systemd 单元、配置校验、按需启动、启动哪个版本以及运维的版本选择。
- 两个 RPM 只包含各自的版本化可执行文件和许可证声明：

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

SRS 自身生成 `-g -O0`，因此 RPM 构建不额外注入 `-O2`。源包中的 3rdparty（ST、SRT、SRTP、OpenSSL、FFmpeg）均已随归档提供；容器运行阶段使用 `--network=none`，不下载未锁定内容。

## 输出契约

`build-rpms.sh` 产出以下文件（`--out-dir`，默认 `dist/`）：

- `opensight-srs6-6.0.184-1.el10.x86_64_v2.rpm`
- `opensight-srs7-7.0.218-1.el10.x86_64_v2.rpm`
- 对应的 `.src.rpm`
- `SHA256SUMS`
- `build-info.json`：输入摘要、构建镜像、每个产物的 NEVRA、SHA-256、大小、文件清单和动态依赖
- `lvr-handoff.json`：可直接并入 LVR `sources.lock.json` 的 `srs6_rpm`/`srs7_rpm` 条目（发布前不含 `url`，不假定远程产物存在）

动态依赖由 RPM 自动生成，预期只需要 `libc`、`libm`、`libstdc++` 和 `libgcc_s`；两个包之间没有依赖或冲突。RPM 不注册任何 systemd 或配置路径。

## 构建

准备一个包含两个固定归档的目录（LVR 的输入缓存即可），然后：

```bash
./build-rpms.sh --source-dir /path/to/pinned/sources --out-dir dist
```

脚本会：

1. 在宿主机校验两个归档的 SHA-256 和大小；
2. 用 `Containerfile` 从固定摘要构建 `opensight-srs-rpm-builder:el10.2`；
3. 在 `--network=none` 的 AlmaLinux 10.2 `x86_64_v2` 容器中运行 `rpmbuild -ba --target x86_64_v2`；
4. 在 `build-info.json` 中记录产物身份并生成 `SHA256SUMS`。

`--release N` 用于构建同一载荷的新 release（例如升级测试的 `2.el10`）。首次发布使用默认值 `1.el10`。

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
