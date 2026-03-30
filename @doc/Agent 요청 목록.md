# Q1. 26.03.30 / Claude Opus 4.6
리포지토리: bandi-shoo/SkiaKrita
현재 솔루션 파일: SkiaKrita.slnx (레포 루트에 이미 존재, 파일명 유지 필수)
목표: Windows 우선(VS2026)으로 시작하되, GUI를 제외한 코어는 향후 macOS 이식이 가능하도록 “순수 C++20 + Skia” 중심으로 설계.
비고: vcxproj 및 폴더 구조는 자유롭게 커스텀해도 됨(단, 깔끔하고 문서화되어야 함).

요청:
아래 초기 부트스트랩을 “단일 Pull Request(PR) 1개”로 완성해줘. 목표는 새로 클론한 개발자가 Visual Studio 2026에서 바로 열어 빌드/실행 가능하고, UML 문서를 텍스트 소스에서 Python으로 생성할 수 있게 만드는 것.

필수 요구사항(이 PR에 반드시 포함):

1) 솔루션 / 프로젝트 구성
- 기존 솔루션 파일명 SkiaKrita.slnx를 유지(이름 변경 금지).
- 최소한 아래 3개 프로젝트를 생성해서 솔루션에 추가(프로젝트 타입은 Windows에서 빌드가 깔끔하게 되는 한 자유):
  A) 실행 파일 앱 프로젝트 (예: apps/krita_shell)
     - “실행 가능” 최소 프로그램(콘솔 출력만 해도 OK, 간단한 윈도우를 띄워도 OK).
     - 아래 라이브러리들을 링크하도록 설정.
  B) 이식 가능한 코어 라이브러리 (예: libs/core)
     - 순수 C++20
     - Win32/ATL/MFC/Qt 등 플랫폼/GUI 의존 금지
  C) Skia 래퍼 라이브러리 (예: libs/graphics_skia)
     - 외부에 노출되는 인터페이스는 순수 C++20 중심(특정 GUI 프레임워크 의존 금지)
     - 앱에서 호출 가능한 최소 API 1개 이상 제공(예: Skia 초기화, Skia 버전/정보 출력, 작은 surface 생성 후 테스트 픽셀/간단 도형 그리기 등)

2) 공통 빌드 설정
- 모든 프로젝트의 C++ 표준: C++20
- MSVC에서 합리적인 기본 설정 적용(예: /permissive-, 경고 레벨은 가능하면 /W4 이상, Unicode 등)
- 디렉터리 구조는 자유롭게 재구성 가능하나, 확장하기 좋고 이해하기 쉽게 정리하고 README에 설명할 것

3) Skia 의존성 관리(vcpkg manifest 모드)
- vcpkg manifest 모드로 Skia를 통합:
  - 레포 루트에 vcpkg.json 추가
  - 프로젝트에서 vcpkg로 설치된 Skia를 사용할 수 있게 설정
- 초기 목표는 “첫 빌드 성공”이므로, 옵션/백엔드(예: Vulkan 등)는 과도하게 켜지 말고 최소 구성으로 시작
- README에 “클린 환경에서 VS2026로 빌드하는 방법(vcpkg 포함)”을 단계별로 문서화

4) UML 문서(텍스트 기반 + 사람이 보기 쉬운 출력물) + Python 자동 생성
- UML은 PlantUML 텍스트(.puml)로 관리:
  - docs/uml/architecture/overall.puml  : 목표(이상적인 전체 구조 UML)
  - docs/uml/progress/current.puml      : 현재 구현 상태(진행 상황 UML)
- 아래 Python 스크립트 추가:
  - tools/uml/generate.py
  - docs/uml/ 아래의 .puml 파일을 재귀적으로 찾아서 docs/uml/out/에 SVG(권장) 또는 PNG로 렌더링
  - 실행 방식은 아래 중 1개를 선택하고 README에 설치/실행 방법을 명확히 기재:
    - (옵션 1) local plantuml.jar + Java로 실행
    - (옵션 2) PATH에 있는 PlantUML CLI를 호출
- README에 단일 커맨드 실행 예시를 반드시 포함:
  - python tools/uml/generate.py

5) 로드맵 메타데이터(다음 단계 자동화를 위한 틀)
- docs/roadmap/next_steps.yml 추가, 최소 섹션:
  - current_state
  - next_task_candidates
  - definition_of_done
- 내용은 최소한으로라도 “다음 단계 후보”가 구체적으로 보이도록 작성(예: core 모듈 경계 확정, skia 래퍼 확장, 렌더링 테스트 추가, CI 추가 등)

6) 문서화(README)
- README.md에 최�� 포함:
  - 프로젝트 목표(Windows 우선, 코어는 C++20+Skia로 이식성 확보, GUI는 분리/추후 교체 가능)
  - VS2026에서의 빌드 방법(vcpkg manifest 포함)
  - UML 생성 방법(Python 스크립트)
  - 디렉터리/모듈 개요

완료(머지) 기준 / 수용 조건:
- Windows에서 VS2026로 SkiaKrita.slnx를 열어 빌드가 성공해야 함
- 실행 파일이 정상 실행되어야 함(콘솔 로그만 출력해도 됨)
- `python tools/uml/generate.py` 실행 시 docs/uml/out/에 출력 파일이 생성되어야 함
- 지정한 UML/로드맵 파일들이 레포에 포함되어야 함

제약:
- 이번 PR에서는 특정 GUI 프레임워크(ATL/MFC/Qt 등)에 “락인”하지 말 것(최소 셸 앱만).
- 핵심은 “코어/Skia 래퍼를 GUI와 분리한 구조 + 빌드 재현성 + UML/로드맵 자동화의 발판”이다.

# Q2. 26.03.30 / Claude Opus 4.6
남은 작업 이어서 해줘