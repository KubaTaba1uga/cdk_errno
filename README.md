# c_minilib_error

**`c_minilib_error`** is a minimal and fast C library for structured error reporting. It supports lightweight stack tracing, optional formatted messages, and a circular ring buffer backend—all designed with embedded and low-overhead systems in mind.

## ✨ Features

- 📌 **Structured error objects**: Includes code, message, file, function, and line
- 🧵 **Inline call trace tracking**: Record stack frames using `cme_return()`
- 🧠 **Formatted errors**: Optional `printf`-style formatting
- ♻️ **Allocation-free runtime**: Uses a circular buffer, no dynamic allocations in hot paths
- 🧪 **Dump utilities**: Export errors to file or string buffer

## 🔧 Usage

```c
#include "c_minilib_error.h"

cme_error_t deep_error(void) {
  return cme_errorf(42, "Sensor read failure: value=%d", -1);
}

cme_error_t wrapped_error(void) {
  return cme_return(deep_error());
}

int main(void) {
  cme_init();

  cme_error_t err = wrapped_error();
  if (err) {
    char buf[1024];
    if (cme_error_dump_to_str(err, sizeof(buf), buf) == 0)
      fprintf(stderr, "%s", buf);
    cme_error_dump_to_file(err, "trace.log");
  }

  cme_destroy();
  return 0;
}
```

## 🧱 API Overview

| Function                   | Description                           |
| -------------------------- | ------------------------------------- |
| `cme_init()`               | Allocate internal ring buffer         |
| `cme_destroy()`            | Free ring buffer                      |
| `cme_error()`              | Create literal string error           |
| `cme_errorf()`             | Create formatted error                |
| `cme_return(err)`          | Propagate error and add current frame |
| `cme_error_dump_to_str()`  | Dump trace to a buffer                |
| `cme_error_dump_to_file()` | Dump trace to a file                  |

## 🛠️ Building

Using [Meson](https://mesonbuild.com/):

```sh
meson setup build
meson compile -C build
```

## ✅ Running Tests

```sh
meson test -C build
```

Unit tests are implemented using [Unity](https://www.throwtheswitch.org/unity).

## ⚙️ Dev Tools

Automate with [Invoke](https://www.pyinvoke.org/):

```sh
inv install     # Setup tools
inv build       # Compile project
inv test        # Run test suite
inv format      # Apply clang-format
inv lint        # Run clang-tidy checks
inv clean       # Clean build artifacts
```

## 📄 License

Licensed under the [MIT License](LICENSE) © 2025 Jakub Buczynski (KubaTaba1uga).

