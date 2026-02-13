from __future__ import annotations

import argparse
import datetime as dt
import random
import re
from collections import Counter
from typing import Dict, List

ELEMENTS = ["wood", "fire", "earth", "metal", "water"]

STEMS = {
    0: "metal",  # 경
    1: "metal",  # 신
    2: "water",  # 임
    3: "water",  # 계
    4: "wood",  # 갑
    5: "wood",  # 을
    6: "fire",  # 병
    7: "fire",  # 정
    8: "earth",  # 무
    9: "earth",  # 기
}

BRANCHES = {
    0: "water",  # 자
    1: "earth",  # 축
    2: "wood",  # 인
    3: "wood",  # 묘
    4: "earth",  # 진
    5: "fire",  # 사
    6: "fire",  # 오
    7: "earth",  # 미
    8: "metal",  # 신
    9: "metal",  # 유
    10: "earth",  # 술
    11: "water",  # 해
}

NUMBER_GROUPS = {
    "wood": list(range(1, 10)),
    "fire": list(range(10, 19)),
    "earth": list(range(19, 28)),
    "metal": list(range(28, 37)),
    "water": list(range(37, 46)),
}


def _weekday_seed(today: dt.date | None = None) -> int:
    today = today or dt.date.today()
    days_until_friday = (4 - today.weekday()) % 7
    friday = today + dt.timedelta(days=days_until_friday)
    return int(friday.strftime("%Y%m%d"))


def _saju_element_score(birth: dt.date, hour: int) -> Counter:
    scores: Counter = Counter()

    year_stem = STEMS[(birth.year - 4) % 10]
    year_branch = BRANCHES[(birth.year - 4) % 12]
    month_stem = STEMS[(birth.year * 12 + birth.month) % 10]
    month_branch = BRANCHES[(birth.month + 1) % 12]
    day_stem = STEMS[(birth.toordinal() + 6) % 10]
    day_branch = BRANCHES[(birth.toordinal() + 8) % 12]
    hour_branch = BRANCHES[((hour % 24) // 2) % 12]
    hour_stem = STEMS[(birth.toordinal() * 12 + hour) % 10]

    for el in [
        year_stem,
        year_branch,
        month_stem,
        month_branch,
        day_stem,
        day_branch,
        hour_stem,
        hour_branch,
    ]:
        scores[el] += 1

    return scores


def _weighted_pool(scores: Counter) -> List[int]:
    if not scores:
        scores = Counter({el: 1 for el in ELEMENTS})

    min_score = min(scores.get(el, 0) for el in ELEMENTS)
    normalized: Dict[str, int] = {
        el: max(1, scores.get(el, 0) - min_score + 1) for el in ELEMENTS
    }

    pool: List[int] = []
    for el, weight in normalized.items():
        pool.extend(NUMBER_GROUPS[el] * (weight * 3))

    return pool


def _parse_birth_date(value: str) -> dt.date:
    """지원 형식: YYYY-MM-DD / YYYY.MM.DD / YYYY/MM/DD / YYYY / 1990년."""
    raw = value.strip()

    year_only = re.fullmatch(r"(\d{4})\D*", raw)
    if year_only:
        return dt.date(int(year_only.group(1)), 1, 1)

    normalized = raw.replace(".", "-").replace("/", "-")
    try:
        return dt.datetime.strptime(normalized, "%Y-%m-%d").date()
    except ValueError as exc:
        raise ValueError("출생일은 YYYY-MM-DD(또는 YYYY) 형식으로 입력해주세요.") from exc


def _parse_birth_hour(value: str) -> int:
    """지원 형식: 0~23, 13시 등 숫자 포함 입력."""
    raw = value.strip()
    match = re.search(r"\d{1,2}", raw)
    if not match:
        raise ValueError("출생 시각에서 숫자를 찾을 수 없습니다.")

    hour = int(match.group())
    if not (0 <= hour <= 23):
        raise ValueError("출생 시각은 0~23 사이여야 합니다.")
    return hour


def generate_lotto_patterns(
    birth_date: str,
    birth_hour: int,
    patterns: int = 5,
    numbers_per_pattern: int = 6,
) -> List[List[int]]:
    birth = _parse_birth_date(birth_date)
    if not (0 <= birth_hour <= 23):
        raise ValueError("출생 시각은 0~23 사이여야 합니다.")

    seed = _weekday_seed() + int(birth.strftime("%Y%m%d")) + birth_hour
    rng = random.Random(seed)

    scores = _saju_element_score(birth, birth_hour)
    pool = _weighted_pool(scores)

    result: List[List[int]] = []
    seen = set()

    while len(result) < patterns:
        picked = sorted(set(rng.choices(pool, k=20)) | set(rng.sample(range(1, 46), 3)))
        if len(picked) < numbers_per_pattern:
            continue

        final = tuple(sorted(rng.sample(picked, numbers_per_pattern)))
        if final in seen:
            continue

        seen.add(final)
        result.append(list(final))

    return result


def _interactive_input() -> tuple[str, int]:
    while True:
        try:
            birth_date = input("출생일(YYYY-MM-DD) 또는 출생년도(YYYY): ").strip()
            birth_hour_input = input("출생 시각(0~23, 예: 13 또는 13시): ").strip()

            _parse_birth_date(birth_date)
            birth_hour = _parse_birth_hour(birth_hour_input)
            return birth_date, birth_hour
        except ValueError as exc:
            print(f"입력 오류: {exc}")
            print("다시 입력해주세요.\n")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="사주 오행 가중치를 반영한 주간 로또 번호 5패턴 생성기"
    )
    parser.add_argument("--birth-date", help="출생일 (YYYY-MM-DD 또는 YYYY)")
    parser.add_argument("--birth-hour", help="출생 시각 (0~23, 또는 13시)")
    return parser.parse_args()


def main() -> None:
    try:
        args = parse_args()

        if args.birth_date is None or args.birth_hour is None:
            birth_date, birth_hour = _interactive_input()
        else:
            birth_date = args.birth_date
            birth_hour = _parse_birth_hour(args.birth_hour)

        patterns = generate_lotto_patterns(birth_date, birth_hour)
        print("이번 주 금요일 기준 추천 번호 5패턴")
        for i, nums in enumerate(patterns, start=1):
            print(f"패턴 {i}: {nums}")
    except (ValueError, EOFError) as exc:
        print(f"오류: {exc}")


if __name__ == "__main__":
    main()
