# Robot de réconfort — DOMBROWSKI & EOUZAN

Projet de semestre, master informatique. Un robot à perception locale
consulte un dictionnaire, fouille une armoire et rapporte un objet à un
résident.

Langage : C++ — Version : 1


## Construire le dossier build (s'il n'y est pas)

```
cmake -S . -B build 
```

## Compiler

```
cmake --build build
```

## Lancer la simulation

```
./build/ImagineRobot <carte.json> <scenario.json> <dossier_donnees> <trace_sortie.json>
```
La commande doit être lancée depuis le dossier ImagineRobot.

Exemple :

```
./build/ImagineRobot data/cartes/appartement_01.json data/cartes/scenario_01.json data/donnees sortie.json
```

Dépendances : Les dépendances sont gérés par CMakeLists.txt, avec les librairies fournies dans vendor.

## Tester

```
./build/ImagineRobot <dossier_tests_fichiers>
```
<dossier_tests_fichiers> correspond au dossier contenant les fichiers de test pour la gestion des erreurs. Les fichiers ne doivent pas être triés dans des dossiers par type d'éléments (cartes, scénarios...). Le test détermine le type d'élément (carte, scenario, armoire ou dictionnaire) )par format, lorsqu'il est valide. Pour valider le chemin illisible, le droit de lecture pourra être retiré à un des fichiers conformes.

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

**Ce qui marche.** *Les commandes conformes, les tests demandés, les réquêtes avec un résident murés, un message sans émotion et une armoire vide.*

**Ce qui ne marche pas.** *Le robot ne ferme pas l'affichage de l'armoire si la requête échoue avec "casiers_vides". L'objet pris par le robot est replacé dans l'armoire si le résident n'est pas accessible, mais n'est pas visible dans l'affichage de l'armoire.*

## Vidéo

`demo_Projet1.webm`, également déclarée dans `rendu.json`.

## Dépot

[https://github.com/DIO6150/Imagine-Robot.git](https://github.com/DIO6150/Imagine-Robot.git)
