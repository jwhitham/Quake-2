
import pygame, sys, struct


pygame.init()
WIDTH = 400
HEIGHT = 300
screen = pygame.display.set_mode((WIDTH * 3, HEIGHT))
scopy = pygame.Surface((WIDTH * 3, HEIGHT))

reffile = file("debugi386/video.bin", "rb")
infile = file("releasei386/video.bin", "rb")
palette = None
while True:
    ch = infile.read(1)
    ref_data = reffile.read(1)
    if ch == "":
        break
    assert ref_data == ch

    if ch == "P":
        palette = infile.read(256 * 4)
        ref_data = reffile.read(256 * 4)
        assert ref_data == palette

    elif ch == "S":
        width = struct.unpack("<I", infile.read(4))[0]
        height = struct.unpack("<I", infile.read(4))[0]
        assert width == WIDTH
        assert height == HEIGHT
        width = struct.unpack("<I", reffile.read(4))[0]
        height = struct.unpack("<I", reffile.read(4))[0]
        assert width == WIDTH
        assert height == HEIGHT

    elif ch == "F":
        frame = struct.unpack("<I", infile.read(4))[0]
        ref_data = struct.unpack("<I", reffile.read(4))[0]
        assert frame == ref_data
        image = infile.read(WIDTH * HEIGHT)
        ref_data = reffile.read(WIDTH * HEIGHT)

        if image != ref_data:
            pal = []
            for idx in xrange(256):
                r = ord(palette[idx * 4 + 0])
                g = ord(palette[idx * 4 + 1])
                b = ord(palette[idx * 4 + 2])
                pal.append((r, g, b))

            scopy.fill((0, 0, 0))

            idx2 = 0
            count = 0
            for y in xrange(HEIGHT):
                for x in xrange(WIDTH):
                    idx = ord(image[idx2])
                    scopy.set_at((x + WIDTH * 2, y), pal[idx])

                    idx = ord(image[idx2])
                    scopy.set_at((x + WIDTH, y), pal[idx])

                    idx = ord(ref_data[idx2])
                    scopy.set_at((x, y), pal[idx])

                    if image[idx2] != ref_data[idx2]:
                        scopy.set_at((x + WIDTH, y), (255, 255, 255))
                        count += 1

                    idx2 += 1

            print 'DIFFERENCE', frame, count
            screen.blit(scopy, (0, 0))
            #pygame.image.save(scopy, "%05u.png" % frame)
            pygame.display.flip()
            #break
        else:
            print 'OK', frame

    else:
        break

e = pygame.event.wait() 
while e.type != pygame.QUIT:
    e = pygame.event.wait() 

pygame.quit()
sys.exit(0)




