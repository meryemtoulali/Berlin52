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

static int* plusProcheVoisin(int start, float* matriceCout, int* marque, int n){
    float coutMin = 0;
    int indiceMin;
    // n = nbrPoins

    int* tour = (int*) malloc((n + 1) * sizeof(int));

    initMarque(marque, n);

    tour[0] = start;
    // start exploration
    int i = start;
    marque[start] = true;
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
    tour[n] = start; //revenir au point de depart
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
                printf("A trouvé un voisin améliorant, cout : %.2f | retour...\n", coutMin);

                // copierTab(tourVoisin, tourCourant, 0, n);
                return tourVoisin;
            }
            // sinon, essayer un autre voisin
        }
    }
    return NULL;
}

void escaladeSimple(float* matriceCout, int* marque, int n){

    printf("*************** Escalade simple ***************\n\n");
    // solution 1 par le plus proche voisin
    int* premierTour = (int*) malloc((n + 1) * sizeof(int));
    int* nextTour = (int*) malloc((n + 1) * sizeof(int));
    int* tourCourant = (int*) malloc((n + 1) * sizeof(int));
    int maxIterations = 400;
    // premierTour contiendra les index, on va convertir plus tard en noms de sommets
    
    // construire un premier chemin par le plus proche voisin
    premierTour = plusProcheVoisin(0, matriceCout, marque, n);
    printf("Le premier tour (par le plus proche voisin) : ");
    afficherTab(premierTour, n);

    // Escalade par 2-opt
    tourCourant = premierTour;
    float coutMin = coutTour(matriceCout, n, tourCourant);
    printf("Cout du premier tour : %f\n\n", coutTour(matriceCout, n, tourCourant));
    
    printf("************ Début de l'exploration ************\n\n");

    // commencer l'exploration
    while(maxIterations >= 0){
        // printf("Le tour courant est : ");
        // afficherTab(tourCourant, n);
        nextTour = voisinSimple(matriceCout, n, tourCourant);
        if(nextTour == NULL){
            printf("************* Fin de l'exploration *************\n\n");
            break;
        }
        tourCourant = nextTour;
        maxIterations--;
    }
    printf("A visité tous les voisins/a atteint le max d'itérations. Solution trouvée : ");
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
    printf("************** Escalade complète **************\n\n");
    // solution 1 par le plus proche voisin
    int* premierTour = (int*) malloc((n + 1) * sizeof(int));
    int* nextTour = (int*) malloc((n + 1) * sizeof(int));
    int* tourCourant = (int*) malloc((n + 1) * sizeof(int));
    int maxIterations = 400;
    // premierTour contiendra les index, on va convertir plus tard en noms de sommets
    
    // construire un premier chemin par le plus proche voisin
    premierTour = plusProcheVoisin(0, matriceCout, marque, n);

    printf("Le premier tour (par le plus proche voisin) : ");
    afficherTab(premierTour, n);

    // Escalade par 2-opt
    tourCourant = premierTour;
    
    float coutMin = coutTour(matriceCout, n, tourCourant);
    printf("Cout du premier tour : %.2f\n\n", coutTour(matriceCout, n, tourCourant));

    printf("************ Début de l'exploration ************\n\n");

    while(maxIterations >= 0){
        // printf("tourCourant est ");
        // afficherTab(tourCourant, n);
        nextTour = voisinComplet(matriceCout, n, tourCourant);

        if(nextTour == NULL){
            printf("\n************* Fin de l'exploration *************\n\n");
            break;
        }
        tourCourant = nextTour;
        printf("Cout du meilleur voisin : %.2f | Continuer...\n", coutTour(matriceCout, n, tourCourant));

        maxIterations--;
    }
    printf("A visité tous les voisins/a atteint le max d'itérations. Solution trouvée : ");
    afficherTab(tourCourant, n);
    printf("Cout : %.2f\n", coutTour(matriceCout, n, tourCourant));

}

static int alea(int lower, int upper){
    return (rand() % (upper-lower+1) + lower);
}

