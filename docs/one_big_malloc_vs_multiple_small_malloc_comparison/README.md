# c_minilib Error Allocation Benchmark

This benchmark compares two error-handling strategies:

- **Single / Static**: One single `malloc` per error (monolithic struct)
- **Multiple / Dynamic**: Multiple `malloc`s per error (individual strings and fields allocated separately)

Each test scenario is run both **with** and **without** backtrace capture.

---

## ⚙️ Benchmark Setup

- `--max=10000`: Total number of errors per run
- `--batch=N`: Errors freed in batches of size N
- `--runs=30`: Repetitions per test
- Backtrace-enabled builds are compiled with `-DCME_ENABLE_BACKTRACE`

---

## 🧪 Results

### No Backtrace

| Scenario                    | Single avg (ms) | Multiple avg (ms) |  Δ time % |
| --------------------------- | --------------- | ----------------- | --------- |
| Cold-start / immediate free |            3.58 |              5.83 |      62.9% |
| Tiny burst (live=4)         |            3.31 |              5.71 |      72.6% |
| Occasional cascade (live=8) |            3.31 |              5.81 |      75.5% |

### With Backtrace

| Scenario                    | Single avg (ms) | Multiple avg (ms) |  Δ time % |
| --------------------------- | --------------- | ----------------- | --------- |
| Cold-start / immediate free |            3.83 |              6.39 |      66.9% |
| Tiny burst (live=4)         |            3.69 |              6.37 |      72.5% |
| Occasional cascade (live=8) |            3.63 |              6.49 |      78.7% |

---

## ✅ Conclusion

- The **single-malloc** strategy is consistently faster across all scenarios.
- Backtrace capture increases runtime slightly, but relative differences remain the same.
- On x86, the **single-malloc strategy is approximately 70% faster** than multiple-malloc on average.
- The **multi-malloc (dynamic)** method introduces significant overhead due to per-field heap allocations.

**Decision:**  
We are going with the **single `malloc` strategy**, as it is clearly more efficient, simpler to manage, and provides optimal performance on modern CPUs.
