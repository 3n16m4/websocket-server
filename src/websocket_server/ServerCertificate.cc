#include "websocket_server/ServerCertificate.hh"

#include <boost/asio/ssl/context.hpp>
#include <boost/asio/buffer.hpp>

#include <string_view>

using namespace std::literals::string_view_literals;

constexpr static std::string_view CERTIFICATE =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDlzCCAn+gAwIBAgIUSJutDnnDnG5B+bbJ/EcWLMGmusswDQYJKoZIhvcNAQEL\n"
    "BQAwWzELMAkGA1UEBhMCREUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM\n"
    "GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEUMBIGA1UEAwwLdGVhbXByb2pla3Qw\n"
    "HhcNMjAxMDMwMTg0NjA5WhcNNDgwMzE3MTg0NjA5WjBbMQswCQYDVQQGEwJERTET\n"
    "MBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQ\n"
    "dHkgTHRkMRQwEgYDVQQDDAt0ZWFtcHJvamVrdDCCASIwDQYJKoZIhvcNAQEBBQAD\n"
    "ggEPADCCAQoCggEBALRb5RIx1SZsR4x56ed0onP/MwpIgnyOGA8rXHejIwa4FRfN\n"
    "ERUgbezatjqxN4mos8rI+kna7vidt4iXGEYEeJQ8xrxYT1EyxOJvM4jhuto3sgCw\n"
    "hWwV4YIHMlVINkg4ss79sfmuwO95msdJ4qlqclhGKH/lRQzXzbL82c4GLryiYmYM\n"
    "RprhaSBwekaILdh43mBwsaEgOM1tz6z4CoNBqQLvaSAecu2XIRe1yQXuP/wSTAQq\n"
    "mzAWmWubSA3h6UPdDIGQM+yVazNyRdvKh+vv8MT6XzDz6zx7SDUjUtFFcWro8OS4\n"
    "m8837SxJz4I2Gk1mk5Ju3sHZirG733Zvnuh0a/cCAwEAAaNTMFEwHQYDVR0OBBYE\n"
    "FBOiO3mUwElm7Iq7ldW+JqiCtRN9MB8GA1UdIwQYMBaAFBOiO3mUwElm7Iq7ldW+\n"
    "JqiCtRN9MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAFj6FXCc\n"
    "7Dqqy+GjtjITMDjG7kzQ/9dtBxv6r+BqqGzZspmXgIt18PhURe1yZFLVFr6ND8wp\n"
    "r/1munxnC8T4sVdVkEE5Z3FAg6fbN22uA135+QOf0Uw/90hOuz766jx9oc6pJR5f\n"
    "gZs/OXhW7YziQI4L8yHYHoBkMWPBCTRjZeDKVKbHZ77Tmu3GVb3CRdJwbmt/8+Ww\n"
    "WQckMm4jNG3Dxfo/jZrcMMinjyGK1ntUxi1jVyeNz1vs0C458ZO/tsVTWJg7Ec9/\n"
    "MCIcapMENM9UGIxORLxkTShXkNYS+oWeilKW//HuhesB1xqIgfGSYlIm65mgmG5e\n"
    "Tc9zhkcacWdAccE=\n"
    "-----END CERTIFICATE-----\n"sv;

