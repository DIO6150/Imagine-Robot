# Robot de réconfort — DOMBROWSKI & EOUZAN

Projet de semestre, master informatique. Un robot à perception locale
consulte un dictionnaire, fouille une armoire et rapporte un objet à un
résident.

Langage : C++ — Version : 1


## Construire le dossier build

```
cmake -S . -B build 
```

## Compiler

```
cmake --build build
```

## Lancer

```
./build/ImagineRobot <carte.json> <scenario.json> <dossier_donnees> <trace_sortie.json>
```

Exemple :

```
./build/ImagineRobot cartes/appartement_01.json cartes/scenario_01.json donnees sortie.json
```

Dépendances : *aucune*, ou la liste et comment les installer.

## Tester

```
./build/ImagineRobot <dossier_tests_fichiers>
```

*Les tests couvrent la planification et replanification de chemin, l'existence d'un chemin de l'agent vers un résident muré, la lecture des émotions dans le dictionnaire, le déplacement du curseur dans l'armoire, la règle de repli et la gestion des erreurs pour les fichiers mal formés.

## Organisation du code

| Fichier | Rôle |
|---|---|
| `agent.py` | point d'entrée, boucle de décision |
| `…` | … |

## État

**Ce qui marche.** *À compléter.*

**Ce qui ne marche pas.** *À compléter — soyez honnêtes, c'est noté en
votre faveur.*

## Vidéo

`demo.mp4` (ou le lien), également déclarée dans `rendu.json`.
