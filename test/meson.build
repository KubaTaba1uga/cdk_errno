# Manual list of tests (edit this list as you add/remove files)
# You can omit 'exe' to derive it from the filename stem.
tests = [
  {'src': 'test_cdk_errno', 'c_args': ['-DCDK_ERROR_BTRACE_ENABLE=0']},
  {'src': 'test_cdk_errno', 'name': 'test_cdk_errno_with_backtrace'},
  {'src': 'test_cdk_errno_backtrace'},
  {'src': 'test_cdk_errno', 'name': 'test_cdk_errno_optimized', 'c_args': ['-DCDK_ERROR_OPTIMIZE']},  
  # {'src': 'test_c_minilib_error_with_backtrace.c', 'c_args': ['-DCME_ENABLE_BACKTRACE']},
  # {'src': 'test_c_minilib_error_ringbuffer.c'},
]

unity_subproject = subproject('unity')

unity_dependency = unity_subproject.get_variable('unity_dep')

test_dependencies = [unity_dependency, cdk_errno_dep]

test_runner = unity_subproject.get_variable('gen_test_runner')

tests_inc = include_directories('../../include')

subdir('test_unity.d')

foreach test : tests
  src = test['src']
  extra_c_args = test.has_key('c_args') ? test['c_args'] : []
  name = test.has_key('name') ? test['name'] : src

  exe = executable(name,
    sources: [src + '.c', test_runner.process(src + '.c')],
    dependencies: test_dependencies,
    include_directories: tests_inc,
    c_args: extra_c_args,
  )

  test(name, exe)
endforeach