constexpr static std::string_view KEY =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC0W+USMdUmbEeM\n"
    "eenndKJz/zMKSIJ8jhgPK1x3oyMGuBUXzREVIG3s2rY6sTeJqLPKyPpJ2u74nbeI\n"
    "lxhGBHiUPMa8WE9RMsTibzOI4braN7IAsIVsFeGCBzJVSDZIOLLO/bH5rsDveZrH\n"
    "SeKpanJYRih/5UUM182y/NnOBi68omJmDEaa4WkgcHpGiC3YeN5gcLGhIDjNbc+s\n"
    "+AqDQakC72kgHnLtlyEXtckF7j/8EkwEKpswFplrm0gN4elD3QyBkDPslWszckXb\n"
    "yofr7/DE+l8w8+s8e0g1I1LRRXFq6PDkuJvPN+0sSc+CNhpNZpOSbt7B2Yqxu992\n"
    "b57odGv3AgMBAAECggEAaNt8ZIWgXMRbHKrMeIpw4GugavwZEP0zs64MBmpMNN3V\n"
    "NY89vUCraCcIBzt9BIWWcfFCwqmMgfJ//+OIczyqNdTX3iyhgJ25pcPs/Enwbmov\n"
    "DQOTrnFSOFyiwL5JANgl7P3F+VnGRfQ3LmIga0q9y+COLz1GVd8pckRbBNLHXVc5\n"
    "aHEF52BQF3Z2ZBgVIKzkDsRLYVwQvDinaC/3pkG3uGmP5jqXxlRzGqQymuQr6PRi\n"
    "tgJzo1uqyAWfsJAfjQw1W8L+8vbbhJWrkQEJpD0cjvaGEX/TnQjtIOK0xYQ74SwT\n"
    "cV7/RNCk0F74Kpksv8aveIptwNy6/0D4WMzwx8vXUQKBgQDaDqr5KQWJQk52cbBj\n"
    "NGYukObyarJIz0RSJHdzEgP3YRFQPAFSyKWbpSGo1vXflUsrOsMtOYhPSXHHuVOe\n"
    "f43eK58fF9qBKJOSItT8N5Js1zM35NRA1NM0g+NvxZOPncxnhUXslpJcxTWV+3yQ\n"
    "r8JDamf7zP0axgMJlykyj9+FgwKBgQDTvfW+2wUr/kkNXk56Uba2/RpH6nGGUUcf\n"
    "b/gsi2v1sWSQuqfJzSrdBtBKfb/u5lpCPmXEAWQl9iNEpxBk73j4IOodrFaEwONL\n"
    "8+R5wbsMmjBPMYgHMwSNsZmctjKvhYsEPJZ9zIqtisHgGhKX0B4JPOH8IUF2TcNY\n"
    "qXnFa0TpfQKBgAl+9gcfuDQABTa3QL0xPPnuKYCJEPaGwCzrlaAXOsD9IynhUxVt\n"
    "03hxuoOFyPNRpxJc3r8LcFozkpbmBTQzzdPNBoLuIP/a6oJErg3FxcSjooFen7er\n"
    "DP+OdssZSZHJh2tCrIbfD8SmeezsJMKQRN92tMyMYsFVujAiuxKOnWdNAoGAMapA\n"
    "1f9xexwt7mDMb4xzGwDLBUtMTYEK+ai9wVUCgaKWQAxIBuGEyEss2Awe1N5bFWal\n"
    "YBAjeXP+WcAF7GbvLOQrhcJCmOjVZ59RW+3A/fe8lRa9HVMuIj4iFcv2n4znSpsb\n"
    "gW5UqNf/GZ2D9u/t26TYLNdwD1u2EjnOQiwjAdECgYEAxXKkud8Hoq4nMaeOk2DY\n"
    "EPw15mqCU4Nw7rivCvZBaTLe4WQ2EQBS5rMmiC5PSt+rFQ52jayX2AdpY+6XEsu0\n"
    "H22eQAMGaUGFHSNFF/X3X2B8K8HjEFg8EL2G29U699eEU451sry4UISDmtHoB/EQ\n"
    "Y44jLqsIf1SoRF9yjx5w1/s=\n"
    "-----END PRIVATE KEY-----\n"sv;

constexpr static std::string_view DH_PARAMETERS =
    "-----BEGIN DH PARAMETERS-----\n"
    "MIIBCAKCAQEAqcV6Ju6LfaJNVjvgwHds19vr1BUTTo+XTPCxu46VO3bhlzValOa0\n"
    "xeNCW7MrrPgulx/7IP0hxeIVRvP+D5cGSuFfE2VZGPv5wjZkZxHbgbv6E9G7uJOF\n"
    "dDRRVNW0bldYtvLJ8f526JyAefFtStKHI5gv7bP+GRgunf4vtFdpoHhEmfHPCGvm\n"
    "iMGaL1IMI0s4kT/JUENRBObMLbaAg01S4II6zoimXicwmd/bzyK1X40ryK8VaqjJ\n"
    "ULj6uJveNKKVxCRGtcMd3y6jeVJKsACg0/mYM9fG+jgSdqEiRl7KBRTpq9kJ6ont\n"
    "CgCI2F8wj3ocrt/RC28hg61vOJsCcJHSawIBAg==\n"
    "-----END DH PARAMETERS-----\n"sv;

void amadeus::loadServerCertificate(ssl::context& _ctx)
{
    try {
        _ctx.set_options(ssl::context::default_workarounds |
                         /*ssl::context::no_tlsv1 | */ssl::context::single_dh_use);

        _ctx.use_certificate_chain(
            asio::buffer(CERTIFICATE.data(), CERTIFICATE.size()));

        _ctx.use_private_key(asio::buffer(KEY.data(), KEY.size()),
                             ssl::context::pem);

        _ctx.use_tmp_dh(
            asio::buffer(DH_PARAMETERS.data(), DH_PARAMETERS.size()));
    } catch (boost::system::system_error const&) {
        throw;
    }
}