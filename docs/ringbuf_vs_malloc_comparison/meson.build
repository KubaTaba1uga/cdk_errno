project('allocator-benchmark', 'c', default_options: ['c_std=c11'])

add_project_arguments('-Wall', '-Wextra', '-O2', language: 'c')

executable('alloc_bench',
  ['main.c', 'allocator_malloc.c', 'allocator_ringbuf.c'],
  include_directories: '.',
)
