# Windows Packaging

1. Release 빌드
2. `windeployqt` 실행
3. `platforms/qwindows.dll`, `sqldrivers/qsqlite.dll` 포함 여부 확인

예시:

```powershell
.\scripts\build_windows.ps1
.\scripts\package_windows.ps1
```
