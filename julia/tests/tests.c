#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "julia.h"


static int proche(double a, double b) { return fabs(a - b) < 1e-9; }

void test_q1(void) {
    Complexe a = cplx_new(1, 2), b = cplx_new(3, -1);
    Complexe s = cplx_add(a, b);
    assert(proche(s.re, 4) && proche(s.im, 1));
    Complexe p = cplx_mul(a, b);
    assert(proche(p.re, 5) && proche(p.im, 5));
    Complexe q = cplx_carre(a);
    assert(proche(q.re, -3) && proche(q.im, 4));
    assert(proche(cplx_norme2(cplx_new(3, 4)), 25));
    puts("Q1 OK");
}

void test_q2(void) {
    Complexe zero = cplx_new(0, 0);
    assert(julia_iterations(zero, zero, 100) == 100);
    assert(julia_iterations(cplx_new(2, 2), zero, 100) == 0);
    assert(julia_iterations(cplx_new(1, 1), zero, 100) == 2);
    assert(julia_iterations(zero, cplx_new(1, 0), 100) == 3);
    assert(julia_iterations(zero, cplx_new(-1, 0), 100) == 100);
    assert(julia_iterations(zero, zero, 0) == 0);
    puts("Q2 OK");
}

void test_q3(void) {
    Complexe z = pixel_vers_complexe(0, 0, 5, 5, -2, 2, -2, 2);
    assert(proche(z.re, -2) && proche(z.im, 2));
    z = pixel_vers_complexe(4, 4, 5, 5, -2, 2, -2, 2);
    assert(proche(z.re, 2) && proche(z.im, -2));
    z = pixel_vers_complexe(2, 2, 5, 5, -2, 2, -2, 2);
    assert(proche(z.re, 0) && proche(z.im, 0));
    z = pixel_vers_complexe(4, 0, 5, 5, -2, 2, -2, 2);
    assert(proche(z.re, 2) && proche(z.im, 2));
    z = pixel_vers_complexe(1, 1, 3, 2, 0, 1, 0, 1);
    assert(proche(z.re, 0.5) && proche(z.im, 0));
    puts("Q3 OK");
}

void test_q4(void) {
    int *img = creer_image(4, 3);
    assert(img != NULL);
    for (int i = 0; i < 12; i++) img[i] = i;
    assert(img[2 * 4 + 3] == 11);
    liberer_image(img);
    assert(creer_image(0, 3) == NULL);
    assert(creer_image(3, -1) == NULL);
    liberer_image(NULL);
    puts("Q4 OK");
}

void test_q5(void) {
    int L = 21, H = 21, M = 50;
    int *img = creer_image(L, H);
    assert(img != NULL);
    calculer_julia(img, L, H, -2, 2, -2, 2, cplx_new(0, 0), M);
    assert(img[0] == 0);
    assert(img[10 * L + 10] == M); 
    assert(img[10 * L + 11] == M); 
    assert(img[10 * L + 18] < M); 
    liberer_image(img);
    puts("Q5 OK");
}

void test_q6(void) {
    int L = 21, H = 21, M = 50;
    int *img = creer_image(L, H);
    calculer_julia(img, L, H, -2, 2, -2, 2, cplx_new(0, 0), M);
    assert(ecrire_ppm("test_q6.ppm", img, L, H, M) == 0);
    assert(ecrire_ppm("/dossier/inexistant/x.ppm", img, L, H, M) == -1);
    liberer_image(img);
    FILE *f = fopen("test_q6.ppm", "r");
    assert(f != NULL);
    char ligne[16];
    assert(fgets(ligne, sizeof ligne, f) != NULL);
    assert(strncmp(ligne, "P3", 2) == 0);
    int w, h;
    assert(fscanf(f, "%d %d", &w, &h) == 2);
    assert(w == L && h == H);
    fclose(f);
    puts("Q6 OK");
}

void test_q7(void) {
    Couleur c = couleur_pour(100, 100);
    assert(c.r == 0 && c.g == 0 && c.b == 0);
    for (int n = 0; n < 100; n++) {
    c = couleur_pour(n, 100);
    assert(c.r || c.g || c.b);
    }
    int distinctes = 0;
    for (int n = 0; n < 100; n++) {
    Couleur a = couleur_pour(n, 100);
    int nouvelle = 1;
    for (int m = 0; m < n; m++) {
    Couleur b = couleur_pour(m, 100);
    if (a.r == b.r && a.g == b.g && a.b == b.b) { nouvelle = 0;
    break; }
    }
    distinctes += nouvelle;
    }
    assert(distinctes >= 10);
    puts("Q7 OK");
}

void test_q8(void) {
    double d; int i;
    assert(parse_double("1.5", &d) == 0 && proche(d, 1.5));
    assert(parse_double("-0.123", &d) == 0 && proche(d, -0.123));
    assert(parse_double("abc", &d) == -1);
    assert(parse_double("1.5x", &d) == -1);
    assert(parse_double("", &d) == -1);
    assert(parse_int("42", &i) == 0 && i == 42);
    assert(parse_int("4.2", &i) == -1);
    assert(parse_int("", &i) == -1);
    puts("Q8 OK");
}

int main(void) {
    test_q1();
    test_q2();
    test_q3();
    test_q4();
    test_q5();
    test_q6();
    test_q7();
    test_q8();
    return 0;
}