#include "websocket_server/ServerCertificate.hh"
#include "websocket_server/CommandLineInterface.hh"

#include <boost/asio/ssl/context.hpp>
#include <boost/asio/buffer.hpp>

void amadeus::loadServerCertificate(ssl::context& _ctx,
                                    std::string_view _certChain,
                                    std::string_view _privKey)
{
    try {
        _ctx.set_options(ssl::context::default_workarounds |
                         ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1);

        _ctx.use_certificate_chain_file(_certChain.data());
        _ctx.use_private_key_file(_privKey.data(), ssl::context::pem);
    } catch (boost::system::system_error const&) {
        throw;
    }
}
