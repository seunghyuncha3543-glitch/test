import sys
import pygame

# 화면 설정
SCREEN_WIDTH = 960
SCREEN_HEIGHT = 540
FPS = 60
TILE_SIZE = 48
GRAVITY = 0.8

# 색상
SKY = (107, 140, 255)
GROUND = (114, 83, 52)
BRICK = (181, 101, 29)
QUESTION = (242, 189, 70)
PIPE = (40, 170, 40)
PLAYER = (230, 60, 60)
ENEMY = (120, 60, 10)


LEVEL_MAP = [
    "................................................................................",
    "................................................................................",
    "................................................................................",
    "................................................................................",
    "................................................................................",
    "...........................Q....................................................",
    "....................B...........................................................",
    "............P...................................................................",
    "................................................................................",
    "................................................................................",
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG",
]


class Block(pygame.sprite.Sprite):
    def __init__(self, x: int, y: int, color: tuple[int, int, int], height: int = TILE_SIZE):
        super().__init__()
        self.image = pygame.Surface((TILE_SIZE, height))
        self.image.fill(color)
        self.rect = self.image.get_rect(topleft=(x, y + (TILE_SIZE - height)))


class Enemy(pygame.sprite.Sprite):
    def __init__(self, x: int, y: int):
        super().__init__()
        self.image = pygame.Surface((TILE_SIZE - 10, TILE_SIZE - 10))
        self.image.fill(ENEMY)
        self.rect = self.image.get_rect(bottomleft=(x + 5, y + TILE_SIZE))
        self.vx = 2

    def update(self, solids: pygame.sprite.Group):
        self.rect.x += self.vx
        for block in solids:
            if self.rect.colliderect(block.rect):
                if self.vx > 0:
                    self.rect.right = block.rect.left
                else:
                    self.rect.left = block.rect.right
                self.vx *= -1


class Player(pygame.sprite.Sprite):
    def __init__(self, x: int, y: int):
        super().__init__()
        self.image = pygame.Surface((TILE_SIZE - 12, TILE_SIZE - 4))
        self.image.fill(PLAYER)
        self.rect = self.image.get_rect(bottomleft=(x + 6, y + TILE_SIZE))
        self.vx = 0
        self.vy = 0
        self.speed = 5
        self.jump_power = 14
        self.on_ground = False

    def handle_input(self):
        keys = pygame.key.get_pressed()
        self.vx = 0
        if keys[pygame.K_LEFT] or keys[pygame.K_a]:
            self.vx = -self.speed
        if keys[pygame.K_RIGHT] or keys[pygame.K_d]:
            self.vx = self.speed
        if (keys[pygame.K_SPACE] or keys[pygame.K_UP] or keys[pygame.K_w]) and self.on_ground:
            self.vy = -self.jump_power
            self.on_ground = False

    def update(self, solids: pygame.sprite.Group):
        self.handle_input()

        # 가로 이동
        self.rect.x += self.vx
        for block in solids:
            if self.rect.colliderect(block.rect):
                if self.vx > 0:
                    self.rect.right = block.rect.left
                elif self.vx < 0:
                    self.rect.left = block.rect.right

        # 세로 이동
        self.vy += GRAVITY
        if self.vy > 18:
            self.vy = 18
        self.rect.y += int(self.vy)

        self.on_ground = False
        for block in solids:
            if self.rect.colliderect(block.rect):
                if self.vy > 0:
                    self.rect.bottom = block.rect.top
                    self.vy = 0
                    self.on_ground = True
                elif self.vy < 0:
                    self.rect.top = block.rect.bottom
                    self.vy = 0


def build_level(map_data: list[str]):
    solids = pygame.sprite.Group()
    decorations = pygame.sprite.Group()
    enemies = pygame.sprite.Group()
    player = None

    for row, line in enumerate(map_data):
        for col, ch in enumerate(line):
            x = col * TILE_SIZE
            y = row * TILE_SIZE

            if ch == "G":
                block = Block(x, y, GROUND)
                solids.add(block)
            elif ch == "B":
                block = Block(x, y, BRICK)
                solids.add(block)
            elif ch == "Q":
                block = Block(x, y, QUESTION)
                solids.add(block)
            elif ch == "P":
                pipe = Block(x, y, PIPE, height=TILE_SIZE)
                solids.add(pipe)
                enemy = Enemy(x + TILE_SIZE * 2, y)
                enemies.add(enemy)

    # 플레이어 시작 위치
    player = Player(2 * TILE_SIZE, 7 * TILE_SIZE)
    return player, solids, decorations, enemies


def draw_background(screen: pygame.Surface):
    screen.fill(SKY)
    pygame.draw.rect(screen, (255, 255, 255), (120, 70, 130, 36), border_radius=18)
    pygame.draw.rect(screen, (255, 255, 255), (380, 100, 165, 44), border_radius=18)
    pygame.draw.rect(screen, (255, 255, 255), (690, 60, 140, 40), border_radius=18)


def main():
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Python Mario 1 Prototype")
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("malgungothic", 24)

    player, solids, decorations, enemies = build_level(LEVEL_MAP)
    camera_x = 0

    running = True
    while running:
        dt = clock.tick(FPS)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False

        player.update(solids)
        enemies.update(solids)

        # 적과 충돌 시 리스폰
        if pygame.sprite.spritecollide(player, enemies, dokill=False):
            player.rect.bottomleft = (2 * TILE_SIZE, 8 * TILE_SIZE)
            player.vx = 0
            player.vy = 0

        camera_x = max(0, player.rect.centerx - SCREEN_WIDTH // 3)

        draw_background(screen)

        for sprite in solids:
            screen.blit(sprite.image, (sprite.rect.x - camera_x, sprite.rect.y))

        for sprite in enemies:
            screen.blit(sprite.image, (sprite.rect.x - camera_x, sprite.rect.y))

        screen.blit(player.image, (player.rect.x - camera_x, player.rect.y))

        guide = font.render("←/→ 이동, Space 점프, ESC 종료", True, (0, 0, 0))
        screen.blit(guide, (20, 16))

        pygame.display.flip()

    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
