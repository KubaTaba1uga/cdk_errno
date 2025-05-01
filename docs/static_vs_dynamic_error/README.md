# Error‑Allocation Benchmark

This repository contains a complete benchmark suite to compare two error‑allocation strategies in C:

- **Static buffer**: a single `malloc` per error, with in‑struct fixed‑size buffers.
- **Dynamic fields**: multiple `malloc` calls per error, one for each string field.

---

## Objectives

- Measure and compare the **runtime** overhead of two allocation strategies for error objects:
  1. **Static**: single `malloc(sizeof *err)` with in‑struct char arrays.
  2. **Dynamic**: one `malloc` per string field (`msg`, `source_file`, etc.).
- Measure **memory usage** (peak heap) under different workloads.
- Identify break‑even points and recommend the most efficient approach for various scenarios.

---

## Hypotheses

- **H₀**: No significant difference in per‑error allocation time between static and dynamic strategies.
- **H₁**: One strategy will show lower time or memory overhead at certain workloads.

---

## Implementation Details

### API Variants

1. **Static version** (`cme_error_static_*`)
   - Single `malloc(sizeof *err)`.
   - In‑struct buffers: `char msg[CME_STR_MAX]`, no additional frees.
   - `destroy` calls `free(err)` only.

2. **Dynamic version** (`cme_error_dyn_*`)
   - `malloc(sizeof *err)` + `malloc`/`strdup` for each string field.
   - `destroy` frees each field, then the struct.

### Benchmark plan

The benchmark set is split into three layers—single-thread micro-cases, multi-thread scalability, and an empty results matrix.

#### 1 — Single-thread burst tests (realistic live-set ≤ 8)  
*Measures per-error latency and cache behaviour when only a handful of errors coexist.*

| Scenario (single thread)            | Total errors `--max` | Live errors `--batch` |
|-------------------------------------|----------------------|-----------------------|
| Cold-start / immediate free         |      10 000          |           1           |
| Tiny burst (call-stack propagation) |      10 000          |           4           |
| Occasional cascade                  |      10 000          |           8           |

---

#### 2 — Thread-scalability burst-1 test  
*Evaluates allocator contention and scalability when many worker threads each create one live error at a time.*

| Threads | Errors per thread | Total errors |         Note         |
|---------|-------------------|--------------|----------------------|
|    1    |       10 000      |     10 000   | baseline             |
|    2    |       10 000      |     20 000   | allocator contention |
|    4    |       10 000      |     40 000   | typical CPU          |
|    8    |       10 000      |     80 000   | stress test          |

*(Each thread runs the burst-1 loop in `test_static` or `test_dynamic`; join at the end.)*

---

#### 3 — Results recording template  
*Fill this with wall-clock time and peak RSS after running the drivers above.*

| Burst / Threads | Static time (ms) | Dynamic time (ms) | Δ % (Dyn-Static) | Static peak KB | Dynamic peak KB | Δ % |
|-----------------|------------------|-------------------|------------------|----------------|-----------------|-----|
| 1 / 1 thr | | | | | | |
| 4 / 1 thr | | | | | | |
| 8 / 1 thr | | | | | | |
| 1 / 2 thr | | | | | | |
| 1 / 4 thr | | | | | | |
| 1 / 8 thr | | | | | | |
```

Simply invoke the binaries like:

```bash
./test_static  --max=10000 --batch=4
./test_dynamic --max=10000 --batch=4
```

and copy the numbers into the template.
