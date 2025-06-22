
![snowflake_1](https://github.com/user-attachments/assets/4e0d7c11-dfa9-4f71-ad3d-be2a5ce11d62)

# ASCII SNOWFLAKE

ASCII Snowflake Generator is a C++23 command-line program that produces intricate, symmetric snowflake patterns on a hexagonal grid using cellular automata and a basic genetic algorithm. The output is rendered entirely in ASCII, and the resulting patterns reflect a blend of structured geometry and emergent complexity.

The project is entirely self-contained. It requires only the C++ standard library and a single vendored header-only dependency: nlohmann/json.

## How to Use  
To run the program:  
`ascii_snowflake.exe settings.json [optional numeric seed]`  
* `settings.json` is a required configuration file (explained below).
* The optional seed ensures repeatable randomness.

## How it Works

The program evolves cellular automata on a hexagonal grid to generate symmetric, snowflake-like patterns. Each snowflake is the result of:

1. A randomly generated rule table (state transition logic)

2. A symmetric seed initialized in a 60° wedge

3. A series of automaton updates

4. A fitness function that scores the result

5. A simple genetic algorithm that selects and recombines the best rule tables over multiple generations

See **Algorithm Details** below for the full breakdown of the algorithm.

## Algorithm Details  
Snowflake generation proceeds through a genetic algorithm that evolves a population of cellular automaton rule tables over several generations.

### Main Loop
Each generation follows this process:

1. **Initial Population**  
A random population of population_sz state tables is created. These define the cellular automaton update rules.

2. **Snowflake Growth and Scoring**  
For each generation:

    * A pool of num_children new candidate rule tables is created by mixing pairs of parent tables (randomly selected from the previous generation).

    * Each child is paired with a random symmetric seed, initialized with a given primordial_soup_density, primordial_soup_radius, and number of num_states.

    * These seeds are evolved into snowflakes by running the cellular automaton for num_iterations steps.

    * Each snowflake is scored using the configured fitness metrics.

3. **Selection**  
The top population_sz snowflakes are selected based on score. Their associated rule tables form the next generation.

4. **Termination Criteria**  
The algorithm tracks whether the average score improves:

    * If the new generation outperforms the previous one (higher mean score), it proceeds.

    * Otherwise, it retries (up to tries_per_generation times) before terminating early.

    * Evolution halts after max_generations or when no improvement is detected.

### Parallel Execution  
To accelerate performance, snowflake generation is parallelized using std::execution::par. Each child rule table and its associated seed are evolved independently, making the process embarrassingly parallel.

### Final Output  
After the final generation:

* The top num_output_snowflakes snowflakes are returned, each representing a distinct state table and its evolved pattern.

## Hex Grids and ASCII Rendering

Hexagonal grids are implemented using hash maps keyed by cube coordinates (q, r, s) with the constraint q + r + s = 0. This coordinate system makes it easier to compute distances, neighbor relationships, and rotations. See [Red Blob Games for a detailed explanation](https://www.redblobgames.com/grids/hexagons/).

### ASCII Layout  
In ASCII rendering, each hex cell is drawn using two adjacent characters (e.g. "[]", "oo", "{}"). These are arranged in a staggered, brick-like pattern:

* Even-numbered rows are offset horizontally to form a proper hex grid topology.

* Most terminal fonts are taller than they are wide, making this two-character format a good visual approximation of a regular hexagon.

* As a result, ASCII becomes a surprisingly effective medium for depicting radial symmetry and hex-based geometry.

## Symmetric Initialization  
Snowflake seeds are initialized within a 60° triangular wedge of the hex grid, then reflected and rotated to create full symmetry.

### Step-by-Step:  
1. **Triangular Region**  
The seed occupies an equilateral triangle in cube coordinates, radiating from the origin.

2. **Bilateral Symmetry**  
Cells within the wedge are mirrored across the vertical axis.

3. **Sixfold Rotation**  
The wedge is cloned and rotated in 60° increments using, forming a complete snowflake.

This process ensures that all snowflakes start with perfect symmetry before any evolution occurs, and because the cellular automaton rules treat all neighborhood directions identically, the snowflakes are guaranteed to retain the symmetry of their initial seeds as they evolve.

## State Tables  
Each state table defines rules for the automaton. It’s a 2D array:

`state_table[current_state][neighbor_sum] → next_state`  
* **Rows** = current cell state (0 = dead; others = alive).

* **Columns** = sum of the six neighboring states.

* **Values** = resulting new state of the cell.

These tables govern the cellular automaton’s evolution behavior.

## Scoring Metrics  
After a snowflake has evolved for a fixed number of iterations, it is scored using several heuristics. These are combined into a weighted fitness value:

* **Connectedness**  
Rewards fully contiguous structures. Partial credit for diagonal-only connectivity.

* **Airiness**  
Favors snowflakes with open space (low density within a bounding radius).

* **Cragginess**  
Detects local edge complexity by checking peripheral cells near dense clusters.

* **Spikiness**  
Measures radial arm extension by checking how much structure lies along the outermost triangle edges.

Each metric has an associated weight, and candidates falling outside density or radius thresholds are discarded.

## JSON Configuration  

All settings are provided via a JSON file:

```json
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
### Parameter Descriptions

| Key | Description |
|-----|-------------|
| `population_sz` | Number of state tables per generation |
| `num_children` | Number of child tables created per generation |
| `num_states` | Possible states per cell (0 = dead) |
| `primordial_soup_density` | Probability a seed cell is alive |
| `primordial_soup_radius` | Radius of triangular seed region |
| `state_table_density` | Probability a rule is nonzero |
| `max_generations` | Evolution stops after this many generations |
| `tries_per_generation` | Retry attempts before skipping a generation |
| `num_iterations` | Iterations per snowflake |
| `num_output_snowflakes` | Number of snowflakes returned at the end |

**Scoring Parameters:**

| Key | Description |
|-----|-------------|
| `connectedness_weight` | Reward for structural cohesion |
| `airiness_weight` | Reward for sparse, open patterns |
| `spikiness_weight` | Emphasizes long radial protrusions |
| `cragginess_weight` | Emphasizes irregular edges |
| `max_density`, `min_density` | Bounds on filled cell ratio |
| `max_radius`, `min_radius` | Bounds on overall snowflake size |

## Sample Output
![snowflake_2](https://github.com/user-attachments/assets/3bce1271-fcca-4b3f-8d58-7c612e4a918c)
![snowflake_3](https://github.com/user-attachments/assets/89e5643e-fdb5-478e-a7b4-1fbc60b65e9e)
![snowflake_4](https://github.com/user-attachments/assets/2c5b853d-2f83-4e9c-b8e9-6dd09d7ce8f8)

