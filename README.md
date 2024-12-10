###################    Dandelifeon -- Le programme    ###################

1. Le but
2. L'idee
3. Comment on le fait marcher
4. Pour les curieux






########### Le but ############

Le but de ce programme est de trouver l'étape n-k du jeu de la vie en donnant en argument l'étape n :

L'étape n peut être constituée de cellules vivantes (= 1 = noires dans mon programme), mortes (= 0 = blanches dans mon programme), ou cellules dont on s'en bat les couilles si elles sont vivantes ou mortes (= 2 = roses dans mon programme)


Ensuite, le programme calcule les étapes précédentes jusqu'à arriver à l'étape n-k, qu'il renvoie (On ne renvoie pas les autres car on peut les retrouver facilement à partir de la n-k).

S'il n'arrive pas à calculer l'étape n-1, il le signale.

S'il ne peut pas calculer au-delà de n-k' avec k' < k, il le signale et renvoie l'étape n-k'.

A noter que dans certains cas, il ne finit jamais car bien que ce soit censé être un backtracking accéléré, parfois il n'est juste pas assez fin pour que l'accélération soit suffisante pour que le programme termine.

Ressources nécessaires : L'usage de la mémoire vive est probablement en O(n) ou n est le nombre de points. A titre d'information, pour la partie importante du programme (l'exécutable) c'est inférieur à 1 Mo pour 625 points (25*25). Pour le script python c'est pas monstrueux non plus.




########### L'idée ############



En fait, c'est juste un backtracking particulier :

Dans un backtracking "de base", on prendrait la première cellule de ce qu'on espère être la grille de l'étape n-1, on essaierait de la mettre à 0, on prend la deuxième, on fait pareil, et dès que ça ne marche plus on remonte, on essaie de fixer la cellule précédente à 1, etc. 
Et ici, "ça marche plus" = il y a une cellule dans le tableau, qui a été fixée à 0 ou 1, dont toutes les voisines ont été fixées à 0 ou 1, et les règles du jeu de la vie impose que son successeur n'est pas égal à celui de l'étape n.

À ce système j'ai apporté 3 améliorations :

1. Un mécanisme qui détecte certains cas où on n'a pas le choix pour l'état d'une cellule : Les cas détectés sont :
a. Au vu de l'état des voisines d'une cellule c, on est forcé de la mettre dans un certain état pour que son successeur soit effectivement celui qu'on a fixé pour l'étape n.
b. Au vu de l'état d'une cellule c et de l'état de ses voisines, on n'a plus le choix pour ses voisines qui n'ont pas encore été fixées : Typiquement, une cellule qui a 1 pour successeur a forcément moins de 3 cellules voisines à 1. On en déduit que, si une cellule a 1 pour successeur, et déjà 3 voisines à 1, toutes ses autres voisines pas encore fixées doivent être fixées à 0.

Ce mécanisme permet de remplir plusieurs cases sans réfléchir et de vérifier ensuite si ça marche.

2. Un système qui détecte les conflits entre la grille qu'on remplit et le mécanisme précédent : Typiquement si on voit une cellule ayant 1 pour successeur mais plus de 4 voisines à 1, on crie -> On dit que ça marche plus, ce qui fait un "ça marche plus" qui crie beaucoup plus tôt que dans le backtracking de base
Il détecte aussi les conflits inhérents au mécanisme précédent : Si à cause d'une voisine une cellule doit être fixée à 1 et à cause d'une autre à 0, il crie.

3. Tout cela s'articule avec un système de priorités : Selon un certain nombre de critères, je fixe une certaine priorité à chaque cellule. Je l'ai pensée de telle sorte que ce soit à peu près représentatif de l'inverse du nombre de cellules qu'on peut encore fixer de manière totalement libre dans un carré 5*5 autour de la cellule avant que l'état de ladite cellule soit forcé par le premier mécanisme. Ceci dit c'est pas exactement ça non plus et j'ai du mal à voir exactement à quoi ça correspond mais c'était mon intuition.
Du coup, si l'état d'une cellule est forcé, elle obtient une priorité infinie.
Et en fait il a été assez simple de faire en sorte que quand le mécanisme 2 crie, la priorité de la cellule en question vaut -1.

Du coup je présente ça comme le 3ème mécanisme parce qu'il faut au moins le premier pour le comprendre, mais en réalité dans la chronologie du programme c'est de lui que tout part.

Et du coup, je trie ensuite les points par priorité décroissante, et je traite le premier de la liste. Et comme du coup on part des points théoriquement les plus chiants à fixer, ça doit éviter qu'on ait besoin de retourner en arrière dans le backtracking alors qu'on est en train de traiter les tout derniers points.

Comment est calculée précisément cette priorité :

Pour la cellule c, on introduit a = le nombre de voisines qu'on peut fixer totalement librement (à 0 ou 1) jusqu'à ce que l'état de c soit forcé par le mécanisme 1. a.
a n'a plus de sens une fois que la cellule est fixée.
On introduit c1 = 1/a.

On introduit cc = le nombre de voisines de c qu'on peut fixer totalement librement jusqu'à ce que l'état des voisines non fixées de c soit forcé par le mécanisme 1.b.

Prenons ensuite une cellule c, et disons que d1, d2, d3,... d8 sont ses voisines, chacune avec des nombres c1 et cc associés, noté c1(d1), c1(d2),...,c1(d8),cc(d1), cc(d2),...,cc(d8)

Si cc(d1),...,cc(d8) et c1(c) sont tous différents de -1 (cas où on a détecté un pb inhérent à ces nombres là, typiquement plus de 4 voisines vivantes et successeur = 1 => cc = -1), on note cf = c1(c) + 1/cc(d1) + 1/cc(d2) + ... + 1/cc(d8)
(On change rien si l'un d'entre eux est égal à 0 : On le fait à la physicienne avec 1/0 = infini)

Et si l'un d'entre eux est égal à -1 on fixe cf à -1.
Et voilà, cf c'est notre priorité !

Le calcul de c1 et cc c'était très chiant par contre, voir le code.




Sur le reste de l'algo :
J'ai cherché à optimiser partout : Typiquement, quand je change l'état d'une cellule, ça change le nombre de voisines de certaines cellules, ça change les c1, cc et cf d'autres,... Mais ! Je ne recalcule que ce qu'il y a à recalculer, je refais pas le taff pour toutes la grille.

Aussi : Typiquement quand on a plusieurs points à priorité infinie, on les fixe en un paquet, mais il ne faut pas recalculer les priorités entre : effectivement si par exemple on a 4 cellules à priorité infinie, on risque de recalculer des cc, de c1 et plein d'autres trucs plusieurs fois.

Exemple : Les 4 cellules sont voisines d'une cellule c.
Du coup peut-être que cf(c) va faire :
cf = 2
cf = 4
cf = 18
cf = 29
Alors que si on fait tout d'un coup et on recalcule ensuite, on va juste avoir cf = 29
Donc voilà en gros on fait des choses inutiles si on fixe plusieurs cellules proches les unes des autres (Bien que ça pourrait éventuellement détecter un problème plus tôt et nous éviter de revenir en arrière et "réinitialiser" ces 4 cellules).

Là c'est pour illustrer, mais en gros le truc c'est que calculer cc, ou cf c'est coûteux, du coup j'essaie au maximum de regrouper les calculs successifs de ces trucs, quitte à m'enfoncer un peu trop loin et à tout remonter juste après : Il me semble qu'en moyenne on y gagne mais faut que j'explique ça en vrai parce que par écrit ça serait laborieux.

Et du coup je regroupe ces calculs quand je m'enfonce (en fixant plusieurs cellules d'un coup), mais aussi quand je remonte (quand je réinitialise des cellules).

