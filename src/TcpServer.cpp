/*
 * TcpServer.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#include "include/TcpServer.h"

TcpServer::TcpServer(boost::asio::io_service& ioService) {
	acceptor = new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), 18206));

	loadLength = 250;
	testLoad = new int[loadLength];
	for (int i = 0; i < loadLength; i++) {
		testLoad[i] = i;
	}
	loadLength *= sizeof(int);

	startAccept();
}

void TcpServer::startAccept()
{
	TcpConnection::pointer newConnection = TcpConnection::create(acceptor->get_io_service());

	acceptor->async_accept(
		newConnection->getSocket(),
		boost::bind(
			&TcpServer::handleAccept,
			this,
			newConnection,
			boost::asio::placeholders::error
		)
	);
}

void TcpServer::handleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error)
{
	if (!error) {
		newConnection->Start(testLoad, loadLength);
	}

	startAccept();
}
