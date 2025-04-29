# Define include directories relative to the project root.
executable(
  'example',
  'example.c',
  dependencies : c_minilib_error_dep,
  c_args : [
    '-g',                         # Enable debug symbols (DWARF info)
    '-fno-pie',                   # Disable position-independent code generation
    '-fno-omit-frame-pointer',    # Preserve frame pointer for accurate backtraces
    '-O0'                         # Disable compiler optimizations (more accurate traces)
  ],
  link_args : [
    '-no-pie',                    # Tell linker to produce a non-PIE executable
    '-O0'                         # Disable linker-level optimizations
  ],
)
