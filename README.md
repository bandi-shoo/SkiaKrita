# SkiaKrita

> **Skia 기반 페인팅 애플리케이션** – 바이브 코딩으로 만들기 프로젝트

## 프로젝트 목표

| 항목 | 설명 |
|------|------|
| **플랫폼** | Windows 우선 (Visual Studio 2026). 코어는 macOS 이식을 염두에 둠 |
| **코어 설계** | 순수 C++20 + Skia. 플랫폼/GUI 프레임워크 의존 없음 |
| **GUI** | 애플리케이션 셸에서 분리. 추후 Win32 윈도우, Cocoa 등으로 교체 가능 |
| **의존성** | vcpkg manifest 모드로 Skia를 통합 |

---

## 디렉터리 구조

```
SkiaKrita/
├── SkiaKrita.slnx               ← Visual Studio 솔루션 (XML 형식)
├── Directory.Build.props         ← 공통 MSBuild 속성 (vcpkg, 경로, vcpkg.props import)
├── Directory.Build.targets       ← 공통 MSBuild 타깃 (Skia include/define, vcpkg.targets import)
├── vcpkg.json                    ← vcpkg 매니페스트 (Skia 의존성 + baseline)
│
├── apps/
│   └── krita_shell/              ← 실행 파일 (콘솔 부트스트랩)
│       ├── krita_shell.vcxproj
│       └── src/main.cpp
│
├── libs/
│   ├── core/                     ← 순수 C++20 코어 라이브러리
│   │   ├── core.vcxproj
│   │   ├── include/core/         ← 공개 헤더
│   │   └── src/                  ← 구현
│   │
│   └── graphics_skia/            ← Skia 래퍼 (공개 API는 순수 C++20)
│       ├── graphics_skia.vcxproj
│       ├── include/graphics_skia/← 공개 헤더 (Skia 헤더 노출 없음)
│       └── src/                  ← Skia 의존 구현 (pimpl)
│
├── docs/
│   ├── uml/
│   │   ├── architecture/         ← 목표 아키텍처 UML (.puml)
│   │   ├── progress/             ← 현재 구현 상태 UML (.puml)
│   │   └── out/                  ← 생성된 SVG (gitignore)
│   └── roadmap/
│       └── next_steps.yml        ← 로드맵 메타데이터
│
├── tools/
│   └── uml/
│       └── generate.py           ← PlantUML → SVG 생성 스크립트
│
└── @doc/                         ← 기타 문서
```

### 프로젝트 구성

| 프로젝트 | 타입 | 설명 |
|----------|------|------|
| `apps/krita_shell` | Application (.exe) | 최소 콘솔 실행 파일. core + graphics_skia 링크 |
| `libs/core` | Static Library (.lib) | 순수 C++20. 플랫폼/GUI 의존 없음 |
| `libs/graphics_skia` | Static Library (.lib) | Skia 래퍼. 공개 헤더에 Skia 타입 노출 없음 (pimpl) |

---

## 빌드 방법 (Windows / VS2026)

### 사전 요구사항

- **Visual Studio 2026** (C++ 데스크톱 개발 워크로드 포함)
- **vcpkg** – VS2026에 기본 내장 (`VCPKG_ROOT` 환경변수가 자동 설정됨).
  별도 설치 시:
  ```
  git clone https://github.com/microsoft/vcpkg.git
  .\vcpkg\bootstrap-vcpkg.bat
  ```
- **vcpkg 시스템 통합** (최초 1회):
  ```
  vcpkg integrate install
  ```
  > VS2026의 내장 vcpkg를 사용하는 경우에도 이 명령을 한 번 실행해야
  > MSBuild에서 vcpkg 패키지를 자동으로 찾을 수 있습니다.

### 빌드 단계

```bash
# 1. 레포 클론
git clone https://github.com/bandi-shoo/SkiaKrita.git
cd SkiaKrita

# 2. Visual Studio에서 솔루션 열기
#    → SkiaKrita.slnx 더블클릭 또는 VS에서 "Open a project or solution"

# 3. 빌드 (VS 내부)
#    → 구성: Debug | x64  또는  Release | x64
#    → Build → Build Solution (Ctrl+Shift+B)
#
#    첫 빌드 시 vcpkg가 Skia를 자동으로 다운로드/빌드합니다.
#    (Skia 빌드에 수 분 소요될 수 있음)

# 4. 실행
#    → krita_shell을 시작 프로젝트로 설정 후 F5
#    → 또는 명령줄:
#       .\x64\Debug\krita_shell.exe
```

### 명령줄 빌드 (MSBuild)

```powershell
msbuild SkiaKrita.slnx /p:Configuration=Debug /p:Platform=x64
```

> **참고**: vcpkg manifest 모드가 활성화되어 있으므로 `vcpkg.json`의
> 의존성이 빌드 시 자동으로 설치됩니다. `Directory.Build.props`에서
> `VcpkgEnableManifest=true`로 설정되어 있으며, vcpkg MSBuild 통합
> 파일(`vcpkg.props` / `vcpkg.targets`)을 명시적으로 import합니다.

---

## UML 다이어그램 생성

UML 소스는 [PlantUML](https://plantuml.com/) 텍스트 파일(`.puml`)로 관리합니다.

### 사전 요구사항 (택 1)

| 방법 | 설치 |
|------|------|
| **환경변수** | `PLANTUML_JAR` 환경변수를 `plantuml.jar` 경로로 설정 |
| **로컬 JAR** | `plantuml.jar`를 `tools/uml/` 디렉터리에 복사 |
| **CLI** | `choco install plantuml` (Chocolatey) 또는 PATH에 `plantuml` 추가 |

> PlantUML은 Java 런타임(JRE 8+)이 필요합니다.

### 실행

```bash
python tools/uml/generate.py
```

생성된 SVG 파일은 `docs/uml/out/` 아래에 소스 디렉터리 구조를 유지하며 출력됩니다:

```
docs/uml/out/
├── architecture/overall.svg
└── progress/current.svg
```

---

## 빌드 설정 요약

| 설정 | 값 |
|------|-----|
| C++ 표준 | C++20 (`/std:c++20`) |
| 경고 레벨 | `/W4` |
| 적합성 모드 | `/permissive-` |
| 문자 집합 | Unicode |
| SDL 검사 | 활성화 |
| 플랫폼 | x64 |
| 도구 집합 | v145 (VS2026) |
| vcpkg 통합 | `Directory.Build.props`/`.targets`에서 vcpkg.props/targets 명시적 import |
| Skia 헤더 | `Directory.Build.targets`에서 `include/skia` 추가 경로 설정 |
| Skia 정의 | `Directory.Build.targets`에서 Debug/Release별 SK_* 전처리기 매크로 설정 |

---

## 라이선스

TBD
