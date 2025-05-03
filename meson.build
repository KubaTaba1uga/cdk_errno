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
if get_option('backtrace')
  add_project_arguments(['-DCME_ENABLE_BACKTRACE'], language: 'c')
endif

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

c_minilib_error_inc = ['include']

c_minilib_error_lib = library('c_minilib_error', include_directories: c_minilib_error_inc)

c_minilib_error_dep = declare_dependency(
  link_with: c_minilib_error_lib,
  include_directories: c_minilib_error_inc
)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif


# ******************************************************************************
# *    Example
# ******************************************************************************
if get_option('examples')
    subdir('example')
endif
