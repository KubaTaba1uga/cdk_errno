# c_minilib_error

**`c_minilib_error`** is a lightweight C library for structured error handling. It captures rich error context, including message, code, file, function, and line number—using a clean, macro-based interface.

## ✨ Features

- **Structured Errors**: Carry error code, message, file, function, and line info.
- **Macro-Based Simplicity**: Use `cme_errorf` for consistent, minimal syntax.
- **Safe Memory Management**: Explicit create/destroy API with fallback error.
- **Fully Tested**: Built-in unit tests with Unity framework.

## 🧠 Example Usage

```c
#include "c_minilib_error.h"
#include <stdio.h>

// Level 3: Generates the error
struct cme_Error *level3(void) {
    return cme_errorf(101, "Level 3 failure: invalid state");
}

// Level 2: Wraps level 3
struct cme_Error *level2(void) {
    return cme_return(level3());
}

// Level 1: Wraps level 2
struct cme_Error *level1(void) {
    return cme_return(level2());
}

int main(void) {
    struct cme_Error *err = level1();
    if (err) {
        // Dump to file
        cme_error_dump_to_file(err, "error_dump.txt");

        // Dump to string buffer
        char buffer[1024];
        cme_error_dump_to_str(err, sizeof(buffer), buffer);
        fprintf(stderr, "%s", buffer);

        // Cleanup
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

## 🧪 Backtrace Decoding (Debugging)

To decode addresses from a dumped backtrace, use the provided Python script:

```bash
tools/translate_err_dump.py <error_dump_file> <executable>
```

**Example:**

```bash
inv build -b
./build/example/example
python3 tools/translate_err_dump.py error_dump.txt build/example/example
```

Make sure `addr2line` is available:

```bash
sudo apt install binutils
```

> ℹ️ **Disable ASLR (Address Space Layout Randomization)**  
Backtrace addresses may vary due to ASLR. For consistent decoding, disable it temporarily (until reboot):

```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

To restore the default (enabled):

```bash
echo 2 | sudo tee /proc/sys/kernel/randomize_va_space
```

## 📄 License

MIT License. See [LICENSE](LICENSE) for full text.

