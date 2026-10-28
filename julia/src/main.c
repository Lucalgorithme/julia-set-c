#include <stdio.h>
#include <string.h>
#include "julia.h"

int main(int argc, char *argv[]){
    if(argc != 3 && argc !=7){
        fprintf(stderr, "Usage : ./julia c_re c_im [largeur hauteur max_iter sortie.ppm]\n");
        return 1;
    }

    int largeur = 800, hauteur = 600, max_iter = 200;
    double c_im, c_re;
    char sortie[50] = "julia.ppm";

    if (parse_double(argv[1], &c_re) != 0 || parse_double(argv[2],&c_im) != 0){
        fprintf(stderr, "Erreur dans la convertion de c_re ou c_im\n");
        return 1;
    }

    if(argc == 7){
        if (parse_int(argv[3], &largeur) != 0 || parse_int(argv[4], &hauteur) !=0 || parse_int(argv[5], &max_iter) != 0){
            fprintf(stderr, "Erreur dans les arguments optionnels\n");
            return 1;
        }
        strcpy(sortie, argv[6]);
    }

    if(largeur < 2){
        fprintf(stderr,"Erreur : la largeur doit être supérieur à 2\n");
        return 1;
    }

    if(hauteur < 2){
        fprintf(stderr,"Erreur : la hauteur doit être supérieur à 2\n");
        return 1;
    }

    if(max_iter < 1){
        fprintf(stderr,"Erreur : le nombre maximum d'itérations doit être supérieur à 1\n");
        return 1;
    }

    double xmax = 1.5, xmin = -1.5, ymax = 1.5 * ((double)hauteur/(double)largeur), ymin = -ymax;

    int *img = creer_image(largeur, hauteur);
    if(img == NULL){
        fprintf(stderr,"Erreur lors de la création de l'image\n");
        return 1;
    }
    calculer_julia(img, largeur, hauteur, xmin ,xmax, ymin, ymax, cplx_new(c_re, c_im), max_iter);
    int code = ecrire_ppm(&sortie[0], img, largeur, hauteur, max_iter);
    liberer_image(img);

    if(code != 0){
        fprintf(stderr, "Erreur lors de l'écriture au format ppm\n");
        return 1;
    }

    return 0;
}