test_c_minilib_error_name = 'test_c_minilib_error.c'

test_c_minilib_error_exe = executable('test_c_minilib_error',
  sources: [
   test_c_minilib_error_name,
   test_runner.process(test_c_minilib_error_name),           
  ],
  dependencies: test_dependencies,
  include_directories: include_directories('../../include'),  
)

test('test_c_minilib_error', test_c_minilib_error_exe)

test_c_minilib_error_with_backtrace_name = 'test_c_minilib_error_with_backtrace.c'

test_c_minilib_error_with_backtrace_exe = executable('test_c_minilib_error_with_backtrace',
  sources: [
   test_c_minilib_error_with_backtrace_name,
   test_runner.process(test_c_minilib_error_with_backtrace_name),           
  ],
  dependencies: test_dependencies,
  c_args: ['-DCME_ENABLE_BACKTRACE'],  
  include_directories: include_directories('../../include'),  
)

test('test_c_minilib_error_with_backtrace', test_c_minilib_error_with_backtrace_exe)

test_c_minilib_error_ringbuffer_name = 'test_c_minilib_error_ringbuffer.c'

test_c_minilib_error_ringbuffer_exe = executable('test_c_minilib_error_ringbuffer',
  sources: [
   test_c_minilib_error_ringbuffer_name,
   test_runner.process(test_c_minilib_error_ringbuffer_name),           
  ],
  dependencies: test_dependencies,
  include_directories: include_directories('../../include'),  
)

test('test_c_minilib_error_ringbuffer', test_c_minilib_error_ringbuffer_exe)
