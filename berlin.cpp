#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "berlin.h"
#include <time.h>


float distance(Point p1, Point p2){
    float dist = 0;
    dist = sqrt( pow((p2.x - p1.x),2) + pow((p2.y - p1.y),2) );
    return dist;
}

void initMarque(int* marque, int n){
    for(int i = 0; i < n; i++){
        marque[i] = false;
    }
}

static void copierTab(int* src, int* dest, int debut, int fin){
    // fin inclu
    for(int i = debut; i <= fin; i++){
        dest[i] = src[i];
    }
}

static void copierTabInverse(int* src, int* dest, int debut, int fin){
    int j=0;
    for(int i = debut; i <= fin; i++){
        dest[i] = src[fin-j];
        j++;
    }
}

static void afficherTab(int* tab, int n){
    // num du point = indice point + 1
    printf("{ ");
    for(int i = 0; i <= n; i++){
        printf("%d -> ", tab[i]+1);
    }
    printf("}\n");
}

static int* opt(int* tour, int* newTour, int i, int k, int n){
    // 0 ... i-1 keep
    copierTab(tour, newTour, 0, i-1);
    // i ... k revert
    copierTabInverse(tour, newTour, i, k);
    // k+1 ... n keep
    copierTab(tour, newTour, k+1, n);
    return newTour;
}

static int* plusProcheVoisin(float* matriceCout, int* marque, int n){
    float coutMin = 0;
    int indiceMin;
    // n = nbrPoins

    int* tour = (int*) malloc((n + 1) * sizeof(int));

    initMarque(marque, n);

    tour[0] = 0;
    // start exploration
    int i = 0;
    marque[0] = true;
    for(int p=1; p<n; p++){
        // p is current tab position
        coutMin = INT_MAX;
        indiceMin = -1;
        for(int j=0; j < n; j++){
            if(!marque[j]
                && matriceCout[i * n + j] < coutMin){
                coutMin = matriceCout[i * n + j];
                indiceMin = j;
            }
        }
        i = indiceMin;
        marque[i] = true;
        tour[p] = i;
    }
    tour[n] = 0; //revenir au point de depart
    return tour;
}

static float coutTour(float* matriceCout, int n, int* tour){
    float cout = 0;
    for(int i = 0; i < n ; i++){
        // ajouter cout de l'arc (i, i+1)
        cout += matriceCout[tour[i] * n + tour[i+1]];
        //printf(" %d ...", cout);
    }
    //printf("\n");
    return cout;
}

static int* voisinSimple(float* matriceCout, int n, int* tourCourant){
    int* tourVoisin = (int*) malloc((n + 1) * sizeof(int));
    float coutMin = coutTour(matriceCout, n, tourCourant);
    // dès qu'on trouve un meilleur voisin, retourner celui-ci
    // sinon, retourner NULL
    for(int i=1; i<n; i++){
        for(int k=i+1; k<n; k++){
            if(tourCourant[k+1] == tourCourant[i-1]) continue;
            opt(tourCourant, tourVoisin, i, k, n);
            // printf("Voisin généré : ");
            // afficherTab(tourVoisin, n);
            float coutVoisin = coutTour(matriceCout, n, tourVoisin);
            // printf("Cout : %d\n", coutVoisin);
            if(coutVoisin < coutMin){
                coutMin = coutVoisin;
                printf("nouveau min ! cout : %.2f | retour...\n", coutMin);

                // copierTab(tourVoisin, tourCourant, 0, n);
                return tourVoisin;
            }
            // sinon, essayer un autre voisin
        }
    }
    return NULL;
}

void escaladeSimple(float* matriceCout, int* marque, int n){
    // solution 1 par le plus proche voisin
    int* premierTour = (int*) malloc((n + 1) * sizeof(int));
    int* nextTour = (int*) malloc((n + 1) * sizeof(int));
    int* tourCourant = (int*) malloc((n + 1) * sizeof(int));
    int maxIterations = 400;
    // premierTour contiendra les index, on va convertir plus tard en noms de sommets
    
    // construire un premier chemin par le plus proche voisin
    premierTour = plusProcheVoisin(matriceCout, marque, n);
    printf("Le premier tour : ");
    afficherTab(premierTour, n);

    // Escalade par 2-opt
    tourCourant = premierTour;
    // lets try to start from somewhere else
    // copierTabInverse(premierTour, tourCourant, 0, n);
    float coutMin = coutTour(matriceCout, n, tourCourant);
    printf("Cout du premier tour : %f\n", coutTour(matriceCout, n, tourCourant));
    // les voisins de premier tour seulement
    // to do : after exhausting neighbors of premierTour, move on to neighbors of tourCourant
    while(maxIterations >= 0){
        // à limiter en temps ou en itérations
        printf("tourCourant est ");
        afficherTab(tourCourant, n);
        nextTour = voisinSimple(matriceCout, n, tourCourant);
        if(nextTour == NULL){
            printf("\nFin de l'exploration !\n");
            break;
        }
        tourCourant = nextTour;
        maxIterations--;
    }
    printf("Exhausted all neighbors/max iterations. Tour courant : ");
    afficherTab(tourCourant, n);
    printf("Cout : %f\n", coutTour(matriceCout, n, tourCourant));

}

