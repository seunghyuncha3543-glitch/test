# Unity 레이싱 프로토타입 테스트 가이드

## 1) 씬 준비
1. Unity에서 빈 3D 프로젝트 생성 (URP 권장).
2. `Scripts/` 폴더를 프로젝트 `Assets/` 아래로 복사.
3. `Prototype_DesertRoad` 씬 생성.
4. Plane/ProBuilder 등으로 도로를 만들고 태그를 지정:
   - 아스팔트: `Road`
   - 비포장: `Dirt`
   - 젖은노면: `Wet`
5. `PlayerCar` 오브젝트 생성:
   - `Rigidbody`
   - `WheelCollider` 4개 (fl, fr, rl, rr)
   - `VehicleController` 부착 후 WheelCollider 참조 연결
6. 빈 오브젝트 `GameSystems` 생성:
   - `SurfaceGripMap`
   - `ResetController` 부착
7. 카메라 설정:
   - `Main Camera`에 `RaceCameraController` 부착
   - target = PlayerCar, vehicle = VehicleController
8. HUD 설정:
   - Canvas + TMP_Text(speed/gear/abs/tcs) + Slider(rpm)
   - `HUDController`에 UI 참조 연결

## 2) 플레이 입력
- W/S: 가속/감속(저속에서 S는 후진)
- A/D: 조향
- Space: 핸드브레이크
- LeftShift: 스포츠 모드
- R: 리셋

## 3) 기능별 테스트 시나리오

### A. 직선 가속 테스트
- 도로 직선 구간에서 W를 10초 이상 유지.
- 기대 결과:
  - 기어가 자동으로 상승.
  - RPM이 오르내리며 변속 시 순간적으로 토크가 끊기는 느낌.
  - 속도 증가가 고속으로 갈수록 둔화.

### B. 급제동 + ABS 테스트
- 120km/h 이상 가속 후 S를 강하게 입력.
- 기대 결과:
  - 감속 중 HUD의 ABS가 간헐적으로 `ABS ON`.
  - 조향 안정성이 완전히 사라지지 않고 제동.

### C. TCS 테스트
- 저속에서 핸들을 꺾은 상태로 W를 급입력.
- 기대 결과:
  - 과도 슬립 시 HUD에 `TCS ON` 표시.
  - 가속 토크가 일시적으로 줄어드는 느낌.

### D. 노면 그립 테스트
- `Road` -> `Dirt` -> `Wet` 순서로 통과.
- 기대 결과:
  - Dirt에서 횡미끄러짐 증가.
  - Wet에서 Road 대비 그립 감소.

### E. 카메라/FOV 테스트
- 저속/고속 반복.
- 기대 결과:
  - 고속일수록 FOV 증가.
  - 급가속 시 카메라가 살짝 뒤로, 급감속 시 앞으로 이동.

### F. 리셋 테스트
- 차량을 회전시키거나 도로 밖으로 이탈 후 R 입력.
- 기대 결과:
  - resetPoint 위치/회전으로 즉시 복귀.
  - 속도 및 각속도 0으로 초기화.

## 4) 튜닝 체크(Inspector)
- `VehicleController`
  - maxBrakeTorque, brakeBiasFront, dragCoefficient, downforceCoefficient
- `Powertrain`
  - gearRatios, shiftUpRpm/DownRpm, drivetrainEfficiency
- `WheelFrictionModel`
  - dirtGrip, wetGrip, tcsSlipThreshold, absSlipThreshold

변수를 조절한 뒤 같은 시나리오를 반복해 비교하면 주행 감각 튜닝이 빠르게 됩니다.

## 5) 자주 발생하는 문제
- 차가 안 움직임: WheelCollider 참조 누락/레이어 충돌 설정 확인.
- HUD 미표시: TMP 참조 누락, Slider 연결 누락 확인.
- ABS/TCS가 항상 꺼짐: 테스트 속도/슬립 부족 가능, 임계값을 낮춰 검증.
- 리셋 불가: `ResetController.resetPoint` 미지정 확인.
