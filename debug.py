import pygame
import sys


sq_size = 40
width_array = 21
height_array = 21

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GRAY = (128, 128, 128)
ROSE = (255, 60, 137)

def init_grid(fname = "", mode = "", width = 0, height = 0):
	tab = []
	if fname:
		with open(fname, "r") as fd:
			for l in fd:
				tab.append([])
				numb, sc, rest = l.partition(";")
				while rest:
					tab[-1].append(int(numb))
					numb, sc, rest = rest.partition(";")
	else:
		if mode == "unknown":
			tab = [[2 for i in range(width)] for j in range(height)]
		elif mode == "blank":
			tab = [[0 for i in range(width)] for j in range(height)]
		else:
			tab = [[rd.randint(0,1) for i in range(width)] for j in range(height)]
	return tab


def affiche(tab, file_flow):
	pygame.init()
	height = len(tab)
	width = len(tab[0])

	size = [width*sq_size, height*sq_size]

	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Vérification")

	playing = True
	clock = pygame.time.Clock()

	while playing:
		line = (file_flow.readline()).strip() #A continuer
		x, nothing, line = line.partition(";")
		y, nothing, state = line.partition(";")
		if state:
			tab[int(y)][int(x)] = int(state)
		clock.tick(50)
		screen.fill(GRAY)
		for i in range(width):
			for j in range(height):
				if tab[j][i] == 0:
					pygame.draw.rect(screen, WHITE, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
				elif tab[j][i] == 1:
					pygame.draw.rect(screen, BLACK, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
				else:
					pygame.draw.rect(screen, ROSE, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
		pygame.display.flip()

		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				playing = False

	pygame.quit()


with open(sys.argv[1], "r") as fd:
	our_tab = init_grid("", "unknown", width_array, height_array)
	affiche(our_tab, fd)
