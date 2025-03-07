/*
 * tlcConnection.hpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#ifndef NETWORK_TLSCONNECTION_HPP_
#define NETWORK_TLSCONNECTION_HPP_

#include "network.hpp"

#include "connection.hpp"
#include <atomic>
#include <thread>
#include <string>

#include <tls.h>

#include "../utils/logger.hpp"

namespace network {

class TlsConnection: public Connection {
public:
	~TlsConnection();

	void connect(std::string ip_address, uint16_t port = 6697,
			bool ignoreServerName = false, bool allowDeprecatedServer = false);

	void send(std::vector<char> data) override;
	void send(std::string data) override;

private:
	bool m_connected = false;

	std::atomic<bool> m_receiveThreadActive = false;
	std::thread *m_receiveThread = nullptr;

	struct tls_config *m_tls_config = nullptr;
	struct tls *m_tls_socket = nullptr;

	static void receiveThreadFunc(TlsConnection *self);
};

} // namespace network

#endif /* NETWORK_TLSCONNECTION_HPP_ */
