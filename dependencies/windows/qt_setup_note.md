# Windows Qt Setup Note

Windows에서도 Qt 개발과 실행이 가능합니다.

권장 구성:

- Windows 10/11
- Qt 6.x
- Qt Creator
- CMake
- Ninja
- Visual Studio 2022 Build Tools / MSVC

Qt는 winget만으로 자동 설치하기보다 Qt Online Installer 또는 Qt Maintenance Tool로 설치하는 것을 권장합니다.

예시 Qt 경로:

```text
C:\Qt\6.6.3\msvc2019_64
C:\Qt\6.8.0\msvc2022_64
```

CMake가 Qt를 못 찾으면 `CMAKE_PREFIX_PATH`를 지정하세요.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2022_64"
```

배포 시에는 `windeployqt`를 사용합니다.

```powershell
windeployqt build\inventory_bom_manager.exe
```
