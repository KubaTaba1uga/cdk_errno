# Define include directories relative to the project root.
inc = include_directories('../include')

executable(
  'example',
  'example.c',
  include_directories : inc,
  link_with : cmx_error,  # Assumes the library target from the root is named "cmx_error"
  )
