# Stacktrace Benchmark

This benchmark compares different stack trace collection methods:
- **POSIX `backtrace()`**
- **Manual return-address (RA) capture**
- **Raw ASM-based return-address trace**

The goal is to evaluate performance and choose the most efficient method.

## Benchmark Results

| Method              | Total Time (ms) | Avg Time per Iteration (ns) |
|---------------------|------------------|------------------------------|
| POSIX backtrace     | 669.34           | 6693.39                      |
| Manual RA trace     | 11.35            | 113.51                       |
| ASM stack-RA trace  | 14.38            | 143.81                       |

## Conclusion

We choose to go with **manual return-address tracing**, as it is the most efficient in terms of execution time while providing adequate stack trace fidelity. POSIX `backtrace()` is significantly slower, and ASM-based methods offer no major benefit over manual RA while being more complex.
