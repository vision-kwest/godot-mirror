/*************************************************************************/
/*  stream_peer_winsock.cpp                                              */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifdef WINDOWS_ENABLED

#include "stream_peer_winsock.h"

#include <winsock2.h>

int winsock_refcount = 0;

static void set_addr_in(struct sockaddr_in& their_addr, const IP_Address& p_host, uint16_t p_port) {

	their_addr.sin_family = AF_INET;    // host byte order
	their_addr.sin_port = htons(p_port);  // short, network byte order
	their_addr.sin_addr = *((struct in_addr*)&p_host.host);
	memset(&(their_addr.sin_zero), '\0', 8);
};

StreamPeerTCP* StreamPeerWinsock::_create() {

	return memnew(StreamPeerWinsock);
};

void StreamPeerWinsock::make_default() {

	StreamPeerTCP::_create = StreamPeerWinsock::_create;

	if (winsock_refcount == 0) {
		WSADATA data;
		WSAStartup(MAKEWORD(2,2), &data);
	};
	++winsock_refcount;
};

void StreamPeerWinsock::cleanup() {

	--winsock_refcount;
	if (winsock_refcount == 0) {

		WSACleanup();
	};
};


Error StreamPeerWinsock::_block(int p_sockfd, bool p_read, bool p_write) const {

	fd_set read, write;
	FD_ZERO(&read);
	FD_ZERO(&write);

	if (p_read)
		FD_SET(p_sockfd, &read);
	if (p_write)
		FD_SET(p_sockfd, &write);

	int ret = select(p_sockfd + 1, &read, &write, NULL, NULL); // block forever
	return ret < 0 ? FAILED : OK;
};

Error StreamPeerWinsock::_poll_connection(bool p_block) const {

	ERR_FAIL_COND_V(status != STATUS_CONNECTING || sockfd == INVALID_SOCKET, FAILED);

	if (p_block) {

		_block(sockfd, false, true);
	};

	struct sockaddr_in their_addr;
	set_addr_in(their_addr, peer_host, peer_port);

	if (::connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == SOCKET_ERROR) {

		int err = WSAGetLastError();
		if (err == WSAEISCONN) {
			status = STATUS_CONNECTED;
			return OK;
		};

		return OK;
	} else {

		status = STATUS_CONNECTED;
		return OK;
	};

	return OK;
};

Error StreamPeerWinsock::write(const uint8_t* p_data,int p_bytes, int &r_sent, bool p_block) {

	if (status == STATUS_NONE || status == STATUS_ERROR) {

		return FAILED;
	};

	if (status != STATUS_CONNECTED) {

		if (_poll_connection(p_block) != OK) {

			return FAILED;
		};

		if (status != STATUS_CONNECTED) {
			r_sent = 0;
			return OK;
		};
	};

	int data_to_send = p_bytes;
	const uint8_t *offset = p_data;
	if (sockfd == -1) return FAILED;
	errno = 0;
	int total_sent = 0;

	while (data_to_send) {

		int sent_amount = send(sockfd, (const char*)offset, data_to_send, 0);
		//printf("Sent TCP data of %d bytes, errno %d\n", sent_amount, errno);

		if (sent_amount == -1) {

			if (WSAGetLastError() != WSAEWOULDBLOCK) {

				perror("shit?");
				disconnect();
				ERR_PRINT("Server disconnected!\n");
				return FAILED;
			};

			if (!p_block) {
				r_sent = total_sent;
				return OK;
			};

			_block(sockfd, false, true);
		} else {

			data_to_send -= sent_amount;
			offset += sent_amount;
			total_sent += sent_amount;
		};
	}

	r_sent = total_sent;

	return OK;
};

