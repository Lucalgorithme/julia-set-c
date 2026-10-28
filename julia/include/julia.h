#ifndef JULIA_H
#define JULIA_H

typedef struct
{
    double re;
    double im;
} Complexe;

typedef struct
{
    unsigned char r, g, b;
} Couleur;

Complexe cplx_new(double re, double im);
Complexe cplx_add(Complexe a, Complexe b);
Complexe cplx_mul(Complexe a, Complexe b);
Complexe cplx_carre(Complexe z);
double cplx_norme2(Complexe z);

int julia_iterations(Complexe z0, Complexe c, int max_iter);

Complexe pixel_vers_complexe(int px, int py, int largeur, int hauteur,
                             double xmin, double xmax,
                             double ymin, double ymax);

int *creer_image(int largeur, int hauteur);
void liberer_image(int *img);

void calculer_julia(int *img, int largeur, int hauteur,
                    double xmin, double xmax, double ymin, double ymax,
                    Complexe c, int max_iter);

int ecrire_ppm(const char *chemin, const int *img,
               int largeur, int hauteur, int max_iter);

Couleur couleur_pour(int n, int max_iter);

int parse_double(const char *s, double *out);
int parse_int(const char *s, int *out);

#endif