{
  "name": "Boule et Billes",
  "version": "release",
  "description": "Jeu "Boule et billes" dans le cadre du DESS en jeux vidéo de 2éme cycle",
  "author": "Équipe alpha (Guenole COEFFIC, Alexandre DEBEDDES, Thibaud SARLAT, Nicolas VIDAL",
  "license": "DDJV",
  "engines": {
    "directx": ">=11",
    "PhysX": ">=4.3"
  }
}
Les contrôles donner ci-dessous ne contiennent aucune touche du pavé numérique
Liste des contrôles du joueur:
- Aller à gauche : Flèche de gauche
- Aller à droite : Flèche de droite
- Accélérer : Flèche du haut
- Réapparaître : R
- Ouvrir le menu : ESCAPE
- Mettre la pause : P
- Enlever la pause : O
- Enlever le HUD : K
- Remettre le HUD : J
- Caméra première personne : G
- Camera troisième personne : H
- Caméra libre : F


Liste des contrôles de la caméra libre:
- Avancer: W
- Aller à gauche : A
- Reculer : S
- Aller à droite : D
- Accélérez : 1
- Ralentir : 2
- Monter : Contrôle gauche
- Descendre : Barre espace

Navigation dans le menu:
Chaque image sont des boutons cliquables.
Le menu offre comporte les fonctionnalité suivante :
- Lancer une nouvelle partie
- Continuer une partie (lorsqu'une partie a déjà été créé)
- Ouvrir les options
- Quitter l'application

Les options de notre jeu sont les suivantes:
- Changer l'effet visuel sur le jeu parmi le mode par défaut,
la radial blur, l'effet de croquis et le noir et blanc.
- Changer la résolution entre 1024x768 et 1920x1080
- Mettre le jeu en fenêtré ou pleine écran
- Choisir le nombre d'ennemie entre 0 et 20

Le joueur (dans notre cas la sphère) augmente de volume au fur et à 
mesure du temps car le terrain est entièrement constitué de neige.

Sur le terrain sont disposé trois différent objet ramassable:
- Un boost de taille
- Un malus de taille
- Un objet qui réduit la taille de tout les adversaire

Si le joueur heurte un obstacle trop rapidement sa taille diminue et crée
un effet de particule de neige qui part du joueur.
