project('error-allocation-benchmark', 'c', version: '1.0', default_options: [
'c_std=c11'])

add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')

add_project_arguments(['-D_POSIX_C_SOURCE=200809L'], language: 'c')

executable(
  'test_multiple_method',
  ['test_multiple.c', 'method_multiple.c'],
  include_directories: '.',
)

executable(
  'test_multiple_method_with_backtrace',
  ['test_multiple.c', 'method_multiple.c'],
  include_directories: '.',
  c_args: ['-DCME_ENABLE_BACKTRACE'],
  link_args: ['-no-pie'],  # this is required for backtraces translation  
)


