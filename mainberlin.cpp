#include <stdio.h>
#include <stdlib.h>
#include "berlin.h"



int main(){
    
    // lire les coordonnées

    FILE * fp;
    fp = fopen ("berlin52.txt", "r");
    int nbrPoints = 0;
    fscanf(fp, "%d\n", &nbrPoints);

    Point *points = (Point*) malloc(nbrPoints * sizeof(Point));
    int index = 0;
    while(fscanf(fp, "%d %f %f\n", &points[index].num, &points[index].x, &points[index].y) == 3){
        index++;
    } 
    fclose(fp);

    // afficher les points lus
    // for(int i = 0; i < 52; i++){
    //     printf("Point %d - x = %f   y = %f\n", points[i].num,points[i].x, points[i].y);
    //     index++;
    // }

    // construire la matrice des couts entre tous les points
    float* matriceCout = (float*) malloc(sizeof(float)*nbrPoints*nbrPoints);
    for(int i = 0; i < nbrPoints; i++){
        for(int j = 0; j < nbrPoints; j++){
            matriceCout[i * nbrPoints + j] = distance(
                points[i], points[j]
            ); 
        }
    }

    // tableau marqués
    int* marque = (int*) malloc(sizeof(float)*nbrPoints);
    initMarque(marque, nbrPoints);

    //escaladeSimple(matriceCout, marque, nbrPoints);
    escaladeComplete(matriceCout, marque, nbrPoints);




}
