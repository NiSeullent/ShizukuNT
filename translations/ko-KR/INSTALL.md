<!--
 PROJECT:     ShizukuNT Korean translation
 LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 PURPOSE:     Korean translation of INSTALL.md (source commit 346eba3d855d)
 COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
-->
# LiberNT 설치하기

> [English](../../INSTALL.md) | [中文](../zh-CN/INSTALL.md) | [Français](../fr-FR/INSTALL.md) | [Español](../es-ES/INSTALL.md) | [日本語](../ja-JP/INSTALL.md) | **한국어** | [العربية](../ar-MA/INSTALL.md)

## 이미지 빌드하기

[LiberNT 직접 빌드하기](README.md#libernt-직접-빌드하기)에 따라 빌드 환경을 갖추고 빌드를 구성합니다. 그런 다음 출력 폴더(예: `output-Clang-amd64-debug`)에서 필요한 이미지를 빌드합니다.

| 명령 | 만들어지는 파일 | 용도 |
|---|---|---|
| `ninja bootcd` | `bootcd.iso` | CD 이미지로 LiberNT를 실행하거나, 그 이미지에서 LiberNT 설치 프로그램을 시작해 디스크에 LiberNT를 설치합니다. |
| `ninja liberntimg` | `LiberNT.img` | LiberNT가 이미 설치된 디스크로 부팅합니다. UEFI와 BIOS 모두에서 부팅됩니다. |
| `ninja liberntvhd` | `LiberNT.vhd`, `LiberNT.vhdx` | 같은 사전 설치 디스크를 가상 머신에서 사용합니다. VHD는 VirtualBox와 Hyper-V 1세대에서, VHDX는 VHD 디스크를 받지 않는 Hyper-V 2세대에서 씁니다. |

각 대상은 필요한 것을 모두 먼저 빌드하며, 파일은 출력 폴더에 기록됩니다.

## 설치 CD로 설치하기

가상 머신이나 PC에서 `bootcd.iso`로 부팅한 다음 LiberNT 설치 프로그램의 안내를 따릅니다. 설치 프로그램은 Windows 설치 프로그램과 같은 방식으로 드라이브를 나열하며, 파티션을 만들고, 삭제하고, 포맷하고, 축소하고, 확장할 수 있습니다. LiberNT를 NTFS에 설치하고 필요한 EFI 시스템 파티션을 만듭니다.

## 사전 설치 디스크 사용하기

`LiberNT.img`는 설치 과정이 필요 없습니다. QEMU 같은 가상 머신에서 시작하거나, USB 드라이브나 SD 카드에 쓴 다음 그 장치로 컴퓨터를 부팅합니다. VHD 디스크를 받는 가상 머신에는 `LiberNT.vhd`를, Hyper-V 2세대 가상 머신에는 보안 부팅을 끈 상태로 `LiberNT.vhdx`를 사용합니다.

사전 설치 디스크에도 LiberNT 설치 프로그램이 들어 있습니다. 실행 중인 시스템에서 다른 디스크에 LiberNT를 설치하려면 `C:\<아키텍처>\libernt.exe`(예: `C:\arm64\libernt.exe`)를 실행합니다.

## 도움 받기

문제가 생기면 아키텍처, 빌드 종류, 빌드한 커밋, 문제에 이르는 단계를 적어 [Issues](https://github.com/LiberNT/LiberNT/issues)에 보고하십시오. 안내서와 메모는 [Wiki](https://github.com/LiberNT/LiberNT/wiki)에 있으며, [Discord](https://discord.gg/BpcKg7FZ2)나 [Matrix](https://matrix.to/#/#libernt:matrix.org)에서 팀에 물어볼 수 있습니다.
