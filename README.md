# ⚡ cdk_error

`cdk_error` is a small, modern C library that brings structured error handling into plain C code.
It works much like `errno`, but adds richer context: not just an error code, but also a message and a lightweight backtrace showing where the error has been passed along. All of this comes without any heap allocations, making it safe and fast even for embedded or low-overhead systems.

What is nice about `cdk_errno` is that on fast path it is as fast as normal `errno`, so if no error occured in the function there is no penalty hit. The difference show only on slow path so when error occurs:
```
❯ ./build/example/bench
5-lvl errno-trace avg:     32.8 ns
5-lvl fmt errno-trace avg: (disabled by CDK_ERROR_OPTIMIZE)
5-lvl int           avg:   5.7 ns
```

With `CDK_ERROR_OPTIMIZE` enabled `cdk_error` is like 6-8 times slower than normal errno. This performence hit is perfectly accaptable in most cases, because there will be plenty of other app critical processes wich will slow you more than this 32ns overhead.

---

## 📦 Getting started

The library is header-only. To use it, copy the single header file into **your own project or library**. Each project should keep its own copy to avoid sharing one global error state across unrelated code.

Create a small wrapper header and add one `.c` file that defines the thread-local variables:

```c
// myerror.h
#ifndef MYERROR_H
#define MYERROR_H

#define CDK_ERROR_FSTR_MAX 512
#define CDK_ERROR_BTRACE_MAX 32
#include "cdk_error.h"

#endif
```

```c
// myerror.c
#include "myerror.h"

_Thread_local cdk_error_t cdk_errno = NULL;
_Thread_local struct cdk_Error cdk_hidden_errno = {0};
```

Every other file in your project just includes `myerror.h`.
The `.c` file is required, because it provides the actual definitions of the thread-local globals; without it you’d only have declarations, and the linker would complain.

🔧 If you’d like to change the prefix (for example, from `cdk_` to `my_`), there’s a helper script:

```bash
python3 tools/change_prefix.py \
  --inf include/cdk_error.h \
  --out my_error.h \
  --old cdk --new my
```

This produces a copy of the header with your own prefix, ready to drop into a project.

---

## ❓ Why copy instead of link?

Unlike traditional libraries, `cdk_error` is designed to be embedded into each project separately. The reason is simple: every library or program should have its **own private error state**.

If multiple components linked against the same global `cdk_errno`, their errors could overwrite each other, leading to confusing or incorrect traces. By copying the header into your project (and optionally renaming the prefix), you guarantee isolation: errors raised inside your library stay inside your library, and don’t clash with others.

---

## 💡 Usage

Here’s the simplest way to use the errno-style API:

```c
#include <stdio.h>
#include "myerror.h"

const char *nested_failing_func(void) {
  if (1) {
    cdk_errno = cdk_errnos(ENOBUFS, "My error");
    return NULL;
  }
  return "All good";
}

int failing_func(void) {
  const char *s = nested_failing_func();
  if (!s) {
    return cdk_ereturn(-1);
  }
  return 13;
}

void api_entry(void) {
  int res = failing_func();
  if (res < 0) {
    cdk_ewrap();
    return;
  }
}

int main(void) {
  char buf[1024];
  cdk_errno = 0;

  api_entry();
  if (cdk_errno) {
    cdk_edumps(sizeof(buf), buf);
    printf("%s", buf);
    return -1;
  }
  return 0;
}
```

The pattern is straightforward: when something fails, set `cdk_errno` with a code or message, return an error value, and wrap it if you need to add another frame. At the top level, dump the error to a buffer or file to see the full trace.

---

### ⚡ Performance

One of the nice things about `cdk_errno` is that on the **fast path** it behaves just like plain `errno`: if no error occurs in a function, there’s no extra overhead at all.
The difference only shows up on the **slow path**, when an error is actually created and propagated:

```
❯ ./build/example/bench
5-lvl errno-trace avg:     32.8 ns
5-lvl fmt errno-trace avg: (disabled by CDK_ERROR_OPTIMIZE)
5-lvl int           avg:   5.7 ns
```

With `CDK_ERROR_OPTIMIZE` enabled (which removes formatted errors), the library is about **6–8× slower than a plain `errno` write** when an error occurs.
That sounds big, but remember: it’s \~32 nanoseconds to build a full 5-level trace. In practice this overhead is negligible compared to real application work (I/O, syscalls, allocations, etc.).

In other words: you get structured errors and backtraces “for free” in the common case, and only pay a small price when something actually goes wrong.

---


## 🛠️ Building examples and tests

This project uses [Meson](https://mesonbuild.com/) for its build system. To build with examples and tests enabled, run:

```sh
meson setup build -Dtests=true -Dexamples=true
meson compile -C build
```

Examples will be placed in `build/example/` and test binaries in `build/test/`. You can run them directly:

```sh
./build/example/example_1
./build/example/example_2
```

and execute the full test suite with:

```sh
meson test -C build
```

🧪 Tests are written using the Unity framework, pulled in automatically as a Meson subproject.

---

## ⚙️ Development workflow

For convenience, there’s an [Invoke](https://www.pyinvoke.org/) setup that automates common tasks:

```sh
inv install   # install meson, ninja, clang-format, clang-tidy, etc.
inv build     # configure and compile (add --debug, --tests, --examples)
inv test      # run test suite
inv format    # apply clang-format
inv lint      # run clang-tidy checks
inv clean     # remove build artifacts
```

This makes it easy to keep the environment consistent and catch issues early.

---

## 📄 License

Released under the [MIT License](LICENSE) © 2025 Jakub Buczynski (KubaTaba1uga).

