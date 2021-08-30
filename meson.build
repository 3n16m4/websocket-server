project('websocket-server', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

inc_dir = include_directories('src/')

subdir('src/websocket_server')

websocket_server_dep = declare_dependency(
  include_directories : inc_dir,
  link_with : websocket_server_lib
)

subdir('src/server-test')

