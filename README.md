# KOREA IT 정적 홈페이지 가이드

이 프로젝트는 **HTML + CSS + JavaScript만으로 만든 정적 홈페이지**입니다.
웹호스팅(카페24, 가비아, AWS S3 정적 호스팅 등)에 그대로 업로드해 사용할 수 있습니다.

## 1) 파일 구조

- `index.html` : 한국어 메인 홈페이지
- `ja/index.html` : 일본어 메인 홈페이지
- `notice/index.html` : 공고 목록 페이지
- `notice/sample-notice.html` : 공고 상세 예시
- `css/style.css` : 전체 스타일
- `js/main.js` : 모바일 메뉴 기능
- `assets/logo.svg` : 로고 파일(교체 가능)
- `assets/symbol.svg` : 심볼 파일(교체 가능)
- `assets/images/` : 일반 이미지 보관
- `assets/notices/` : 공고 첨부 파일 보관(PDF 등)

## 2) 초보자 수정 포인트

### A. 회사 정보 수정
- `index.html` / `ja/index.html` / `notice/index.html`의 footer에서 아래 항목을 수정하세요.
  - 회사명
  - 대표번호
  - 이메일
  - 주소

### B. 메인 문구 수정
- `index.html`의 `<section class="hero">` 내부 문구를 수정하면 한국어 첫 화면 문구가 바뀝니다.
- `ja/index.html`의 같은 영역을 수정하면 일본어 첫 화면 문구가 바뀝니다.

### C. 서비스 항목 수정
- `index.html`의 `id="service"` 섹션에서 `<article class="card">` 블록을 수정/복사하면 서비스 항목을 추가할 수 있습니다.

### D. 공고 추가(정적 게시판 방식)
1. `notice/sample-notice.html` 파일을 복사합니다.
2. 파일명을 예: `notice-2026-01.html` 처럼 변경합니다.
3. 제목/날짜/본문을 수정합니다.
4. `notice/index.html`의 공고 목록 영역에 새 파일 링크를 추가합니다.
5. "현재 등록된 공고가 없습니다." 문구는 공고가 있을 때 삭제하세요.

## 3) 로고/심볼 교체 방법

- `assets/logo.svg` 파일을 새 로고 파일로 교체합니다.
- `assets/symbol.svg` 파일을 새 심볼 파일로 교체합니다.
- 파일명 유지 시 HTML 수정 없이 반영됩니다.

## 4) 배포 방법

1. 전체 파일을 압축(zip)합니다.
2. 호스팅 파일관리자 또는 FTP로 업로드합니다.
3. 루트 경로에 `index.html`이 있는지 확인합니다.
4. `your-domain.com/ja/`, `your-domain.com/notice/` 접속 확인

## 5) 기술 메모

- 외부 프레임워크(React, Next.js) 없이 제작
- 반응형 지원(모바일 메뉴 포함)
- SEO 기본 메타 태그 및 OG 태그 포함