Error StreamPeerWinsock::read(uint8_t* p_buffer, int p_bytes,int &r_received, bool p_block) {

	if (!is_connected()) {

		return FAILED;
	};

	if (status != STATUS_CONNECTED) {

		if (_poll_connection(p_block) != OK) {

			return FAILED;
		};

		if (status != STATUS_CONNECTED) {
			r_received = 0;
			return OK;
		};
	};

	int to_read = p_bytes;
	int total_read = 0;
	errno = 0;

	while (to_read) {

		int read = recv(sockfd, (char*)p_buffer + total_read, to_read, 0);

		if (read == -1) {

			if (WSAGetLastError() != WSAEWOULDBLOCK) {

				perror("shit?");
				disconnect();
				ERR_PRINT("Server disconnected!\n");
				return FAILED;
			};

			if (!p_block) {

				r_received = total_read;
				return OK;
			};
			_block(sockfd, true, false);
		} else if (read == 0) {
			disconnect();
			return ERR_FILE_EOF;
		} else {

			to_read -= read;
			total_read += read;
		};
	};

	r_received = total_read;

	return OK;
};

Error StreamPeerWinsock::put_data(const uint8_t* p_data,int p_bytes) {

	int total;
	return write(p_data, p_bytes, total, true);
};

Error StreamPeerWinsock::put_partial_data(const uint8_t* p_data,int p_bytes, int &r_sent) {

	return write(p_data, p_bytes, r_sent, false);
};

Error StreamPeerWinsock::get_data(uint8_t* p_buffer, int p_bytes) {

	int total;
	return read(p_buffer, p_bytes, total, true);
};

Error StreamPeerWinsock::get_partial_data(uint8_t* p_buffer, int p_bytes,int &r_received) {

	return read(p_buffer, p_bytes, r_received, false);
};

StreamPeerTCP::Status StreamPeerWinsock::get_status() const {

	if (status == STATUS_CONNECTING) {
		_poll_connection(false);
	};

	return status;
};


bool StreamPeerWinsock::is_connected() const {

	if (status == STATUS_NONE || status == STATUS_ERROR) {

		return false;
	};
	if (status != STATUS_CONNECTED) {
		return true;
	};

	return (sockfd!=INVALID_SOCKET);
};

void StreamPeerWinsock::disconnect() {

	if (sockfd != INVALID_SOCKET)
		closesocket(sockfd);
	sockfd=INVALID_SOCKET;

	status = STATUS_NONE;

	peer_host = IP_Address();
	peer_port = 0;
};

void StreamPeerWinsock::set_socket(int p_sockfd, IP_Address p_host, int p_port) {

	sockfd = p_sockfd;
	status = STATUS_CONNECTING;
	peer_host = p_host;
	peer_port = p_port;
};

Error StreamPeerWinsock::connect(const IP_Address& p_host, uint16_t p_port) {

	ERR_FAIL_COND_V( p_host.host == 0, ERR_INVALID_PARAMETER);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		ERR_PRINT("Socket creation failed!");
		disconnect();
		//perror("socket");
		return FAILED;
	};

	unsigned long par = 1;
	if (ioctlsocket(sockfd, FIONBIO, &par)) {
		perror("setting non-block mode");
		disconnect();
		return FAILED;
	};

	struct sockaddr_in their_addr;
	set_addr_in(their_addr, p_host, p_port);

	if (::connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == SOCKET_ERROR) {

		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			ERR_PRINT("Connection to remote host failed!");
			disconnect();
			return FAILED;
		};
		status = STATUS_CONNECTING;
	} else {
		status = STATUS_CONNECTED;
	};

	peer_host = p_host;
	peer_port = p_port;

	return OK;
};

void StreamPeerWinsock::set_nodelay(bool p_enabled) {
    ERR_FAIL_COND(!is_connected());
    int flag=p_enabled?1:0;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
}


IP_Address StreamPeerWinsock::get_connected_host() const {

	return peer_host;
};

uint16_t StreamPeerWinsock::get_connected_port() const {

	return peer_port;
};

StreamPeerWinsock::StreamPeerWinsock() {

	sockfd = INVALID_SOCKET;
	status = STATUS_NONE;
	peer_port = 0;
};

StreamPeerWinsock::~StreamPeerWinsock() {

	disconnect();
};


#endif
