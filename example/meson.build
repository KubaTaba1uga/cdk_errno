# Define include directories relative to the project root.
inc = include_directories('../include')

executable(
  'example',
  'example.c',
  include_directories : inc,
  link_with : cmx_error,
  c_args : [
    '-g',                         # Enable debug symbols (DWARF info)
    '-fno-pie',                   # Disable position-independent code generation
    '-fno-omit-frame-pointer',    # Preserve frame pointer for accurate backtraces
    '-DCME_ENABLE_BACKTRACE',     # Enable backtrace capture in the code
    '-O0'                         # Disable compiler optimizations (more accurate traces)
  ],
  link_args : [
    '-no-pie',                    # Tell linker to produce a non-PIE executable
    '-O0'                         # Disable linker-level optimizations
  ],
)
