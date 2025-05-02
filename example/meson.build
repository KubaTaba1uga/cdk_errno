# Define include directories relative to the project root.
executable(
  'example',
  'example.c',
  dependencies : c_minilib_error_dep,
  c_args : [
    '-O0'                         # Disable compiler optimizations (more accurate traces)
  ],
  link_args : [
    '-O0'                         # Disable linker-level optimizations (more accurate traces)
  ],
)
