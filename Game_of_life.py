#!/usr/bin/env python3
# -*- coding: utf-8 -*-


#import matplotlib.pyplot as plt
#import matplotlib.animation as am

class tableau(): #Pratique pour s'affranchir des bords des tableaux
	
	def __init__(self, tab, d, x_b = 0, y_b = 0):
		self.d_val = d
		self.x_b = x_b
		self.y_b = y_b
		self.w = len(tab[0])
		self.h = len(tab)
		self.id = tab
	
	#def show(self, inter = 'nearest'):
	#	plt.figure()
	#	plt.imshow(self.id, cmap = 'gray', interpolation = inter)
	#	plt.pause(0.1)
	
	def copy(self):
		temp = [[self.id[i][j] for j in range(self.w)] for i in range(self.h)]
		return tableau(temp, self.d_val, self.x_b, self.y_b)
	
	def reverse_y(self):
		temp = []
		for i in range(self.h):
			temp.append(self.id[self.h - i - 1])
		self.id = temp
	
	def add_c(self, c):
		if c>0:
			for j in range(self.h):
				self.id[j] = self.id[j] + [self.d_val]
		else:
			for j in range(self.h):
				self.id[j] = [self.d_val] + self.id[j]
			self.x_b -= 1
		self.w += 1
	
	def add_l(self, c):
		if c>0:
			self.id = self.id + [[self.d_val for i in range(self.w)]]
		else:
			self.id = [[self.d_val for i in range(self.w)]] + self.id
			self.y_b -= 1
		self.h += 1
	
	def elt(self,x,y):
		if x<self.x_b or y<self.y_b or x>=self.x_b+self.w or y>=self.y_b+self.h:
			return self.d_val
		else:
			return self.id[y-self.y_b][x-self.x_b]
	
	def chg_elt(self,x,y,e):
		if x<self.x_b or x>=self.x_b+self.w:
			self.add_c(x-self.x_b)
			self.chg_elt(x,y,e)
		elif y<self.y_b or y>=self.y_b+self.h:
			self.add_l(y-self.y_b)
			self.chg_elt(x,y,e)
		else:
			self.id[y-self.y_b][x-self.x_b] = e
	
	def include(self,left,right,bottom,top):
		self.chg_elt(left,bottom,self.elt(left,bottom))
		self.chg_elt(right,top,self.elt(right,top))


#def video(l, ms):
#	fig, ax = plt.subplots()
#	ax.imshow(l[0],cmap='gray')
#	ims = [[ax.imshow(l[i],cmap='gray',animated=True)] for i in range(len(l))]
#	ani = am.ArtistAnimation(fig, ims, interval=ms, blit=True, repeat_delay=5000)
#	plt.show()













def vois(tab,i,j): #Retourne le nb de voisins vivants
	S = -tab.elt(i,j)
	for k in range(3):
		for l in range(3):
			S += tab.elt(i+k-1,j+l-1)
	return S


def vie(tab,i,j): #True si est en vie a l'etape d'apres, False sinon
	if tab.elt(i,j) == 0:
		if vois(tab,i,j) == 3:
			return 1
		else:
			return 0
	else:
		if vois(tab,i,j) == 2 or vois(tab,i,j) == 3:
			return 1
		else:
			return 0

def etape(tab):
	height = tab.h
	width = tab.w
	result = tab.copy()
	u = 0
	for i in range(height):
		for j in range(width):
			u = vie(tab,i,j)
			result.chg_elt(i,j,u)
	return result



def etapes(tab,k): #Retourne les tableaux correspondants a k etapes du jeu de la vie + l'etape initiale
	height = tab.h
	width = tab.w
	temp = tab.copy()
	R = [tab.copy()]
	u = 0
	for l in range(k):
		temp = etape(temp)
		R.append(temp.copy())
	return R


def norm(R): #Retourne la liste d'arrays a partir de la liste de tableaux
	return [e.id for e in R]






if __name__ == "__main__":
	import pygame
	import edit
	import sys
	init_tab = edit.init_grid(sys.argv[1])
	tab = tableau(init_tab, 0)

	pygame.init()
	sq = edit.sq_size

	size = [tab.w*sq, tab.h*sq]

	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Vérification")

	playing = True
	clock = pygame.time.Clock()

	while playing:
		clock.tick(30)
		screen.fill(edit.GRAY)
		for i in range(tab.w):
			for j in range(tab.h):
				if (tab.id)[j][i] == 0:
					pygame.draw.rect(screen, edit.WHITE, [i*sq + 1, j*sq + 1, sq - 2, sq - 2])
				elif (tab.id)[j][i] == 1:
					pygame.draw.rect(screen, edit.BLACK, [i*sq + 1, j*sq + 1, sq - 2, sq - 2])
				else:
					pygame.draw.rect(screen, edit.ROSE, [i*sq + 1, j*sq + 1, sq - 2, sq - 2])
		pygame.display.flip()

		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				playing = False
			elif event.type == pygame.MOUSEBUTTONUP:
				if event.button == 1:
					tab = etape(tab)

	pygame.quit()










#t = tableau([[0]],0,1,1)
#t.chg_elt(25,25,0)

#l = [[3,3],[3,4],[2,4]]
#l = []
#x = 2
#y = 2

#for i in range(3):
#	l+= [[x+6*i,y],[x+1+6*i,y],[x+2+6*i,y+1],[x+2+6*i,y+2],[x+3+6*i,y+3],[x+4+6*i,y+3],[x+5+6*i,y+2],[x+5+6*i,y+1]]
"""l += [[x+18,y]]
for i in range(5):
	l.append([x+19,y+i+1])
l.append([x+20,y+3])
l += [[x+20,y+6],[x+21,y+6],[x+21,y+5]]
"""

#for e in l:
#	t.chg_elt(e[0],e[1],1)

#N = norm(etapes(t,30))
#video(N,4000)
#plt.figure()
#for I in N:
#	plt.imshow(I)
#	plt.pause(0.1)
#	input()
