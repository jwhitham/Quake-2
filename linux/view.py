
import pygame, sys


pygame.init()
WIDTH = 400
HEIGHT = 300
screen = pygame.display.set_mode((WIDTH, HEIGHT))
scopy = pygame.Surface((WIDTH, HEIGHT))

infile = file("debugi386/video.bin", "rb")
palette = None
frame = 0
while True:
    ch = infile.read(1)
    if ch == "P":
        palette = infile.read(256 * 4)

    elif ch == "F":

        if frame > 50000:
            pal = []
            for idx in xrange(256):
                r = ord(palette[idx * 4 + 0])
                g = ord(palette[idx * 4 + 1])
                b = ord(palette[idx * 4 + 2])
                pal.append((r, g, b))

            image = infile.read(WIDTH * HEIGHT)
            scopy.fill((0, 0, 0))

            idx2 = 0
            for y in xrange(HEIGHT):
                for x in xrange(WIDTH):
                    idx = ord(image[idx2])
                    scopy.set_at((x, y), pal[idx])
                    idx2 += 1

            print 'frame update', frame
            screen.blit(scopy, (0, 0))
            #pygame.image.save(scopy, "%05u.png" % frame)
            pygame.display.flip()

            e = pygame.event.poll() 
            while e.type != pygame.NOEVENT:
                if e.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit(0)

                e = pygame.event.poll() 

        else:
            infile.seek(WIDTH * HEIGHT, 1)

        frame += 125
    else:
        break

e = pygame.event.wait() 
while e.type != pygame.QUIT:
    e = pygame.event.wait() 

pygame.quit()
sys.exit(0)