void recuitSimule(float* matriceCout, int* marque, int n){
    printf("************** Recuit Simulé **************\n\n");

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
    premierTour = plusProcheVoisin(0, matriceCout, marque, n);

    printf("Le premier tour (par le plus proche voisin) : ");
    afficherTab(premierTour, n);
    printf("Cout du premier tour : %.2f\n", coutTour(matriceCout, n, premierTour));

    // Escalade par 2-opt
    tourCourant = premierTour;
    srand(time(0));

    printf("************ Début de l'exploration ************\n\n");

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
                    // printf("mauvais candidat choisit | ");
                    copierTab(tourCandidat, tourCourant, 0, n);
                } //else printf("aucun choisi | ");
            }

        }

        printf("Resultat du palier à T = %.2f : Cout %.2f\n", T, coutTour(matriceCout, n, tourCourant));
        // printf("Tour courant : ");
        // afficherTab(tourCourant, n);
        T = alpha * T;
    }

    printf("\n************* Fin de l'exploration *************\n\n");
    printf("Solution trouvée : ");
    afficherTab(tourCourant, n);
    printf("Cout : %.2f\n", coutTour(matriceCout, n, tourCourant));

}


typedef struct Chromosome {
    int* tour;
    float cout;
} Chromosome;

void afficherCh(Chromosome ch, int n){
    afficherTab(ch.tour, n);
    printf("Cout : %.2f\n", ch.cout);
}

int trouver(int x, int* tab, int n){
    for(int i=0; i<n; i++){
        if (tab[i] == x) return true;
    }
    return false;

}

void populationInit(Chromosome *population, int pop_size, float* matriceCout, int* marque, int n){
    //int* tour = (int*) malloc((n + 1) * sizeof(int));
    int start = 0;

    for(int i = 0; i < pop_size; i++){
        start = alea(0, n-1);
        population[i].tour = plusProcheVoisin(start, matriceCout, marque, n);
        population[i].cout = coutTour(matriceCout, n, population[i].tour);
    }
}

int selection(Chromosome *population, float* selection_prob, int pop_size){
    // generate a random number between 0 and 1
    float r = (float) rand() / (float) RAND_MAX;  
    float sum_prob = 0.0;
    for (int i = 0; i < pop_size; i++) {
        sum_prob += selection_prob[i];
        if (r <= sum_prob) {
            return i;
            break;
        }
    }
    return alea(0, pop_size);
}

void reproduire(Chromosome enfant, Chromosome parent1, Chromosome parent2, int n, float pc, int* compt_croisement){
    // croiser selon le taux de croisement
    float r = (float)rand()/RAND_MAX;
    if(r > pc){
        // pas de croisement
        // enfant = meilleur parent
        if(parent1.cout < parent2.cout){
            copierTab(parent1.tour, enfant.tour, 0, n);
        } else{
            copierTab(parent2.tour, enfant.tour, 0, n);
        }
    } else {
        *compt_croisement += 1;
        // croisement par Order Crossover (OX) operator
        // pick random positions p1 and p2
        int p1 = alea(0, n-2);
        int p2 = alea(p1+1, n-1);

        int* segment = (int*)malloc((p2-p1+1)*sizeof(int));
        int* remaining = (int*)malloc((n-(p2-p1+1))*sizeof(int));

        // segment = parent[p1..p2]
        copierTab(&parent1.tour[p1], segment,0, p2-p1);

        // child[p1..p2] = segment from parent 1
        copierTab(segment, &enfant.tour[p1], 0, p2-p1);

        // fill remaining points from parent 2 and skip dupplicates
        for(int i=0, j=0; i<n; i++){
            if(!trouver(parent2.tour[i], segment, p2-p1+1)){
                remaining[j] = parent2.tour[i];
                j++;
            }
        }


        // fill remaining  spots in child
        // skip [p1..p2] because they've already been filled
        for(int i =0, j=0; i<n; i++){
            if(i == p1){
                i = p2;
                continue;
            }
            enfant.tour[i] = remaining[j];
            j++;
        }
        enfant.tour[n] = enfant.tour[0]; // fermer le circuit
    }
}

void mutation(Chromosome enfant, int n){
    int p1 = alea(1, n-1);
    int p2 = alea(p1, n-1);
    int temp = enfant.tour[p2];
    enfant.tour[p2] = enfant.tour[p1];
    enfant.tour[p1] = temp;

}

