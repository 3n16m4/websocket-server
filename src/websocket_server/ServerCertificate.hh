#ifndef WEBSOCKET_SERVER_SERVER_CERTIFICATE_HH
#define WEBSOCKET_SERVER_SERVER_CERTIFICATE_HH

#include "websocket_server/asiofwd.hh"

namespace amadeus {
/// \brief Helper function to load the server self signed certificate into the
/// given SSL-Context. Furthermore, it configures the SSL-Context by setting the
/// necessary options.
void loadServerCertificate(ssl::context& _ctx);
}

#endif // !WEBSOCKET_SERVER_SERVER_CERTIFICATE_HH