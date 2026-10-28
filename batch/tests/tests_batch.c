#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "batch.h"
#include "julia.h"

static int proche(double a, double b) { return fabs(a - b) < 1e-9; }

void test_q1(void) {
    char longue[65];
    assert(nom_valide("lapin") == 1);
    assert(nom_valide("a-b_c1") == 1);
    assert(nom_valide("") == 0);
    assert(nom_valide("../x") == 0);
    assert(nom_valide("a;b") == 0);
    assert(nom_valide("a b") == 0);
    memset(longue, 'a', 63); longue[63] = '\0';
    assert(nom_valide(longue) == 1);
    memset(longue, 'a', 64); longue[64] = '\0';
    assert(nom_valide(longue) == 0);

    Job j;
    assert(parse_job("lapin -0.123 0.745 800 600 200\n", &j) == 0);
    assert(strcmp(j.nom, "lapin") == 0);
    assert(proche(j.c_re, -0.123) && proche(j.c_im, 0.745));
    assert(j.largeur == 800 && j.hauteur == 600 && j.max_iter == 200);
    assert(parse_job("x\t0\t0\t10\t10\t5", &j) == 0);          
    assert(parse_job("", &j) == 1);
    assert(parse_job("   \n", &j) == 1);
    assert(parse_job("# commentaire", &j) == 1);
    assert(parse_job("lapin -0.123 0.745 800 600", &j) == -1);  
    assert(parse_job("lapin -0.123 0.745 800 600 200 en_trop", &j) == -1);
    assert(parse_job("../evil 0 0 800 600 200", &j) == -1);
    assert(parse_job("a;b 0 0 800 600 200", &j) == -1);
    assert(parse_job("x abc 0 800 600 200", &j) == -1);
    assert(parse_job("x 0 0 1 600 200", &j) == -1);
    assert(parse_job("x 0 0 100000 10 5", &j) == -1);
    assert(parse_job("x 0 0 10 10 0", &j) == -1);
    puts("Q1 OK");
}

void test_q2(void) {
    char buf[64];
    assert(construire_chemin(buf, sizeof buf, "ppm", "lapin", "ppm") == 0);
    assert(strcmp(buf, "ppm/lapin.ppm") == 0);
    assert(construire_chemin(buf, sizeof buf, "png", "lapin", "png") == 0);
    assert(strcmp(buf, "png/lapin.png") == 0);
    char petit[8];
    assert(construire_chemin(petit, sizeof petit, "ppm", "lapin", "ppm") == -1);
    assert(construire_chemin(petit, 0, "ppm", "lapin", "ppm") == -1);
    puts("Q2 OK");
}

void test_q3(void) {
    assert(creer_dossier("test_dossier") == 0);
    assert(creer_dossier("test_dossier") == 0);              /* déjà là */
    FILE *f = fopen("test_fichier", "w");
    assert(f != NULL);
    fclose(f);
    assert(creer_dossier("test_fichier") == -1);             /* c'est un fichier */
    assert(creer_dossier("/dossier/inexistant/x") == -1);
    rmdir("test_dossier");
    remove("test_fichier");
    puts("Q3 OK");
}

void test_q4(void) {
    char *a1[] = {"true", NULL};
    char *a2[] = {"false", NULL};
    char *a3[] = {"commande_qui_nexiste_pas", NULL};
    char *a4[] = {"sh", "-c", "exit 3", NULL};
    char *a5[] = {"sh", "-c", "kill -9 $$", NULL};
    assert(lancer_commande(a1) == 0);
    assert(lancer_commande(a2) == 1);
    assert(lancer_commande(a3) == 127);
    assert(lancer_commande(a4) == 3);
    assert(lancer_commande(a5) == -1);      /* tué par un signal */
    puts("Q4 OK");
}

void test_q5(void) {
    Job j = {"x", -0.123, 0.745, 800, 600, 200};
    ArgsTexte a;
    assert(formater_arguments(&j, &a) == 0);
    assert(strtod(a.c_re, NULL) == j.c_re);        /* aller-retour exact */
    assert(strtod(a.c_im, NULL) == j.c_im);
    assert(strcmp(a.largeur, "800") == 0);
    assert(strcmp(a.hauteur, "600") == 0);
    assert(strcmp(a.max_iter, "200") == 0);
    Job k = {"y", -0.390541, -0.586788, 1000, 800, 500};
    assert(formater_arguments(&k, &a) == 0);
    assert(strtod(a.c_re, NULL) == k.c_re);
    assert(strtod(a.c_im, NULL) == k.c_im);
    puts("Q5 OK");
}

void test_q6(void) {
    Job j = {"test_job", -0.123, 0.745, 100, 80, 50};
    assert(creer_dossier("t6_png") == 0);

    /* échec : le dossier des ppm n'existe pas, donc julia échoue */
    assert(executer_job(&j, "t6_inexistant", "t6_png") == -1);
    assert(access("t6_png/test_job.png", F_OK) != 0);     /* convert n'a pas tourné */

    /* succès */
    assert(creer_dossier("t6_ppm") == 0);
    assert(executer_job(&j, "t6_ppm", "t6_png") == 0);
    assert(access("t6_ppm/test_job.ppm", F_OK) == 0);
    assert(access("t6_png/test_job.png", F_OK) == 0);

    remove("t6_ppm/test_job.ppm");
    remove("t6_png/test_job.png");
    rmdir("t6_ppm");
    rmdir("t6_png");
    puts("Q6 OK");
}

void test_q7(void) {
    FILE *f = fopen("jobs_test.txt", "w");
    assert(f != NULL);
    fputs("# jobs de test\n", f);
    fputs("lapin -0.123 0.745 100 80 50\n", f);
    fputs("mauvais abc 0 100 80 50\n", f);
    fputs("\n", f);
    for (int i = 0; i < 1000; i++) fputc('a', f);          /* ligne trop longue */
    fputc('\n', f);
    fputs("dragon -0.75 0.1 100 80 50\n", f);
    fclose(f);

    int ok = -1, echecs = -1;
    assert(traiter_fichier("jobs_test.txt", "t7_ppm", "t7_png", &ok, &echecs) == 0);
    assert(ok == 2 && echecs == 2);
    assert(access("t7_ppm/lapin.ppm", F_OK) == 0);
    assert(access("t7_png/dragon.png", F_OK) == 0);
    assert(access("t7_png/mauvais.png", F_OK) != 0);

    assert(traiter_fichier("fichier_inexistant.txt", "t7_a", "t7_b", &ok, &echecs) == -1);
    assert(access("t7_a", F_OK) != 0);                     /* rien n'a été créé */

    remove("t7_ppm/lapin.ppm");
    remove("t7_ppm/dragon.ppm");
    remove("t7_png/lapin.png");
    remove("t7_png/dragon.png");
    rmdir("t7_ppm");
    rmdir("t7_png");
    remove("jobs_test.txt");
    puts("Q7 OK");
}

int main(void) {
    test_q1();
    test_q2();
    test_q3();
    test_q4();
    test_q5();
    //test_q6();
    //test_q7(); // changer #define CHEMIN_JULIA "../bin/julia" pour tester
    return 0;
}