#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Point Location Settings
#define NB_POINTS 100
#define BOUND 50

// Genetic Algorithm settings
#define GENERATION_COUNT 50000
#define POPULATION_SIZE 1000
#define MUTATION_RATE 0.15
#define NB_PARENTS 200

// Useful Structures
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int path[NB_POINTS];
    double cycle_length;
    double fitness_value;
} Solution;

double distance(Point p1, Point p2) {return sqrt((double)(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));}

void initPointsDists(Point points[NB_POINTS], double dists[NB_POINTS][NB_POINTS]){
    for (int i = 0; i < NB_POINTS; i++){
        points[i].x = rand() % (BOUND + 1);
        points[i].y = rand() % (BOUND + 1);
    }

    for (int i = 0; i < NB_POINTS; i++){
        dists[i][i] = 0.0;
        for (int j = i + 1; j < NB_POINTS; j++){
            double d_ij = distance(points[i], points[j]);
            dists[i][j] = d_ij;
            dists[j][i] = d_ij;
        }
    }
}

double eval_fitness(double cycle_length){return 1 / pow(cycle_length, 1.5);}

void initPopulation(Solution population[POPULATION_SIZE], Point points[NB_POINTS]){
    for (int i = 0; i < POPULATION_SIZE; i++){
        for (int j = 0; j < NB_POINTS; j++){
            population[i].path[j] = j;
        }

        for (int j = NB_POINTS - 1; j > 0; j--) {
            int randomIndex = rand() % (j + 1);
            int temp = population[i].path[j];
            population[i].path[j] = population[i].path[randomIndex];
            population[i].path[randomIndex] = temp;
        }

        double cycle_length = 0.0;
        for (int j = 0; j < NB_POINTS; j++){
            cycle_length += distance(points[population[i].path[j]], points[population[i].path[(j+1)%NB_POINTS]]);
        }

        population[i].cycle_length = cycle_length;
        population[i].fitness_value = eval_fitness(cycle_length);
    }
}

void showPopulation(Solution population[POPULATION_SIZE]){
    for (int i = 0; i < POPULATION_SIZE; i++){
        printf("%d : [", i);
        for (int j = 0; j < NB_POINTS; j++){
            printf("%d", population[i].path[j]);
            if (j < NB_POINTS - 1){
                printf(", ");
            }
        }
        printf("], cycle_length = %f, fitness = %f\n", population[i].cycle_length, population[i].fitness_value);
    }
}

void showParents(Solution parents[NB_PARENTS]){
    for (int i = 0; i < NB_PARENTS; i++){
        printf("%d : [", i);
        for (int j = 0; j < NB_POINTS; j++){
            printf("%d", parents[i].path[j]);
            if (j < NB_POINTS - 1){
                printf(", ");
            }
        }
        printf("], cycle_length = %f, fitness = %f\n", parents[i].cycle_length, parents[i].fitness_value);
    }
}

void copySolution(Solution* dest, const Solution* src) {
    for (int i = 0; i < NB_POINTS; i++) {
        dest->path[i] = src->path[i];
    }

    dest->cycle_length = src->cycle_length;
    dest->fitness_value = src->fitness_value;
}

int compareSolutions(const void *a, const void *b) {
    Solution *solA = (Solution *)a;
    Solution *solB = (Solution *)b;
    if (solA->cycle_length < solB->cycle_length) {
        return -1;
    } else if (solA->cycle_length > solB->cycle_length) {
        return 1;
    } else {
        return 0;
    }
}

void sortPopulation(Solution population[POPULATION_SIZE]) {
    qsort(population, POPULATION_SIZE, sizeof(Solution), compareSolutions);
}

void applyMutation(Solution population[POPULATION_SIZE], Point points[NB_POINTS]){
    for (int i = 0; i < POPULATION_SIZE; i++){
        double p = (double) rand() / RAND_MAX;
        if (p < MUTATION_RATE){
            int i1, i2;
            do {
                i1 = rand() % NB_POINTS;
                i2 = rand() % NB_POINTS;
            } while (i1 == i2);

            int temp = population[i].path[i1];
            population[i].path[i1] = population[i].path[i2];
            population[i].path[i2] = temp;

            double cycle_length = 0.0;
            for (int j = 0; j < NB_POINTS; j++){
                cycle_length += distance(points[population[i].path[j]], points[population[i].path[(j+1)%NB_POINTS]]);
            }
            population[i].cycle_length = cycle_length;
            population[i].fitness_value = eval_fitness(cycle_length);
        }
    }
}

void selectParents(Solution population[POPULATION_SIZE], Solution parents[NB_PARENTS]){
    double totalFitness = 0.0;
    double fitnessVals[POPULATION_SIZE];

    for (int i = 0; i < POPULATION_SIZE; i++){
        fitnessVals[i] = population[i].fitness_value;
        totalFitness += fitnessVals[i];
    }

    for (int i = 0; i < NB_PARENTS; i++){
        double randValue = ((double) rand() / RAND_MAX) * totalFitness;
        double partialSum = 0.0;
        int parentIndex = 0;

        for (int j = 0; j < POPULATION_SIZE; j++){
            partialSum += fitnessVals[j];
            if (partialSum >= randValue){
                parentIndex = j;
                break;
            }
        }
        
        copySolution(&parents[i], &population[parentIndex]);
    }
}

