Base — Algorithms and Foundations

Overview

- Provides core algorithms and primitives used by higher layers:
  - State-space search (uninformed and informed)
  - Action planning over binary world states
  - Graph utilities with DFS/BFS/topological sort and cycle detection
  - Differential evolution genetic optimizer
  - Discrete query table and information gain tooling
  - Typed node accessors for VFS values

Key Modules

- ActionPlanner.*
  - Models actions as transitions over a `BinaryWorldState` with preconditions, postconditions, and cost.
  - `ActionPlanner` holds actions and the atom count; `ActionPlannerWrapper` provides name→index mapping and pretty printing.
  - `ActionNode` is a `VfsValueExt` for planning states: caches hash, cost, action id, and links to planner and goal.
  - Typical flow:
    - `SetSize(action_count, atom_count)` → name atoms/actions via wrapper
    - Set per-action pre/post conditions and costs
    - Query possible transitions from a given world state

- SearchAlgos.*
  - Interfaces:
    - `Generator`: supplies the initial node and generates sub-values (children). Call `SetValueFunction` to write to values.
    - `TerminalTester`: returns whether a node is terminal.
    - `HeuristicEval`: scores `Utility`, heuristic `Estimate`, and `Distance` between nodes; also an optional `GetResultString`.
    - `Searcher`: orchestrates algorithm runs given the above; `Search(src)` returns the path as vector of `Val*`.
  - Ready-made strategies:
    - Adversarial: `MiniMax`, `AlphaBeta`
    - Uninformed: `BreadthFirst`, `UniformCost`, `DepthFirst`, `DepthLimited`
    - Informed: `BestFirst`, `AStar` (with open/closed sets and path reconstruction)
  - Helpers: `GeneratorRandom`, `NoSubTerminal`, `SimpleHeuristic`.

- Graph.*
  - Directed graph with key indexing and adjacency in/out lists.
  - DFS/BFS traversal using a `DfsVisitor` (e.g., `CycleDetector`).
  - Topological sort; edge-adding by indices or arbitrary `Value` keys.

- GeneticOptimizer.h
  - Differential evolution solver (Best1/Random1/RandToBest1, bin/exp variants) with configurable scale/probability and population.
  - `Init(dimension, population_count, strategy)` → set bounds (uniform/normdist/manual) → iterate `Start()`/`Stop(energy)` in a loop while `SolveCheck()`.
  - Exposes best and per-population energies and vectors.

- QueryTable.h
  - Templated discrete table with predictors/targets; computes entropy and information gain, and can produce readable summaries per predictor category with weighted scores.

- Node.h
  - Typed access shim around `VfsValue` and its `ext`, casting to `T` with conveniences for pointer/value access and conversion.

Usage Patterns

- Action planning with search
  - Define atoms/events, describe pre/post conditions and costs via `ActionPlannerWrapper`.
  - Implement a `Generator` that emits successor `ActionNode` values using `ActionPlanner::GetPossibleStateTransition`.
  - Provide a `TerminalTester` that checks whether the goal state is reached, and a `HeuristicEval` to guide `AStar`.

- A* over a tree of `Val`
  - Implement `Generator::GenerateSubValues(val)` to append children; use `fs` and `set_value` callback as needed.
  - Provide `HeuristicEval::Estimate(n)` and `Distance(a,b)` for admissibility.
  - Call `AStar::Search(src)` and inspect `GetBest()` if you stop early.

- Differential evolution
  - Initialize ranges via `Min()/Max()` or `MinMax()`; seed random type with `SetRandomTypeUniform()` or `SetRandomTypeNormDist()`.
  - Iterate: `Start()` to propose trial vector; evaluate; `Stop(energy)` to accept/reject; proceed until `IsEnd()`.

- QueryTable insight
  - Add target and predictors (vectors of discrete categories), call `GetLargestInfoGainPredictor()` or consume `GetInfoGains()` and `GetInfoString()` for diagnostics.

Extension Points

- Search
  - Supply your own `Generator`, `TerminalTester`, or `HeuristicEval` for domain-specific search.
  - Extend `Searcher` or add a new strategy implementing `SearchBegin/Iteration/End`.

- Planner
  - Add specialized terminal tests or heuristics tailored for `ActionNode` and `BinaryWorldState`.

- Optimizer
  - Implement additional strategies following existing `Best*/Random*` patterns; keep mutation/crossover semantics consistent.

Gotchas

- Keep `ActionPlanner`’s `atom_count` consistent with any `BinaryWorldState` instances you pass in; mismatches corrupt pre/post condition mapping.
- `Node<T>` requires that `VfsValue.ext` actually contains a `T` (check `CLASSTYPE`/initializers).
- `AStar` maintains open/closed sets using pointer identity; ensure node lifetime is stable while searching.

Public Surface

- Include umbrella: `#include <AI/Core/Base/Base.h>`
- Notable enums: `Enum.h` provides `SearchStrategyType`, `HeuristicType`, `GeneratorType`, `TerminalTestType` for higher-level configuration.

