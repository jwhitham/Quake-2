
import pygame, sys


pygame.init()
screen = pygame.display.set_mode((320, 240))
scopy = pygame.Surface((320, 240))

infile = file("debugi386/video.bin", "rb")
pal = []
frame = 0
while True:
    ch = infile.read(1)
    if ch == "P":
        palette = infile.read(256 * 4)
        pal = []
        for idx in xrange(256):
            r = ord(palette[idx * 4 + 0])
            g = ord(palette[idx * 4 + 1])
            b = ord(palette[idx * 4 + 2])
            pal.append((r, g, b))

    elif ch == "F":
        image = infile.read(320 * 240)
        scopy.fill((0, 0, 0))

        idx2 = 0
        for y in xrange(240):
            for x in xrange(320):
                idx = ord(image[idx2])
                scopy.set_at((x, y), pal[idx])
                idx2 += 1

        screen.blit(scopy, (0, 0))
        pygame.image.save(scopy, "%05u.png" % frame)
        frame += 1
        pygame.display.flip()

        e = pygame.event.poll() 
        while e.type != pygame.NOEVENT:
            if e.type == pygame.QUIT:
                pygame.quit()
                sys.exit(0)

            e = pygame.event.poll() 
    else:
        break

e = pygame.event.wait() 
while e.type != pygame.QUIT:
    e = pygame.event.wait() 

pygame.quit()
sys.exit(0)




