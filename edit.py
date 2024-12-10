import pygame
import sys
import random as rd

sq_size = 40

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GRAY = (128, 128, 128)
ROSE = (255, 60, 137)




def init_grid(fname = "", mode = "", width = 0, height = 0): #Initialise la grille à partir d'un fichier ou de rien
	tab = []
	if fname:
		with open(fname, "r") as fd:
			for l in fd:
				tab.append([])
				numb, sc, rest = l.partition(";") #Cette methode est putain de magique c'est trop la vie
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



def edit_grid(tab, modulo):

	height_array = len(tab)
	width_array = len(tab[0])

	pygame.init()

	size = [width_array*sq_size, height_array*sq_size]

	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Je me douche au mana haaaaaaan")

	playing = True
	clock = pygame.time.Clock()

	while playing:
		clock.tick(30)
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				playing = False
			elif event.type == pygame.MOUSEBUTTONUP:
				if event.button == 1:
					x, y = pygame.mouse.get_pos()
					nx, ny = x//sq_size, y//sq_size
					tab[ny][nx] = (tab[ny][nx] + 1)%modulo


		screen.fill(GRAY)
		for i in range(width_array):
			for j in range(height_array):
				if tab[j][i] == 0:
					pygame.draw.rect(screen, WHITE, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
				elif tab[j][i] == 1:
					pygame.draw.rect(screen, BLACK, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
				else:
					pygame.draw.rect(screen, ROSE, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
		pygame.display.flip()

	pygame.quit()



def save_grid(tab, fname):
	final = ""
	for l in tab:
		for e in l:
			final += str(e) + ";"
		final += "\n"

	with open(fname, "w") as fd:
		fd.write(final)





def affiche(tab):
	pygame.init()

	height_array = len(tab)
	width_array = len(tab[0])

	size = [width_array*sq_size, height_array*sq_size]

	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Je me douche au mana haaaaaaan")

	playing = True
	clock = pygame.time.Clock()

	while playing:
		clock.tick(30)
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				playing = False

		screen.fill(GRAY)
		for i in range(width_array):
			for j in range(height_array):
				if tab[j][i] == 0:
					pygame.draw.rect(screen, WHITE, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
				elif tab[j][i] == 1:
					pygame.draw.rect(screen, BLACK, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
				else:
					pygame.draw.rect(screen, ROSE, [i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
		pygame.display.flip()

	pygame.quit()

def affiche_mod(tab): #affichage prevu pour le mode C du programme
	m = tab[0][0]
	for line in tab:
		for e in line:
			if e > m:
				m = e

	pygame.init()

	height_array = len(tab)
	width_array = len(tab[0])

	size = [width_array*sq_size, height_array*sq_size]

	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Je me douche au mana haaaaaaan")

	playing = True
	clock = pygame.time.Clock()

	while playing:
		clock.tick(30)
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				playing = False

		screen.fill(GRAY)
		for i in range(width_array):
			for j in range(height_array):
				pygame.draw.rect(screen, (254*tab[j][i]//m, 254*tab[j][i]//m, 254*tab[j][i]//m),
					[i*sq_size + 1, j*sq_size + 1, sq_size - 2, sq_size - 2])
		pygame.display.flip()

	pygame.quit()




def affiche_list(tab_list): #Affiche la liste de tableau passee en argument -> tu cliques ca passe au suivant
	n = 0
	tab = tab_list[n]
	height_array = len(tab)
	width_array = len(tab[0])

	pygame.init()

	size = [width_array*sq_size, height_array*sq_size]

	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Vérification")

	playing = True
	clock = pygame.time.Clock()

	while playing:
		clock.tick(30)
		tab = tab_list[n]
		screen.fill(GRAY)
		for i in range(width_array):
			for j in range(height_array):
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
			elif event.type == pygame.MOUSEBUTTONUP:
				if event.button == 1:
					n = (n + 1) %len(tab_list)

	pygame.quit()




if __name__ == "__main__":
	if len(sys.argv) >= 2:
		succ = init_grid(sys.argv[1])
		width_array = len(succ)
		height_array = len(succ[0])
	else:
		mode = input("Mode ?\n")
		width_array = int(input("Width ?\n"))
		height_array = int(input("Height ?\n"))
		succ = init_grid("", mode, width_array, height_array)
	edit_grid(succ, 3)

	out_file = input("Filename ?\n")
	if out_file:
		save_grid(succ, out_file)