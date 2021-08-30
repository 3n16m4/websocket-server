exe = executable('server_test', 'server_test.cpp',
  dependencies : [websocket_server_dep, openssl_dep, threads_dep, fmt_dep, magic_enum_dep, boost_dep, nlohmann_json_dep],
  install : true)

test('basic', exe)
