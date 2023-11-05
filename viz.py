#!/usr/bin/env python3
import matplotlib.pyplot as plt

def main():
    with open("genetic.log", "r") as genetic_log:
        file = genetic_log.readlines()
        file = [f[:-1] for f in file]
        bound = int(file[0])
        x_pos = file[1]
        y_pos = file[2]
        x_int = [int(x) for x in x_pos.rsplit(',')]
        y_int = [int(y) for y in y_pos.rsplit(',')]

        points = [(x, y) for (x, y) in zip(x_int, y_int)]
        n = len(points)

        plt.figure(figsize = (10, 10), dpi = 200)
        for i, (xi, yi) in enumerate(points):
            facecolor = 'blue'
            circle = plt.Circle((xi, yi), radius=0.5 * (bound/20)**0.50, fill=False, edgecolor=facecolor, linewidth=2, label=f'Point {i}', zorder=2)
            plt.gca().add_patch(circle)
        
        bounds = [0, bound]
        plt.xlim(bounds)
        plt.xticks(bounds)
        plt.ylim(bounds)
        plt.yticks(bounds)

        plt.axis('equal')
        plt.savefig(f"outputs/genetic_tsp.png")
        plt.close() 

        line = file[3];
        div_line = line.rsplit(" : ")
        print(f"Render of the heuristic")
        indexes = [int(i) for i in div_line[1][1:-1].rsplit(',')]

        plt.figure(figsize = (10, 10), dpi = 100)
        for i, (xi, yi) in enumerate(points):
            facecolor = 'red' if i == indexes[0] else 'blue'
            circle = plt.Circle((xi, yi), radius=0.5 * (bound/20)**0.50, fill=False, edgecolor=facecolor, linewidth=2, label=f'Point {i}', zorder=2)
            plt.gca().add_patch(circle)
            # plt.text(xi, yi, str(i), fontsize=12*((bound/20)**0.5), ha='center', va='center', zorder=3)

        for i in range(n):
            plt.plot(
                [points[indexes[i]][0], points[indexes[(i+1) % n]][0]],
                [points[indexes[i]][1], points[indexes[(i+1) % n]][1]],
                linewidth=0.5, color='black'
            )

        bounds = [0, bound]
        plt.xlim(bounds)
        plt.xticks(bounds)
        plt.ylim(bounds)
        plt.yticks(bounds)

        plt.suptitle("Rendering of the heuristic")
        plt.title("Travelling Salesperson Problem")
        plt.axis('equal')
        plt.savefig(f"outputs/genetic_tsp_.png")
        plt.close() 

        file = file[4:]
        for line in file:
            div_line = line.rsplit(" : ")
            gen_no = int(div_line[0])
            print(f"Rendering at generation {gen_no}")
            indexes = [int(i) for i in div_line[1][1:-1].rsplit(',')]

            plt.figure(figsize = (10, 10), dpi = 100)
            for i, (xi, yi) in enumerate(points):
                facecolor = 'red' if i == indexes[0] else 'blue'
                circle = plt.Circle((xi, yi), radius=0.5 * (bound/20)**0.50, fill=False, edgecolor=facecolor, linewidth=2, label=f'Point {i}', zorder=2)
                plt.gca().add_patch(circle)
                # plt.text(xi, yi, str(i), fontsize=12*((bound/20)**0.5), ha='center', va='center', zorder=3)

            for i in range(n):
                plt.plot(
                    [points[indexes[i]][0], points[indexes[(i+1) % n]][0]],
                    [points[indexes[i]][1], points[indexes[(i+1) % n]][1]],
                    linewidth=0.5, color='black'
                )

            bounds = [0, bound]
            plt.xlim(bounds)
            plt.xticks(bounds)
            plt.ylim(bounds)
            plt.yticks(bounds)

            plt.suptitle(f"Cycle found at gen. {gen_no} :")
            plt.title("TSP - Genetic Algorithm")
            plt.axis('equal')
            plt.savefig(f"outputs/genetic_tsp_{gen_no}.png")
            plt.close() 

if __name__ == "__main__":
    main()