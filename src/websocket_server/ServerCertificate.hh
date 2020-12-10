#ifndef WEBSOCKET_SERVER_SERVER_CERTIFICATE_HH
#define WEBSOCKET_SERVER_SERVER_CERTIFICATE_HH

#include "websocket_server/asiofwd.hh"

#include <string_view>

namespace amadeus {
class Config;
/// \brief Helper function to load the server self signed certificate into the
/// given SSL-Context. Furthermore, it configures the SSL-Context by setting the
/// necessary options.
void loadServerCertificate(ssl::context& _ctx, std::string_view _certChain,
                           std::string_view _privKey);
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SERVER_CERTIFICATE_HH
