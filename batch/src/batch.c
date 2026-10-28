#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "batch.h"
#include "julia.h"

int nom_valide(const char *s) {
    if (s == NULL) {
        return 0;
    }
    
    size_t len = strlen(s);
    if (len < 1 || len > 63) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalnum(c) && c != '-' && c != '_') {
            return 0;
        }
    }

    return 1;
}

int parse_job(const char *ligne, Job *job){
    if(ligne == NULL || job == NULL){
        return -1;
    }

    char buffer[256];
    int n = snprintf(buffer, sizeof(buffer), "%s", ligne);

    if(n < 0 || n >= 256){
        return -1;
    }

    char *token = strtok(buffer, " \t\r\n");
    if(token == NULL){
        return 1; // Ligne vide
    }

    if(token[0] == '#'){
        return 1; // Ligne de commentaire
    }

    char *nom = token;
    char *c_re = strtok(NULL, " \t\r\n");
    char *c_im = strtok(NULL, " \t\r\n");
    char *largeur_str = strtok(NULL, " \t\r\n");
    char *hauteur_str = strtok(NULL, " \t\r\n");
    char *iteration_str = strtok(NULL, " \t\r\n");
    char *sup = strtok(NULL, " \t\r\n");

    if(nom == NULL || c_re == NULL || c_im == NULL || largeur_str == NULL || hauteur_str == NULL || iteration_str == NULL){
        return -1;
    }

    if(sup != NULL){
        return -1; // Trop de champs
    }

    double re_val, im_val;
    int larg_val, haut_val, max_iter_val;

    if(nom_valide(nom) == 0){
        return -1;
    }
    if(parse_double(c_re, &re_val) != 0){
        return -1;
    }
    if(parse_double(c_im, &im_val) != 0){
        return -1;
    }
    if(parse_int(largeur_str, &larg_val) != 0){
        return -1;
    }
    if(parse_int(hauteur_str, &haut_val) != 0){
        return -1;
    }
    if(parse_int(iteration_str, &max_iter_val) != 0){
        return -1;
    }

    if(larg_val < 2 || larg_val > 10000){
        return -1;
    }
    if(haut_val < 2 || haut_val > 10000){
        return -1;
    }
    if(max_iter_val < 1 || max_iter_val > 100000){
        return -1;
    }

    snprintf(job->nom, sizeof(job->nom), "%s", nom);
    job->c_re = re_val;
    job->c_im = im_val;
    job->largeur = larg_val;
    job->hauteur = haut_val;
    job->max_iter = max_iter_val;

    return 0;
}

int construire_chemin(char *dest, size_t taille, const char *dossier, const char *nom, const char *extension){
    if (dest == NULL || taille == 0 || dossier == NULL || nom == NULL || extension == NULL) {
        return -1;
    }

    int ret = snprintf(dest, taille, "%s/%s.%s", dossier, nom, extension);

    if (ret < 0 || (size_t)ret >= taille) {
        return -1;
    }

    return 0;
}

int creer_dossier(const char *chemin){
    if (mkdir(chemin, 0755) == 0) {
        return 0;
    }

    if (errno == EEXIST) {
        struct stat st;
        if (stat(chemin, &st) == 0 && S_ISDIR(st.st_mode)) {
            return 0;
        }
    }

    return -1;
}

int lancer_commande(char *const argv[]) {
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);
    } else {
        int status;
        int res;

        do {
            res = waitpid(pid, &status, 0);
        } while (res < 0 && errno == EINTR);

        if (res < 0) {
            return -1;
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

int formater_arguments(const Job *job, ArgsTexte *a){
    int ret1 = snprintf(a->c_re, sizeof(a->c_re), "%.17g", job->c_re);
    if(ret1 < 0 || (size_t)ret1 >= sizeof(a->c_re)){
        return -1;
    }

    int ret2 = snprintf(a->c_im, sizeof(a->c_im), "%.17g", job->c_im);
    if(ret2 < 0 || (size_t)ret2 >= sizeof(a->c_im)){
        return -1;
    }

    int ret3 = snprintf(a->largeur, sizeof(a->largeur), "%d", job->largeur);
    if(ret3 < 0 || (size_t)ret3 >= sizeof(a->largeur)){
        return -1;
    }

    int ret4 = snprintf(a->hauteur, sizeof(a->hauteur), "%d", job->hauteur);
    if(ret4 < 0 || (size_t)ret4 >= sizeof(a->hauteur)){
        return -1;
    }

    int ret5 = snprintf(a->max_iter, sizeof(a->max_iter), "%d", job->max_iter);
    if(ret5 < 0 || (size_t)ret5 >= sizeof(a->max_iter)){
        return -1;
    }

    return 0;
}

int executer_job(const Job *job, const char *dossier_ppm, const char *dossier_png){
    char chemin_ppm[256], chemin_png[256];

    if(construire_chemin(chemin_ppm, sizeof(chemin_ppm), dossier_ppm, job->nom, "ppm") != 0){
        return -1;
    }

    if(construire_chemin(chemin_png, sizeof(chemin_png), dossier_png, job->nom, "png") != 0){
        return -1;
    }

    ArgsTexte a;
    if(formater_arguments(job, &a) != 0){
        return -1;
    }

    char *argv_julia[] = {CHEMIN_JULIA, a.c_re, a.c_im, a.largeur, a.hauteur, a.max_iter, chemin_ppm, NULL};
    if(lancer_commande(argv_julia) != 0){
        return -1;
    }

    char *argv_convert[] = {"convert", chemin_ppm, chemin_png, NULL};
    if (lancer_commande(argv_convert) != 0) {
        return -1;
    }

    return 0;
}

int traiter_fichier(const char *chemin_jobs, const char *dossier_ppm, const char *dossier_png, int *nb_ok, int *nb_echecs) {
    FILE *f = fopen(chemin_jobs, "r");
    if (f == NULL) {
        return -1;
    }

    if (creer_dossier(dossier_ppm) != 0 || creer_dossier(dossier_png) != 0) {
        fclose(f);
        return -1;
    }

    if (nb_ok) *nb_ok = 0;
    if (nb_echecs) *nb_echecs = 0;

    char ligne[1024];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (ligne[0] == '\n' || ligne[0] == '\r' || ligne[0] == '#') {
            continue;
        }

        Job job;
        if (parse_job(ligne, &job) != 0) {
            if (nb_echecs) (*nb_echecs)++;
        } else if (!nom_valide(job.nom)) {
            if (nb_echecs) (*nb_echecs)++;
        } else {
            if (executer_job(&job, dossier_ppm, dossier_png) == 0) {
                if (nb_ok) (*nb_ok)++;
            } else {
                if (nb_echecs) (*nb_echecs)++;
            }
        }
    }

    fclose(f);
    return 0;
}