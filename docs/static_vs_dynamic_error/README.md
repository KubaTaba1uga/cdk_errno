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

#### ✅ Immediate Free Tests

| Strategy | Total Errors | Binary Name                 |
|----------|--------------|-----------------------------|
| Static   | 1,000        | `static_immediate_1000`     |
| Static   | 100,000      | `static_immediate_100000`   |
| Static   | 1,000,000    | `static_immediate_1000000`  |
| Dynamic  | 1,000        | `dynamic_immediate_1000`    |
| Dynamic  | 100,000      | `dynamic_immediate_100000`  |
| Dynamic  | 1,000,000    | `dynamic_immediate_1000000` |

---

#### ✅ Batch Free Tests

| Strategy | Batch Size | Total Errors | Binary Name                    |
|----------|------------|--------------|--------------------------------|
| Static   | 10         | 1,000        | `static_batch_10_1000`         |
| Static   | 10         | 100,000      | `static_batch_10_100000`       |
| Static   | 10         | 1,000,000    | `static_batch_10_1000000`      |
| Static   | 1,000      | 100,000      | `static_batch_1000_100000`     |
| Static   | 1,000      | 1,000,000    | `static_batch_1000_1000000`    |
| Dynamic  | 10         | 1,000        | `dynamic_batch_10_1000`        |
| Dynamic  | 10         | 100,000      | `dynamic_batch_10_100000`      |
| Dynamic  | 10         | 1,000,000    | `dynamic_batch_10_1000000`     |
| Dynamic  | 1,000      | 100,000      | `dynamic_batch_1000_100000`    |
| Dynamic  | 1,000      | 1,000,000    | `dynamic_batch_1000_1000000`   |

