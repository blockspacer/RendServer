/*
 * TcpServer.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#include "include/TcpServer.h"

TcpServer::TcpServer(boost::asio::io_service& ioService) {
	this->gm = &GameManager::GetInstance();

	acceptor = new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), 18206));

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
	if (error) {
		cout << "Error " << error.value() << error.message() << endl; //standard out goes to file
		startAccept();
		return;
	}

	boost::array<char, 1024> buffer;
	boost::system::error_code readError;

	size_t len = newConnection->getSocket().read_some(boost::asio::buffer(buffer), readError);

	if (readError == boost::asio::error::eof) {
		cout << "connection terminated" << endl;
		startAccept();
		return; // Connection closed cleanly by peer.
	} else if (readError) {
		cout << "Error " << readError.value() << readError.message() << endl;
		startAccept();
		return;
	} else if (len < sizeof(ServerMessage)) {
		cout << "Recieved invalid message of size " << len << endl;
		startAccept();
		return;
	}

	string ip = newConnection->getSocket().remote_endpoint().address().to_string();
	int sendSize = 0;
	char* px;
	ServerMessage* msg = (ServerMessage*) buffer.c_array();

	switch (msg->type) {
		case JOIN:
			msg = gm->AcceptJoin(ip);
			break;
		case RESPAWN:
			msg = gm->Respawn(ip, msg->playerId);
			break;
		case LEAVE:
			msg = gm->Leave(ip, msg->playerId);
			break;
		case STATUS:
			msg = gm->GetGameStatus();
			break;
	}

	px = reinterpret_cast<char*>(msg);
	sendSize = sizeof(msg);

	newConnection->Start(px, sendSize);

	startAccept();
}
