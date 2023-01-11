# 24sh
Un shell simple avec une faute d'orthographe au titre.

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

# English version
Nan j'blague démerdez vous putain d'anglais  
Bref j'en ai marre de faire un README mais la page github est quand même vachement plus jolie comme ça