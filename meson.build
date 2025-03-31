# C Minilib Error
# ******************************************************************************
# *    Project
# ******************************************************************************
project('c_minilib_error', 'c',
         version: '0.0.1',
	 meson_version: '>=1.0.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)


# ******************************************************************************
# *    App
# ******************************************************************************
# Enable sanitizers
# add_project_arguments('-fsanitize=address,undefined', language: 'c')
subdir('src')

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

cmx_error = library('cmx_error', sources, include_directories: ['include'])

# ******************************************************************************
# *    Tests
# ******************************************************************************
subdir('test')
