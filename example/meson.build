examples_lib = library('cdk_examples_lib',
  sources: sources,
  include_directories: cdk_errno_inc,
)
examples_dep = declare_dependency(
  link_with: examples_lib,
  include_directories: cdk_errno_inc
)

executable(
  'example_1',
  sources: ['example_1.c', 'example_1_lib.c'],
  dependencies: [examples_dep],
)

executable(
  'example_2',
  sources: ['example_2.c', 'example_2_lib.c'],
  dependencies: [examples_dep],
)

