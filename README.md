# 24sh
Un shell simple avec une faute de frappe au titre.

# Description du projet
Ce projet est un mini shell divisé en 9 parties :

1. main : Le fichier principal qui exécute le shell.
2. parser : Le module qui analyse les commandes entrées par l'utilisateur.
3. parser_rules : Les règles utilisées pour analyser les commandes.
4. parser_print : Une fonction pour afficher les résultats de l'analyse.
5. lexer : Le module pour analyser les entrées lexicalement.
6. variable : Les fonctions pour gérer les variables d'environnement.
7. exec : Les fonctions pour exécuter les commandes.
8. echo : La fonction pour afficher des chaînes de caractères.
9. test_suite : Les cas de test pour vérifier le bon fonctionnement du shell.

Ce projet vous permet de comprendre comment fonctionne un shell en utilisant des fonctions de base de l'analyse de commandes, de l'analyse lexicale et de l'exécution de commandes.
Il est en cours de création, le projet durera un mois, chaque semaine est appelée une "Step". Toutes les parties seront améliorées à chaque Step.

## Build
Il faut installer meson pour compiler le projet `sudo apt install meson`.  
À la première utilisation:
```
meson setup builddir
```
Pour build le projet:
```
ninja -C builddir
```

## Utilisation
Il y a trois manières d'utiliser 24 sh:
- En appelant l'exécutable avec l'option -c `./42sh -c "echo Input as string"`
- En lisant un fichier donne en argument `./42sh script.sh`
- Ou lire depuis la sortie standard quand aucune source n'est donnée `./42sh < script.sh` ou `cat script.sh | ./42sh`

# Répartition des tâches
|               | Victor        | Guillaume     | Titouan       | Corentin      |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| main          |       X       |               |               |       X       |
| parser        |       X       |               |       X       |               |
| parser_rules  |       X       |               |               |               |
| parser_print  |       X       |       X       |               |       X       |
| lexer         |               |       X       |               |               |
| variable      |               |       X       |               |       X       |
| exec          |               |       X       |       X       |               |
| echo          |               |               |       X       |               |
| test_suite    |               |               |       X       |       X       |

# Steps
## Step 1
- [X] main
- [X] parser
- [X] parser_rules
- [X] parser_print
- [X] lexer
- [X] variable
- [X] exec
- [X] echo
- [X] test_suite

## Step 2
- [X] main
- [X] parser
- [X] parser_rules
- [X] parser_print
- [X] lexer
- [ ] variable
- [ ] exec
- [X] echo
- [ ] test_suite
