# C Development Kit: Errno
# ******************************************************************************
# *    Project
# ******************************************************************************
project('cdk_errno', 'c',
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
  add_project_arguments(['-DCDK_EBACKTRACE_ENABLE'], language: 'c')
endif

# add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

subdir('src')

cdk_errno_inc = include_directories('include')

cdk_errno_lib = library('cdk_errno', sources, include_directories: cdk_errno_inc)

cdk_errno_dep = declare_dependency(
  link_with: cdk_errno_lib,
  include_directories: cdk_errno_inc
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
