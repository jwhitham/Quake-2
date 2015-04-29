
import pygame, sys


pygame.init()
screen = pygame.image.load("colormap.png")
image = []
for y in xrange(320):
	line = []
	image.append(line)
	for x in xrange(256):
		(r, g, b, a) = screen.get_at((x, y))
		line.append((r, g, b))

file("color.py", "wt").write("color = " + repr(image) + "\n")

pygame.quit()




