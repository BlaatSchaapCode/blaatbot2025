#pragma once

#include "TcpConnection.hpp"

#include <gnutls/gnutls.h>

namespace geblaat {
class GnuTlsConnection : public TcpConnection {
  public:
    GnuTlsConnection();
    ~GnuTlsConnection();

    int setConfig(const nlohmann::json &config) override;

    int connect(void) override;

    void send(std::vector<char> data) override;

  private:
    gnutls_certificate_credentials_t xcred = {};
    gnutls_session_t session = {};

    bool ignoreInvalidCerficiate = false;
    bool ignoreInsecureProtocol = false;

    void onConnected() override;
    void onDisconnected() override;
    static void receiveThreadFunc(GnuTlsConnection *self);
};
} // namespace geblaat
