<!--
 PROJECT:     ShizukuNT Korean translation
 LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 PURPOSE:     Korean translation of CODING_STYLE.md (source commit 346eba3d855d)
 COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
-->
# 코딩 스타일

> [English](../../CODING_STYLE.md) | [中文](../zh-CN/CODING_STYLE.md) | [Français](../fr-FR/CODING_STYLE.md) | [Español](../es-ES/CODING_STYLE.md) | [日本語](../ja-JP/CODING_STYLE.md) | **한국어** | [العربية](../ar-MA/CODING_STYLE.md)

이 글은 LiberNT의 새 코드에 써야 할 일반적인 코딩 스타일 지침을 설명합니다. 이 지침은 C와 C++ 소스 파일에만 적용됩니다. 이 지침은 ReactOS 코딩 스타일에서 왔으며, ReactOS 구성원들이 2013년 10월 회의에서 합의한 것입니다.

기존 코드는 관련 없는 변경의 일부로 이 스타일로 바꾸지 않습니다. 자세한 내용은 [기존 코드의 서식 변경에 관한 참고 사항](#기존-코드의-서식-변경에-관한-참고-사항)을 참조하십시오.

다른 소스(Wine 등)와 동기화되는 코드는 다시 작성하면 안 됩니다. 동기화되는 파일을 추적할 때는 [3rd Party Files.txt](../../media/doc/3rd%20Party%20Files.txt)와 [WINESYNC.txt](../../media/doc/WINESYNC.txt) 파일을 쓸 수 있습니다.

## 파일 구조

1. LiberNT의 새 소스 코드 파일에는 모두 다음과 같은 파일 머리글이 있어야 합니다.

```
/*
 * PROJECT:     LiberNT Kernel
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Does cool things like Memory Management
 * COPYRIGHT:   Copyright 2017 Arno Nymous <abc@mailaddress.com>
 *              Copyright 2017 Mike Blablabla <mike@blabla.com>
 */
```

https://spdx.org/licenses 에 있는 SPDX 라이선스 식별자를 쓰십시오.
그러면 라이선스 도구가 소스 파일을 분석할 수 있습니다!

파일에 크게 기여해 파일 전체나 일부를 책임질 수 있다면 그 파일의 `COPYRIGHT` 부분에 자신을 더하십시오. 이 목록에는 파일마다 3명을 넘게 적지 않습니다. 파일에 이미 있는 저작권 줄과 라이선스 줄은 절대 지우거나 바꾸지 마십시오.

옛 머리글의 `FILE` 줄은 지워야 합니다.

2. LiberNT 코드베이스는 문서 생성기 [Doxygen](https://www.doxygen.nl/)을 쓰며, `Doxyfile`은 최상위 디렉터리에 있습니다. 따라서 함수에는 알맞은 머리글을 붙이십시오. 자세한 내용은 ReactOS 위키의 [API Documentation](https://reactos.org/wiki/Documentation_Guidelines#API_Documentation)을 참조하십시오.

## 들여쓰기와 줄 너비

1. 줄 너비는 최대 **200자**입니다.
2. 어떤 줄 끝에도 공백이나 탭을 붙이지 마십시오.
3. 들여쓰기는 **공백 4개**로 하고, 탭은 쓰지 마십시오!
4. switch 문에서는 case 레이블과 case 안의 문을 모두 들여 씁니다.

**올바른 예:**
```c
switch (Condition)
{
    case 1:
        DoSomething();
        break;

    case 2:
    {
        DoMany();
        ManyMore();
        OtherThings();
        break;
    }
}
```

**잘못된 예:**
```c
switch(Condition)
{
case 1:
     DoSomething();
     break;
case 2:
    DoMany();
    ManyMore();
    OtherThings();
    break;
}
```

5. 함수 호출이 한 줄에 들어가지 않으면 인수를 다음과 같이 맞춥니다.
```c
FunctionCall(arg1,
             arg2,
             arg3);
```

6. 함수 머리글은 다음 형식을 따릅니다(예의 순서를 지킵니다).
```c
static    // scope identifier
CODE_SEG("PAGE")    // section placement
// other attributes
BOOLEAN    // return type
FASTCALL    // calling convention
IsOdd(
    _In_ UINT32 Number);
```

## 공백

1. 단항 연산자 앞뒤에는 공백을 넣지 마십시오.  
**올바른 예:** `i++;`  
**잘못된 예:** `i ++;`

2. 이항 연산자와 삼항 연산자 앞뒤에는 공백을 넣으십시오.  
**올바른 예:** `a = b + c;`  
**잘못된 예:** `a=b+c;`

3. 쉼표와 세미콜론 앞에는 공백을 넣지 마십시오.

**올바른 예:**
```c
for (int i = 0; i < 5; i++)
    DoSomething();

func1(a, b);
```

**잘못된 예:**
```c
for (int i = 0; i < 5 ; i++)
    DoSomething();

func1(a , b) ;
```

4. 제어문과 그 괄호 사이에는 공백을 넣으십시오.

**올바른 예:**
```c
if (Condition)
    DoSomething();
```

**잘못된 예:**
```c
if(Condition)
    DoSomething();
```

5. 함수와 그 괄호 사이, 괄호와 그 내용 사이에는 공백을 넣지 마십시오.

**올바른 예:**
```c
func(a, b);
```

**잘못된 예:**
```c
func (a, b);
func( a, b );
```

## 줄 바꿈

1. 문마다 한 줄씩 씁니다.

**올바른 예:**
```c
x++;
y++;

if (Condition)
    DoSomething();
```

**잘못된 예:**
```c
x++; y++;

if (Condition) DoSomething();
```

## 중괄호

1. 중괄호(`{`와 `}`)는 항상 따로 한 줄에 둡니다.
2. 한 줄짜리 제어 절에는 중괄호를 써도 되지만 필수는 아닙니다. 다만 주석이 더 붙은 한 줄짜리 제어 절은 예외입니다.

**올바른 예:**
```c
if (Condition)
    DoSomething();

if (Condition)
{
    DoSomething();
}

if (Condition)
{
    // This is a comment
    DoSomething();
}

if (A_Very || (Very && Long || Condition) &&
    On_Many && Lines)
{
    DoSomething();
}

if (Condition)
    DoSomething();
else
    DoSomethingElse();

if (Condition)
{
    DoSomething();
}
else
{
    DoSomethingElse();
    YetAnother();
}
```

**잘못된 예:**
```c
if (Condition) {
    DoSomething();
}

if (Condition)
    // This is a comment
    DoSomething();

if (A_Very || (Very && Long || Condition) &&
    On_Many && Lines)
    DoSomething();

if (Condition)
    DoSomething();
else {
    DoSomethingElse();
    YetAnother();
}
```

## 제어 구조

1. 제어 절에서 뒤집힌 논리를 쓰지 마십시오.  
**올바른 예:** `if (i == 1)`  
**잘못된 예:** `if (1 == i)`

2. 제어 구조를 너무 깊이 겹치지 마십시오. "트리 스타일"보다 "선형 스타일"을 쓰십시오. 코드를 더 깔끔하게 하는 데 도움이 되면(예: 정리 경로) `goto`를 쓰십시오.

**올바른 예:**
```c
if (!func1())
    return;

i = func2();
if (i == 0)
    return;

j = func3();
if (j == 1)
    return;
...
```

**잘못된 예:**
```c
if (func1())
{
    i = func2();
    if (func2())
    {
        j = func3();
        if (func3())
        {
            ...
        }
    }
}
```

## 이름 짓기

1. 변수와 함수 이름은 대문자로 시작합니다. Win32용으로 개발할 때는 헝가리안 표기법을 써도 되지만 필수는 아닙니다. 헝가리안 표기법을 쓰지 않는다면 이름의 첫 글자도 대문자여야 합니다(lowerCamelCase 금지). 밑줄을 구분자로 쓰지도 마십시오.

**올바른 예:**
```c
PLIST_ENTRY FirstEntry;
VOID NTAPI IopDeleteIoCompletion(PVOID ObjectBody);
PWSTR pwszTest;
```

**잘못된 예:**
```c
PLIST_ENTRY first_entry;
VOID NTAPI iop_delete_io_completion(PVOID objectBody);
PWSTR pwsztest;
```

2. 함수와 변수 이름을 줄여 쓰지 말고, 가능하면 뜻을 드러내는 동사를 쓰십시오.

3. 불리언 값 앞에는 가능하고 쓰임에 맞는다면 "is", "did"처럼 뜻 있는 동사를 붙이십시오.

**올바른 예:**
```c
BOOLEAN IsValid;
BOOLEAN DidSendData;
```

**잘못된 예:**
```c
BOOLEAN Valid;
BOOLEAN SentData;
```

## 주석

1. 한 줄에 들어갈 수 있는 주석에 줄을 낭비하지 마십시오.

**올바른 예:**
```c
// This is a one-line comment

/* This is a C-style comment */

// This is a comment over multiple lines.
// We don't define any strict rules for it.
```

**잘못된 예:**
```c
//
// This comment wastes two lines
//
```

## Null, false, 0

1. 널 포인터는 `NULL`로 씁니다. 드물게 개발 환경이 다른 널 포인터(예: C++11 `nullptr`)를 권장한다면 물론 그것을 써도 됩니다. 다만 값 `0`은 쓰지 마십시오.

2. Win32/NT의 불리언 값은 `TRUE`와 `FALSE`로 씁니다. 드물게 C/C++ `bool` 변수를 쓴다면 `true`와 `false`로 씁니다.

3. ANSI나 OEM 문자열을 끝내거나 그 종결 문자를 확인할 때는 `ANSI_NULL`을 씁니다. 유니코드 문자열이나 와이드 문자열이면 `UNICODE_NULL`을 씁니다.

## 기존 코드의 서식 변경에 관한 참고 사항

- 파일 전체의 서식을 바꾸면서 코드 변경을 함께 넣지 마십시오. 둘은 별도 커밋으로 나눕니다.
- 커밋이 서식 변경만으로 이루어져 있다면 커밋 메시지 앞에 *[FORMATTING]* 표시를 붙여 분명히 밝히십시오.

## 그 밖의 사항

- API를 쓰는 데 필요하지 않다면 `LARGE_INTEGER`/`ULARGE_INTEGER`를 쓰지 마십시오. 대신 `INT64`/`UINT64`를 쓰십시오
- 헤더에는 가드 정의 대신 `#pragma once`를 쓰십시오
- 필요하지 않다면(대개 API나 내보내는 기호의 경우) 함수에 호출 규칙을 지정하지 마십시오

## 자동 코드 스타일 도구 사용

최상위 디렉터리의 `.clang-format` 파일이 이 스타일을 clang-format용으로 기술합니다. 위의 기존 코드 서식 변경에 관한 참고 사항에 따라 새 코드에만 쓰십시오.

## 함께 보기

- ReactOS 위키의 [Kernel Coding Style](https://reactos.org/wiki/Kernel_Coding_Style)
- ReactOS 위키의 [GNU Indent](https://reactos.org/wiki/GNU_Indent)