int* voisinComplet(float* matriceCout, int n, int* tourCourant){
    int* tourVoisin = (int*) malloc((n + 1) * sizeof(int));
    int* meilleurVoisin = (int*) malloc((n + 1) * sizeof(int));
    meilleurVoisin[0] = -1;
    float coutMin = coutTour(matriceCout, n, tourCourant);
    // parcourir tous les voisins pour chercher le meilleur
    // enfin, retourner le meilleur voisin
    for(int i=1; i<n; i++){
        for(int k=i+1; k<n; k++){
            if(tourCourant[k+1] == tourCourant[i-1]) continue;
            opt(tourCourant, tourVoisin, i, k, n);
            // printf("Voisin généré : ");
            // afficherTab(tourVoisin, n);
            float coutVoisin = coutTour(matriceCout, n, tourVoisin);
            // printf("Cout : %d\n", coutVoisin);
            if(coutVoisin < coutMin){
                coutMin = coutVoisin;
                printf("nouveau min ! cout : %.2f  |  assigning...\n", coutMin);

                // utiliser copierTab au lieu de l'affectation
                // car tourVoisin va changer dans l'itération suivante
                // mais meilleurVoisin pointera toujours dessus
                copierTab(tourVoisin, meilleurVoisin, 0, n);
            }
            // sinon, essayer un autre voisin
        }
    }
    if(meilleurVoisin[0] != -1) return meilleurVoisin;
    else return NULL;
}

void escaladeComplete(float* matriceCout, int* marque, int n){
    // solution 1 par le plus proche voisin
    int* premierTour = (int*) malloc((n + 1) * sizeof(int));
    int* nextTour = (int*) malloc((n + 1) * sizeof(int));
    int* tourCourant = (int*) malloc((n + 1) * sizeof(int));
    int maxIterations = 400;
    // premierTour contiendra les index, on va convertir plus tard en noms de sommets
    
    // construire un premier chemin par le plus proche voisin
    premierTour = plusProcheVoisin(matriceCout, marque, n);

    printf("Le premier tour : ");
    afficherTab(premierTour, n);

    // Escalade par 2-opt
    tourCourant = premierTour;
    
    float coutMin = coutTour(matriceCout, n, tourCourant);
    printf("Cout du premier tour : %.2f\n", coutTour(matriceCout, n, tourCourant));
    // les voisins de premier tour seulement
    while(maxIterations >= 0){
        // à limiter en temps ou en itérations
        printf("while : tourCourant est ");
        afficherTab(tourCourant, n);
        nextTour = voisinComplet(matriceCout, n, tourCourant);
        if(nextTour == NULL){
            printf("Fin de l'exploration !\n");
            break;
        }
        tourCourant = nextTour;
        maxIterations--;
    }
    printf("Exhausted all neighbors/max iterations. Tour courant : ");
    afficherTab(tourCourant, n);
    printf("Cout : %.2f\n", coutTour(matriceCout, n, tourCourant));

}

static int alea(int lower, int upper){
    return (rand() % (upper-lower+1) + lower);
}


void recuitSimule(float* matriceCout, int* marque, int n){
    // solution 1 par le plus proche voisin
    int* premierTour = (int*) malloc((n + 1) * sizeof(int));
    int* nextTour = (int*) malloc((n + 1) * sizeof(int));
    int* tourCandidat = (int*) malloc((n + 1) * sizeof(int));

    int* tourCourant = (int*) malloc((n + 1) * sizeof(int));


    int essais = pow(n/2, 2);
    //float Tdepart = 1650;
    float Tdepart = 3000;
    float T = Tdepart;
    float Tarret = 5;
    float coutCourant = 0;
    int i = 0, k = 0;
    float dE = 0;
    float alpha = 0.98, p = 0, A = 0;



    // construire un premier chemin par le plus proche voisin
    premierTour = plusProcheVoisin(matriceCout, marque, n);

    printf("Le premier tour : ");
    afficherTab(premierTour, n);
    printf("Cout du premier tour : %.2f\n", coutTour(matriceCout, n, premierTour));

    // Escalade par 2-opt
    tourCourant = premierTour;
    srand(time(0));

    while(T > Tarret){
        for(int essai = 0; essai < essais; essai++){
            coutCourant = coutTour(matriceCout, n, tourCourant);
            i = alea(1,n-2);
            k = alea(i+1, n-1);

            opt(tourCourant, tourCandidat, i, k, n);
            dE = coutTour(matriceCout, n, tourCandidat) - coutTour(matriceCout, n, tourCourant);

            //printf("Tour courant : ");
            //afficherTab(tourCourant, n);
            //printf("Cout courant : %.2f\n", coutTour(matriceCout, n, tourCourant));

            //printf("Tour candidat : ");
            //afficherTab(tourCandidat, n);
            //printf("Cout candidat : %.2f\n", coutTour(matriceCout, n, tourCandidat));
            //printf("%.2f  |  ", dE);

            if(dE < 0){
                // found better circuit
                copierTab(tourCandidat, tourCourant, 0, n);
                //printf("bon candidat choisit | ");
            }
            else {
                A = (float)rand()/RAND_MAX;
                p = exp(-dE / T);
                //printf("A = %.2f,   p = %.2f\n", A, p);
                if (A <= p){
                    // accept worse circuit
                    //printf("mauvais candidat choisit | ");
                    copierTab(tourCandidat, tourCourant, 0, n);
                } //else printf("aucun choisi | ");
            }

        }

        // printf("\n\nResultat du palier : T = %.2f\n", T);
        // printf("Tour courant : ");
        // afficherTab(tourCourant, n);
        // printf("Cout : %.2f\n", coutTour(matriceCout, n, tourCourant));
        T = alpha * T;
    }

    printf("\n\nResultat du recuit simule : ");
    afficherTab(tourCourant, n);
    printf("Cout : %.2f\n", coutTour(matriceCout, n, tourCourant));

}