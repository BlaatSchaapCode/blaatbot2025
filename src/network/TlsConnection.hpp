/*
 * tlcConnection.hpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#ifndef NETWORK_TLSCONNECTION_HPP_
#define NETWORK_TLSCONNECTION_HPP_

#include "network.hpp"

#include <atomic>
#include <string>
#include <thread>

#include <tls.h>

#include "../utils/logger.hpp"
#include "Connection.hpp"

namespace network {

class cTlsConnection : public cConnection {

  public:
    using cConnection::cConnection;
    cTlsConnection();
    ~cTlsConnection();

    int connect() override;

    void send(std::vector<char> data) override;

    int setIgnoreInvalidCerficiate(bool ignoreInvalidCerficiate) override;
    int setIgnoreInsecureProtocol(bool ignoreInsecureProtocol) override;


  protected:

    bool ignoreInvalidCerficiate = false;
	bool ignoreInsecureProtocol = false;

  private:
    bool m_connected = false;

    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    struct tls_config *m_tls_config = nullptr;
    struct tls *m_tls_socket = nullptr;

    static void receiveThreadFunc(cTlsConnection *self);
};

} // namespace network

#endif /* NETWORK_TLSCONNECTION_HPP_ */
