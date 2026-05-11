# Unity 3D 오픈월드 레이싱 프로토타입 — 1단계 실행 프롬프트 (정제본)

아래 프롬프트를 AI 코딩 도구에 그대로 입력하세요.

```text
너는 Unity(C#) 기반의 시니어 게임플레이/차량물리 개발자다.
목표: “현실적인 엑셀·브레이크·타이어 접지감을 가진 고성능 스포츠카 기반 3D 레이싱 프로토타입”을 1단계로 구현한다.

[중요 제약]
- 실제 상표/로고/에셋/사운드 복제 금지.
- Forza Horizon 5는 ‘참고 분위기’만 사용.
- 독자적인 UI, 차량명, 맵 구성으로 구현.

[이번 1단계 범위 (반드시 구현)]
1) 차량 컨트롤러(3인칭 기준)
2) 현실적인 엑셀/브레이크(아날로그 입력 0~1)
3) 자동 변속기(8단), RPM, 기어비, 토크 커브
4) 기본 타이어 접지/슬립(간이 TCS + ABS)
5) 3인칭 추적 카메라(속도 연동 FOV, 가감속 카메라 오프셋)
6) HUD(속도 km/h, 기어, RPM bar, ABS/TCS 상태)
7) 기본 테스트 씬(긴 직선 + 완만한 커브 + 오프로드 구간)
8) 차량 리셋(R 키)

[입력]
- W/RT: throttle
- S/LT: brake/reverse
- A,D/좌우스틱: steer
- Space: handbrake
- LeftShift: sport mode
- C: camera mode cycle
- R: reset

[물리 요구]
- engineForce = throttle * torqueCurve(rpm) * gearRatio * finalDrive * drivetrainEfficiency
- dragForce = Cd * speed^2
- rollingResistance = Crr * speed
- longitudinalAccel = (engineForce - dragForce - rollingResistance - brakeForce) / mass
- brake는 바퀴 잠김 임계 slip에서 ABS 펄싱으로 감쇠
- sport mode에서는 throttle response 증가 + upshift RPM 지연

[권장 기본값]
- mass: 1650kg
- maxTorque: 610Nm
- maxPower: 532hp(토크커브로 근사)
- drivetrain: AWD 기본 (RWD 전환 가능하도록 enum)
- transmission: 8AT
- topSpeed target: 310km/h(드래그/기어비로 근사)
- dragCoefficient: 0.30
- brakeBias: front 0.62 / rear 0.38
- ABS/TCS: On

[코드 구조]
다음 파일을 생성하고 전체 코드를 제공해라.
- Scripts/Vehicle/VehicleController.cs
- Scripts/Vehicle/Powertrain.cs
- Scripts/Vehicle/WheelFrictionModel.cs
- Scripts/Camera/RaceCameraController.cs
- Scripts/UI/HUDController.cs
- Scripts/Game/ResetController.cs
- Scripts/Game/SurfaceGripMap.cs

각 파일마다:
- 어떤 GameObject에 붙이는지
- Inspector에서 설정할 필드
- 서로 참조 연결 방법
을 명확히 설명해라.

[씬 구성]
- Scene: Prototype_DesertRoad
- PlayerCar 프리팹(차체 Rigidbody + WheelCollider 4개 + 시각 휠 메시)
- Road(아스팔트), Dirt(비포장) 콜라이더 분리
- Directional Light + Skybox + 후처리(약한 motion blur)

[완성 기준]
- 정지→100km/h 가속이 즉시 튀지 않고 점진적
- 고속에서 가속 둔화(공기저항 체감)
- 급브레이크 시 감속 안정 + ABS 개입 로그 확인
- HUD에 speed/gear/RPM/ABS/TCS 실시간 반영
- 카메라가 속도에 따라 FOV 상승
- R키로 차량 자세/위치 리셋

[출력 형식]
1. 구현 순서 체크리스트
2. 파일별 전체 C# 코드
3. Unity 에디터 설정 순서(초보자도 따라할 수 있게)
4. 테스트 시나리오 5개(직선가속, 급제동, 코너링, 오프로드, 리셋)
5. 다음 단계(타임어택/드리프트/AI 차량) 확장 포인트
```

---

## 왜 이 버전이 좋은가

- 요구사항 전체는 유지하면서도, **코딩 도구가 바로 실행 가능한 범위(1단계)**로 잘려 있습니다.
- “파일명/오브젝트/인스펙터 설정/테스트”를 강제해 결과물 품질 편차를 줄입니다.
- 법적 리스크(상표/에셋 복제) 방지 조건을 처음부터 명시합니다.
