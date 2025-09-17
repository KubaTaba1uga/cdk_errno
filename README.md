# cdk_error

**`cdk_error`** is a modern, fast, errno-like C library for structured error handling.
It combines the simplicity of `errno` with extra context: error codes, messages, and lightweight manual backtraces — all without heap allocations.

## ✨ Features

* 📌 **Error objects**: numeric code, optional string/format message, file, function, and line
* 🧵 **Manual backtrace tracking**: append frames with `cdk_error_wrap()` or `cdk_ereturn()`
* 🧠 **Formatted messages**: optional `printf`-style formatting (disabled if optimized)
* ♻️ **Allocation-free**: uses thread-local or caller-provided storage, no mallocs in hot paths
* 🧪 **Dump utilities**: export human-readable traces to buffer or file
* 🔄 **Two APIs**:

  * Local error objects (`struct cdk_Error`)
  * Thread-local errno analogue (`cdk_errno`)

## 🔧 Usage

### Local error object

```c
#include "cdk_error.h"

struct cdk_Error err;

int open_sensor(void) {
  return cdk_errorf(&err, EIO, "Sensor read failure: value=%d", -1);
}
```

### Thread-local errno-like API

```c
#include "cdk_error.h"

int deep_error(void) {
  cdk_errno = cdk_errnof(EIO, "Sensor read failure: value=%d", -1);
  return -1;
}

int wrapped_error(void) {
  int ret = deep_error();
  if (ret < 0){
	return cdk_ereturn(ret);
  }

  return 13 + ret;
}

int main(void) {
  char buf[1024];

  if (wrapped_error() < 0) {
    cdk_edumps(sizeof(buf), buf);
    fputs(buf, stderr);
  }
  
  return 0;
}
```

## 🧱 API Overview

| Function / Macro                                 | Description                        |
| ------------------------------------------------ | ---------------------------------- |
| `cdk_errori(err, code)`                          | Create error with numeric code     |
| `cdk_errors(err, code, msg)`                     | Create error with literal string   |
| `cdk_errorf(err, code, fmt, ...)`                | Create error with formatted string |
| `cdk_error_wrap(err)`                            | Add current frame to an error      |
| `cdk_error_return(ret, err)`                     | Wrap error and return value        |
| `cdk_error_dumps(err, buf_size, buf)`            | Dump trace to buffer               |
| `cdk_errnoi()` / `cdk_errnos()` / `cdk_errnof()` | Thread-local errno-style API       |
| `cdk_ewrap()` / `cdk_ereturn()`                  | Wrap/propagate with errno analogue |
| `cdk_edumps()`                                   | Dump `cdk_errno` to buffer         |

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

Tests use [Unity](https://www.throwtheswitch.org/unity).

## ⚙️ Dev Tools

Automate with [Invoke](https://www.pyinvoke.org/):

```sh
inv install     # Install dependencies
inv build       # Compile project
inv test        # Run test suite
inv format      # Apply clang-format
inv lint        # Run clang-tidy checks
inv clean       # Clean build artifacts
```

## 📄 License

Licensed under the [MIT License](LICENSE) © 2025 Jakub Buczynski (KubaTaba1uga).
