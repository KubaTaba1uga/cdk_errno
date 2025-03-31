# c_minilib_error

**`c_minilib_error`** is a lightweight C library for structured error handling. It captures rich error context, including message, code, file, function, and line number—using a clean, macro-based interface.

## ✨ Features

- **Structured Errors**: Carry error code, message, file, function, and line info.
- **Macro-Based Simplicity**: Use `cme_errorf` for consistent, minimal syntax.
- **Safe Memory Management**: Explicit create/destroy API with fallback error.
- **Minimal Footprint**: Written in pure C11, with no external dependencies.
- **Fully Tested**: Built-in unit tests with Unity framework.

## 🧠 Example Usage

```c
#include "c_minilib_error.h"

int main(void) {
    struct cme_Error *err = cme_errorf(1, "Something failed: %s", "reason");

    if (err) {
        fprintf(stderr, "[%s:%d] %s\n", err->source_file, err->source_line, err->msg);
        cme_error_destroy(err);
    }

    return 0;
}
```

## ⚙️ Build Instructions

Using [Meson](https://mesonbuild.com/):

```sh
meson setup build
meson compile -C build
```

## ✅ Run Tests

```sh
meson test -C build
```

Tests are based on [Unity](https://www.throwtheswitch.org/unity).

## 🧰 Development Tools

Automated with [Invoke](https://www.pyinvoke.org/):

```sh
inv install    # Install required tools
inv build      # Configure & compile
inv test       # Run tests
inv format     # Format source files (clang-format)
inv lint       # Run static analysis (clang-tidy)
inv clean      # Remove build & temp files
```

## 📄 License

MIT License. See [LICENSE](LICENSE) for full text.

