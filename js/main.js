// 모바일 메뉴 토글 기능
// - 초보자 수정 포인트: 아래 선택자는 HTML 클래스명과 맞아야 동작합니다.
(function () {
  const toggleButton = document.querySelector('.menu-toggle');
  const gnb = document.querySelector('.gnb');

  if (!toggleButton || !gnb) return;

  // 메뉴 버튼 클릭 시 열림/닫힘 처리
  toggleButton.addEventListener('click', function () {
    const isOpen = gnb.classList.toggle('open');
    toggleButton.setAttribute('aria-expanded', String(isOpen));
  });

  // 메뉴 링크 클릭 후 모바일 메뉴 자동 닫기
  gnb.querySelectorAll('a').forEach(function (link) {
    link.addEventListener('click', function () {
      gnb.classList.remove('open');
      toggleButton.setAttribute('aria-expanded', 'false');
    });
  });
})();
