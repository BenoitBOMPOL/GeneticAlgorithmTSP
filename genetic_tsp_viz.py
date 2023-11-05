#!/usr/bin/env python3

from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
from random import shuffle, randint, choices, sample, random, seed


# Genetic Algorithm Constants
POPULATION_SIZE = 5000
GENERATION_COUNT = 50_000
MUTATION_RATE = 0.15
NB_PARENTS = 100

# Settings of the TSP
N = 50
BOUND_MAX = 20
BOUNDS = [0, BOUND_MAX]

def build_points(nb_points):
    points = []
    while len(points) < nb_points:
        x = randint(0, BOUND_MAX)
        y = randint(0, BOUND_MAX)
        if (x, y) not in points:
            points.append((x, y))
    return points

def build_dist_matrix(points):
    n = len(points)
    dists = np.zeros((n, n))
    for u in range(n - 1):
        for v in range(u + 1, n):
            d_uv = np.linalg.norm(np.array(points[u]) - np.array(points[v]))
            dists[u][v] = d_uv
            dists[v][u] = d_uv
    return dists

POINTS = build_points(N)
DISTS = build_dist_matrix(POINTS)

def alt_shuffle():
    indexes = list(range(1, N))
    shuffle(indexes)
    return [0] + list(indexes)

def get_cycle_length(indexes):
    cycle_indexes = indexes + [indexes[0]]
    total_distance = np.sum([DISTS[cycle_indexes[i]][cycle_indexes[i+1]] for i in range(N)])
    return total_distance

def get_cycle_fitness(indexes):
    return 1 / (get_cycle_length(indexes)**2)

def get_parents_from_roulette(pop : list[list[int]]):
    fitness_vals = [get_cycle_fitness(ind) for ind in pop]
    total_fitness = sum(fitness_vals)
    probs = [fit / total_fitness for fit in fitness_vals]
    parents = choices(pop, probs, k=NB_PARENTS)
    return parents

def valid_crossover(p1, p2):
    cross_point = randint(1, N - 2)
    child = p1[:cross_point]
    missing_elements = set(p2) - set(child)
    child.extend([elem for elem in p2 if elem in missing_elements])
    return child

def visualization(indexes, gen_no):
    plt.figure(figsize=(10, 10), dpi=200)

    for i, (x, y) in enumerate(POINTS):
        facecolor = 'red' if i == 0 else 'blue'
        circle = plt.Circle((x, y), radius=0.5, fill=False, edgecolor=facecolor, linewidth=2, label=f'Point {i}', zorder=2)
        plt.gca().add_patch(circle)
        plt.text(x, y, str(i), fontsize=12, ha='center', va='center', zorder=3)

    for i in range(N):
        plt.plot(
            [POINTS[indexes[i]][0], POINTS[indexes[(i+1) % N]][0]],
            [POINTS[indexes[i]][1], POINTS[indexes[(i+1) % N]][1]],
            linewidth=0.5, color='black'
        )

    plt.xlim(BOUNDS)
    plt.xticks(BOUNDS)
    plt.ylim(BOUNDS)
    plt.yticks(BOUNDS)

    plt.suptitle(f"Cycle found at gen. {gen_no} :")
    plt.title("TSP - Genetic Algorithm")
    plt.axis('equal')
    plt.savefig(f"genetic_tsp_{gen_no}.png")
    plt.close()

# Finding a best starting individual by heuristic
def get_heuristical_best():
    indexes = [0]
    available_indexes = list(range(N))

    while len(indexes) < N:
        last_index = indexes[-1]
        curr_min = float("inf")
        curr_index = -1
        for index in [i for i in available_indexes if i not in indexes]:
            if DISTS[last_index][index] < curr_min:
                curr_min = DISTS[last_index][index]
                curr_index = index
        indexes.append(curr_index)
    return indexes

GENERATIONS_UPDATE = []
BEST_CYCLE_LENGTH = []

def main():
    """
        TODO
    """
    population = [alt_shuffle() for _ in range(POPULATION_SIZE)]
    population.sort(key=get_cycle_length)
    best_individual = deepcopy(population[0])
    best_cycle_length = get_cycle_length(best_individual)

    for gen_no in range(GENERATION_COUNT):
        # print(f"Generation n°{gen_no}")
        parents = get_parents_from_roulette(population)
        children = []
        for p_id in range(0, NB_PARENTS, 2):
            p1 = parents[p_id]
            p2 = parents[p_id + 1]
            c_12 = valid_crossover(p1, p2)
            c_21 = valid_crossover(p2, p1)
            children.extend([c_12, c_21])
        population[-len(children):] = children

        for i in range(POPULATION_SIZE):
            if random() < MUTATION_RATE:
                i1, i2 = sample(range(1, N), 2)
                population[i][i1], population[i][i2] = population[i][i2], population[i][i1]

        population.sort(key=get_cycle_length)

        best_cycle_length_gen = get_cycle_length(population[0])
        if best_cycle_length_gen < best_cycle_length or gen_no == 0:
            GENERATIONS_UPDATE.append(gen_no)
            BEST_CYCLE_LENGTH.append(best_cycle_length_gen)
            best_individual = deepcopy(population[0])
            print(f"[GA-TSP] Best cycle found at generation {gen_no} with a cycle length = {round(best_cycle_length_gen, 4)}")
            best_cycle_length = best_cycle_length_gen
            visualization(best_individual, gen_no)

    visualization(best_individual, GENERATION_COUNT)


if __name__ == "__main__":
    heuristical_best = get_heuristical_best()
    heuristical_cycle_length = get_cycle_length(heuristical_best)
    print(f"Heuristical reference : {round(heuristical_cycle_length, 4)}")
    visualization(heuristical_best, "heuristic")

    main()

    plt.plot(GENERATIONS_UPDATE, BEST_CYCLE_LENGTH, color = "blue")
    plt.plot([0, GENERATION_COUNT], [heuristical_cycle_length, heuristical_cycle_length], color = "red", linestyle = "dashed")
    plt.show()
