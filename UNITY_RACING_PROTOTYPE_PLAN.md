# 웹 실행 버전 레이싱 프로토타입

요청에 따라 Unity 스크립트 중심 구조 대신, 브라우저에서 즉시 실행 가능한 웹 3D 버전으로 전환했습니다.

## 실행 방법
1. 프로젝트 루트에서 정적 서버 실행
   - `python3 -m http.server 8080`
2. 브라우저에서 접속
   - `http://localhost:8080/web/`

## 파일 구성
- `web/index.html`: 캔버스 + HUD
- `web/styles.css`: HUD/레이아웃 스타일
- `web/main.js`: 3D 씬, 차량 물리, 카메라, HUD 업데이트

## 구현 요소
- W/S/A/D, Space, Shift, C, R 입력 지원
- 토크 커브 기반 가속 + 자동변속(8단)
- 공기저항/구름저항
- 간이 ABS/TCS 개입
- 카메라 모드 전환(C)
- 속도/기어/RPM/ABS/TCS HUD

## 주의
- 실제 게임/브랜드의 로고, UI, 에셋, 상표를 복제하지 않았습니다.
