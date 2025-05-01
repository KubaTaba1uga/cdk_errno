project('error-allocation-benchmark', 'c', version: '1.0', default_options: ['c_std=c11'])

add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')

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
