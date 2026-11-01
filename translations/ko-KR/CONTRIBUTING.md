<!--
 PROJECT:     ShizukuNT Korean translation
 LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 PURPOSE:     Korean translation of CONTRIBUTING.md (source commit 346eba3d855d)
 COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
-->
# 기여 방법

> [English](../../CONTRIBUTING.md) | [中文](../zh-CN/CONTRIBUTING.md) | [Français](../fr-FR/CONTRIBUTING.md) | [Español](../es-ES/CONTRIBUTING.md) | [日本語](../ja-JP/CONTRIBUTING.md) | **한국어** | [العربية](../ar-MA/CONTRIBUTING.md)

LiberNT는 버그 보고, 테스트, 수정, 새 기능, 문서를 환영합니다. 이 페이지는 작업을 보내는 방법을 설명합니다. 모든 변경이 따르는 엔지니어링 규칙은 [AGENTS.md](../../AGENTS.md)에 있으며, 사람 기여자와 AI 도구에 똑같이 적용됩니다.

- [할 수 있는 일](#할-수-있는-일)
- [기여하는 방법](#기여하는-방법)
- [저작권과 라이선스](#저작권과-라이선스)
- [시작하기 좋은 곳](#시작하기-좋은-곳)

**법적 고지:** 독점 소프트웨어인 Microsoft Windows의 소스 코드를 본 적이 있다면, 저작권 침해 가능성 때문에 기여를 받아들일 수 없습니다. 기여하기 전에 다음 내용이 사실임을 확약해야 합니다.
>본인은 LiberNT에 대한 기여와 양립할 수 없는 라이선스 아래에 있는 Windows 운영 체제의 어떤 버전의 소스 코드도,
>제안된 프로젝트와 관련될 수 있는 어떤 Microsoft 제품의 소스 코드도 사용하거나 본 적이 없음을 이에 맹세합니다.
>여기에는 유출된 Windows NT 3.5, NT 4, 2000 소스 코드와
>Windows Research Kernel이 포함되며, 이에 한정되지 않습니다.

LiberNT는 클린룸 방식으로 만듭니다. Windows SDK와 WDK 헤더, Windows 11 공개 심볼, Microsoft 문서, Windows 11에서의 블랙박스 실행을 바탕으로 합니다. 쓸 수 있는 것과 쓸 수 없는 것은 [근거 자료](../../AGENTS.md#sources-of-truth)를 참조하십시오.

## 할 수 있는 일

### 버그 보고하기

버그는 [Issues](https://github.com/LiberNT/LiberNT/issues)에 보고하십시오. 아키텍처(amd64, i386, arm64, riscv64), 빌드 종류, 빌드한 커밋, 문제에 이르는 단계를 적어 주십시오. 직렬 포트의 디버그 로그가 있으면 큰 도움이 됩니다.

### 버그 고치기

증상이 아니라 근본 동작을 고치고, 올바른 최소한의 변경을 하십시오. 수정은 실제 부팅이나 테스트 실행에서 동작이 확인된 뒤에야 인정됩니다.

### 테스트 고치기

테스트는 LiberNT를 Windows의 동작과 대조합니다. 그중 상당수는 Windows 11에서도 모두 통과하지는 않으므로, 테스트를 통과시키려고 LiberNT를 바꾸기 전에 같은 테스트를 Windows 11에서 실행하고 결과를 검사 항목별로 비교하십시오. Windows 11도 어떤 검사에 실패한다면 잘못된 것은 기대값입니다. [Windows 11과 대조하는 테스트](../../AGENTS.md#testing-against-windows-11)를 참조하십시오.

### LiberNT 테스트하기

가상 머신이나 실제 하드웨어에서 LiberNT를 실행하고 무엇이 망가지는지 보고하십시오. 드라이버에는 실제 하드웨어가 가장 중요합니다. 가상 머신은 드라이버가 설치되고 로드된다는 것만 증명합니다.

### 새 기능 구현하기

호환성의 기준은 Windows 11입니다. 큰 작업을 시작하기 전에 [Issues](https://github.com/LiberNT/LiberNT/issues)나 [Discord](https://discord.gg/BpcKg7FZ2)에서 누군가 이미 작업하고 있는지 물어보십시오.

### 문서 쓰기

안내서와 설계 메모는 [Wiki](https://github.com/LiberNT/LiberNT/wiki)에 둡니다. [클린룸 리버스 엔지니어링] 관행을 지키고, 소스 코드, 문서, 테스트 실행이 보여 주는 내용만 쓰십시오.

### 서드 파티 코드

Wine과 동기화하는 구성 요소는 [media/doc/WINESYNC.txt](../../media/doc/WINESYNC.txt)에, 그 밖의 서드 파티 코드는 [media/doc/3rd Party Files.txt](../../media/doc/3rd%20Party%20Files.txt)에 나열되어 있습니다. 그런 코드에 대한 LiberNT의 변경은 `#ifdef __REACTOS__` 안에 두고 원래 코드는 `#else` 분기에 남겨, 다음 동기화 때도 변경이 유지되게 하십시오.

## 기여하는 방법

소통을 효율적으로 하기 위해 이슈 보고와 커뮤니티 토론에는 영어를 쓰기를 권장합니다.

### 풀 리퀘스트

작업은 `main-nt10` 브랜치를 대상으로 하는 [풀 리퀘스트]로 보내십시오. 풀 리퀘스트를 쓰면 GitHub의 저장소에 푸시한 변경을 다른 사람에게 알릴 수 있습니다. 풀 리퀘스트를 연 뒤에는 병합되기 전까지 팀과 변경을 논의하고 검토하며 후속 커밋을 더할 수 있습니다.

열기 전에:

- 작업하는 아키텍처만이 아니라 amd64, i386, arm64를 모두 빌드하십시오.
- 변경을 다루는 부팅이나 테스트를 실행하고, 무엇을 실행했는지 풀 리퀘스트에 적으십시오.
- 아키텍처별 코드는 아키텍처 계층에 두십시오. [아키텍처 중립성](../../AGENTS.md#architecture-neutrality)을 참조하십시오.

### 커밋 스타일

커밋 스타일은 __[커밋 템플릿]__ 파일에 정의되어 있습니다. 참고로 쓰거나 `git config commit.template .gitmessage`로 켜십시오. 그러면 로컬 저장소의 새 커밋에 이 템플릿이 처음 커밋 메시지로 들어갑니다. 전체 규칙은 [변경과 커밋](../../AGENTS.md#changes-and-commits)에 있습니다.

### AI 사용

AI 도구를 써도 됩니다. 풀 리퀘스트에 AI 도구가 어느 부분을 도왔는지, 어떻게 테스트했는지 알려 주십시오. AI가 쓴 코드도 다른 모든 코드와 같은 규칙을 따르며 사람이 검토합니다.

### 규칙과 권장 사항

- ***실명**과 **실제 메일 주소**를 쓰십시오.* 익명의 코드 기여는 받지 않습니다!
  - 코드나 번역을 바꾸는 모든 커밋에는 작성자의 법적 실명 전체(라틴 문자, 발음 구별 기호 허용)가 있어야 합니다.
  - GitHub 프로필([여기의 Name 필드][GitHub Profile Settings])에도 커밋에 적은 것과 같은 전체 이름을 설정하기를 권장합니다.
- 미디어 변경, 곧 아트(배경 화면, 테마, 아이콘, 소리)와 코드 밖 문서의 변경에는 예외가 있습니다.
  - 이런 경우에 한해 작성자 이름으로 별명이나 가명을 쓸 수 있으며, GitHub 프로필([여기의 Name 필드][GitHub Profile Settings])에도 커밋에 적은 것과 같은 이름을 설정하기를 권장합니다.
- 어떤 경우든 작성자는 실제 메일 주소를 써야 합니다. 여기에는 git 커밋(`user.email` 설정)과 GitHub [메일 설정][GitHub Email Settings]이 포함되며, 그곳의 "Keep my email addresses private" 확인란은 선택을 해제해야 합니다.
  - *개인 정보를 지키려면* GitHub [메일 설정][GitHub Email Settings]에서 커밋에 쓰일 알맞은 "Primary email address"를 고르십시오.
- *기여 내용을 제대로 설명하십시오.* 해당하면 관련 이슈 번호를 적으십시오.
- *관련된 변경만 넣으십시오.* 검토자가 바뀌는 기존 소스 코드에 대해 기억해야 할 정보가 줄어 검토가 쉬워집니다.
- *제출하기 전에 비슷한 풀 리퀘스트를 찾아보십시오.* 비슷한 풀 리퀘스트나 이슈가 이미 열려 있을 수 있습니다. 그런 경우 그곳에 의견을 달고 검토하십시오.
- *기여는 작게, 주제에 집중해서 하십시오.* 작업하다 마주친 관련 없는 문제를 고치고 싶어도 참고, 대신 그 문제에 대한 이슈를 여십시오.
- *우리의 __[코딩 스타일]__ 규칙을 지키십시오.*
- *질문을 두려워하지 마십시오.* [Discord](https://discord.gg/BpcKg7FZ2)나 [Matrix](https://matrix.to/#/#libernt:matrix.org)에서 팀에 물어보십시오.

## 저작권과 라이선스

여러분이 쓴 것의 저작권은 여러분에게 남습니다. LiberNT에는 저작권 양도도, 기여자 라이선스 계약도 없습니다.

- 새 파일은 [코드 스타일](../../AGENTS.md#code-style)에 나온 머리글로 시작하며, `COPYRIGHT` 줄에 이름과 메일 주소를 적고 SPDX 라이선스 식별자를 씁니다.
- 기존 파일을 크게 바꿀 때는 기존 줄 아래에 자신의 `COPYRIGHT` 줄을 더할 수 있습니다.
- 기존 저작권이나 라이선스 고지를 지우거나 바꾸지 말고, 최상위 디렉터리의 라이선스 전문을 고치지 마십시오.
- 바꾸는 파일의 라이선스를 유지하십시오. ReactOS에서 물려받은 코드는 머리글에 적힌 라이선스를 유지합니다.
- GNU GPL 3.0과 호환되지 않는 라이선스의 코드를 복사하지 말고, Linux의 GPL 코드도 복사하지 마십시오.

LiberNT와 ReactOS를 만든 사람들은 [CREDITS.md](../../CREDITS.md)를 참조하십시오.

## 시작하기 좋은 곳

- 열려 있는 [Issues](https://github.com/LiberNT/LiberNT/issues)를 살펴보십시오.
- 실패하는 테스트를 찾아 Windows 11과 비교하고, 통과하게 만드십시오.
- 안내서와 설계 메모는 [Wiki](https://github.com/LiberNT/LiberNT/wiki)에서 읽으십시오.
- [Discord](https://discord.gg/BpcKg7FZ2)나 [Matrix](https://matrix.to/#/#libernt:matrix.org)에서 도움을 청하십시오.

  [클린룸 리버스 엔지니어링]:                                    https://en.wikipedia.org/wiki/Clean_room_design
  [풀 리퀘스트]:                                                 https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests
  [GitHub Profile Settings]:                                     https://github.com/settings/profile
  [GitHub Email Settings]:                                       https://github.com/settings/emails
  [코딩 스타일]:                                                 CODING_STYLE.md
  [커밋 템플릿]:                                                 ../../.gitmessage

# 함께 보기

- [풀 리퀘스트 관리 규칙](PULL_REQUEST_MANAGEMENT.md)
- [행동 강령](CODE_OF_CONDUCT.md)
