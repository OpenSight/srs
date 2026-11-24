# 对应源码与重链接说明

本文件说明 `opensight-srs6` / `opensight-srs7` 二进制 RPM 的对应源码、重链接
步骤和固定输入的限制。每个包也会在安装时生成同一说明：

- `/usr/share/licenses/opensight-srs6/CORRESPONDING-SOURCE`
- `/usr/share/licenses/opensight-srs7/CORRESPONDING-SOURCE`

运行时说明由 `render-notice.py` 从 `sources.lock.json` 生成，因此其中的 URL 和
SHA-256 不会与构建使用的字节脱节。

## 固定输入

| 包 | 版本 | 公开源码归档 | SHA-256 | 大小 |
| --- | --- | --- | --- | ---: |
| `opensight-srs6` | 6.0.184 | `https://opensight-public.oss-cn-hangzhou.aliyuncs.com/software/srs-opensight-v6.0-20260222.zip` | `366cc70ef07799ef162b5c6f07b4660ea6e45cef41bbdc02173d13a7a7edfb57` | 34,397,478 |
| `opensight-srs7` | 7.0.218 | `https://codeload.github.com/OpenSight/srs/zip/77c14a5eab4849832ecc1833ddfdd70479f3d74c` | `60570ffdbd5f478d1b0485ae3b89d4b506cd893904ed5baec2cb629757e11849` | 36,779,987 |

这些归档已经包含静态链接进可执行文件的全部组件源码：SRS、OpenSSL 1.1.1l、
SRT（v6 1.5.3 / v7 1.5.6）、libsrtp 2.3.0、FFmpeg 4.2、Opus 1.3.1、State
Threads 1.9.1，以及内嵌的 json-parser 和 http-parser（v6）/ llhttp（v7）。

**固定字节限制：** 提交的 `sources.lock.json` 固定的是原始字节。
`build-rpms.sh` 在解压前校验 SHA-256 和大小，修改过的归档会被拒绝；不要
指望提交的 pin 接受新字节。

## 构建前置条件

- x86-64-v2 Linux 用户空间，与设备一致：AlmaLinux 10.2，基础镜像
  `quay.io/almalinuxorg/almalinux@sha256:607a7368ceaff7b76991a9d33ca489dcd6f2b699deaa65ef712d7b8d661ddcc3`。
- `gcc`、`gcc-c++`、`make`、`cmake`、`patch`、`perl`、`tar`、`unzip`、
  `diffutils`、`findutils`、`automake`、`pkgconf-pkg-config`、`which`、
  `python3`、`tcl`、`openssl`、`openssl-devel`、`nasm`（CRB）、
  `rpm-build`、`cpio`。
- rootless podman（构建脚本默认 `podman`，可用 `PODMAN` 覆盖）。

## A. 未修改源码的重链接

1. 从上面的 URL 匿名下载归档，并校验 SHA-256 和大小。
2. 在本目录运行：

   ```bash
   ./build-rpms.sh --source-dir <包含归档的目录> --out-dir <输出目录>
   ```

   脚本会校验锁文件、在固定 AlmaLinux 10.2 `x86_64_v2` 容器中解压归档，并用
   原始 configure 选项构建：

   ```text
   ./configure --generic-linux=on --sanitizer=off \
       --with-stat --srt=on --rtc=on --h265=on \
       --extra-flags='-march=x86-64-v2 -mtune=generic'
   ```

   SRS 自身生成 `-g -O0`；不要额外加 `-O2`。

## B. 修改自带组件后重链接

提交的 pin 不能接受修改后的字节。选择以下任一准确做法：

### B1. 直接从解压树重链接

```bash
unzip srs-opensight-v6.0-20260222.zip      # 或 v7 归档
cd srs-opensight-v6.0/trunk                # 或 v7 根目录/trunk
# 在 3rdparty/ 下修改组件
./configure --generic-linux=on --sanitizer=off \
    --with-stat --srt=on --rtc=on --h265=on \
    --extra-flags='-march=x86-64-v2 -mtune=generic'
make -j"$(nproc)"
./objs/srs -v
```

### B2. 使用本地替换归档和本地锁副本

1. 复制整个 `opensight-rpm` 打包目录到本地工作区。
2. 制作替换归档，计算其 SHA-256 和大小。
3. 只修改**副本**的 `sources.lock.json` 中相应的文件、URL、SHA-256 和大小。
4. 在副本中运行 `build-rpms.sh`，并把生成的对应源码说明与替换归档一起发布。

提交到本仓库的锁和已发布说明仍固定原始字节；替换归档必须使用自己的身份和
校验值。

## 源包

每个二进制 RPM 都有同名 `.src.rpm`，其中包含原始归档、spec、锁文件、校验器、
许可证文本和构建脚本。SRPM 仅本地保留，不发布到 OSS。获取对应源码无需注册
或付费。
