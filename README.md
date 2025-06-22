
![snowflake_1](https://github.com/user-attachments/assets/4e0d7c11-dfa9-4f71-ad3d-be2a5ce11d62)

# ASCII SNOWFLAKE GENERATOR

This is a C++23 command-line program that generates ASCII images of symmetric, snowflake-like patterns on a hexagonal grid using cellular automata and a simple genetic algorithm. The codebase has no dependencies beyond the C++ standard library and the header-only library nlohmann/json, which is vendored into the codebase.

Usage:

`ascii_snowflake.exe settings.json [optional numeric seed for rnd gen]`

## HOW IT WORKS

The basic idea of this program is the following ... (The algorithm is a little more complex but the following is the gist of it)

1. **Generate a Population of Random State Tables**  
Each state table defines update rules for a cellular automaton operating on a hex grid. These tables are initialized randomly at the start of a run.

2. **Grow Snowflakes via Cellular Automata**  
Each state table is used to evolve a snowflake. The automaton begins with a random, symmetric triangular seed and runs for a fixed number of steps. 

3. **Score Each Snowflake**  
A scoring function evaluates how "snowflake-like" each result is, using a combination of structural and aesthetic heuristics.

4. **Select the Best Snowflakes**  
The top-scoring snowflakes are selected, and their associated state tables are retained for breeding.

5. **Mix the Best State Tables**  
New state tables (children) are created by randomly mixing entries from two parent state tables. Each entry is independently chosen from either parent with 50% probability.

6. **Repeat**  
The new generation of state tables replaces the old, and the process repeats until no improvement is observed or a fixed number of generations has passed.

## HEX GRIDS AND COORDINATES

Hexagonal grids are stored as hash tables that map hex coordinates to cell states. Each cell uses cube coordinates, represented as a triplet (q, r, s) such that q + r + s = 0. This coordinate system simplifies neighborhood queries, rotations, and distance calculations on a regular hex grid.

### ASCII Hex Grid Rendering
In ASCII output, each hex cell is rendered using two adjacent characters (e.g. "[]", "oo", or "{}"). When these character pairs are arranged in a staggered, brick-layer pattern, they replicate the topology of a true hex grid:

* Even rows are offset horizontally, just like rows of bricks or honeycomb cells.

* This works particularly well with monospaced ASCII fonts, because most terminal characters are taller than they are wide.

* As a result, two adjacent ASCII characters — when rendered side by side — more closely approximate the bounding shape of a true hexagon than square pixels would.

This makes ASCII an unexpectedly effective medium for visualizing hexagonal structures like snowflakes.

### STATE TABLES

Each state table defines the evolution rules for an automaton. It is a 2D array of the form:

`state_table[current_state][neighbor_sum] → next_state`

* Rows represent the current state of a cell (e.g. 0 for dead, 1-N for alive states).

* Columns represent the sum of states in the cell’s six neighboring hexes.

* Values specify the cell's next state given its current state and neighborhood.

## SYMMETRIC INITIALIZATION

### Triangular Region
Each snowflake begins from a random but symmetric seed defined within a triangular wedge — a 60° sector of the full snowflake, expressed in cube coordinates. This is a small equilateral triangle of hexes radiating out from the origin.

### Bilateral Symmetry
Random cell states are assigned within this triangle, but every cell is mirrored horizontally across the wedge’s vertical axis. This enforces bilateral symmetry using a flip_horz() function.

### Sixfold Symmetry via Rotation
The symmetric wedge is cloned and rotated around the origin in 60° increments using a rotate() function. The union of all six rotated copies forms a snowflake with strict six-fold radial symmetry.

This guarantees that all initial snowflakes are perfectly symmetric, even before any evolution occurs.

## SCORING METRICS

After running the automaton for a fixed number of iterations, the resulting snowflake is evaluated using a fitness function composed of the following metrics:

### Connectedness
Checks whether the snowflake forms a single contiguous structure. Partial credit is given if the snowflake is only diagonally connected.

### Airiness
Rewards snowflakes that are not overly dense. Calculated as the proportion of empty space in a bounding hex region.

### Cragginess
Measures edge irregularity by identifying peripheral cells that are near clusters of live neighbors and checking for local connectivity.

### Spikiness
Measures how much of the snowflake’s mass lies along the long outer edges of the original triangular wedge. Snowflakes with sharp, radial arm-like protrusions receive higher scores.

Each metric is weighted individually and combined into a final score.

## EVOLUTION AND TERMINATION

A population of state tables is evolved over multiple generations:

* In each generation, snowflakes are grown from the current population of state tables.

* The top performers are selected and used to breed new state tables by entry-wise mixing.

* If the mean score improves, the new generation becomes the basis for the next.

* Otherwise, evolution halts after a fixed number of stagnant attempts.

At the end, the best snowflakes from the final generation are returned.

## OUTPUT

The result is a set of six-fold symmetric hex grid patterns — ASCII snowflakes — grown and selected based on structural beauty and complexity. Each snowflake reflects the behavior of its originating state table and embodies the emergent properties of the underlying automaton.

## JSON SETTINGS FILE

The program is configured via a JSON file passed as the first command-line argument. It must include the following keys:
```
{
  "population_sz": 20,
  "num_children": 200,
  "num_states": 5,
  "primordial_soup_density": 0.35,
  "primordial_soup_radius": 5,
  "state_table_density": 0.5,
  "max_generations": 30,
  "tries_per_generation": 25,
  "num_iterations": 30,
  "num_output_snowflakes": 5,
  "score_params": {
    "connectedness_weight": 1.0,
    "airiness_weight": 3.0,
    "spikiness_weight": 15.0,
    "cragginess_weight": 0.0,
    "max_density": 0.5,
    "max_radius": 35,
    "min_radius": 30
  }
}
```
### PARAMETER DESCRIPTIONS

* population_sz:  
Number of state tables kept per generation.

* num_children:  
Number of children created per generation (before selection).

* num_states:  
Number of possible cell states (0 = dead, others are alive).

* primordial_soup_density:  
Probability a cell is alive in the initial triangle.

* primordial_soup_radius:  
Radius (in hexes) of the triangular seed region.

* state_table_density:  
Probability that a transition rule is nonzero.

* max_generations:  
Maximum number of generations to evolve.

* tries_per_generation:  
Max retry attempts before giving up on a generation.

* num_iterations:  
Number of CA iterations to run per snowflake.

* num_output_snowflakes:  
How many top snowflakes to return.

Inside score_params:

* connectedness_weight:  
Weight for structure connectivity.

* airiness_weight:  
Weight for sparseness / negative space.

* spikiness_weight:  
Weight for sharp, radial edge adherence.

* cragginess_weight:  
Weight for irregular edges.

* max_density, min_density:  
Reject any snowflake denser / less dense than this.

* max_radius, min_radius:  
Reject snowflakes outside this size range.

![snowflake_2](https://github.com/user-attachments/assets/3bce1271-fcca-4b3f-8d58-7c612e4a918c)
![snowflake_3](https://github.com/user-attachments/assets/89e5643e-fdb5-478e-a7b4-1fbc60b65e9e)
![snowflake_4](https://github.com/user-attachments/assets/2c5b853d-2f83-4e9c-b8e9-6dd09d7ce8f8)

