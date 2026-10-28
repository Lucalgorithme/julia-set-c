#include <stdio.h>
#include <stdlib.h>
#include "batch.h"

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 4) {
        fprintf(stderr, "Usage : %s <fichier_config> [dossier_ppm dossier_png]\n", argv[0]);
        return 1;
    }

    const char *dossier_ppm = "ppm";
    const char *dossier_png = "png";
    if (argc == 4) {
        dossier_ppm = argv[2];
        dossier_png = argv[3];
    }

    int nb_ok = 0;
    int nb_echecs = 0;

    if (traiter_fichier(argv[1], dossier_ppm, dossier_png, &nb_ok, &nb_echecs) != 0) {
        fprintf(stderr, "Erreur critique lors du traitement du fichier.\n");
        return 1;
    }

    printf("Succès : %d\n", nb_ok);
    printf("Echecs : %d\n", nb_echecs);

    return nb_echecs > 0 ? 1 : 0;
}