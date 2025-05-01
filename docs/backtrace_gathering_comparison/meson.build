project('backtrace-benchmark', 'c', version: '1.0', default_options: ['c_std=c11'])

add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')
add_project_arguments(['-D_POSIX_C_SOURCE=200809L', '-fno-omit-frame-pointer', '-no-pie'], language: 'c')

executable(
  'benchmark_bt',
  ['main.c', 'method_backtrace.h'],
  include_directories: '.',
  link_args: ['-rdynamic', '-no-pie'],
)
