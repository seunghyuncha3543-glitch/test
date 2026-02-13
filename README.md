# Python3로 슈퍼마리오1 스타일 프로토타입

이 저장소는 `pygame`으로 만든 **슈퍼마리오1 느낌의 2D 횡스크롤 플랫폼 게임 기본형**입니다.

## 실행 방법

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install pygame
python3 mario1.py
```

## 조작

- `←/→` 또는 `A/D`: 이동
- `Space` 또는 `↑/W`: 점프
- `ESC`: 종료

## 구현된 핵심 요소

- 중력 + 점프
- 타일 충돌 처리 (바닥/브릭/물음표 블록/파이프)
- 간단한 적(좌우 왕복)
- 카메라 스크롤

## 다음 단계 추천

1. 스프라이트 이미지 적용
2. 코인/아이템 시스템
3. 스테이지 데이터 파일 분리 (JSON/TMX)
4. 점수/목숨/UI
5. 사운드(BGM/SFX)
