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
./build/ImagineRobot data/cartes/appartement_01.json data/cartes/scenario_01.json data/donnees sortie.json
```

Dépendances : Les dépendances sont gérés par CMakeLists.txt, avec les librairies fournies dans vendor.

## Tester

```
./build/ImagineRobot <dossier_tests_fichiers>
```
<dossier_tests_fichiers> correspond au dossier contenant les fichiers de test pour la gestion des erreurs. Les fichiers ne doivent pas être triés dans des dossiers par type d'éléments (cartes, scénarios...). Le test détermine le type d'élément par le format, lorsqu'il est valide. Pour valider le chemin illisible, le droit de lecture pourra être retiré à un des fichiers conformes.

Exemple :

```
./build/ImagineRobot data/tests_fichiers
```

Les tests couvrent la planification et replanification de chemin, l'existence d'un chemin de l'agent vers un résident muré, la lecture des émotions dans le dictionnaire, le déplacement du curseur dans l'armoire, la règle de repli et la gestion des erreurs pour les fichiers mal formés.

## Organisation du code

| Dossier/Fichier | Rôle |
|---|---|
| `vendor` | dossier de librairies externes |
| `CMakeLists.txt` | gestion des dépendances, construction du dossier build |
| `Main.cpp` | point d'entrée, lancement de test et écriture de trace |
| `Simulation` | lance la simulation, affichage, recuperation |
| `Playground` | chargement des données dans les éléments correspondant, environnement |
| `Map` | carte |
| `Script` | scénario |
| `ItemStash` | armoire |
| `Dictionary` | dictionnaire |
| `AgentController` | corps de l'agent : action, interface d'intéraction de l'agent sur l'environnement |
| `Brain` | tête de l'agent : decision, mémorisation, décisions |
| `Common` | fichiers contenant des structures ou classes partagées (émotions, positions, directions...) |
| `Error` | codes de retour et d'erreurs |
| `Parsers` | organisation de la vérification et du chargement des données, unique à chaque élément de la simulation |
| `Render` | affichage visuel de la simulation |
| `JSONParser` | gestion général et séquentiel de la vérification et du chargement des données |
| `Pos` | type représentant les positions |
| `JSONParser` | gestion de l'ouverture d'un fichier .json |

## État

**Ce qui marche.** *À compléter.*

**Ce qui ne marche pas.** *À compléter — soyez honnêtes, c'est noté en
votre faveur.*

## Vidéo

`demo.mp4` (ou le lien), également déclarée dans `rendu.json`.
