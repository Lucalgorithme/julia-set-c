#ifndef BATCH_H
#define BATCH_H

#include <stddef.h>

#define CHEMIN_JULIA "bin/julia"

typedef struct {
    char   nom[64];
    double c_re, c_im;
    int    largeur, hauteur, max_iter;
} Job;

typedef struct {
    char c_re[40];
    char c_im[40];
    char largeur[16];
    char hauteur[16];
    char max_iter[16];
} ArgsTexte;


int nom_valide(const char *s);                 /* 1 si valide, 0 sinon */
int parse_job(const char *ligne, Job *job);    /* 0 ok, 1 ignorer, -1 invalide */

int construire_chemin(char *dest, size_t taille, const char *dossier,
                      const char *nom, const char *extension);

int creer_dossier(const char *chemin);

int lancer_commande(char *const argv[]);

int formater_arguments(const Job *job, ArgsTexte *a);

int executer_job(const Job *job, const char *dossier_ppm, const char *dossier_png);

int traiter_fichier(const char *chemin_jobs, const char *dossier_ppm,
                    const char *dossier_png, int *nb_ok, int *nb_echecs);

#endif