<!--
 PROJECT:     ShizukuNT Korean translation
 LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 PURPOSE:     Korean UI translation glossary and style guide
 COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
-->

# ShizukuNT 한국어 UI 번역 용어집과 스타일 가이드

이 문서는 ShizukuNT(LiberNT 기반)의 사용자 인터페이스를 한국어로 옮기는 모든 번역자가 따르는 기준입니다. 사람 번역자와 AI 도구에 똑같이 적용합니다. 대상은 리소스 스크립트(`.rc`), 메시지 테이블(`.mc`), INF의 `[Strings.0412]`, 설치 프로그램 문자열입니다. README 같은 문서 번역은 3장(문체)과 4장(표기)만 따릅니다.

## 목차

1. [범위와 적용 순서](#1-범위와-적용-순서)
2. [출처와 라이선스](#2-출처와-라이선스)
3. [문체](#3-문체)
4. [표기와 띄어쓰기](#4-표기와-띄어쓰기)
5. [액세스 키(&)](#5-액세스-키)
6. [문장 부호와 기호](#6-문장-부호와-기호)
7. [자리표시자와 서식 문자열](#7-자리표시자와-서식-문자열)
8. [번역하지 않는 것](#8-번역하지-않는-것)
9. [숫자, 날짜, 단위](#9-숫자-날짜-단위)
10. [리소스 파일 규칙](#10-리소스-파일-규칙)
11. [피해야 할 표기](#11-피해야-할-표기)
12. [용어집](#12-용어집)
13. [혼동하기 쉬운 낱말](#13-혼동하기-쉬운-낱말)
14. [검토 절차와 커밋](#14-검토-절차와-커밋)

## 1. 범위와 적용 순서

번역어를 정할 때는 다음 순서를 따릅니다.

1. 이 문서의 규칙과 12장의 용어집
2. 같은 모듈의 영어 원문: 뜻, 자리표시자, 액세스 키 문자, 줄바꿈
3. Wine 11.21 `po/ko.po`: 용어집에 없는 낱말을 정할 때 일관성을 확인하는 참고 자료
4. 트리에 이미 있는 한국어 번역: 참고만 합니다. 이 문서와 다르면 이 문서를 따르고, 기존 파일은 그 파일을 고칠 때 맞춥니다.

용어집에 없는 낱말이 둘 이상의 모듈에 나오면, 번역하기 전에 같은 변경 안에서 이 문서에 먼저 추가합니다. 용어를 바꿀 때는 근거(빈도, 혼동 사례)를 커밋 본문에 적고, 이미 번역된 리소스는 후속 변경에서 맞춥니다.

## 2. 출처와 라이선스

- **자체 작성**: 이 문서의 규칙과 번역어는 이 프로젝트에서 직접 정했습니다.
- **용어 선정 근거**: 트리의 영어 리소스 211개 파일(`*/lang/en-US.rc` 189개, Wine형 `lang/*_En.rc` 22개)에서 메뉴 항목, 팝업, 대화 상자 컨트롤, 캡션, `STRINGTABLE` 문자열을 뽑았습니다. 액세스 키 `&`, 끝의 `...`과 `:`, 탭 뒤의 가속 키를 떼고 대소문자를 무시해 같은 문자열이 나오는 모듈 수로 순위를 매겼습니다. 12장 표의 **모듈** 열이 그 수입니다. `—`는 문자열 전체로는 나오지 않지만 장치, 네트워크, 저장소, 보안 영역의 낱말로 자주 쓰여 함께 정한 용어입니다. 측정 기준은 커밋 `346eba3d855d`의 트리입니다.
- **Wine 11.21 `po/ko.po`** (LGPL-2.1-or-later, PO-Revision-Date 2025-12-14): 낱말 대응이 서로 맞는지만 대조했습니다. 12장 표의 **Wine ko.po** 열에서 `=`는 같은 번역, 낱말은 ko.po가 다르게 옮긴 것(여러 개면 빈도순으로 두 개), 빈칸은 ko.po에 같은 원문이 없다는 뜻입니다. 낱말 하나의 대응(예: File → 파일)은 일반 어휘이며, ko.po의 문장이나 메시지 번역은 이 문서에 옮기지 않았습니다(부분 클린룸).
- **사용 금지**: Microsoft Terminology Collection 같은 Microsoft 용어 모음(라이선스 미확인), Windows 한국어판의 UI 문자열과 MUI 리소스, 유출된 Microsoft 소스 코드([AGENTS.md](../../AGENTS.md#sources-of-truth)). Windows 11 한국어판은 블랙박스로 관찰한 사실(같은 개념을 같은 낱말로 부르는지)만 참고하고, 문구를 옮겨 적지 않습니다.
- **ko.po 문장의 재사용**: 번역된 리소스 파일은 원본 영어 파일의 라이선스를 따릅니다. 그래서 ko.po의 번역문은 영어 파일이 LGPL-2.1-or-later 또는 GPL 계열이라 LGPL 문장을 받아들일 수 있는 경우에만 쓰고, 커밋 본문에 출처(Wine 11.21 `po/ko.po`)를 적습니다. MIT, BSD 같은 허용형 라이선스 파일에는 ko.po 문장을 쓰지 말고 이 용어집으로 직접 번역합니다. 재사용한 문장도 이 문서의 문체(3장)와 용어에 맞게 고칩니다. ko.po는 메시지에 해요체(`-하세요`)를 쓰는 곳이 있으므로 합쇼체로 바꿉니다.

## 3. 문체

### 3.1 형태별 어미

| 대상 | 형태 | 예 |
| --- | --- | --- |
| 메뉴 명령, 단추, 탭, 열 머리글, 그룹 상자, 레이블 | 명사 또는 명사구(`-기` 명사형 포함) | 열기, 다른 이름으로 저장, 이름 바꾸기, 사용자 이름: |
| 확인란, 옵션 단추 | 명사구. 동작은 `~ 표시`, `~ 사용`, `~ 허용` 꼴 | 숨김 파일 표시, 프록시 서버 사용 |
| 설명문, 도움말, 상태 표시줄의 명령 설명 | 합쇼체 평서 `-합니다` | 이 창을 닫습니다. |
| 사용자에게 하는 지시 | 합쇼체 명령 `-하십시오` | 계속하려면 [다음]을 클릭하십시오. |
| 확인 질문 | `-하시겠습니까?` | 이 파일을 삭제하시겠습니까? |
| 오류와 결과 메시지 | `-습니다`, `-할 수 없습니다` | 파일을 찾을 수 없습니다. |
| 상태 값 | 명사, `-됨`, `~ 중` | 연결됨, 설치 중, 사용 안 함 |
| 마법사 첫 페이지와 끝 페이지 제목 | 명사구 | 하드웨어 추가 마법사 시작, 하드웨어 추가 마법사 완료 |

### 3.2 피할 문체

- 해요체(`-해요`, `-하세요`), 해라체(`-한다`, `-하라`), 반말은 쓰지 않습니다. 부탁은 `-해 주십시오`로 씁니다(Please wait → 잠시 기다려 주십시오).
- 영어의 `you`, `your`를 `당신`으로 옮기지 않습니다. 주어는 대개 생략합니다(Your computer must be restarted → 컴퓨터를 다시 시작해야 합니다).
- 번역 투를 피합니다: `~에 의해` 남용, `~를 가지고 있습니다`(→ `~이 있습니다`), 이중 피동 `되어지다`, 불필요한 `~하는 것이 가능합니다`(→ `~할 수 있습니다`).
- 영어 문장이 마침표로 끝나면 한국어도 마침표로 끝냅니다. 레이블과 단추에는 마침표를 붙이지 않습니다.

### 3.3 용어의 일관성

- 같은 대화 상자 안에서 같은 영어 낱말은 같은 한국어로 옮깁니다. 문맥상 다른 뜻이면 13장의 구별을 따릅니다.
- 다른 리소스에 있는 UI 요소 이름을 문장에서 가리킬 때는 그 요소의 번역과 글자 하나까지 같게 씁니다(예: 단추가 `마침`이면 설명도 `[마침]을 클릭하십시오`).

### 3.4 슬라이더 양 끝 레이블

슬라이더 양 끝에 붙는 짧은 레이블은 명사형 대신 부사형을 씁니다: 느리게/빠르게, 짧게/길게, 작게/크게, 적게/많게. 해상도처럼 정도를 나타내면 `낮게`/`높게`를 씁니다.

## 4. 표기와 띄어쓰기

- 한글 맞춤법, 표준어 규정, 외래어 표기법(국립국어원)을 따릅니다. 자주 틀리는 표기는 11장에 모았습니다.
- 띄어쓰기
  - 의존 명사는 띄어 씁니다: `할 수 없습니다`, `설치 중`, `사용 안 함`, `시작하는 중`.
  - 전문 용어와 합성어는 이 용어집에 적힌 형태로 통일합니다: 바로 가기, 미리 보기, 새로 고침, 다시 시작, 작업 표시줄, 대화 상자, 운영 체제, 붙여넣기, 덮어쓰기, 우선순위, 매개변수, 일련번호, 전화번호.
  - 아라비아 숫자 뒤의 한글 단위는 붙여 씁니다: `5초`, `16비트`, `항목 3개`. 라틴 문자 단위는 영어 원문의 띄어쓰기를 따릅니다: `512 MB`, `60 Hz`.
- 라틴 문자 그대로 쓰는 낱말
  - 제품과 상표 이름: ShizukuNT, LiberNT, ReactOS, Windows, Wine, Bluetooth, Wi-Fi, USB, Hyper-V, VirtualBox, QEMU
  - 약어: CPU, DNS, DHCP, IP, URL, PID, ID, BIOS, UEFI, NTFS, FAT32, KB, MB, GB
  - 키 이름: Ctrl, Alt, Shift, Enter, Esc, Tab, Caps Lock, Num Lock, Scroll Lock, Del, Ins, Home, End, PgUp, PgDn, F1–F12
- 라틴 문자 뒤의 조사는 읽는 소리로 고릅니다: `DNS를`, `USB를`, `Wi-Fi에`, `ID가`.
- 나열의 `and`는 쉼표와 `및`, `or`는 `또는`으로 씁니다: Date and Time → 날짜 및 시간, A, B and C → A, B 및 C.

## 5. 액세스 키(&)

### 5.1 형식

한국어 레이블 뒤에 영어 원문의 액세스 키 문자를 대문자로 괄호에 넣어 붙입니다. 괄호 앞에는 공백을 두지 않습니다.

| 영어 원문 | 한국어 |
| --- | --- |
| `&File` | `파일(&F)` |
| `Save &As...` | `다른 이름으로 저장(&A)...` |
| `&Name:` | `이름(&N):` |
| `Lar&ge Icons` | `큰 아이콘(&G)` |
| `&Copy\tCtrl+C` | `복사(&C)\tCtrl+C` |
| `< &Back` / `&Next >` | `< 뒤로(&B)` / `다음(&N) >` |

- `(&X)`는 레이블의 맨 끝에 두되, 끝의 `:`, `...`, `>`, `>>`와 탭 뒤 가속 키보다 앞에 둡니다.
- 문자는 영어 원문과 같은 것을 씁니다. 사용자 문서와 습관이 언어와 상관없이 같아지도록 하기 위해서입니다.
- 영어 원문에 `&`가 없으면 한국어에도 넣지 않습니다. 리터럴 앰퍼샌드 `&&`는 그대로 둡니다.
- 한글 낱자에는 액세스 키를 걸지 않습니다. 한글 입력 상태에서는 그 키가 눌리지 않습니다.

### 5.2 중복 금지

- 같은 메뉴 막대, 같은 팝업 메뉴 단계, 같은 대화 상자(정적 레이블을 포함한 모든 컨트롤) 안에서 액세스 키 문자는 하나씩만 씁니다.
- 영어 원문 자체에 중복이 있으면 한국어에서는 남는 문자 중 레이블 낱말과 관련 있는 문자로 고치고, 영어 원문의 결함은 따로 보고합니다(영어 수정은 별도 커밋).
- 리소스에서 읽어 메뉴나 레이블로 쓰는 `STRINGTABLE` 문자열도 같은 형식을 따릅니다.

## 6. 문장 부호와 기호

- 말줄임표는 원문 그대로 둡니다. `...`(마침표 세 개)는 `...`로, `…`(U+2026)은 `…`로 씁니다. 메뉴의 `...`은 대화 상자가 열린다는 표시이므로 지우지 않습니다.
- 레이블 끝의 `:`는 유지하고 앞에 공백을 넣지 않습니다.
- 괄호는 반각 `( )`를 씁니다. 한국어 뒤에 붙여 쓰되, 영어 원문이 괄호 앞을 띄웠으면 그대로 둡니다: `하이 컬러(16비트)`, `모든 파일 (*.*)`.
- 전각 문장 부호(`：`, `，`, `！`, `？`, `（`, `）`)와 전각 공백은 쓰지 않습니다.
- 따옴표
  - 영어 원문에 따옴표가 있으면 같은 종류를 같은 자리에 둡니다. RC 문자열 안의 큰따옴표는 `""`로 씁니다.
  - 문장에서 UI 요소 이름을 가리킬 때는 대괄호를 씁니다: `[다음]을 클릭하십시오`.
  - 원문에 따옴표가 없지만 어순이 바뀌어 파일 이름이나 값의 경계가 흐려지면 작은따옴표를 더할 수 있습니다: `'%s' 파일을 찾을 수 없습니다.`
- 원문의 `/` 구분은 유지합니다: 대/소문자 구분, 시간/날짜, 국가/지역.
- 느낌표는 원문에 있을 때만 씁니다.

## 7. 자리표시자와 서식 문자열

- 영어 원문의 자리표시자는 모두, 같은 횟수로, 철자 그대로 남깁니다: `%s`, `%d`, `%u`, `%ld`, `%lu`, `%I64d`, `%x`, `%04X`, `%-10s`, `%.2f`, `%c`, `%ls`, `%S`, `%%`, `%1`, `%2`, `%1!s!`, `%2!d!`, `{0}`, `[1]`(MSI).
- `printf` 계열 자리표시자(`%s`, `%d` 등)는 영어와 **같은 순서**를 지켜야 합니다. 이 계열에는 순서를 바꾸는 표기가 없으므로, 한국어 어순이 다르면 문장 구조를 바꿉니다.
  - `Page %d of %d` → `%d/%d페이지`
  - `%s free of %s` → `사용 가능 %s / 전체 %s`
- `FormatMessage`의 `%1`, `%2`와 `{0}`, `{1}`은 번호로 가리키므로 순서를 자유롭게 바꿀 수 있습니다.
- 이스케이프와 줄바꿈(`\n`, `\r\n`, `\t`, `\\`, `""`)은 영어와 같은 위치와 개수로 둡니다. 콘솔 메시지 끝의 `\n`과, 문자열을 이어 붙이는 데 쓰이는 앞뒤 공백도 그대로 둡니다.
- 문자열을 나누거나 합치지 않습니다. 리소스 ID와 순서는 영어 파일과 같아야 합니다.
- 자리표시자 바로 뒤에 조사를 붙이지 않도록 문장을 짭니다(`'%s' 파일을 열 수 없습니다.`). 피할 수 없으면 `을(를)`, `이(가)`, `은(는)`, `와(과)`, `(으)로`를 씁니다.
- 한국어에는 복수형이 없습니다. `%d file(s)` → `파일 %d개`. 영어가 단수와 복수 문자열을 따로 두면 둘 다 같은 구조로 옮깁니다: `1 item` → `항목 1개`, `%d items` → `항목 %d개`.

## 8. 번역하지 않는 것

- 파일 이름과 확장명, 경로(`C:\`, `%SystemRoot%`, `\Device\...`), 레지스트리 키와 값 이름, 환경 변수 이름
- 서비스와 드라이버의 내부 이름(표시 이름은 번역합니다), 모듈 이름, 명령 이름과 명령줄 스위치(`/?`, `/s`, `-a`, `--help`), API, 함수, 구조체, 상수, 창 클래스 이름, GUID와 CLSID, URL, 메일 주소, 장치 ID(`PCI\VEN_...`)
- **특수 폴더의 디스크 이름**: `dll/win32/shell32/lang/en-US.rc`의 "Shell folder path default values" 묶음(`IDS_PROGRAMS "Start Menu\\Programs"`, `IDS_PERSONAL "My Documents"`, `IDS_PROGRAM_FILES "Program Files"`, `IDS_APPDATA "Application Data"` 등)과 `dll/win32/userenv/lang`의 같은 성격 문자열은 디스크의 폴더 이름이 되므로 영어 그대로 둡니다. 사용자에게 보이는 표시 이름(12.4절의 문서, 사진, 음악 등)만 번역합니다.
- **내장 계정과 그룹 이름**(Administrator, Administrators, Users, Guests, Everyone, SYSTEM, LOCAL SERVICE, NETWORK SERVICE, Authenticated Users, Power Users, Backup Operators 등): 스크립트와 ACL 도구가 이름으로 찾는 식별자이므로 번역하지 않습니다. Windows 11 한국어판이 이 이름을 지역화하는지는 블랙박스 확인이 필요합니다(INFO_NEEDED: `LookupAccountSidW`로 S-1-5-32-544 등의 이름을 한국어 UI에서 조회). 확인 전까지 영어를 유지합니다.
- 제품과 상표 이름, 키 이름(4장)
- 법률 문구: 라이선스 전문(GPL 등), 저작권 표시 줄(`Copyright (C) 연도 이름`), `All rights reserved.` 같은 고지 문구
- 프로그램이 비교하거나 파싱하는 문자열: 응답 문자 집합(예: `cmd`의 `"YN"`, `"YNA"`)은 한글 입력 상태에서도 라틴 문자로 답할 수 있도록 번역하지 않습니다.
- 리소스 편집기가 넣은 자리 문자열(`Static`, `List1`, `Slider1`, `Tab1` 등)과 실행 중에 코드가 덮어쓰는 빈 레이블
- 어떤 문자열이 식별자로도 쓰이는지 분명하지 않으면 `LoadString` 호출부를 확인합니다. 경로, 비교, 레지스트리 이름에 쓰이면 번역하지 않고 커밋 본문에 그 이유를 적습니다.

## 9. 숫자, 날짜, 단위

- 날짜, 시간, 숫자, 통화 형식을 문자열에 직접 쓰지 않습니다. 형식은 로캘(`GetDateFormatEx`, `GetNumberFormatEx`)이 정합니다. 원문에 형식 그림(`MM/dd/yyyy`, `HH:mm`)이 있으면 형식 문자는 그대로 둡니다.
- 단위: 바이트, KB, MB, GB, TB, 비트, 픽셀, Hz, DPI, %는 12.13절을 따릅니다. 초, 분, 시간, 일은 숫자에 붙여 씁니다(`%d초`, `%d분`).
- 서수: `first` → `첫째`, `1st` → `1번째`.
- 표준 시간대 표시 이름의 `(UTC+09:00)` 같은 앞부분은 바꾸지 않습니다.

## 10. 리소스 파일 규칙

### 10.1 파일, 인코딩, 언어

- 파일 이름: ReactOS형 모듈은 `lang/ko-KR.rc`, Wine형 모듈은 그 모듈의 명명을 따라 `lang/<이름>_Ko.rc`입니다.
- 인코딩: UTF-8, BOM 없음, LF 줄바꿈. 트리의 한국어 리소스 52개(`ko-KR.rc` 29개, `*_Ko.rc` 23개)가 모두 이 형식입니다.
- `#pragma code_page(65001)`: 트리의 관례는 모듈 최상위 `.rc`(`rsrc.rc`, `resource.rc`, `<모듈>.rc`)에서 ASCII가 아닌 문자를 담은 언어 파일의 `#include`보다 **앞에** 한 번 두는 것입니다(`dll/win32/shell32/shell32.rc`처럼 영어 블록이 그보다 먼저 올 수는 있습니다). 언어 파일 안에는 쓰지 않습니다(한국어 파일 52개 중 0개). 새 한국어 파일을 연결할 모듈의 최상위 `.rc`에 이 줄이 없거나 한국어 `#include`보다 뒤에 있으면, 한국어 `#include`보다 앞(보통 첫 `#ifdef LANGUAGE_` 블록 앞)에 `#pragma code_page(65001)`을 둡니다.
- 언어 선언: 파일 머리글 다음 첫 문장으로 `LANGUAGE LANG_KOREAN, SUBLANG_DEFAULT`를 씁니다. 트리의 한국어 파일 52개가 모두 이 형식이며, SDK 헤더에서 `SUBLANG_DEFAULT`와 `SUBLANG_KOREAN`은 둘 다 `0x01`이라 결과 언어 ID는 `0x0412`로 같습니다.
- 머리글: 새 ReactOS형 파일은 [AGENTS.md](../../AGENTS.md#code-style)의 머리글을 씁니다. `LICENSE`는 번역 원본인 영어 파일의 라이선스입니다. 영어 파일에 머리글이 없으면(트리의 `en-US.rc` 189개 중 152개) 모듈 최상위 `.rc`나 주 소스 파일의 라이선스를 씁니다. `PURPOSE`는 `Korean resource file`, `COPYRIGHT`는 번역자입니다. Wine형 파일은 기존 `*_Ko.rc`처럼 영어 파일의 LGPL 머리글을 그대로 둡니다.

```c
/*
 * PROJECT:     LiberNT Shell32
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     Korean resource file
 * COPYRIGHT:   Copyright 2026 Hong Gildong <gildong@example.com>
 */

LANGUAGE LANG_KOREAN, SUBLANG_DEFAULT
```

- 내용: 영어 파일을 복사해 문자열만 번역합니다. 리소스 종류와 ID, 컨트롤 수와 ID, `STYLE`과 `EXSTYLE`, 메뉴 구조, `STRINGTABLE` 항목 수가 영어 파일과 같아야 합니다.

### 10.2 대화 상자 글꼴

- `FONT` 문은 영어 파일과 **같은 크기, 같은 이름, 같은 추가 인자**로 둡니다. 영어 파일의 대부분은 `FONT 8, "MS Shell Dlg"`입니다.
- `"굴림"`, `"Gulim"`, `"돋움"`, `"맑은 고딕"`, `"Malgun Gothic"` 같은 한국어 글꼴 이름을 하드코딩하지 않습니다.
  - 대화 상자 단위(DLU)는 글꼴 크기에서 계산되므로 크기를 바꾸면 모든 컨트롤 배치가 영어와 달라집니다.
  - `MS Shell Dlg`는 글꼴 치환(FontSubstitutes)으로 한글을 그릴 수 있는 글꼴에 연결됩니다. 한국어 설치에서 이 치환을 쓰는 값은 `base/setup/lib/muifonts.h`의 `KoreanFonts[]`이며, 치환 대상 글꼴은 WP 08-1에서 정합니다.
  - 트리의 기존 한국어 대화 상자 중 `FONT 9, "굴림"` 계열 139개는 이 규칙에 맞게 바꿉니다(WP 08-4).
- 좌표는 영어와 같게 둡니다. 한국어가 잘리면 먼저 용어집 안에서 더 짧은 표현을 고르고, 그래도 잘리면 다른 컨트롤과 겹치지 않는 범위에서 그 컨트롤만 넓힙니다. ID와 스타일은 바꾸지 않습니다.

### 10.3 모듈에 연결하기

모듈 최상위 `.rc`에서 `ja-JP` 블록을 찾아 바로 뒤에 같은 모양으로 `ko-KR` 블록을 넣습니다. 언어 블록은 파일 이름 순서(`ja-JP` 다음 `ko-KR`)로 놓여 있습니다.

```c
#ifdef LANGUAGE_JA_JP
    #include "lang/ja-JP.rc"
#endif
#ifdef LANGUAGE_KO_KR
    #include "lang/ko-KR.rc"
#endif
```

Wine형 모듈은 `#include "lang/<이름>_Ja.rc"` 블록 뒤에 `#include "lang/<이름>_Ko.rc"` 블록을 넣습니다(예: `dll/win32/comctl32/rsrc.rc`).

- `LANGUAGE_KO_KR`은 `sdk/cmake/localization.cmake`가 `I18N_LANG`이 `ko-KR`이거나 `all`(기본값)일 때 정의합니다.
- 모듈의 `CMakeLists.txt`가 언어 파일을 이름으로 나열하면(예: `add_rc_deps(... lang/en-US.rc ...)`) 한국어 파일도 같은 목록에 더합니다. `file(GLOB ... lang/*.rc)`를 쓰는 모듈은 따로 고칠 것이 없습니다.
- 확인: 기본 `I18N_LANG=all`로 그 모듈을 amd64, i386, arm64에서 빌드합니다(AGENTS.md).

### 10.4 메시지 테이블(.mc)

- `LanguageNames`에 `Korean=0x412:MSG00412`를 기존 `Japanese=0x411:MSG00411`과 같은 형식으로 더하고, 각 메시지에 `Language=Korean` 블록을 둡니다.
- 삽입 인자(`%1`, `%2!d!`, `%n`, `%r`, `%0`), 줄 구조, 블록 끝의 `.` 줄은 영어 블록과 같게 둡니다.
- 원본 파일의 인코딩과 줄바꿈을 그대로 따릅니다(예: `sdk/include/reactos/mc/errcodes.mc`는 UTF-8, CRLF).

### 10.5 INF

- `[Strings]`와 같은 키 집합으로 `[Strings.0412]` 절을 만들고 표시 문자열만 번역합니다. 절 이름, 키, 경로, 서비스 이름, 하드웨어 ID는 번역하지 않습니다.
- `[Strings.0411]` 절 바로 다음에 둡니다.
- 원본 파일의 인코딩과 줄바꿈을 그대로 따릅니다. `media/inf`의 원본은 UTF-8(`shortcuts.inf`처럼 BOM이 있는 파일은 BOM 유지)이며, `add_driver_inf`로 등록된 INF는 빌드 때 UTF-16LE로 바뀝니다.

## 11. 피해야 할 표기

점검 도구(WP 08-2)가 경고할 표기입니다. 왼쪽 열은 리소스에 쓰지 않습니다.

| 피할 표기 | 쓸 표기 | 비고 |
| --- | --- | --- |
| 데스크탑 | 데스크톱 | 외래어 표기법 |
| 디렉토리 | 디렉터리 | 외래어 표기법 |
| 라이센스 | 라이선스 | 외래어 표기법 |
| 메세지 | 메시지 | 외래어 표기법 |
| 컨텐츠, 컨텐트 | 콘텐츠 | 외래어 표기법 |
| 억세스, 접근(Access 번역어) | 액세스 | 용어 통일 |
| 핫스팟 | 핫스폿 | 외래어 표기법 |
| 윈도우(창의 뜻) | 창 | 제품 이름 Windows는 라틴 문자로 |
| 다이얼로그, 대화상자 | 대화 상자 | |
| 셋업 | 설치, 설치 프로그램 | |
| 프로퍼티, 등록 정보 | 속성 | |
| 디폴트 | 기본값 | |
| 리스트 | 목록 | |
| 아이템 | 항목 | |
| 에러 | 오류 | |
| 디바이스 | 장치 | |
| 패스워드, 비밀번호 | 암호 | 용어 통일 |
| 유저 | 사용자 | |
| 폰트 | 글꼴 | |
| 파라미터, 매개 변수 | 매개변수 | |
| 우선 순위 | 우선순위 | |
| 일련 번호, 시리얼 번호 | 일련번호 | |
| 사용자정의, 사용자 정의 | 사용자 지정 | Custom, Customize |
| 재부팅 | 다시 시작 | |
| 어플리케이션, 어플, 애플리케이션 | 응용 프로그램, 앱 | Application, App |
| 바탕화면 | 바탕 화면 | |
| 아니오(대답) | 아니요 | 표준어 |
| 클릭하세요, 하세요 | 클릭하십시오, 하십시오 | 3장 문체 |
| 되어집니다 | 됩니다 | 이중 피동 |
| 할수, 될수 | 할 수, 될 수 | 의존 명사 띄어쓰기 |
| 안됩니다(부정) | 안 됩니다 | 부정 부사 띄어쓰기 |
| 굴림, Gulim, 돋움, 맑은 고딕(FONT 문) | MS Shell Dlg | 10.2절 |

## 12. 용어집

열의 뜻은 2장에 있습니다. **English** 열은 액세스 키, 끝의 `...`, `:`를 뗀 형태이며, 한국어에 붙이는 방법은 5장과 6장을 따릅니다. 같은 영어 낱말이 문맥에 따라 다르게 옮겨지는 경우는 **비고**와 13장을 봅니다. 모두 914개 항목이며, 그중 791개는 트리의 영어 리소스에 문자열 전체로 나옵니다.

### 12.1 공용 단추와 대화 상자 명령

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| OK | 확인 |  | = | 78 |
| Cancel | 취소 |  | = | 80 |
| Apply | 적용 |  | = | 4 |
| Close | 닫기 |  | = | 22 |
| Yes | 예 |  | = | 22 |
| No | 아니요 | '아니오'로 쓰지 않습니다. | = | 23 |
| Yes to All | 모두 예 |  | = | 2 |
| No to All | 모두 아니요 |  |  | — |
| Help | 도움말 |  | = | 38 |
| Retry | 다시 시도 |  | 재시도 | 4 |
| Ignore | 무시 |  | = | 4 |
| Abort | 중단 |  | = | 3 |
| Continue | 계속 |  | = | 1 |
| Back | 뒤로 |  | = | 6 |
| &lt; Back | &lt; 뒤로 |  | = | 2 |
| Next | 다음 |  | = | 6 |
| Next &gt; | 다음 &gt; |  | = | 2 |
| Finish | 마침 |  | 종료 | 2 |
| Browse | 찾아보기 |  | = | 17 |
| Default | 기본값 | 목록에서 '기본 항목'을 뜻하면 '기본' | = | 5 |
| (Default) | (기본값) |  | (기본) | 3 |
| Restore Defaults | 기본값 복원 |  |  | 6 |
| Use Default | 기본값 사용 |  |  | 2 |
| Set as Default | 기본값으로 설정 |  |  | 3 |
| Reset | 재설정 | 처음 상태로 되돌리는 뜻이 분명하면 '초기화' | = | 3 |
| Add | 추가 |  | = | 14 |
| Remove | 제거 |  | = | 17 |
| Delete | 삭제 |  | = | 23 |
| New | 새로 만들기 | 명사 앞에서는 '새': New Folder → 새 폴더 | = | 19 |
| Change | 변경 |  | 바꾸기 | 4 |
| Modify | 수정 |  | = | 3 |
| Rename | 이름 바꾸기 |  | = | 9 |
| Select | 선택 |  | = | 6 |
| Select All | 모두 선택 |  | = | 7 |
| Clear | 지우기 |  | = | 4 |
| Show | 표시 |  | = | 2 |
| Hide | 숨기기 |  | = | — |
| More | 자세히 | 확장 단추 'More >>' → '자세히 >>'. 슬라이더 양 끝의 'Less/More'는 '적게/많게' | 더 | 4 |
| Less | 간단히 | 확장 단추를 접는 뜻. 슬라이더 끝은 '적게' |  | 3 |
| Details | 자세히 | 보기 방식. 정보 묶음(탭·창 제목)은 '세부 정보' | = | 14 |
| Options | 옵션 |  | = | 16 |
| Settings | 설정 |  | = | 13 |
| Properties | 속성 | '등록 정보'로 쓰지 않습니다. | = | 24 |
| Advanced | 고급 |  | = | 20 |
| General | 일반 |  | = | 23 |
| Customize | 사용자 지정 |  |  | 7 |
| Custom | 사용자 지정 |  | 사용자정의 / 사용자 정의 | 4 |
| Preview | 미리 보기 |  | = | 7 |
| Run | 실행 |  | = | 5 |
| Open | 열기 |  | = | 15 |
| Save | 저장 |  | = | 10 |
| Save As | 다른 이름으로 저장 |  | = | 16 |
| Print | 인쇄 |  | = | 8 |
| Find | 찾기 |  | = | 7 |
| Search | 검색 |  | = | 6 |
| Replace | 바꾸기 |  | = | 4 |
| Move Up | 위로 이동 |  | = | 8 |
| Move Down | 아래로 이동 |  | = | 8 |
| Up | 위로 |  | 위 | 5 |
| Down | 아래로 |  | 아래 | 4 |
| Install | 설치 |  | = | 8 |
| Uninstall | 제거 |  |  | 6 |
| Enable | 사용 |  | = | 7 |
| Disable | 사용 안 함 |  | 사용 중지 | 7 |
| Start | 시작 |  | = | 8 |
| Stop | 중지 |  | = | 11 |
| Pause | 일시 중지 |  |  | 3 |
| Resume | 계속 | 'Restart'(다시 시작)와 구별 |  | 2 |
| Restart | 다시 시작 |  | = | 7 |
| Connect | 연결 |  | = | 5 |
| Disconnect | 연결 끊기 |  |  | 5 |
| Configure | 구성 |  | 설정 | 4 |
| Preferences | 기본 설정 |  |  | 3 |
| Advanced Settings | 고급 설정 |  |  | 4 |
| Advanced Options | 고급 옵션 |  | 추가 옵션 | 3 |
| Change Icon | 아이콘 변경 |  | = | 5 |
| Refresh | 새로 고침 |  | = | 12 |
| Import | 가져오기 |  | 불러오기 | 3 |
| Export | 내보내기 |  | = | 4 |
| Test | 테스트 |  |  | 3 |

### 12.2 메뉴 명령

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| File | 파일 |  | = | 27 |
| Edit | 편집 |  | = | 20 |
| View | 보기 |  | = | 16 |
| Insert | 삽입 |  | = | 1 |
| Format | 서식 | 텍스트·문서 서식. 디스크는 '포맷', 데이터 형식은 '형식' | = | 6 |
| Tools | 도구 |  |  | 5 |
| Window | 창 |  | = | 4 |
| Favorites | 즐겨찾기 |  | = | 8 |
| Action | 동작 | MMC의 '동작' 메뉴 | 행동 | 8 |
| New Window | 새 창 |  | = | 2 |
| Page Setup | 페이지 설정 |  | = | 3 |
| Print Preview | 인쇄 미리 보기 |  | 인쇄 미리보기 | 2 |
| Exit | 끝내기 | 프로그램을 마치는 메뉴 명령 | = | 27 |
| Quit | 끝내기 |  | = | — |
| Undo | 실행 취소 |  | = | 8 |
| Redo | 다시 실행 |  | = | 3 |
| Cut | 잘라내기 |  | = | 6 |
| Copy | 복사 |  | = | 14 |
| Paste | 붙여넣기 |  | = | 9 |
| Paste Shortcut | 바로 가기 붙여넣기 |  |  | 1 |
| Invert Selection | 선택 영역 반전 |  |  | 2 |
| Find Next | 다음 찾기 |  | = | 5 |
| Find What | 찾을 내용 |  | = | 3 |
| Replace With | 바꿀 내용 |  | = | 1 |
| Replace All | 모두 바꾸기 |  | = | 1 |
| Match Case | 대/소문자 구분 |  | = | 3 |
| Match Whole Word Only | 단어 단위로 일치 |  | 단어 단위로 | 2 |
| Go To | 이동 |  | = | 2 |
| Time/Date | 시간/날짜 |  | 날짜/시간 | 1 |
| Word Wrap | 자동 줄 바꿈 |  | = | 1 |
| Font | 글꼴 |  | = | 7 |
| Toolbar | 도구 모음 |  | = | 2 |
| Toolbars | 도구 모음 |  | = | 3 |
| Status Bar | 상태 표시줄 |  | = | 9 |
| Large Icons | 큰 아이콘 |  | = | 6 |
| Small Icons | 작은 아이콘 |  | = | 7 |
| List | 목록 |  | = | 7 |
| Tiles | 타일 |  |  | — |
| Arrange Icons | 아이콘 정렬 |  | = | 3 |
| Arrange Icons By | 아이콘 정렬 기준 |  |  | 1 |
| Auto Arrange | 자동 정렬 |  | = | 1 |
| Align to Grid | 눈금에 맞춤 |  |  | 1 |
| Sort By | 정렬 기준 |  |  | 1 |
| Ascending | 오름차순 |  |  | — |
| Descending | 내림차순 |  |  | — |
| Zoom In | 확대 |  | = | — |
| Zoom Out | 축소 |  | = | — |
| Full Screen | 전체 화면 |  | = | 2 |
| Cascade | 계단식 배열 |  | = | 4 |
| Tile Horizontally | 가로 바둑판식 배열 |  | = | 4 |
| Tile Vertically | 세로 바둑판식 배열 |  | = | 3 |
| Minimize | 최소화 |  | = | 4 |
| Maximize | 최대화 |  | = | 3 |
| Restore | 복원 |  | = | 5 |
| Move | 이동 |  | = | 3 |
| Size | 크기 | 창 시스템 메뉴의 명령은 '크기 조정' | = | 13 |
| Always On Top | 항상 위 |  | = | 2 |
| Help Topics | 도움말 항목 |  | = | 7 |
| Contents | 목차 | 도움말의 목차. 파일·폴더의 내용은 '내용' | = | 2 |
| Index | 색인 |  | 인덱스 / 목차 | — |
| About | 정보 | 'About %s' → '%s 정보' | = | 28 |
| About %s | %s 정보 |  | = | 2 |
| Add to Favorites | 즐겨찾기에 추가 |  | 즐겨찾기 추가 / 즐겨 찾기에 추가 | 4 |
| Organize Favorites | 즐겨찾기 정리 |  |  | 2 |

### 12.3 창과 컨트롤

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Dialog | 대화 상자 | '다이얼로그', '대화상자'로 쓰지 않습니다. | 대화상자 | 2 |
| Message Box | 메시지 상자 |  |  | 2 |
| Button | 단추 |  |  | 1 |
| Check box | 확인란 |  |  | — |
| Radio button | 옵션 단추 |  | 라디오 버튼 | 1 |
| List box | 목록 상자 |  |  | — |
| Combo box | 콤보 상자 |  | = | 1 |
| Text box | 텍스트 상자 |  |  | — |
| Tab | 탭 |  |  | — |
| Slider | 슬라이더 |  | = | 1 |
| Scroll bar | 스크롤 막대 |  | 스크롤바 | 1 |
| Title bar | 제목 표시줄 |  | 제목바 | 1 |
| Menu | 메뉴 |  |  | 1 |
| Menu bar | 메뉴 모음 |  | 메뉴바 | 1 |
| Context menu | 컨텍스트 메뉴 | 바로 가기(Shortcut)와 헷갈리지 않도록 |  | — |
| Tooltip | 도구 설명 |  |  | — |
| Progress bar | 진행률 표시줄 |  | = | 1 |
| Icon | 아이콘 |  |  | 2 |
| Cursor | 커서 |  | = | 2 |
| Pointer | 포인터 |  |  | 1 |
| Border | 테두리 |  | 가장자리 | 2 |
| Background | 배경 |  | = | 2 |
| Separator | 구분선 |  | 구분 기호 | 2 |
| Wizard | 마법사 |  | = | 1 |
| Page | 페이지 |  | = | — |
| Item | 항목 |  | 목록 | 1 |
| Items | 항목 | '%d items' → '항목 %d개' |  | — |
| Selection | 선택 영역 |  | = | 2 |
| Label | 레이블 |  | = | 2 |
| Title | 제목 |  | = | 2 |
| Caption | 캡션 |  |  | — |
| Expand | 펼치기 | 트리 노드. 파티션·볼륨의 'Extend'는 '확장' | 확장 | 3 |
| Collapse | 접기 |  | 축소 | 2 |
| Closes this window. | 이 창을 닫습니다. | 시스템 메뉴 명령 설명 |  | 3 |
| Moves this window. | 이 창을 이동합니다. |  |  | 3 |
| Resizes this window. | 이 창의 크기를 조정합니다. |  |  | 3 |

### 12.4 셸과 바탕 화면

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Desktop | 바탕 화면 | 원격·가상 데스크톱처럼 컴퓨터 환경을 뜻하면 '데스크톱' | = | 7 |
| Start Menu | 시작 메뉴 |  | = | 4 |
| All Programs | 모든 프로그램 |  |  | — |
| Programs | 프로그램 |  | = | 6 |
| Program | 프로그램 |  | = | 2 |
| Application | 응용 프로그램 |  | 프로그램 | 2 |
| Applications | 응용 프로그램 |  | 프로그램 | 2 |
| Apps | 앱 |  |  | 1 |
| Taskbar | 작업 표시줄 |  |  | 2 |
| Lock the taskbar | 작업 표시줄 잠금 |  |  | 2 |
| Taskbar and Start Menu | 작업 표시줄 및 시작 메뉴 |  |  | 2 |
| Notification | 알림 |  |  | 2 |
| Notifications | 알림 |  |  | 1 |
| Notification area | 알림 영역 |  |  | 1 |
| Quick settings | 빠른 설정 |  |  | 1 |
| Quick Launch | 빠른 실행 |  |  | 1 |
| Show desktop | 바탕 화면 보기 |  |  | — |
| Task View | 작업 보기 |  |  | — |
| Virtual desktop | 가상 데스크톱 |  | = | — |
| Clock | 시계 |  | = | 3 |
| Calendar | 달력 |  |  | 2 |
| Recycle Bin | 휴지통 |  |  | 3 |
| Empty Recycle Bin | 휴지통 비우기 |  |  | 2 |
| Original location | 원래 위치 |  | = | 2 |
| Date deleted | 삭제한 날짜 |  | 삭제된 날짜 | 2 |
| This PC | 내 PC |  |  | 2 |
| My Computer | 내 컴퓨터 |  | = | 2 |
| Computer | 컴퓨터 |  |  | 2 |
| Documents | 문서 | 표시 이름. 디스크 폴더 이름은 번역하지 않습니다(8장). | = | 4 |
| My Documents | 내 문서 | 표시 이름 |  | 4 |
| Pictures | 사진 | 표시 이름 | 그림 | 1 |
| Music | 음악 | 표시 이름 | = | 2 |
| Videos | 동영상 | 표시 이름 | 비디오 | — |
| Downloads | 다운로드 | 표시 이름 | = | — |
| Recent | 최근 항목 |  |  | 3 |
| Control Panel | 제어판 |  | = | 4 |
| Administrative Tools | 관리 도구 |  | = | 2 |
| Fonts | 글꼴 |  | = | 3 |
| Printers | 프린터 |  |  | 3 |
| Startup | 시작 프로그램 |  | = | 3 |
| Send To | 보내기 |  |  | 1 |
| Templates | 템플릿 |  | Templates | 2 |
| File Explorer | 파일 탐색기 |  |  | 2 |
| LiberNT Explorer | LiberNT 탐색기 | 제품 이름 'LiberNT'는 번역하지 않습니다. |  | 3 |
| Folder Options | 폴더 옵션 |  |  | 2 |
| Shortcut | 바로 가기 |  |  | 1 |
| Create Shortcut | 바로 가기 만들기 |  | = | 2 |
| Create Link | 링크 만들기 |  | = | 3 |
| Shortcut key | 바로 가기 키 |  |  | 2 |
| Target | 대상 |  |  | 2 |
| Start in | 시작 위치 |  |  | 1 |
| Open file location | 파일 위치 열기 |  |  | 3 |
| Open With | 연결 프로그램 |  | 열기 | 1 |
| Pin to taskbar | 작업 표시줄에 고정 |  |  | — |
| Unpin | 고정 해제 |  |  | — |
| Pinned | 고정됨 |  |  | 1 |
| Run as | 다른 사용자로 실행 |  |  | 2 |
| Run as administrator | 관리자 권한으로 실행 |  | = | — |
| Search Results | 검색 결과 |  |  | 2 |
| Address bar | 주소 표시줄 |  | = | 2 |
| Navigation pane | 탐색 창 |  |  | — |
| Preview pane | 미리 보기 창 |  |  | — |
| Up One Level | 한 단계 위로 |  | = | 3 |
| Forward | 앞으로 |  | = | 3 |
| Home | 홈 |  | = | 1 |
| Wallpaper | 배경 화면 |  |  | — |
| Screen Saver | 화면 보호기 |  |  | 2 |
| Theme | 테마 |  | = | 1 |
| Themes | 테마 |  |  | 2 |
| Appearance | 모양 |  | 모양새 | 3 |
| Personalization | 개인 설정 |  |  | 1 |
| Visual Effects | 시각 효과 |  |  | 2 |
| Window shadows | 창 그림자 |  |  | 2 |
| Show window contents while dragging | 끄는 동안 창 내용 표시 |  |  | 3 |
| Lock | 잠금 |  |  | 1 |
| Sign out | 로그아웃 |  |  | 1 |
| Log off | 로그오프 |  |  | — |
| Log Off %s | %s 로그오프 |  |  | 2 |
| Switch User | 사용자 전환 |  |  | 1 |
| Shut down | 시스템 종료 |  |  | 5 |
| Sleep | 절전 |  |  | 2 |
| Hibernate | 최대 절전 |  |  | 4 |
| Stand by | 대기 |  |  | 2 |

### 12.5 파일, 폴더, 저장소

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Folder | 폴더 |  | = | 3 |
| Folders | 폴더 |  | = | 3 |
| New Folder | 새 폴더 |  | = | 2 |
| Name | 이름 |  | = | 23 |
| %s File | %s 파일 |  |  | 3 |
| File version | 파일 버전 |  |  | 3 |
| System Folder | 시스템 폴더 |  |  | 3 |
| Network Location | 네트워크 위치 |  |  | 3 |
| Synchronize | 동기화 |  |  | 3 |
| Directory | 디렉터리 | '디렉토리'로 쓰지 않습니다. |  | — |
| Path | 경로 |  | = | 4 |
| File name | 파일 이름 | 'Filename'도 같습니다. | = | 2 |
| File extension | 파일 확장명 |  |  | 3 |
| Type | 형식 | 파일·데이터의 형식. 분류는 '유형'(장치 유형, 시작 유형) | = | 15 |
| File Types | 파일 형식 |  |  | 2 |
| Size on disk | 디스크 할당 크기 |  |  | 3 |
| Total size | 전체 크기 |  |  | 5 |
| Contains | 포함 |  |  | 1 |
| Location | 위치 |  | = | 6 |
| Date modified | 수정한 날짜 |  |  | 3 |
| Created | 만든 날짜 |  |  | 3 |
| Modified | 수정한 날짜 |  | 수정됨 | 6 |
| Accessed | 액세스한 날짜 |  |  | 2 |
| Attributes | 특성 | 'Properties'(속성)와 구별 | 속성 | 8 |
| Read Only | 읽기 전용 | 'Read-only'도 같습니다. | = | 3 |
| Hidden | 숨김 |  | = | 2 |
| Archive | 보관 |  | 아카이브 | 1 |
| Compressed | 압축됨 |  | = | — |
| Encrypted | 암호화됨 |  |  | 1 |
| Compression | 압축 |  |  | 2 |
| Extract | 압축 풀기 |  |  | 2 |
| Owner | 소유자 |  | = | 4 |
| Untitled | 제목 없음 |  | = | 2 |
| Text Document | 텍스트 문서 |  | = | 2 |
| All files (\*.\*) | 모든 파일 (\*.\*) |  | = | 4 |
| Text files (\*.txt) | 텍스트 파일 (\*.txt) |  | = | 2 |
| Document | 문서 |  | = | 2 |
| Overwrite | 덮어쓰기 |  |  | — |
| Confirm File Replace | 파일 바꾸기 확인 |  |  | 2 |
| Confirm Delete | 삭제 확인 |  |  | 2 |
| Copy To | 다음 위치로 복사 |  |  | 2 |
| Load | 불러오기 | 파일·구성의 '불러오기'. 드라이버·모듈은 '로드' |  | 1 |
| Download | 다운로드 |  |  | — |
| Drive | 드라이브 |  |  | 3 |
| Drives | 드라이브 |  | = | 2 |
| Disk | 디스크 |  | = | — |
| Local Disk | 로컬 디스크 |  |  | 3 |
| Removable Disk | 이동식 디스크 |  |  | 2 |
| CD Drive | CD 드라이브 |  |  | 2 |
| 3.5 Floppy | 3.5인치 플로피 디스크 |  |  | 2 |
| Network Drive | 네트워크 드라이브 |  |  | — |
| Map Network Drive | 네트워크 드라이브 연결 |  |  | 1 |
| Disconnect Network Drive | 네트워크 드라이브 연결 끊기 |  | = | 2 |
| Volume | 볼륨 | 소리 크기와 디스크 볼륨 모두 '볼륨' | = | 6 |
| Volume Label | 볼륨 레이블 |  |  | 2 |
| Volume Serial Number | 볼륨 일련번호 |  |  | — |
| Partition | 파티션 |  |  | 2 |
| Primary | 기본 | 파티션 종류는 '주'(주 파티션) |  | 3 |
| File System | 파일 시스템 |  |  | 5 |
| Quick Format | 빠른 포맷 |  |  | 1 |
| Allocation unit size | 할당 단위 크기 |  |  | 1 |
| Total clusters | 전체 클러스터 |  |  | 2 |
| Sector Size | 섹터 크기 |  |  | 2 |
| Capacity | 용량 |  |  | 3 |
| Free space | 사용 가능한 공간 |  |  | 4 |
| Used space | 사용 중인 공간 |  |  | 1 |
| Space available | 사용 가능한 공간 |  |  | 2 |
| Eject | 꺼내기 |  |  | 3 |
| Mount | 마운트 |  |  | 2 |
| Shrink | 축소 |  |  | 1 |
| Extend | 확장 |  |  | 1 |
| Disk Management | 디스크 관리 |  |  | 2 |
| Storage | 저장소 |  |  | 1 |
| Backup | 백업 |  |  | 2 |
| Recovery | 복구 |  |  | 2 |
| Repair | 복구 |  |  | 2 |
| Share | 공유 |  |  | — |
| Shared | 공유됨 |  |  | 2 |

### 12.6 설정, 제어판, 표시

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| System | 시스템 |  | = | 8 |
| Display | 디스플레이 | 화면 장치·설정. '표시하다'는 '표시' |  | 6 |
| Display settings | 디스플레이 설정 |  | = | 2 |
| Screen | 화면 |  |  | — |
| Screen resolution | 화면 해상도 |  | = | — |
| Resolution | 해상도 |  | = | 1 |
| Refresh rate | 새로 고침 빈도 |  |  | 1 |
| Monitor | 모니터 |  |  | 2 |
| Orientation | 방향 |  | = | 3 |
| Landscape | 가로 |  | = | 4 |
| Portrait | 세로 |  | = | 4 |
| Scale | 배율 |  |  | 2 |
| Stretch | 늘이기 | 배경 화면 위치 |  | 3 |
| Tile | 바둑판식 배열 | 배경 화면 위치 |  | 3 |
| Ratio | 비율 |  |  | 3 |
| Custom size | 사용자 지정 크기 |  |  | 3 |
| Standard | 표준 |  |  | 6 |
| Element | 요소 |  |  | 3 |
| Video | 비디오 | 장치·형식. 사용자 폴더 표시 이름은 '동영상' | = | 3 |
| Brightness | 밝기 |  |  | 1 |
| Color | 색 |  | 색상 | 3 |
| Colors | 색 |  |  | 4 |
| Color scheme | 색 구성표 |  |  | 2 |
| Color Palette | 색상표 |  |  | 2 |
| Custom colors | 사용자 지정 색 |  | 사용자 정의 색상 | 1 |
| 16 Colors | 16색 |  |  | 2 |
| 256 Colors | 256색 |  |  | 3 |
| High Color (16 bit) | 하이 컬러(16비트) |  |  | 3 |
| True Color (24 bit) | 트루 컬러(24비트) |  |  | 3 |
| True Color (32 bit) | 트루 컬러(32비트) |  |  | 2 |
| Sound | 소리 |  | 사운드 | 6 |
| Sound scheme | 소리 구성표 |  |  | 2 |
| Audio | 오디오 |  | = | 4 |
| Mouse | 마우스 |  |  | 3 |
| Pointer speed | 포인터 속도 |  |  | 2 |
| Double-click speed | 두 번 클릭 속도 |  |  | 2 |
| Click | 클릭 |  |  | — |
| Double-click | 두 번 클릭 |  |  | — |
| Drag | 끌기 |  |  | — |
| Keyboard | 키보드 |  |  | 6 |
| Key | 키 | 키보드의 키와 레지스트리 키 모두 '키' | = | 2 |
| Repeat delay | 재입력 지연 시간 |  |  | 3 |
| Repeat rate | 반복 속도 |  |  | 3 |
| Character repeat | 문자 반복 |  |  | 2 |
| Cursor blink rate | 커서 깜박임 속도 |  |  | 2 |
| Keyboard layout | 키보드 레이아웃 |  |  | 1 |
| Input language | 입력 언어 |  |  | 1 |
| Input method | 입력기 |  |  | — |
| Language bar | 언어 표시줄 |  |  | 2 |
| Accessibility | 접근성 |  |  | 2 |
| Magnifier | 돋보기 |  |  | 3 |
| On-Screen Keyboard | 화상 키보드 |  |  | 3 |
| StickyKeys | 고정 키 |  |  | 2 |
| FilterKeys | 필터 키 |  |  | 2 |
| ToggleKeys | 토글 키 |  |  | 1 |
| MouseKeys | 마우스 키 |  |  | 2 |
| High contrast | 고대비 |  |  | 1 |
| Invert colors | 색 반전 |  |  | 2 |
| Date | 날짜 |  | = | 7 |
| Time | 시간 |  |  | 5 |
| Date and Time | 날짜 및 시간 |  | = | 1 |
| Time zone | 표준 시간대 |  |  | 5 |
| Daylight saving time | 일광 절약 시간 |  |  | — |
| Region | 지역 |  |  | 1 |
| Country/region | 국가/지역 |  | = | 3 |
| Language | 언어 |  |  | 5 |
| Currency | 통화 |  |  | 2 |
| Number | 숫자 | 차례나 식별 번호를 뜻하면 '번호' |  | 1 |
| Short date | 간단한 날짜 |  |  | 2 |
| Long date | 자세한 날짜 |  |  | 2 |
| Measurement system | 단위계 |  |  | 2 |
| Metric | 미터법 |  |  | 3 |
| Power | 전원 |  |  | 4 |
| Power Options | 전원 옵션 |  |  | 1 |
| Power plan | 전원 관리 계획 |  |  | — |
| Battery | 배터리 |  |  | — |
| Plugged in | 전원 연결됨 |  |  | 2 |
| Charging | 충전 중 |  |  | 2 |
| Low battery alarm | 배터리 부족 알림 |  |  | 2 |
| Critical battery alarm | 배터리 위험 알림 |  |  | 2 |
| Power status | 전원 상태 |  |  | 2 |
| Power Meter | 전원 측정기 |  |  | 2 |
| Airplane mode | 비행기 모드 |  |  | 1 |

### 12.7 장치, 드라이버, 하드웨어

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Device | 장치 | '디바이스'로 쓰지 않습니다. | = | 4 |
| Devices | 장치 |  |  | 3 |
| Device Manager | 장치 관리자 |  |  | 1 |
| Device name | 장치 이름 |  |  | 3 |
| Device type | 장치 유형 |  |  | 2 |
| Device status | 장치 상태 |  |  | 1 |
| Default device | 기본 장치 |  |  | 2 |
| Driver | 드라이버 |  |  | 3 |
| Drivers | 드라이버 |  |  | 1 |
| Driver Provider | 드라이버 공급자 |  |  | 2 |
| Driver Date | 드라이버 날짜 |  |  | 2 |
| Driver Version | 드라이버 버전 |  |  | 2 |
| Update Driver | 드라이버 업데이트 |  |  | 1 |
| Roll Back Driver | 드라이버 롤백 |  |  | 1 |
| Remove Driver | 드라이버 제거 |  |  | 2 |
| Digital Signer | 디지털 서명자 |  |  | 1 |
| Hardware | 하드웨어 |  |  | 5 |
| Hardware ID | 하드웨어 ID |  |  | 1 |
| Compatible IDs | 호환 ID |  |  | 2 |
| Add hardware | 하드웨어 추가 |  |  | 2 |
| Safely Remove Hardware | 하드웨어 안전하게 제거 |  |  | 2 |
| Unknown device | 알 수 없는 장치 |  |  | 4 |
| Plug and Play | 플러그 앤 플레이 |  |  | — |
| Resources | 리소스 |  |  | 3 |
| Conflict | 충돌 |  |  | — |
| I/O range | I/O 범위 |  |  | 1 |
| Memory range | 메모리 범위 |  |  | 1 |
| Manufacturer | 제조업체 |  | 제조사 | 4 |
| Model | 모델 |  | = | 5 |
| Vendor | 공급업체 |  |  | 3 |
| Provider | 공급자 |  | = | 3 |
| Serial Number | 일련번호 |  | 일련 번호 / 시리얼 번호 | 4 |
| Firmware | 펌웨어 |  |  | — |
| Controller | 컨트롤러 |  |  | 2 |
| Adapter | 어댑터 |  |  | 1 |
| Display adapter | 디스플레이 어댑터 |  |  | 1 |
| Chip Type | 칩 종류 |  |  | 2 |
| DAC Type | DAC 종류 |  |  | 2 |
| Processor | 프로세서 |  |  | 4 |
| Memory | 메모리 |  |  | 4 |
| Physical memory | 실제 메모리 |  |  | — |
| Total Physical Memory | 전체 실제 메모리 |  |  | 2 |
| Available Physical Memory | 사용 가능한 실제 메모리 |  |  | 2 |
| Virtual memory | 가상 메모리 |  |  | 2 |
| Paging file | 페이징 파일 |  |  | 1 |
| No paging file | 페이징 파일 없음 |  |  | 2 |
| Printer | 프린터 |  | = | 2 |
| Default Printer | 기본 프린터 |  | = | 2 |
| Print queue | 인쇄 대기열 |  |  | — |
| Port | 포트 |  | = | 3 |
| Local Port | 로컬 포트 |  | = | 2 |
| Serial port | 직렬 포트 |  |  | 1 |
| Parallel port | 병렬 포트 |  |  | 1 |
| Baud rate | 전송 속도 |  | = | 2 |
| Bits per second | 비트/초 |  |  | 1 |
| Data bits | 데이터 비트 |  | = | 2 |
| Parity | 패리티 |  | = | 3 |
| Stop bits | 정지 비트 |  | = | 3 |
| Flow control | 흐름 제어 |  | = | 3 |
| Even | 짝수 |  |  | 2 |
| Odd | 홀수 |  |  | 2 |
| Mark | 마크 | 패리티 값 | = | 2 |
| Modem | 모뎀 |  |  | 2 |
| Game Controllers | 게임 컨트롤러 |  | = | 2 |
| Axis | 축 |  |  | — |
| Calibrate | 보정 |  |  | — |
| Speaker | 스피커 |  |  | — |
| Microphone | 마이크 |  |  | — |
| Playback | 재생 |  |  | 1 |
| Recording | 녹음 |  |  | 3 |
| Volume Control | 볼륨 조절 |  |  | 2 |
| Speaker Volume | 스피커 볼륨 |  |  | 2 |
| Balance | 균형 |  |  | 1 |
| Mute | 음소거 |  | = | 2 |
| Camera | 카메라 |  |  | 1 |
| Scanner | 스캐너 |  |  | — |
| Bluetooth | Bluetooth | 상표이므로 라틴 문자 그대로 |  | 1 |
| Paired | 페어링됨 |  |  | 2 |
| Boot | 부팅 |  |  | 1 |
| Boot Options | 부팅 옵션 |  |  | 2 |
| Boot Device | 부팅 장치 |  |  | 2 |
| Safe mode | 안전 모드 |  |  | 1 |
| Secure Boot | 보안 부팅 |  |  | — |

### 12.8 네트워크와 인터넷

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Network | 네트워크 |  |  | 4 |
| Network Connections | 네트워크 연결 |  |  | 3 |
| Network adapter | 네트워크 어댑터 |  |  | 1 |
| Network Components | 네트워크 구성 요소 |  |  | 2 |
| Local Area Connection | 로컬 영역 연결 |  |  | — |
| Connection | 연결 |  | = | 2 |
| Connections | 연결 |  | = | 2 |
| Connected | 연결됨 |  |  | 4 |
| Disconnected | 연결 끊김 |  | = | 3 |
| Not Connected | 연결 안 됨 |  |  | 2 |
| Connecting to %s | %s에 연결하는 중 |  | %s 연결중 | 2 |
| Connect using | 연결에 사용할 장치 |  |  | 2 |
| IP address | IP 주소 |  | = | 5 |
| Subnet mask | 서브넷 마스크 |  |  | 3 |
| Default gateway | 기본 게이트웨이 |  | = | 3 |
| DNS server | DNS 서버 |  |  | 2 |
| DNS Servers | DNS 서버 |  |  | 2 |
| DHCP Enabled | DHCP 사용 |  | DHCP 가능 | 2 |
| Physical Address | 물리적 주소 |  | 물리 주소 | 2 |
| MAC address | MAC 주소 |  |  | — |
| Host | 호스트 |  |  | 1 |
| Host name | 호스트 이름 |  |  | 1 |
| Protocol | 프로토콜 |  |  | — |
| Proxy | 프록시 |  | = | 5 |
| Proxy server | 프록시 서버 |  | = | 2 |
| Use a proxy server | 프록시 서버 사용 |  | = | 2 |
| Internet | 인터넷 |  | = | 2 |
| Internet Options | 인터넷 옵션 |  |  | — |
| Browser | 브라우저 |  |  | — |
| Home page | 홈페이지 |  | 홈 페이지 | 2 |
| Cookies | 쿠키 |  |  | 2 |
| Cache | 캐시 |  |  | — |
| History | 기록 |  | = | 2 |
| Address | 주소 |  | = | 3 |
| Link | 링크 |  | = | 2 |
| Wireless network | 무선 네트워크 |  |  | — |
| Wi-Fi | Wi-Fi | 상표이므로 라틴 문자 그대로 |  | 2 |
| Ethernet | 이더넷 |  | = | 1 |
| Signal strength | 신호 강도 |  |  | 1 |
| Security key | 보안 키 |  |  | — |
| Mobile hotspot | 모바일 핫스폿 |  |  | — |
| Firewall | 방화벽 |  |  | — |
| Packets | 패킷 |  |  | 1 |
| Sent | 보냄 |  | = | 1 |
| Received | 받음 |  | = | 1 |
| Speed | 속도 |  |  | 3 |
| Duration | 지속 시간 |  |  | 2 |
| Activity | 활동 |  |  | 3 |
| Remote | 원격 |  |  | — |
| Remote Desktop | 원격 데스크톱 |  |  | 2 |
| Local | 로컬 |  |  | 2 |
| Server | 서버 |  | = | 3 |
| Client | 클라이언트 |  | = | 1 |
| Domain | 도메인 |  |  | 4 |
| Workgroup | 작업 그룹 |  |  | 3 |
| Computer Name | 컴퓨터 이름 |  |  | 3 |
| Full computer name | 전체 컴퓨터 이름 |  |  | 1 |
| Dial | 전화 걸기 |  | 다이얼 | 3 |
| Dial-up | 전화 접속 |  |  | — |
| Dialing Rules | 전화 걸기 규칙 |  |  | 2 |
| Area code | 지역 번호 |  |  | 2 |
| Phone number | 전화번호 |  |  | 1 |
| Country/region code | 국가/지역 번호 |  |  | 2 |
| Callback | 콜백 |  |  | 2 |
| Phone and Modem Options | 전화 및 모뎀 옵션 |  |  | 2 |
| Timeout | 시간 제한 | 설정값. 결과로서 '시간이 초과되었습니다' | 시간 초과 | 3 |
| Offline | 오프라인 |  |  | 3 |
| Online | 온라인 |  |  | 3 |

### 12.9 보안, 계정, 인증서

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Security | 보안 |  | = | 4 |
| User | 사용자 |  | = | 4 |
| Users | 사용자 | 내장 그룹 이름 'Users'는 번역하지 않습니다(8장). | 사용 | 4 |
| User name | 사용자 이름 |  | = | 11 |
| User account | 사용자 계정 |  |  | — |
| Account | 계정 |  |  | 2 |
| Account type | 계정 유형 |  |  | 1 |
| Account is disabled | 계정을 사용할 수 없습니다 |  |  | 2 |
| Administrator | 관리자 | 계정 이름 'Administrator'는 번역하지 않습니다(8장). |  | 3 |
| Standard user | 표준 사용자 |  |  | 2 |
| Guest | 게스트 |  |  | 2 |
| Group | 그룹 |  |  | 3 |
| Member of | 소속 그룹 |  |  | 2 |
| Full name | 전체 이름 |  | = | 4 |
| Password | 암호 | '패스워드', '비밀번호'로 쓰지 않습니다. | = | 13 |
| Confirm password | 암호 확인 |  | = | 5 |
| Old password | 이전 암호 |  |  | 2 |
| New password | 새 암호 |  |  | 4 |
| Change Password | 암호 변경 |  |  | 2 |
| Reset Password | 암호 재설정 |  |  | 2 |
| Password hint | 암호 힌트 |  |  | 3 |
| The passwords don't match. | 암호가 일치하지 않습니다. |  |  | 2 |
| Caps Lock is on | Caps Lock이 켜져 있습니다 |  | Caps Lock 켜짐 | 2 |
| Log on | 로그온 |  |  | 1 |
| Logon | 로그온 |  |  | — |
| Sign in | 로그인 |  |  | 1 |
| Profile | 프로필 |  |  | 2 |
| User profile | 사용자 프로필 |  |  | 2 |
| Credentials | 자격 증명 |  |  | — |
| Smart Card Logon | 스마트 카드 로그온 |  | = | 2 |
| Permissions | 사용 권한 |  |  | 2 |
| Allow | 허용 |  | = | 1 |
| Deny | 거부 |  | = | 1 |
| Full Control | 모든 권한 |  |  | 1 |
| Read | 읽기 |  | = | 2 |
| Write | 쓰기 |  | = | — |
| Read & execute | 읽기 및 실행 |  |  | — |
| Special permissions | 특별 사용 권한 |  |  | 1 |
| Privilege | 권한 |  |  | — |
| Access | 액세스 | '접근'은 쓰지 않습니다(외래어 '액세스'로 통일). | 접근 | 1 |
| Access denied | 액세스가 거부되었습니다 |  |  | — |
| Auditing | 감사 |  |  | — |
| Encrypt | 암호화 |  |  | — |
| Decrypt | 암호 해독 |  |  | — |
| Certificate | 인증서 |  | = | 1 |
| Certificates | 인증서 |  | = | 3 |
| Issued To | 발급 대상 |  | 발행 대상 / 발행목적 | 2 |
| Issued By | 발급자 |  | 발행자 | 2 |
| Issuer | 발급자 |  | 발행자 | 2 |
| Expiration Date | 만료 날짜 |  | 만기일 | 4 |
| Valid from | 유효 기간 시작 |  | 유효기간(시작) | 1 |
| Valid to | 유효 기간 끝 |  | 유효기간(끝) | 1 |
| Intended purposes | 사용 목적 |  |  | 1 |
| Friendly name | 알기 쉬운 이름 |  | 애칭 | 3 |
| Thumbprint | 지문 |  |  | — |
| Signature | 서명 |  | = | 1 |
| Certificate store | 인증서 저장소 |  | 인증서 보관소 | 1 |
| Private key | 개인 키 |  |  | — |
| Public key | 공개 키 |  | 공용 키 | 1 |
| Key usage | 키 사용 |  | = | 1 |
| Certificate Trust List | 인증서 신뢰 목록 |  | = | 2 |
| Subject | 주체 | 인증서의 주체. 메일·문서의 'Subject'는 '제목' | 제목 | 3 |

### 12.10 시스템, 프로세스, 서비스, 진단

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| System Properties | 시스템 속성 |  |  | 2 |
| System Information | 시스템 정보 |  | = | 3 |
| System Configuration | 시스템 구성 |  | = | 2 |
| LiberNT Setup | LiberNT 설치 프로그램 | 제품 이름은 번역하지 않습니다. |  | 4 |
| System type | 시스템 종류 |  |  | 3 |
| System Manufacturer | 시스템 제조업체 |  |  | 3 |
| System Model | 시스템 모델 |  |  | 3 |
| Operating system | 운영 체제 |  |  | 1 |
| OS Name | OS 이름 |  |  | 2 |
| OS Version | OS 버전 |  | = | 2 |
| Version | 버전 |  | = | 12 |
| Build | 빌드 |  | = | 3 |
| Copyright | 저작권 |  | = | 3 |
| License | 라이선스 | '라이센스'로 쓰지 않습니다. |  | 1 |
| License agreement | 사용권 계약 |  |  | — |
| Product ID | 제품 ID |  |  | 4 |
| Registered Owner | 등록된 소유자 |  |  | 2 |
| Organization | 조직 |  | = | 1 |
| Performance | 성능 |  | = | 3 |
| Task Manager | 작업 관리자 |  | = | 3 |
| Task | 작업 |  | = | 2 |
| End Task | 작업 끝내기 |  | = | 1 |
| Process | 프로세스 |  |  | — |
| End Process | 프로세스 끝내기 |  | = | 1 |
| End Process Tree | 프로세스 트리 끝내기 |  | = | 1 |
| Priority | 우선순위 | '우선 순위'로 띄어 쓰지 않습니다. |  | 1 |
| Set Priority | 우선순위 설정 |  | 우선 순위 설정 | 1 |
| Realtime | 실시간 |  | = | 1 |
| High | 높음 |  | = | 8 |
| Above Normal | 보통 초과 |  | 보통 이상 | — |
| Normal | 보통 |  | = | 6 |
| Below Normal | 보통 미만 |  | 보통 이하 | — |
| Low | 낮음 |  | = | 8 |
| Affinity | 선호도 |  |  | 1 |
| Set Affinity | 선호도 설정 |  | = | 1 |
| CPU Usage | CPU 사용량 |  | = | 1 |
| CPU Time | CPU 시간 |  | = | 1 |
| Mem Usage | 메모리 사용량 |  | = | 3 |
| Page Faults | 페이지 폴트 |  | = | 2 |
| Handles | 핸들 |  | = | 1 |
| Threads | 스레드 |  | = | 1 |
| Image Name | 이미지 이름 |  | = | 3 |
| Command line | 명령줄 |  | = | 1 |
| System Idle Process | 시스템 유휴 프로세스 |  | 시스템 유휴 시간 프로세스 | 2 |
| Uptime | 작동 시간 |  |  | — |
| Commit charge | 커밋 사용량 |  |  | — |
| Kernel memory | 커널 메모리 |  |  | — |
| Paged | 페이징 |  | 페이지 됨 | 1 |
| Nonpaged | 비페이징 |  | 페이지 안 됨 | 1 |
| Total | 합계 |  | = | 2 |
| Available | 사용 가능 |  | 사용 가능함 | 4 |
| Free | 여유 | 메모리·공간의 남은 양. 문장 '사용 가능한 공간'과 구별 |  | 2 |
| Cached | 캐시됨 |  |  | — |
| Not responding | 응답 없음 |  | 응답하지 않음 | 1 |
| Service | 서비스 |  |  | 4 |
| Services | 서비스 |  |  | 3 |
| Service name | 서비스 이름 |  |  | 2 |
| Display name | 표시 이름 |  |  | 3 |
| Startup type | 시작 유형 |  |  | 1 |
| Automatic | 자동 |  | = | 4 |
| Manual | 수동 |  |  | 3 |
| Running | 실행 중 |  | = | 4 |
| Stopped | 중지됨 |  |  | 6 |
| Paused | 일시 중지됨 |  | = | 4 |
| Starting | 시작하는 중 |  |  | — |
| Stopping | 중지하는 중 |  |  | — |
| Log On As | 로그온 계정 |  |  | 1 |
| Local System | 로컬 시스템 |  |  | — |
| Dependencies | 종속성 |  |  | 1 |
| Path to executable | 실행 파일 경로 |  |  | 1 |
| Start parameters | 시작 매개변수 |  |  | 1 |
| Parameter | 매개변수 | '매개 변수', '파라미터'로 쓰지 않습니다. |  | — |
| Argument | 인수 |  |  | — |
| Switch | 스위치 |  |  | — |
| Syntax | 구문 |  |  | — |
| Usage | 사용법 |  |  | 2 |
| Command | 명령 |  |  | 3 |
| Command Prompt | 명령 프롬프트 |  |  | — |
| Console | 콘솔 |  | = | 2 |
| Terminal | 터미널 |  |  | — |
| Script | 스크립트 |  | = | 1 |
| Batch file | 배치 파일 |  |  | — |
| Environment Variables | 환경 변수 |  |  | 2 |
| System variables | 시스템 변수 |  |  | 2 |
| User variables | 사용자 변수 |  |  | 1 |
| Variable | 변수 |  |  | 1 |
| Value | 값 |  | = | 8 |
| Registry | 레지스트리 |  | = | 2 |
| Registry Editor | 레지스트리 편집기 |  | = | 1 |
| Subkey | 하위 키 |  |  | — |
| Value name | 값 이름 |  | = | 1 |
| Value data | 값 데이터 |  | = | 1 |
| Data | 데이터 |  | = | 3 |
| String value | 문자열 값 |  | = | 1 |
| Binary value | 이진 값 |  | 이진값 | 1 |
| (value not set) | (값이 설정되지 않음) |  | = | 2 |
| Event Viewer | 이벤트 뷰어 |  |  | 1 |
| Event | 이벤트 |  |  | 2 |
| Log | 로그 |  |  | 2 |
| Source | 원본 |  | = | 5 |
| Event ID | 이벤트 ID |  |  | 1 |
| Category | 범주 |  |  | 4 |
| Level | 수준 |  |  | 2 |
| Information | 정보 |  | = | 5 |
| Warning | 경고 |  | = | 6 |
| Error | 오류 | '에러'로 쓰지 않습니다. | = | 14 |
| Critical | 위험 |  |  | — |
| Success | 성공 |  | = | 5 |
| Failure | 실패 |  |  | — |
| Startup and Recovery | 시작 및 복구 |  |  | 2 |
| Write debugging information | 디버깅 정보 쓰기 |  |  | 2 |
| Automatically restart | 자동으로 다시 시작 |  |  | 2 |
| Startup Selection | 시작 모드 선택 |  |  | 2 |
| Normal Startup | 정상 모드로 시작 |  |  | 1 |
| Selective Startup | 선택 모드로 시작 |  |  | 2 |
| Load System Services | 시스템 서비스 로드 |  |  | 2 |
| Load Startup Items | 시작 항목 로드 |  |  | 2 |
| Scheduled task | 예약된 작업 |  |  | — |
| Component | 구성 요소 |  |  | 1 |
| Components | 구성 요소 |  |  | — |
| Features | 기능 |  |  | 1 |
| Update | 업데이트 |  |  | — |
| Compatibility | 호환성 |  |  | 2 |
| Troubleshoot | 문제 해결 |  |  | 2 |
| Diagnostics | 진단 |  |  | 1 |

### 12.11 상태와 메시지

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Ready | 준비 |  | 준비됨 | 2 |
| Done | 완료 |  | = | 1 |
| Complete | 완료 |  |  | 1 |
| Completed | 완료됨 |  |  | 1 |
| Failed | 실패 |  |  | — |
| Unknown | 알 수 없음 |  | = | 13 |
| Unknown error | 알 수 없는 오류 |  | = | 1 |
| Unknown error: %d\\n | 알 수 없는 오류: %d\\n |  |  | 3 |
| Invalid parameter format - %s\\n | 잘못된 매개변수 형식 - %s\\n |  |  | 3 |
| Info | 정보 |  |  | 4 |
| Not available | 사용할 수 없음 |  | 사용 가능하지 않음 | 5 |
| Unavailable | 사용할 수 없음 |  | = | 5 |
| Busy | 사용 중 |  | = | 4 |
| Please Wait | 잠시 기다려 주십시오 |  |  | 2 |
| Loading | 불러오는 중 |  |  | — |
| Processing | 처리 중 |  |  | — |
| Downloading | 다운로드 중 |  | = | 2 |
| Installing | 설치 중 |  | = | 4 |
| Installed | 설치됨 |  |  | 4 |
| Updating | 업데이트 중 |  |  | 2 |
| Searching | 검색 중 |  |  | 1 |
| Enabled | 사용 |  |  | 3 |
| Disabled | 사용 안 함 |  | 사용 중지된 장치 / 사용하지 않음 | 8 |
| On | 켬 |  | 작동 | 5 |
| Off | 끔 |  | 비작동 | 5 |
| Turn on | 켜기 |  |  | — |
| Turn off | 끄기 |  |  | 1 |
| Activate all | 모두 활성화 |  |  | 2 |
| Deactivate all | 모두 비활성화 |  |  | 2 |
| Active | 활성 |  |  | 1 |
| Selected | 선택됨 |  | = | 3 |
| Empty | 비어 있음 |  |  | 2 |
| (Empty) | (비어 있음) |  |  | 3 |
| None | 없음 |  | = | 18 |
| (None) | (없음) |  | = | 7 |
| &lt;None&gt; | &lt;없음&gt; |  | = | 3 |
| All | 모두 |  | = | 4 |
| &lt;All&gt; | &lt;모두&gt; |  | = | 2 |
| Other | 기타 |  | = | 6 |
| Never | 안 함 |  |  | 2 |
| Always | 항상 |  |  | — |
| Recommended | 권장 |  |  | 2 |
| Required | 필수 |  |  | 2 |
| Optional | 선택 사항 |  |  | 1 |
| Unspecified | 지정되지 않음 |  |  | 2 |
| Invalid | 잘못됨 | 문장 속에서는 '잘못된 …', '…이(가) 잘못되었습니다' |  | 2 |
| Syntax error | 구문 오류 |  | 문법 오류 | 2 |
| Not Present | 없음 |  |  | 2 |
| Welcome | 환영합니다 |  |  | 2 |
| Today | 오늘 |  | = | 2 |
| Result | 결과 |  | = | 2 |
| Message | 메시지 | '메세지'로 쓰지 않습니다. | = | 3 |
| Status | 상태 |  | = | 14 |
| State | 상태 |  | = | 2 |
| Progress | 진행률 |  |  | 2 |

### 12.12 인쇄, 글꼴, 텍스트

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Paper | 용지 |  | = | 2 |
| Paper size | 용지 크기 |  | = | — |
| Paper source | 용지 공급 |  |  | — |
| Margins | 여백 |  |  | — |
| Header | 머리글 |  | = | 1 |
| Footer | 바닥글 |  | = | 1 |
| Copies | 매수 |  | 복사본 | 1 |
| Collate | 한 부씩 인쇄 |  | 정렬 | 1 |
| Pages | 페이지 |  | = | 2 |
| Page range | 페이지 범위 |  |  | — |
| Current page | 현재 페이지 |  | = | 1 |
| Print to file | 파일로 인쇄 |  | = | 1 |
| Font style | 글꼴 스타일 |  | = | 2 |
| Font Size | 글꼴 크기 |  |  | 2 |
| Font Name | 글꼴 이름 |  |  | 2 |
| Bold | 굵게 |  | = | 2 |
| Italic | 기울임꼴 |  | = | 2 |
| Underline | 밑줄 |  | = | 2 |
| Strikeout | 취소선 |  | = | 1 |
| Regular | 보통 |  | = | 1 |
| Effects | 효과 |  | = | 3 |
| Sample | 견본 |  | 샘플 | 2 |
| Character | 문자 |  | = | 1 |
| Characters | 문자 |  |  | — |
| Character Map | 문자표 |  |  | 2 |
| Character set | 문자 집합 |  |  | — |
| Code page | 코드 페이지 |  |  | 1 |
| Unicode | 유니코드 |  |  | 2 |
| Encoding | 인코딩 |  | = | 1 |
| Text | 텍스트 |  | = | 4 |
| Line | 줄 |  |  | 2 |
| Column | 열 |  | = | 1 |
| Row | 행 |  | = | 1 |
| Text Labels | 텍스트 레이블 |  |  | 2 |
| Comment | 메모 | 'Description'(설명)과 구별 | 설명 | 5 |
| Comments | 메모 |  | 설명 / 주석 | 4 |
| Description | 설명 |  | = | 17 |
| Notes | 참고 |  |  | 3 |
| Content | 내용 |  | = | 3 |

### 12.13 단위, 방향, 크기, 색

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Bytes | 바이트 |  | = | 4 |
| bits | 비트 |  | bits | 1 |
| seconds | 초 |  | = | 7 |
| minutes | 분 |  | = | 5 |
| hours | 시간 |  | 시 | 2 |
| days | 일 |  |  | 3 |
| sec. | 초 |  |  | 2 |
| pixels | 픽셀 |  |  | 1 |
| %lux%lu pixels | %lux%lu 픽셀 |  |  | 2 |
| Width | 너비 |  | = | 2 |
| Height | 높이 |  | = | 2 |
| Length | 길이 |  |  | 2 |
| Left | 왼쪽 |  | = | 4 |
| Right | 오른쪽 |  | = | 4 |
| Top | 위쪽 |  | 위 / 맨 위 | 2 |
| Bottom | 아래쪽 |  | 아래 / 맨 아래 | 2 |
| Center | 가운데 |  | = | 3 |
| Horizontal | 가로 |  |  | — |
| Vertical | 세로 |  |  | 2 |
| Angle | 각도 |  |  | 2 |
| Degrees | 도 |  |  | 2 |
| Fast | 빠르게 | 슬라이더 양 끝 레이블(3.4절) |  | 3 |
| Slow | 느리게 | 슬라이더 양 끝 레이블(3.4절) |  | 3 |
| Short | 짧게 | 슬라이더 양 끝 레이블(3.4절) |  | 2 |
| Long | 길게 | 슬라이더 양 끝 레이블(3.4절) |  | 2 |
| Small | 작게 |  | = | — |
| Large | 크게 |  | = | 1 |
| Medium | 중간 |  | = | 2 |
| From | 시작 | 범위의 시작(페이지, 날짜) | = | 4 |
| To | 끝 | 범위의 끝 | = | 7 |
| Space | 공간 | 패리티 값은 '스페이스' |  | 3 |
| Light | 밝음 | 테마·모드 이름 | = | 2 |
| Dark | 어두움 | 테마·모드 이름 | = | 2 |
| Black | 검은색 |  | 검정 | 2 |
| White | 흰색 |  | = | 3 |
| Red | 빨간색 |  | 빨강 | 3 |
| Green | 초록색 |  | 녹색 | 3 |
| Blue | 파란색 |  | 파랑 | 3 |
| Yellow | 노란색 |  | 노랑 | 3 |
| Purple | 보라색 |  | 자주 | 3 |
| Orange | 주황색 |  |  | 2 |
| Pink | 분홍색 |  |  | 2 |
| Gray | 회색 |  | = | 2 |
| Teal | 청록색 |  | 청록 | 3 |
| Navy | 남색 |  | 짙은 파랑 | 2 |

### 12.14 보조 프로그램과 게임

| English | 한국어 | 비고 | Wine ko.po | 모듈 |
| --- | --- | --- | --- | ---: |
| Notepad | 메모장 |  | = | 1 |
| Calculator | 계산기 |  |  | 1 |
| Paint | 그림판 |  |  | 1 |
| Clipboard | 클립보드 |  |  | — |
| Game | 게임 |  | = | 3 |
| New Game | 새 게임 |  |  | 1 |
| Deal | 카드 돌리기 |  |  | 1 |
| Deal again? | 카드를 다시 돌리시겠습니까? |  |  | 2 |
| Deck | 카드 묶음 |  |  | 2 |
| Score | 점수 |  |  | — |
| Statistics | 통계 |  |  | — |
| Congratulations, you win! | 축하합니다. 이겼습니다! |  |  | 2 |
| Quit the current game? | 현재 게임을 끝내시겠습니까? |  |  | 2 |

## 13. 혼동하기 쉬운 낱말

영어에서 뜻이 가까운 낱말은 한국어에서도 구별해 옮깁니다. 한 대화 상자에 둘 이상이 함께 나오면 특히 이 표를 지킵니다.

| 영어 | 한국어 | 구별 |
| --- | --- | --- |
| Delete / Remove / Uninstall / Clear | 삭제 / 제거 / 제거 / 지우기 | Delete는 대상 자체가 없어짐(파일, 키). Remove는 목록이나 구성에서 빠짐. Uninstall은 프로그램과 장치의 제거. Clear는 내용이나 표시를 비움. Remove와 Uninstall이 한 화면에 함께 나오면 Remove를 `목록에서 제거`로 씁니다. |
| Close / Exit / Quit / End / Terminate | 닫기 / 끝내기 / 끝내기 / 끝내기 / 종료 | Close는 창이나 문서, Exit와 Quit는 프로그램, End는 작업과 프로세스(작업 끝내기), Terminate는 강제 종료. Shut down은 `시스템 종료`. |
| Cancel / Abort / Stop / Pause | 취소 / 중단 / 중지 / 일시 중지 | Cancel은 대화 상자나 진행 중인 요청을 무르는 것, Abort는 오류 상황의 중단, Stop은 서비스나 재생을 멈추는 것. |
| Restart / Reboot / Resume / Retry / Reset | 다시 시작 / 다시 시작 / 계속 / 다시 시도 / 재설정 | 서비스 화면처럼 Restart와 Resume이 함께 나오는 곳이 있으므로 Resume은 `계속`. |
| Enable / Disable | 사용 / 사용 안 함 | 기능, 장치, 계정의 사용 여부. 문장에서는 `사용하도록 설정`, `사용하지 않도록 설정`. |
| Activate / Deactivate | 활성화 / 비활성화 | 창, 제품, 프로필처럼 '활성 상태'가 따로 있는 대상. |
| Turn on / Turn off, On / Off | 켜기 / 끄기, 켬 / 끔 | 스위치 모양 설정과 전원. |
| Settings / Options / Preferences / Configuration / Properties / Attributes | 설정 / 옵션 / 기본 설정 / 구성 / 속성 / 특성 | 파일의 읽기 전용·숨김 같은 Attributes는 Properties(속성)와 같은 대화 상자에 나오므로 `특성`. |
| Find / Search / Browse / Look in | 찾기 / 검색 / 찾아보기 / 찾는 위치 | Find는 열린 문서나 목록 안에서, Search는 저장소나 네트워크 전체에서. |
| Format | 서식 / 포맷 / 형식 | 텍스트와 문서의 모양은 `서식`, 디스크 초기화는 `포맷`, 데이터와 파일의 구조는 `형식`. |
| Type / Kind / Category / Class | 형식·유형 / 종류 / 범주 / 클래스 | 파일과 데이터의 Type은 `형식`, 분류로서의 Type(장치 유형, 시작 유형, 계정 유형)은 `유형`. |
| Change / Modify / Edit / Replace / Update | 변경 / 수정 / 편집 / 바꾸기 / 업데이트 | Replace는 찾아 바꾸기와 파일 바꾸기. Update는 소프트웨어와 드라이버의 새 버전. |
| Install / Setup / Set up | 설치 / 설치 프로그램 / 설정하다 | 제품 이름 `LiberNT Setup`은 `LiberNT 설치 프로그램`. Page Setup은 `페이지 설정`. |
| Load / Open / Import | 불러오기·로드 / 열기 / 가져오기 | 파일과 구성은 `불러오기`, 드라이버와 모듈은 `로드`. |
| Log on / Log off / Sign in / Sign out | 로그온 / 로그오프 / 로그인 / 로그아웃 | 영어 원문의 구별을 그대로 따릅니다. |
| Folder / Directory | 폴더 / 디렉터리 | 셸과 대화 상자는 `폴더`, 명령줄 도구와 API 문맥은 `디렉터리`. |
| Drive / Disk / Volume / Partition | 드라이브 / 디스크 / 볼륨 / 파티션 | 드라이브 문자가 붙은 것은 `드라이브`, 물리 장치는 `디스크`. |
| Desktop | 바탕 화면 / 데스크톱 | 셸의 바탕 화면과 그 폴더는 `바탕 화면`, 원격·가상 데스크톱과 데스크톱 컴퓨터는 `데스크톱`. |
| Application / App / Program | 응용 프로그램 / 앱 / 프로그램 | 원문의 구별을 따릅니다. |
| Comment / Description / Notes | 메모 / 설명 / 참고 | 바로 가기 속성의 Comment와 서비스의 Description이 서로 다른 낱말이 되도록 합니다. |
| Error / Failure / Fault | 오류 / 실패 / 폴트 | Fault는 페이지 폴트처럼 기술 용어로만. |
| Invalid / Bad / Incorrect | 잘못된 / 잘못된 / 올바르지 않은 | 문장 끝에서는 `~이(가) 잘못되었습니다`, `~이(가) 올바르지 않습니다`. |
| Access / Permission / Privilege / Right | 액세스 / 사용 권한 / 권한 / 권리 | Right는 사용자 권리 할당(User Rights Assignment) 문맥. |
| Time | 시간 / 시각 | 길이는 `시간`, 특정 때는 문맥에 따라 `시각`도 씁니다. 레이블 Time은 `시간`. |
| Default / Standard / Normal / Regular | 기본값·기본 / 표준 / 보통 / 보통 | 글꼴 스타일 Regular와 우선순위 Normal은 모두 `보통`. |
| Details / More | 자세히 / 자세히 | 둘이 한 화면에 함께 나오면 More를 `더 보기`로 씁니다. |

## 14. 검토 절차와 커밋

- 번역 변경은 한국어 원어민 검토자 두 명의 승인을 받습니다.
- Wine `ko.po` 같은 외부 번역에서 기계적으로 반입한 문자열은 사람이 번역한 변경과 **별도 커밋**으로 나누고, 커밋 본문에 출처와 버전을 적습니다(2장 라이선스 조건).
- 커밋 제목은 `[COMPONENT]` 형식입니다(예: `[SHELL32] Update Korean translation`). 본문 형식은 [AGENTS.md](../../AGENTS.md#changes-and-commits)를 따르며, 낱말 몇 개를 고친 작은 변경은 제목만 씁니다.
- 번역을 커밋하는 사람의 이름은 [CONTRIBUTING.md](CONTRIBUTING.md#규칙과-권장-사항)에 따라 법적 실명(라틴 문자)이어야 합니다.
- 제출 전 확인 목록
  1. 리소스 ID, 컨트롤 수, `STRINGTABLE` 항목 수가 영어 파일과 같습니다.
  2. 자리표시자의 종류, 개수, (printf 계열이면) 순서가 영어와 같습니다.
  3. 액세스 키가 `(&X)` 형식이고 메뉴와 대화 상자 안에서 겹치지 않습니다.
  4. `FONT` 문이 영어 파일과 같습니다.
  5. 11장의 표기가 없습니다.
  6. 8장의 번역 금지 문자열을 건드리지 않았습니다.
  7. 기본 `I18N_LANG=all`로 amd64, i386, arm64 빌드가 됩니다.
  8. 한국어 UI의 가상 머신에서 대화 상자를 열어 잘리거나 겹치는 컨트롤이 없음을 봅니다.
- 점검 도구(WP 08-2)가 들어오면 위 1~6번은 도구로 확인합니다. 한국어 리소스의 존재와 로드는 apitest(`LoadStringKorean`, WP 08-3)로 확인합니다.
