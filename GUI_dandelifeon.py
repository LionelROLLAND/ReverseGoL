import pygame
import random as rd
import sys
import subprocess
import Game_of_life as GL
import edit

Name = "Le_Dandi"




def take_arg():
	#Name = input("Nom de fichier ?\n")
	#Name = "Prototype"
	in_file = Name + "_in.txt"
	out_file = Name + "_out.txt"
	pred_file = Name + "_pred.txt"

	Opt = input("Carte (C)/Retour (R) ?\n")
	nb = -1
	if Opt == "R":
		nb = input("Nb de retours ? (min. 1)\n")
	return in_file, out_file, pred_file, int(nb)



def compute(in_file, out_file, pred_file, nb, prio_file = ""):
	arg_list = ["./Dandelifeon", "-i", in_file, "-o", out_file, "-p", pred_file]
	if prio_file:
		arg_list += ["-l", prio_file]

	if nb > 0:
		arg_list.append("-R")
	else:
		arg_list.append("-C")
	subprocess.run(arg_list)



#Appelle le programme en C n fois en gerant les fichiers pour remonter n fois dans les etapes :
def result(in_file, out_file, pred_file, n, prio_file = ""):
	tab = []
	temp = []
	for i in range(n):
		compute(in_file, out_file, pred_file, n, prio_file)
		tab = edit.init_grid(out_file)
		if tab == []:
			print("Stopped at " + str(i+1))
			return temp
		temp = edit.init_grid(out_file)
		edit.save_grid(tab, in_file)
	return tab


def verif(tab, n): #Derniere etape du programme, il recalcule lui-meme les successeurs de n-k et les affiche
	t = GL.tableau(tab, 0, 0, 0)
	N = GL.norm(GL.etapes(t,n))
	edit.affiche_list(N)


succ = []
width_array = 0
height_array = 0
if len(sys.argv) >= 2:
	succ = edit.init_grid(sys.argv[1])
	width_array = len(succ)
	height_array = len(succ[0])
else:
	width_array = int(input("Width ?\n"))
	height_array = int(input("Height ?\n"))
	succ = edit.init_grid("", "blank", width_array, height_array)

pred = []

if len(sys.argv) >= 3:
	pred = edit.init_grid(sys.argv[2])
else:
	pred = edit.init_grid("", "unknown", width_array, height_array)

edit.edit_grid(succ, 3)

edit.edit_grid(pred, 3)

in_file, out_file, pred_file, nb = take_arg()

edit.save_grid(succ, in_file)
edit.save_grid(pred, pred_file)

prio = ""

if len(sys.argv) >= 4:
	prio = sys.argv[3]

res = result(in_file, out_file, pred_file, nb, prio)

if res == []:
	print("Aucun résultat :'(")
else:
	verif(res, nb)


