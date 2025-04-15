# C Minilib Error
# ******************************************************************************
# *    Project
# ******************************************************************************
project('c_minilib_error', 'c',
         version: '0.0.1',
	 meson_version: '>=1.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)

# ******************************************************************************
# *    Library
# ******************************************************************************
if get_option('backtrace').enabled()
  add_project_arguments('-DCME_ENABLE_BACKTRACE', language: 'c')
endif

subdir('src')

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

cmx_error_inc = ['include']
cmx_error_lib = library('cmx_error', sources, include_directories: cmx_error_inc)

cmx_error_dep = declare_dependency(
  link_with: cmx_error_lib,
  include_directories: cmx_error_inc
)

# ******************************************************************************
# *    Tests
# ******************************************************************************
subdir('test')

# ******************************************************************************
# *    Example
# ******************************************************************************
subdir('example')