En plus de ça, quand on recalcule cf il faut retrier les points, et pareil c'est coûteux, du coup je préfère ne pas retrier le même point plusieurs fois pour rien.

C'est à peu près tout pour le fonctionnement de l'algo.


########### Comment on le fait marcher ############

1ère étape :
Installer python et pygame si besoin, s'assurer que python est dans la variable PATH de l'OS

Puis, au plus simple :

Ouvrir Dandelifeon/Dev/ dans un terminal
Lancer la commande suivante :
python3 GUI_Dandelifeon.py

Il demandera alors via la CLI d'entrer les dimensions de la grille souhaitées.

Une fois fait, une grille apparaît. Il faut y rentrer l'étape n à laquelle on veut arriver, en cliquant sur les cases pour changer leur état, puis fermer la fenêtre.

Apparaît ensuite une grille pour l'étape n-1, dans laquelle on peut prédifinir certains points : Si on veut une étape n-1 quelconque, on peut laisser tout en rose (= indéfini) mais si par exemple on veut contraindre l'étape n-1 (au hasard, que le carré 3*3 du milieu soit entièremen composé de cellules mortes), on fixe ces contraintes (donc par exemple mettre le carré 3*3 du centre en blanc). A noter que dans le cas de plusieurs retourrs consécutifs, ce tableau des contraintes initiales sera toujours le même.
Fermer cette fenêtre ensuite.

