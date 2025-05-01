project('error-allocation-benchmark', 'c', version: '1.0', default_options: [
'c_std=c11'])

add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')

add_project_arguments(['-D_POSIX_C_SOURCE=200809L', '-fno-omit-frame-pointer', '-no-pie'], language: 'c')

executable(
  'test_static',
  ['static.c', 'test_static.c'],
  include_directories: '.',
)

executable(
  'test_dynamic',
  ['dynamic.c', 'test_dynamic.c'],
  include_directories: '.',
)

dl_dep = dependency('dl', required : true)

executable(
  'test_static_backtrace',
  ['static.c', 'test_static.c'],
  include_directories: '.',
  c_args: ['-D_GNU_SOURCE', '-DCME_ENABLE_BACKTRACE'],
  dependencies: dl_dep,
  link_args: ['-rdynamic', '-no-pie'],  
)

executable(
  'test_dynamic_backtrace',
  ['dynamic.c', 'test_dynamic.c'],
  include_directories: '.',
  c_args: ['-D_GNU_SOURCE', '-DCME_ENABLE_BACKTRACE'],
  dependencies: dl_dep,
  link_args: ['-rdynamic', '-no-pie'],  
)

