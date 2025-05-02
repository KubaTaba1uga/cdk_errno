# 📊 Error Allocation Strategy Benchmark

This benchmark compares two allocation strategies for `struct cme_Error`:

- 🔁 **Preallocated Ring Buffer** – zero-allocation, fast reuse
- 📦 **`malloc()` on-the-fly** – dynamic allocation per error

The goal is to measure raw performance for creating simple structured error objects.

---

## 🔬 Benchmark Details

- **Iterations:** 1,000,000
- **Build Type:** `-O2`, single-threaded
- **Struct Size:** ~300B
- **String Formatting:** disabled (to isolate alloc path)

---

## 📈 Results (ns per iteration)

| Strategy              | Total Time (ms) | Avg Time (ns) | Speedup vs malloc |
|-----------------------|------------------|----------------|--------------------|
| 🧠 Malloc (`calloc`)   | 100.65 ms        | 100.65 ns      | 1.0× (baseline)     |
| 🚀 Ring Buffer         | 13.99 ms         | 13.99 ns       | **7.2× faster**     |

---

## ✅ Summary

- Ring buffer allocation is **~7× faster** than dynamic allocation.
- Performance is comparable to raw `int`/`struct` usage in CPU L1/L2 cache.
- Suitable for high-frequency, short-lived error tracking (e.g. parsers, embedded, game loops).
- `malloc()` still performs reasonably well due to `tcache`, but has higher per-call cost.
