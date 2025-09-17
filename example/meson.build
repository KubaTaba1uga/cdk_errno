executable(
  'example_1',
  sources: ['example_1.c', 'example_1_lib.c'],
  include_directories: cdk_error_inc,  
)

executable(
  'example_2',
  sources: ['example_2.c', 'example_2_lib.c'],
  include_directories: cdk_error_inc,  
)

executable(
  'bench',
  sources: ['bench.c', 'example_2_lib.c'],
  c_args: ['-DCDK_ERROR_OPTIMIZE', '-O3', '-DNDEBUG'],
  include_directories: cdk_error_inc,  
)