void algorithmeGenetique(float* matriceCout, int* marque, int n){

    int gen_count = 0, max_gen_count = 100;
    int pop_size = 10000; //taille population

    float pc = 0.6; //taux de croisement
    float pm = 1.75 / (pop_size * sqrt(n+1)); // taux de mutation
    if(pm < 0.002) pm = 0.002;
    srand(time(0));
    float cout_moyen, meilleur_cout;
    int compt_croisement, compt_mutation, meilleur_indice;

    // allouer de la mémoire pour la population
    Chromosome *population = (Chromosome*) malloc(pop_size * sizeof(Chromosome));
    for (int i = 0; i < pop_size; i++) {
        population[i].tour = (int*) malloc((n + 1) * sizeof(int));
    }
    Chromosome *next_population = (Chromosome*) malloc(pop_size * sizeof(Chromosome));
    for (int i = 0; i < pop_size; i++) {
        next_population[i].tour = (int*) malloc((n + 1) * sizeof(int));
    }

    float* selection_prob = (float*)malloc(pop_size * sizeof(float));

    // ETAPE 1 - INITIALISATION
    // créer la population initiale par le plus proche voisin,
    // avec des points de départ aléatoires
    populationInit(population, pop_size, matriceCout, marque, n);

    while(gen_count <= max_gen_count){

        next_population = (Chromosome*) malloc(pop_size * sizeof(Chromosome));
        for (int i = 0; i < pop_size; i++) {
            next_population[i].tour = (int*) malloc((n + 1) * sizeof(int));
        }

        cout_moyen = 0;
        meilleur_cout = INT_MAX;
        compt_croisement = 0;
        compt_mutation = 0;
        meilleur_indice = -1;

        // ETAPE 2 - SELECTION

        // Calculate the total fitness of the population
        // (generation stats at the same time)
        float total_fitness = 0.0;
        for (int i = 0; i < pop_size; i++) {
            total_fitness += 1.0 / population[i].cout;  // assume lower cost is better fitness
            cout_moyen += population[i].cout;
            if(population[i].cout < meilleur_cout){
                meilleur_cout = population[i].cout;
                meilleur_indice = i;
            }
        }
        cout_moyen = cout_moyen/pop_size;

        // Calculate the selection probability for each individual
        for (int i = 0; i < pop_size; i++) {
            selection_prob[i] = (1.0 / population[i].cout) / total_fitness;
        }

        int x = -1, y = -1;
        for(int i = 0; i < pop_size; i ++){
            x = selection(population, selection_prob, pop_size);
            y = selection(population, selection_prob, pop_size);
            if(x == -1 || y == -1) printf("Aucun parent sélectionné.\n");

            // ETAPE 3 - CROISEMENT
            reproduire(next_population[i], population[x], population[y], n, pc, &compt_croisement);
            next_population[i].cout = coutTour(matriceCout, n, next_population[i].tour);

            // ETAPE 4 - MUTATION
            float r = (float)rand()/RAND_MAX;
            if(r < pm){
                mutation(next_population[i], n);
                compt_mutation += 1;
            }
        }   

        printf("\n*********** GENERATION %d - STATS ***********\n", gen_count);
        printf("Moyenne des coûts : %.2f\n", cout_moyen);
        printf("Meilleur coût : %.2f\n", meilleur_cout);
        printf("Nombre de croisements : %d/%d\n", compt_croisement, pop_size);
        printf("Nombre de mutations : %d/%d\n", compt_mutation, pop_size);

        if(meilleur_cout < 7500){
            printf("\n\nCritère de fin atteint.\n");
            printf("Le meilleur circuit trouvé est :");
            afficherCh(population[meilleur_indice], n);

            break;
            // end of program
            // si on trouve un enfant suffisemment adéquat
        }
        if(gen_count == max_gen_count){
            printf("\n\nMax de générations atteint.\n");
            printf("Le meilleur circuit trouvé est :");
            afficherCh(population[meilleur_indice], n);
        }

        // ETAPE 5 - REMPLACEMENT
        // l'étape du croisement comporte la stratégie de remplacement
        // s'il y a croisement, l'enfant remplace les parents
        // s'il n'y a pas de croisement, le meilleur parent est conservé
        // ce processus est répété pop_size fois
        population = next_population;

        gen_count ++;

    }
    

}

