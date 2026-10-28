#include "julia.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Complexe cplx_new(double re, double im){
    Complexe new_complexe;
    new_complexe.im = im;
    new_complexe.re = re;
    return new_complexe;
}

Complexe cplx_add(Complexe a, Complexe b){
    return cplx_new(a.re + b.re, a.im + b.im);
}

Complexe cplx_mul(Complexe a, Complexe b){
    return cplx_new((a.re*b.re) - (a.im*b.im), (a.re*b.im) + (a.im*b.re));
}

Complexe cplx_carre(Complexe z){
    Complexe new_complexe = cplx_mul(z,z);
    return new_complexe;
}

double cplx_norme2(Complexe z){
    return (z.re*z.re) + (z.im*z.im);
}

int julia_iterations(Complexe z0, Complexe c, int max_iter){
    Complexe z_courant = z0;
    int i=0;
    while(i < max_iter && cplx_norme2(z_courant) <= 4)
    {
        z_courant = cplx_add(cplx_carre(z_courant),c);
        i++;
    }
    return i;
}

Complexe pixel_vers_complexe(int px, int py, int largeur, int hauteur, double xmin, double xmax, double ymin, double ymax){
    return cplx_new(xmin + px * ((xmax - xmin) / (largeur - 1)), ymax - py * ((ymax - ymin) / (hauteur - 1)));
}

int *creer_image(int largeur, int hauteur){
    if(largeur <= 0 || hauteur <= 0)return NULL;
    int *img = malloc((size_t)largeur * hauteur * sizeof(*img));
    return img;
}

void liberer_image(int *img){
    free(img);
}

void calculer_julia(int *img, int largeur, int hauteur, double xmin, double xmax, double ymin, double ymax, Complexe c, int max_iter){
    Complexe z;
    for (int py = 0; py < hauteur;py++){
        for (int px = 0; px < largeur;px++){
            z = pixel_vers_complexe(px, py, largeur, hauteur, xmin, xmax, ymin, ymax);
            img[py * largeur + px] = julia_iterations(z, c, max_iter);
        }
    }
}

int ecrire_ppm(const char *chemin, const int *img, int largeur, int hauteur, int max_iter){
    FILE *f = fopen(chemin, "w");
    if (f == NULL) {
        return -1;                 
    }
    fprintf(f, "P3\n");
    fprintf(f, "%d %d\n", largeur, hauteur);
    fprintf(f, "255\n");

    for(int py = 0; py < hauteur ;py++){
        for(int px = 0; px < largeur;px++){
            int n = img[py * largeur + px];
            Couleur couleur = couleur_pour(n, max_iter);
            fprintf(f, "%d %d %d\n", couleur.r, couleur.g, couleur.b); 
        }
    }

    fclose(f);
    return 0;
}

Couleur couleur_pour(int n, int max_iter) {
    Couleur couleur;
    if (n == max_iter) {
        couleur.r = 0;
        couleur.g = 0;
        couleur.b = 0;
    } else {
        couleur.r = (unsigned char)(sin(0.1 * n + 0) * 127 + 128);
        couleur.g = (unsigned char)(sin(0.1 * n + 2) * 127 + 128);
        couleur.b = (unsigned char)(sin(0.1 * n + 4) * 127 + 128);
    }
    return couleur;
}

int parse_double(const char *s, double *out){
    char *fin;
    if(strcmp(s,"") == 0){
        return -1;
    }
    double d = strtod(s,&fin);
    if((strcmp(fin,s) == 0) || !(strcmp(fin,"\0") == 0)){
        return -1;
    }
    *out = d;
    return 0;
}

int parse_int(const char *s, int *out){
    char *fin;
    if(strcmp(s,"") == 0){
        return -1;
    }
    int d = strtol(s,&fin,10);
    if((strcmp(fin,s) == 0) || !(strcmp(fin,"\0") == 0)){
        return -1;
    }
    *out = d;
    return 0;
}