<!--
 PROJECT:     ShizukuNT Korean translation
 LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 PURPOSE:     Korean translation of PULL_REQUEST_MANAGEMENT.md (source commit 346eba3d855d)
 COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
-->
# 풀 리퀘스트 관리 규칙

> [English](../../PULL_REQUEST_MANAGEMENT.md) | [中文](../zh-CN/PULL_REQUEST_MANAGEMENT.md) | [Français](../fr-FR/PULL_REQUEST_MANAGEMENT.md) | [Español](../es-ES/PULL_REQUEST_MANAGEMENT.md) | [日本語](../ja-JP/PULL_REQUEST_MANAGEMENT.md) | **한국어** | [العربية](../ar-MA/PULL_REQUEST_MANAGEMENT.md)

열려 있지만 진행이 멈춘 PR의 수를 적정하게 유지하기 위해 다음 규칙을 고려해야 합니다.
- PR이 승인을 하나 이상 받았다면, 추가 의견을 1주일 기다린 뒤 병합할 수 있습니다.
  - 변경이 승인을 3개 이상 받았거나 충분히 사소하다고 판단되면 바로 병합할 수 있습니다.
- PR이 "changes requested"(변경 요청) 상태로 너무 오래 머물러 있고 작성자가 작업 중이라는 표시가 없으면 PR을 닫습니다.
  - 기준: 작은 PR은 2주입니다. PR이 크면 더 길어질 수 있습니다.
  - 추가 의견이 있거나 새 변경을 했다면 언제든 PR을 다시 열 수 있습니다.
- 특정한 사람의 검토가 필요하면 PR을 그 사람에게 할당하십시오. GitHub의 "review requested"(검토 요청) 기능에만 기대지 마십시오.
- PR 레이블이 있다는 것을 기억하십시오. 풀 리퀘스트에 알맞은 레이블을 붙여 범위를 나타내거나, 더 많은 관심을 끌거나, 단순히 찾아보기 쉽게 할 수 있습니다.
- 의견을 달기 위해서 보이는 모든 것에 의견을 달아야 한다고 생각하지 마십시오. GitHub, Discord, Matrix 어디에서든 마찬가지입니다.

또한 도움을 주는 기여자에게 무례하게 보이지 않도록 다음은 삼가십시오.
- 기여자에게 관련 없는 작업을 요구하기
- 이유를 밝히지 않고 닫기
- 곧 그 코드를 다시 작성할 생각으로 병합하기

PR을 병합하기 전에 [기여 규칙](CONTRIBUTING.md#규칙과-권장-사항)을 따르는지 확인하되, 무엇보다 다음을 확인하십시오.
- 작성자가 PR의 모든 커밋에 실제 메일 주소를 적었는지 확인합니다
- PR에 코드나 번역이 들어 있으면, 작성자가 PR의 모든 커밋에 별명이나 가명이 아닌 법적 실명 전체를 적었는지 확인합니다
- PR에 미디어(배경 화면, 테마, 아이콘, 소리)나 코드 밖의 문서가 들어 있으면, 작성자가 PR의 모든 커밋에 이름이나 가명을 적었는지 확인합니다
- PR에 코드와 미디어 변경이 섞여 있으면 코드가 있는 PR로 다룹니다
- PR에 "Squash and merge" 방식을 쓰기 전에 알아 둘 중요한 사항:
  - 작성자의 GitHub 프로필 이름이 커밋의 이름과 같은지 확인합니다. 다르면 작성자에게 맞게 설정해 달라고 요청합니다.
  - 작성자가 GitHub 프로필에 이름을 설정하기를 원하지 않으면:
    - PR에 "no squash merge" 레이블을 붙여야 합니다.
	- 모든 커밋 메시지가 [.gitmessage](../../.gitmessage)의 형식에 맞는지 확인합니다.
	- 이 경우 PR은 결국 "Rebase and merge" 방식이나 수동으로 병합해야 합니다.
  - PR에서 "Squash and merge" 단추를 누르면 작성자가 no-reply 메일 주소를 쓰지 않는지 확인할 수 있습니다 -
  커밋 메시지 아래에 `This commit will be authored by <address@email.com>`이라는 문구가 표시됩니다
