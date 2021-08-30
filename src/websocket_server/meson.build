src_files = [
  'CommandLineInterface.cc',
  'HttpSession.cc',
  'Listener.cc',
  'Logger.cc',
  'PlainHttpSession.cc',
  'PlainTCPSession.cc',
  'PlainWebSocketSession.cc',
  'ServerCertificate.cc',
  'SharedState.cc',
  'SSLHttpSession.cc',
  'SSLTCPSession.cc',
  'SSLWebSocketSession.cc',
  'TCPSession.cc',
  'WebSocketSession.cc',
  'WebSocketSessionFactory.cc'
]

openssl_dep = dependency('openssl')
threads_dep = dependency('threads')
fmt_dep = dependency('fmt')
magic_enum_dep = dependency('magic_enum')
boost_dep = dependency('boost')
nlohmann_json_dep = dependency('nlohmann_json')

websocket_server_lib = static_library('websocket-server',
  src_files,
  include_directories : inc_dir,
  dependencies : [openssl_dep, threads_dep, fmt_dep, magic_enum_dep, boost_dep, nlohmann_json_dep],
  install : true)