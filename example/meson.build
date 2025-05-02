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

executable('bench',
  'bench.c',                              # your benchmark file
  dependencies   : c_minilib_error_dep,   # from root meson.build
  c_args : [
    '-O0',                         # Disable compiler optimizations (more accurate traces)
    '-DCME_ENABLE_BACKTRACE'    
  ],
  link_args : [
    '-O0'                         # Disable linker-level optimizations (more accurate traces)
  ],

)