# Inventory BOM Manager Qt

BOM 기반 범용 재고 관리 소프트웨어 초기 구현입니다.

이 프로젝트는 특정 품목 1세트를 만들 때 필요한 재료(BOM)를 등록하고, 현재 보유 재료로 몇 세트를 제작할 수 있는지 계산합니다. 모든 입고/출고/조정/제작 사용 내역은 이벤트 로그로 저장되며, JSONL 로그 파일을 다른 PC에서 불러오면 동일한 재고 상태를 재구성할 수 있도록 설계했습니다.

## 1. 핵심 개념

```text
Product        제작품 / 완제품
Material       재료 / 부품 / 소모품
BomItem        제품 1세트당 필요한 재료와 수량
InventoryEvent 입고, 출고, 조정, 제작 사용 이벤트 로그
```

현재 재고는 별도 수동 값이 아니라 `inventory_events.quantity_delta` 합계로 계산합니다.

```text
현재 재고 = 모든 입고/출고/조정/제작 사용 로그의 합계
제작 가능 세트 수 = 각 BOM 재료의 현재 재고 / 1세트당 필요 수량 중 최솟값
```

## 2. Ubuntu 의존성 설치

```bash
chmod +x dependencies/ubuntu/install.sh
./dependencies/ubuntu/install.sh
```

또는 직접 설치:

```bash
sudo apt update
xargs -a dependencies/ubuntu/apt-packages.txt sudo apt install -y
```

## 3. Ubuntu 빌드

```bash
chmod +x scripts/build_ubuntu.sh
./scripts/build_ubuntu.sh
```

수동 빌드:

```bash
mkdir -p build
cd build
cmake .. -G Ninja
cmake --build .
./inventory_bom_manager
```

실행 경로에 `inventory_bom.db`가 자동 생성됩니다.

## 4. Windows 개발

Windows에서도 Qt 개발이 가능합니다. 같은 소스코드를 Windows에서 다시 빌드하여 `.exe`를 만듭니다.

```powershell
.\dependencies\windows\install.ps1
.\scripts\build_windows.ps1
```

Qt 6는 Qt Online Installer 또는 Qt Maintenance Tool로 설치하는 것을 권장합니다. 자세한 내용은 `dependencies/windows/qt_setup_note.md`를 참고하세요.

## 5. 폴더 구조

```text
dependencies/
├── ubuntu/             Ubuntu apt 목록 및 설치 스크립트
└── windows/            Windows winget 목록 및 설치 스크립트

scripts/                빌드/패키징 스크립트

src/
├── app/                앱 초기화 및 서비스 연결
├── core/
│   ├── models/         Product, Material, BomItem, InventoryEvent
│   └── services/       비즈니스 로직
├── data/               SQLite 연결, 스키마, Repository
├── ui/                 Qt Widgets 화면
└── utils/              공통 유틸
```

## 6. 현재 구현 기능

- 제품 등록 / 수정 / 삭제
- 재료 등록 / 수정 / 삭제
- 재료별 현재 재고 계산
- 제품별 BOM 등록 / 수정 / 삭제
- 제품별 제작 가능 세트 수 계산
- 구매 입고 / 사용 출고 / 재고 조정
- 제품 제작 시 BOM 기준 재료 자동 차감
- 전체 이벤트 로그 조회
- JSONL 내보내기 / 불러오기
- 이벤트 UUID 중복 검사로 중복 반영 방지

## 7. 다음 확장 후보

- Excel import/export
- 바코드/QR 코드 입력
- 재료 단가 및 구매처 관리
- 프로젝트/발주번호별 출고 관리
- 로그 충돌 해결 UI
- Windows 설치 프로그램 생성
