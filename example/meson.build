examples_cc_args = ['-O0', '-DCDK_ERRNO_LSTR_ENABLE=1', '-DCDK_ERRNO_FSTR_ENABLE=1']

examples_lib = library('cdk_examples_lib', sources, include_directories: cdk_error_inc,
  c_args : examples_cc_args,
  link_args : examples_cc_args,
)
examples_dep = declare_dependency(
  link_with: examples_lib,
  include_directories: cdk_error_inc
)

# Define include directories relative to the project root.
executable(
  'code_example',
  'code_example.c',
  dependencies: [examples_dep],
  c_args : examples_cc_args,
  link_args : examples_cc_args,
)

executable(
  'lstring_example',
  'lstring_example.c',
  dependencies: [examples_dep],
  c_args : examples_cc_args,
  link_args : examples_cc_args,
)

