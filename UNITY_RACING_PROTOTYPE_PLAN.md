# Unity 3D 레이싱 프로토타입 (실구현 1단계)

이 저장소는 요청하신 통합 프롬프트를 바탕으로, Unity에서 바로 붙여서 시작할 수 있는 **1단계 C# 스크립트 구현본**을 제공합니다.

## 포함된 스크립트
- `Scripts/Vehicle/Powertrain.cs`
- `Scripts/Vehicle/WheelFrictionModel.cs`
- `Scripts/Vehicle/VehicleController.cs`
- `Scripts/Camera/RaceCameraController.cs`
- `Scripts/UI/HUDController.cs`
- `Scripts/Game/SurfaceGripMap.cs`
- `Scripts/Game/ResetController.cs`

## 오브젝트 연결 가이드
1. `PlayerCar` (Rigidbody + WheelCollider 4개)
   - `VehicleController` 부착
   - FL/FR/RL/RR WheelCollider 참조 연결
   - `SurfaceGripMap` 참조 연결
2. `Main Camera`
   - `RaceCameraController` 부착
   - `target = PlayerCar Transform`, `vehicle = PlayerCar.VehicleController`
3. `HUD Canvas`
   - `HUDController` 부착
   - Speed/Gear/ABS/TCS Text, RPM Slider 연결
4. `GameSystems`
   - `SurfaceGripMap`, `ResetController` 부착
   - `ResetController.resetPoint`에 도로 위 빈 오브젝트 지정

## 입력
- W/S: 가속/브레이크(후진 포함)
- A/D: 조향
- Space: 핸드브레이크
- LeftShift: 스포츠 모드
- R: 리셋

## 구현 포인트
- 토크 커브 + 8단 자동변속
- 공기저항/구름저항/다운포스
- ABS(락 감지 시 제동 감쇠), TCS(슬립 감지 시 토크 컷)
- 노면 태그(`Road`, `Dirt`, `Wet`) 기반 그립 변화
- 속도 연동 FOV 3인칭 카메라
- HUD 속도/기어/RPM/ABS/TCS 표시

## 주의
- 특정 게임의 로고/맵/UI/사운드/에셋을 복제하지 마세요.
- 상표 차량은 레퍼런스 수준으로만 사용하세요.