Ensuite, la CLI demande de fixer le mode : C c'était plutôt pour du debug, c'est pour avoir une carte visuelle des priorités, mais je suis même pas sûr que tout est encore bien paramétré pour que ça marche.

R, c'est pour revenir en arrière dans le jeu de la vie

Dans le cas où on choisit R, on va nous demander de combien d'étapes on veut revenir en arrière : Si on veut l'étape n-1 on met 1, si on veut n-k on met k.

Puis il calcule, et dans le meilleur des cas il affiche ensuite l'étape n-k
En cliquant dans la fenêtre on peut aller à l'étape n-k + 1, et ainsi de suite jusqu'à n, puis ça boucle.

S'il n'arrive qu'à aller à l'étape n-k' avec k' < k, il va afficher cette étape, et la suite sera un peu bizarre : En cliquant ça va aussi avancer dans les étapes, mais là ça finira par dépasser l'étape n.






########### Pour les curieux ############


Afin de faciliter les essais multiples, on peut lancer GUI_Dandelifeon.py à partir d'un fichier pour la grille n.
Pour ça :
python3 GUI_Dandelifeon.py <Nom du fichier>

Fouiller dans les fichiers créés lors de l'exécution du programme sans fichier pour comprendre le formatage + fouiller dans le code python pour comprendre comment générer des fichiers facilement (avec l'interface graphique de l'exécution sans fichier) sans pleurer du sang

Instructions pour fouiller dans le code C :
Il y a différents dossiers qui correspondent à différentes versions. Ce n'est pas garanti que toutes fonctionnent.

Les dossiers "-Dev" contiennent énormément de lignes de debug commentées et des fonctions inutiles (En commentaire, ou pas en commentaire) tandis que le codes des autres a été réduit au maximum (en laissant les commentaires nécessaires à la compréhension)

La 1.1 est simplement une version organisée du code de la 1.0 : La 1.0 est un gros blob en un fichier alors que la 1.1 est découpée en plein de fichiers.

Pour comprendre dans un premier temps, je conseille de regarder la 1.1 (version non -Dev). Les fichiers importants dedans sont le début de plist.h pour la structure de point, specific.c, et main.c

La 1.0 fonctionne, ainsi que la 1.1 : Par défaut j'ai mis la 1.1
La 2.0 ne correspond à rien pour le moment, je sais juste ce que ce sera


Pour tester les différentes versions :
Installer gcc si besoin et s'assurer qu'il est dans la variable PATH de l'OS
Installer make si besoin et s'assurer qu'il est dans la variable PATH de l'os
Ouvrir le dossier de la version dans un terminal, puis lancer la commande :
make

Il sort un fichier "Dandelifeon" ou "Dandelifeon.exe". Le renommer "Dandelifeon" dans ce cas. Le placer ensuite dans le dossier Dev, et faire tout comme j'ai dit au début : Le programme va maintenant tourner avec la version sélectionnée.


Test contient divers fichiers de test qui m'ont été utilea.