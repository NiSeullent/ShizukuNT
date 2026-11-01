<!--
 PROJECT:     ShizukuNT Korean translation
 LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 PURPOSE:     Korean translation of README.md (source commit 346eba3d855d)
 COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
-->
> [English](../../README.md) | [中文](../zh-CN/README.md) | [Français](../fr-FR/README.md) | [Español](../es-ES/README.md) | [日本語](../ja-JP/README.md) | **한국어** | [العربية](../ar-MA/README.md)

<p align="center">
  <img src="../../media/graphics/branding/libernt-banner.png" width="480" alt="LiberNT">
</p>

> [!IMPORTANT]
> LiberNT는 활발히 개발 중입니다. [Releases](https://github.com/LiberNT/LiberNT/releases) 페이지의 빌드는 테스트용 Debug 시험판입니다. 가상 머신이나 여분의 하드웨어에서 실행하고, 평소에 쓰는 중요한 컴퓨터에서는 실행하지 마십시오.

<div align="center">

# [LiberNT](https://libernt.com)
### 자유 NT 운영 체제

**NT 아키텍처를 향한 애정으로 만들었습니다. Windows® 11 호환, amd64·arm64·riscv64 지원.**

[![Build](https://img.shields.io/github/actions/workflow/status/LiberNT/LiberNT/build.yml?branch=main-nt10&label=Build&style=for-the-badge&logo=githubactions&logoColor=white)](https://github.com/LiberNT/LiberNT/actions/workflows/build.yml)
[![License: GPL 3.0](https://img.shields.io/badge/License-GPL_3.0-0161B6?style=for-the-badge&logo=gnu&logoColor=white)](#라이선스)
[![Discord](https://img.shields.io/badge/Discord-Join-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/BpcKg7FZ2)
[![Matrix](https://img.shields.io/badge/Matrix-%23libernt-000000?style=for-the-badge&logo=matrix&logoColor=white)](https://matrix.to/#/#libernt:matrix.org)

<img src="https://img.shields.io/github/repo-size/LiberNT/LiberNT?style=flat-square&color=blue" alt="Repo Size">
<img src="https://img.shields.io/github/stars/LiberNT/LiberNT?style=flat-square&color=yellow" alt="Stars">
<img src="https://img.shields.io/github/forks/LiberNT/LiberNT?style=flat-square&color=lightgrey" alt="Forks">
<img src="https://img.shields.io/github/last-commit/LiberNT/LiberNT/main-nt10?style=flat-square&color=green" alt="Last Commit">
<img src="https://img.shields.io/github/issues/LiberNT/LiberNT?style=flat-square&color=orange" alt="Issues">

<br>

[![Download the latest build](https://img.shields.io/badge/Download-Latest_build-0161B6?style=for-the-badge&logo=github&logoColor=white)](https://github.com/LiberNT/LiberNT/releases)

</div>

## 후원

<p align="center">
  <a href="https://github.com/sponsors/ahmedarif193">
    <img src="https://img.shields.io/badge/Sponsor-LiberNT-ea4aaa?style=for-the-badge&logo=githubsponsors&logoColor=white" alt="Sponsor LiberNT">
  </a>
</p>

---

<p align="center">
  <img width="1000" alt="arm64에서 실행 중인 LiberNT 바탕 화면" src="../../media/graphics/screenshots/libernt-desktop-arm64.png">
</p>

<p align="center"><sub>arm64에서 실행 중인 LiberNT 바탕 화면. QEMU에서 실행한 빌드 nt10.0-043-2555</sub></p>

---

LiberNT는 자유 NT 운영 체제입니다. 이름에 그 뜻이 담겨 있습니다. *liber*는 라틴어로 **자유로운**이라는 뜻입니다. LiberNT는 **Windows® 11 호환성**을 목표로 하며, 자유로운 시도와 실험적인 기능을 위한 여지도 남겨 둡니다.

LiberNT는 안정성과 최신 하드웨어를 목표로 커널 공간 구성 요소와 드라이버에 폭넓은 작업을 더했으며, 64비트 플랫폼인 **amd64, arm64, riscv64**에 집중합니다.

우리의 목표는 오랫동안 유지 관리할 수 있는 시스템입니다. **안정적이고, 믿을 수 있고, 효율적이며**, 임베디드 장치에서도 실행되고, 최신 운영 체제가 갖춰야 할 모든 기능을 지원할 수 있는 시스템을 지향합니다.

> [!NOTE]
> NT가 처음이십니까? **NT**는 최신 Windows의 모든 버전을 떠받치는 아키텍처로, 커널, 드라이버 모델, 시스템 서비스가 여기에 해당합니다. LiberNT는 이 아키텍처 위에 만든 자유 운영 체제이며, Windows 11을 스스로를 견주는 기준으로 삼습니다.

---

## 목차

- [LiberNT에 포함된 것](#libernt에-포함된-것)
- [지원 플랫폼](#지원-플랫폼)
- [시작하기](#시작하기)
  - [1단계: 빌드 다운로드](#1단계-빌드-다운로드)
  - [2단계: 부팅하기](#2단계-부팅하기)
  - [3단계: LiberNT 설치하기](#3단계-libernt-설치하기)
- [LiberNT 직접 빌드하기](#libernt-직접-빌드하기)
- [빌드 호스트](#빌드-호스트)
- [프로젝트 구조](#프로젝트-구조)
- [기여하기](#기여하기)
- [AI 사용](#ai-사용)
- [라이선스](#라이선스)
- [프로젝트 소개](#프로젝트-소개)

---

## LiberNT에 포함된 것

LiberNT는 최신 하드웨어와 플랫폼을 NT 아키텍처로 가져옵니다.

| 기능 | 사용자에게 주는 의미 |
| --- | --- |
| **64비트 우선** | 커널, HAL, FreeLoader가 amd64, arm64, riscv64에서 실행됩니다. |
| **UEFI 부팅** | UEFI 펌웨어에서 부팅하며, GOP 프레임 버퍼와 부팅 초기의 화면 표시를 지원합니다. 사전 설치 디스크는 UEFI와 BIOS 모두에서 부팅됩니다. |
| **최신 버스와 저장소** | PCIe, PCI, 플러그 앤 플레이, xHCI 기반 USB, SD/eMMC, 그리고 저장소와 버스 드라이버. |
| **보드 지원** | Raspberry Pi와 그 RP1 I/O 컨트롤러를 포함한 실제 보드. |
| **Windows 11 드라이버 모델** | Windows 11(NT10) 호환성과 드라이버 모델 작업을 Windows 11 자체와 대조해 확인합니다. |
| **Arm에서 그래픽과 x86 앱** | Mesa를 사용하는 그래픽 드라이버 모델과 FEX를 사용하는 WoW64는 켤 수 있는 빌드 옵션입니다. |
| **LiberNT 설치 프로그램** | Windows 설치 프로그램과 같은 방식으로 드라이브를 나열하고, 파티션을 만들고, 삭제하고, 포맷하고, 축소하고, 확장하며, 필요한 EFI 시스템 파티션과 함께 LiberNT를 NTFS에 설치합니다. |
| **나만의 빌드** | `menuconfig`에서 대상, 컴파일러, 빌드 종류를 고르고 기능을 켜거나 끌 수 있습니다. |

> [!TIP]
> [Releases](https://github.com/LiberNT/LiberNT/releases) 페이지의 각 빌드에는 이전 빌드 이후 바뀐 내용이 적혀 있습니다.

---

## 지원 플랫폼

| 아키텍처 | 미리 빌드된 이미지 | 소스에서 빌드 | 비고 |
| --- | :---: | :---: | --- |
| **amd64** | ✓ | ✓ | Clang(기본값) 또는 GCC |
| **arm64** | ✓ | ✓ | Raspberry Pi 지원 포함 |
| **riscv64** | ✓ | ✓ | JH7110 보드용 이미지를 따로 빌드합니다 |
| **i386** | | ✓ | 32비트 x86 |

릴리스 빌드에는 PowerPC 이미지도 들어 있습니다.

---

## 시작하기

### 1단계: 빌드 다운로드

[Releases](https://github.com/LiberNT/LiberNT/releases) 페이지의 모든 빌드는 이미지와 아키텍처마다 ZIP 파일 하나로 제공됩니다.

| 파일 | 내용 |
| --- | --- |
| `bootcd-iso-…-<arch>-…zip` | `bootcd.iso`. LiberNT를 실행하거나 LiberNT 설치 프로그램을 시작하는 CD 이미지 |
| `LiberNT-img-…-<arch>-…zip` | `LiberNT.img`. LiberNT가 이미 설치된 디스크 |
| `LiberNT-vhd-…-<arch>-…zip` | `LiberNT.vhd`. VHD 기반 가상 머신용의 같은 사전 설치 디스크 |
| `LiberNT-vhdx-…-<arch>-…zip` | `LiberNT.vhdx`. Hyper-V 2세대용의 같은 사전 설치 디스크 |

> [!NOTE]
> 이 빌드들은 GitHub Actions가 `main-nt10` 브랜치에서 만든 Clang Debug 빌드입니다.

### 2단계: 부팅하기

LiberNT를 가장 빨리 써 보는 방법은 사전 설치 디스크입니다.

1. `LiberNT.img`(또는 `LiberNT.vhd`)의 압축을 풉니다
2. **QEMU** 같은 가상 머신에서 시작하거나, VHD를 **VirtualBox** 또는 **Hyper-V** 1세대 가상 머신에서 사용합니다. Hyper-V 2세대는 `LiberNT.vhdx`만 받으며, 보안 부팅을 꺼야 합니다
3. 또는 `LiberNT.img`를 USB 드라이브나 SD 카드에 쓰고 그 장치로 컴퓨터를 부팅합니다. UEFI와 BIOS 모두에서 부팅됩니다

> [!CAUTION]
> `LiberNT.img`를 USB 드라이브나 SD 카드에 쓰면 그 안의 모든 내용이 지워집니다. 먼저 대상 장치를 다시 한번 확인하십시오.

### 3단계: LiberNT 설치하기

가상 머신이나 PC에서 `bootcd.iso`로 부팅한 다음 **LiberNT 설치 프로그램**의 안내를 따릅니다.

사전 설치 디스크에도 LiberNT 설치 프로그램이 들어 있습니다. 실행 중인 시스템에서 다른 디스크에 LiberNT를 설치하려면 `C:\<아키텍처>\libernt.exe`(예: `C:\arm64\libernt.exe`)를 실행합니다.

> [!CAUTION]
> 설치 프로그램은 파티션을 삭제하고 포맷할 수 있습니다. 설치하기 전에 대상 디스크의 중요한 데이터를 백업하십시오.

전체 설치 안내는 [INSTALL.md](INSTALL.md)를 참조하십시오.

---

## LiberNT 직접 빌드하기

LiberNT는 크로스 빌드 툴체인인 RosBE로 빌드합니다. 아래 단계는 Linux와 macOS, 그리고 WSL2를 통한 Windows에서 동작합니다.

### 1. 툴체인 설치

사용하는 컴퓨터용 Git, CMake, Ninja, C/C++ 컴파일러가 필요합니다. RosBE는 LiberNT를 빌드하는 컴파일러를 더해 줍니다.

```sh
curl -fsSL https://raw.githubusercontent.com/LiberNT/libernt-build-toolchain/main/rosbe-unix-bootstrap.sh | sh
```

> [!TIP]
> Windows에서는 WSL2를 권장합니다. `wsl --install`로 설치하고, 추가된 Linux 터미널을 열어 그 안에서 Linux 단계를 따르십시오. 소스는 `/mnt/c` 아래가 아니라 Linux 파일 시스템 안에 두십시오. `/mnt/c`에서는 빌드가 더 느립니다.

### 2. 소스 받기

```sh
git clone https://github.com/LiberNT/LiberNT.git
cd LiberNT
```

> [!NOTE]
> 처음 구성할 때 빌드에 필요한 서브모듈과 외부 소스도 함께 받아 옵니다.

### 3. 구성

```sh
./configure.sh
```

옵션 없이 실행하면 `output-Clang-amd64-debug` 폴더에 Clang을 사용하는 AMD64 Debug 빌드를 구성합니다. 폴더 이름은 항상 `output-<컴파일러>-<아키텍처>-<debug|release>` 형식을 따릅니다.

| 옵션 | 동작 |
| --- | --- |
| `-a`, `--arch <arch>` | 대상 아키텍처를 고릅니다: `amd64`(기본값), `i386`, `arm64`, `riscv64`. |
| `--clang` | RosBE의 Clang/LLVM으로 빌드합니다. 이것이 기본값입니다. |
| `--gcc` | Clang 대신 RosBE의 GCC로 빌드합니다. macOS용 RosBE에는 GCC가 없습니다. |
| `-r`, `--release` | 최적화된 Release 빌드를 만듭니다. 이 옵션이 없으면 디버그하기 쉬운 Debug 빌드가 만들어집니다. |
| `menuconfig` | 먼저 메뉴를 열어 선택 구성 요소를 고릅니다. 선택한 내용은 출력 폴더에 보관됩니다. |
| `makefiles` | Ninja 대신 `make`를 사용합니다. |
| `--no-feeds-update` | `feeds.conf`에 나열된 외부 소스를 받아 오지 않습니다. 오프라인일 때 등에 씁니다. |
| `-D<name>=<value>` | 설정을 CMake에 그대로 넘깁니다. |

예:

```sh
./configure.sh -a arm64              # Clang을 사용하는 ARM64 Debug 빌드
./configure.sh -a i386 --gcc         # GCC를 사용하는 32비트 x86 빌드
./configure.sh -a amd64 --release    # 최적화된 AMD64 빌드
```

빌드를 더 자세히 정하려면 `menuconfig`를 붙입니다.

```sh
./configure.sh menuconfig
```

Linux 커널의 메뉴와 비슷한 메뉴가 열립니다. 여기서 대상, 컴파일러, 빌드 종류를 고르고 코드 생성과 튜닝, 그래픽 드라이버 모델과 Mesa, WoW64와 FEX, 커널 디버깅, 테스트 실행 같은 기능을 켜거나 끕니다. 옵션에서 `?`를 누르면 도움말이 나오고, `S`는 저장, `Q`는 끝내기입니다. 선택한 내용은 출력 폴더에 저장되어 다시 구성할 때마다 쓰입니다.

### 4. 빌드

출력 폴더로 이동해 원하는 대상을 지정하여 Ninja를 실행합니다.

```sh
cd output-Clang-amd64-debug
ninja liberntimg
```

| 명령 | 만들어지는 파일 | 용도 |
| --- | --- | --- |
| `ninja bootcd` | `bootcd.iso` | 가상 머신이나 PC에서 CD 이미지로 LiberNT를 설치합니다. |
| `ninja liberntimg` | `LiberNT.img` | LiberNT가 이미 설치된 디스크로 부팅합니다. QEMU 같은 가상 머신에서 시작하거나 USB 드라이브나 SD 카드에 씁니다. UEFI와 BIOS 모두에서 부팅됩니다. |
| `ninja liberntvhd` | `LiberNT.vhd`, `LiberNT.vhdx` | 같은 사전 설치 디스크를 가상 머신에서 사용합니다. VHD는 VirtualBox와 Hyper-V 1세대에서, VHDX는 VHD 디스크를 받지 않는 Hyper-V 2세대에서 씁니다. |

각 대상은 필요한 것을 모두 먼저 빌드하며, 파일은 출력 폴더에 기록됩니다.

---

## 빌드 호스트

| 호스트 | 컴파일러 | 비고 |
| --- | --- | --- |
| **Linux** x86_64 / aarch64 | Clang과 GCC | RosBE 툴체인 전체 |
| **macOS** Intel / Apple Silicon | Clang | RosBE에는 macOS용 GCC 빌드가 없습니다 |
| **Windows** | Clang과 GCC | WSL2를 통해 Linux 단계를 따릅니다 |

모든 호스트에서 Git, CMake, Ninja, Flex, Bison을 패키지 관리자로 설치하십시오. RosBE는 크로스 컴파일러만 제공합니다. 미리 빌드된 툴체인은 [LiberNT/libernt-build-toolchain](https://github.com/LiberNT/libernt-build-toolchain)에 있습니다.

---

## 프로젝트 구조

```text
LiberNT/
├── base/            # 응용 프로그램, 서비스, 설치 프로그램, 셸
├── boot/            # FreeLoader, 부팅 데이터, 부팅 이미지
├── bootloader/      # 부팅 관리자, Windows 로더, UEFI와 Open Firmware 지원
├── dll/             # ntdll과 Win32 라이브러리를 포함한 사용자 모드 DLL
├── drivers/         # 커널 모드 드라이버: 버스, 저장소, 파일 시스템, 네트워크, 입력 등
├── hal/             # x86, Arm, Arm64, PowerPC, RISC-V용 하드웨어 추상화 계층
├── media/           # 글꼴, 테마, INF 파일, 소리, 브랜드 자료
├── modules/         # 선택 응용 프로그램, 테스트, 배경 화면
├── ntoskrnl/        # NT 커널과 실행부(executive)
├── sdk/             # 헤더, 라이브러리, 빌드 도구
├── submodules/      # 외부 구성 요소의 레시피와 패치
├── subsystems/      # CSRSS와 Windows 하위 시스템
├── win32ss/         # win32k, USER, GDI, 디스플레이 드라이버
├── configure.sh     # Linux, macOS, WSL2에서 빌드를 구성
└── configure.cmd    # Windows에서 빌드를 구성
```

---

## 기여하기

버그 보고와 기능 제안은 [Issues](https://github.com/LiberNT/LiberNT/issues)에 올려 주십시오. 안내서와 메모는 [Wiki](https://github.com/LiberNT/LiberNT/wiki)에서 찾을 수 있습니다.

[CONTRIBUTING.md](CONTRIBUTING.md)와 [PULL_REQUEST_MANAGEMENT.md](PULL_REQUEST_MANAGEMENT.md)를 참조하십시오. 큰 변경을 계획하고 있다면 먼저 [Issues](https://github.com/LiberNT/LiberNT/issues)나 [Discord](https://discord.gg/BpcKg7FZ2)에서 문의하십시오.

> [!WARNING]
> **법적 고지:** 독점 소프트웨어인 Microsoft Windows의 소스 코드(유출된 Windows NT 3.5, NT 4, 2000 소스 코드와 Windows Research Kernel을 포함하되 이에 한정되지 않음)를 본 적이 있다면, 저작권 침해 가능성 때문에 기여를 받아들일 수 없습니다.

---

## AI 사용

분명히 밝혀 둡니다. AI는 LiberNT를 만드는 방식의 일부입니다. AI는 코드 검토와 작성을 돕고, AI가 쓴 모든 것은 사람이 다시 검토합니다. 무엇이든 메인 브랜치에 들어가기 전에 사람이 종단 간(end-to-end) 테스트를 실행해 검증합니다. 우리는 이 프로젝트를 무척 아끼기 때문에 회귀가 슬쩍 지나가도록 두지 않습니다.

AI는 문서를 읽고, 정의를 추출하고, 버그를 고치는 일도 돕습니다. 어떤 버그든 공식 Microsoft Learn 문서의 설명과 우리 함수의 실제 동작을 비교하여, 동작이 정확히 어디서 다른지 알 수 있게 해 줍니다.

다른 도구와 마찬가지로 AI는 우리가 목표를 이루도록 돕기 위해 있습니다. AI도 우리가 스스로 지키는 규칙, 곧 [AGENTS.md](../../AGENTS.md)에 정한 규칙을 똑같이 따릅니다.

- Windows SDK와 WDK 헤더, Windows 11 공개 심볼, Microsoft 문서, Windows 11에서의 블랙박스 실행을 바탕으로 만듭니다.
- 클린룸 원칙을 지킵니다. 디스어셈블하거나 디컴파일한 Windows 코드를 옮겨 적지 않으며, Linux의 GPL 코드를 복사하지 않습니다.
- 수정은 실제 부팅이나 테스트 실행에서 동작하는 것을 확인한 뒤에야 수정으로 인정합니다. 테스트가 통과하면 앞으로 도입할 CI가 그 상태를 유지하여 전체 진척을 지킵니다.
- 테스트 결과는 Windows 11에서 실행한 같은 테스트와 검사 항목별로 비교합니다.

> [!TIP]
> 기여할 때 AI를 사용하는 분도 환영합니다. 풀 리퀘스트에 AI가 어느 부분을 도왔는지, 어떻게 테스트했는지만 알려 주십시오. 우리가 바라는 것은 그 정직함뿐입니다.

---

## 라이선스

LiberNT는 [GNU GPL 3.0](../../COPYING)으로 배포됩니다. 다른 프로젝트에서 물려받은 코드는 파일 머리글에 적힌 라이선스를 유지하며, 대부분 GPL 2.0 이상입니다. 그 파일들이 가리키는 GPL 2.0, LGPL 2.1, BSD 라이선스 전문은 [COPYING2](../../COPYING2), [COPYING.LIB](../../COPYING.LIB), [COPYING.ARM](../../COPYING.ARM)에 있습니다.

LiberNT에서 이루어진 변경의 저작권은 Copyright © 2026 LiberNT 기여자들에게 있습니다. 물려받은 코드와 서드 파티 코드는 작성자의 저작권 표시를 유지합니다. [CREDITS.md](../../CREDITS.md)를 참조하십시오.

LiberNT는 독립 프로젝트입니다. Microsoft와 제휴 관계가 없으며 Microsoft의 보증이나 후원을 받지 않습니다. Windows®는 Microsoft Corporation의 등록 상표입니다.

---

## 프로젝트 소개

LiberNT는 **LiberNT 기여자들**이 만들며, [Ahmed Arif](https://github.com/ahmedarif193)가 시작하고 유지 관리합니다.

- [LiberNT 조직 방문하기](https://github.com/LiberNT)
- [웹사이트](https://libernt.com)
- [최신 빌드 다운로드](https://github.com/LiberNT/LiberNT/releases)
- [Wiki](https://github.com/LiberNT/LiberNT/wiki)
- [Discord](https://discord.gg/BpcKg7FZ2) 또는 [Matrix](https://matrix.to/#/#libernt:matrix.org)에서 대화하기
