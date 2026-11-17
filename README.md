# 00.chip8 — CHIP-8 에뮬레이터 (C + SDL2)

학습 문서는 [doc/README.md](doc/README.md)에 있어요.

## 폴더 구조
```
00.chip8/
|-- doc/             # 학습 문서
|-- src/main.c       # SDL 창, 메인 루프
|-- roms/            # 테스트/게임 ROM (.ch8)
|-- CMakeLists.txt   # 빌드 설정 (SDL2 2.30.9를 자동으로 내려받아 정적 링크)
`-- build/           # 빌드 결과물 (git 제외)
```

## 빌드 환경
- Visual Studio 2026 Community (MSVC, "C++를 사용한 데스크톱 개발")
- CMake 4.x
- SDL2는 따로 설치할 필요 없음 (처음 configure 할 때 CMake가 소스를 받아서 함께 빌드)

## 빌드 & 실행

처음 한 번 (프로젝트 생성 + SDL2 다운로드, 1분 정도 걸림):
```bash
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
```

빌드:
```bash
cmake --build build --config Debug
```

실행:
```bash
./build/Debug/chip8.exe
```

Visual Studio에서 작업하려면 `build/chip8.slnx`를 열고 `chip8` 프로젝트를 시작 프로젝트로 지정하세요. F5를 누르면 디버거와 함께 실행돼요.

## 조작
- `ESC` 또는 창 닫기: 종료