bool isElementInArray(const int arr[], int element, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == element) {
            return true;
        }
    }
    return false;
}

void validCrossover(const Solution* parent1, const Solution* parent2, Solution* child, Point points[NB_POINTS]) {
    int crossPoint = rand() % (NB_POINTS - 2) + 1;

    for (int i = 0; i < crossPoint; i++) {
        child->path[i] = parent1->path[i];
    }

    int missingIndex = crossPoint;

    for (int i = 0; i < NB_POINTS; i++) {
        int element = parent2->path[i];

        if (!isElementInArray(child->path, element, missingIndex)) {
            child->path[missingIndex++] = element;
        }
    }

    double child_length = 0.0;
    for (int j = 0; j < NB_POINTS; j++) {
        child_length += distance(points[child->path[j]], points[child->path[(j + 1) % NB_POINTS]]);
    }
    child->cycle_length = child_length;
    child->fitness_value = eval_fitness(child_length);
}

void buildChildren(Solution population[POPULATION_SIZE], Solution parents[NB_PARENTS], Solution children[NB_PARENTS], Point points[NB_POINTS]){
    for (int c = 0; c < NB_PARENTS; c += 2){
        Solution p1 = parents[c];
        Solution p2 = parents[c + 1];
        validCrossover(&p1, &p2, &children[c], points);
        validCrossover(&p2, &p1, &children[c+1], points);
        copySolution(&population[POPULATION_SIZE - 1 - c], &children[c]);
        copySolution(&population[POPULATION_SIZE - 2 - c], &children[c + 1]);
    }
}

void get_heuristical_best(Solution *best, double dists[NB_POINTS][NB_POINTS]) {
    int indexes[NB_POINTS];
    int available_indexes[NB_POINTS];

    for (int i = 0; i < NB_POINTS; i++) {
        indexes[i] = -1;
        available_indexes[i] = i;
    }

    indexes[0] = 0;
    available_indexes[0] = -1;

    for (int count = 1; count < NB_POINTS; count++) {
        int last_index = indexes[count - 1];
        double curr_min = INFINITY;
        int curr_index = -1;

        for (int index = 0; index < NB_POINTS; index++) {
            if (available_indexes[index] != -1) {
                double dist = dists[last_index][index];
                if (dist < curr_min) {
                    curr_min = dist;
                    curr_index = index;
                }
            }
        }

        indexes[count] = curr_index;
        available_indexes[curr_index] = -1;
    }

    for (int i = 0; i < NB_POINTS; i++) {
        best->path[i] = indexes[i];
    }
    best->cycle_length = 0.0;
    for (int i = 0; i < NB_POINTS; i++) {
        int j = (i + 1) % NB_POINTS;
        best->cycle_length += dists[best->path[i]][best->path[j]];
    }
    best->fitness_value = eval_fitness(best->cycle_length);
}


int main() {
    srand(time(NULL));

    Point points[NB_POINTS];
    double dists[NB_POINTS][NB_POINTS];
    Solution population[POPULATION_SIZE];
    Solution parents[NB_PARENTS];
    Solution children[NB_PARENTS];

    initPointsDists(points, dists);
    
    FILE *file = fopen("genetic.log", "w");
    if (file == NULL) {
        return EXIT_FAILURE;
    }

    fprintf(file, "%d\n", BOUND);               // First Line : Graphical Bound
    for (int i = 0; i < NB_POINTS; i++) {
        fprintf(file, "%d", points[i].x);       // Sec.  Line : X pos of points
        if (i < NB_POINTS - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");
    for (int i = 0; i < NB_POINTS; i++) {
        fprintf(file, "%d", points[i].y);       // Trd.  Line : Y pos of points
        if (i < NB_POINTS - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    initPopulation(population, points);
    sortPopulation(population);

    Solution best_cycle;
    copySolution(&best_cycle, &population[0]);
    double best_cycle_length = best_cycle.cycle_length;
    double best_cycle_length_gen = best_cycle.cycle_length;

    Solution heuristical_best;
    get_heuristical_best(&heuristical_best, dists);
    double heuristical_length = heuristical_best.cycle_length;
    printf("Heuristical reference : %f\n", heuristical_length);

    fprintf(file, "h : [");
    for (int i = 0; i < NB_POINTS; i++) {
        fprintf(file, "%d", heuristical_best.path[i]);
        if (i < NB_POINTS - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "]\n");

    // Heuristical basis
    // for (int i = 0; i < POPULATION_SIZE; i++){
    //     copySolution(&population[i], &heuristical_best);
    // }

    for (int g = 0; g < GENERATION_COUNT; g++){
        selectParents(population, parents);
        buildChildren(population, parents, children, points);
        applyMutation(population, points);
        sortPopulation(population);
        best_cycle_length_gen = population[0].cycle_length;
        if (best_cycle_length_gen < best_cycle_length || g == 0){
            fprintf(file, "%d : [", g);
            for (int i = 0; i < NB_POINTS; i++) {
                fprintf(file, "%d", population[0].path[i]);         // Order of points in the same line
                if (i < NB_POINTS - 1) {
                    fprintf(file, ",");
                }
            }
            fprintf(file, "]\n");
            printf("[GA-TSP] Best cycle found at generation %d with a cycle length %f.\n", g, best_cycle_length_gen);
            best_cycle_length = best_cycle_length_gen;
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}