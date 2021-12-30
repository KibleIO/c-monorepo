#include "MultiServer.h"

MultiServer::MultiServer() {
	Init();
	head = NULL;
}

bool MultiServer::Init() {
	int o = 1;
	destination.sin_family = AF_INET;
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&o,
				   sizeof o) != 0) {
		cout << "failed to set SO_REUSEADDR" << endl;
	}
	if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEPORT, (char *)&o,
				   sizeof o) != 0) {
		cout << "failed to set SO_REUSEPORT" << endl;
	}

	return (startedUp = (!(mainSocket < 0)));
}

bool MultiServer::Listen(int port) {
	int o = 1;

	if (!startedUp) {
		return false;
	}
	destination.sin_port = htons(port);
	destination.sin_addr.s_addr = INADDR_ANY;

	if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&o,
				   sizeof o) != 0) {
		cout << "failed to set SO_REUSEADDR" << endl;
	}
	if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEPORT, (char *)&o,
				   sizeof o) != 0) {
		cout << "failed to set SO_REUSEPORT" << endl;
	}

	if (bind(mainSocket, (sockaddr *)&destination, sizeof(destination)) < 0) {
		cout << "couldn't bind " << strerror(errno) << endl;
		return false;
	}
	if (listen(mainSocket, 5) < 0) {
		cout << "couldn't listen " << strerror(errno) << endl;
		return false;
	}
	return true;
}

int MultiServer::Accept() {
	int o = 1;

	Sockets *s = new Sockets;
	s->next = head;
	head = s;
	sockaddr_in clientAddress;
	int clientSize = sizeof(clientAddress);
	cout << "attempting to accept" << endl;
	head->socket = accept(mainSocket, (sockaddr *)&clientAddress,
						  (socklen_t *)&clientSize);
	if (head->socket < 0) {
		cout << "returned zero" << endl;
		head = s->next;
		delete s;
		return 0;
	}
	int o_buf = 700000;
	setsockopt(head->socket, SOL_SOCKET, SO_SNDBUF, &o_buf, sizeof(o_buf));

	if (setsockopt(head->socket, SOL_SOCKET, SO_REUSEADDR, (char *)&o,
				   sizeof o) != 0) {
		cout << "failed to set SO_REUSEADDR" << endl;
	}
	if (setsockopt(head->socket, SOL_SOCKET, SO_REUSEPORT, (char *)&o,
				   sizeof o) != 0) {
		cout << "failed to set SO_REUSEPORT" << endl;
	}

	return head->socket;
}

void MultiServer::Remove(int pid) {
	Sockets *temp = head;
	Sockets *prev = NULL;
	mtx.lock();
	while (temp != NULL) {
		if (temp->socket == pid) {
			shutdown(temp->socket, 2);
			close(temp->socket);
			if (prev != NULL)
				prev->next = temp->next;
			else
				head = temp->next;
			delete temp;
			break;
		} else {
			prev = temp;
			temp = temp->next;
		}
	}
	mtx.unlock();
}

void MultiServer::CloseConnection() {
	shutdown(mainSocket, 2);
	close(mainSocket);
	Sockets *temp;
	while (head != NULL) {
		shutdown(head->socket, 2);
		close(head->socket);
		temp = head->next;
		delete head;
		head = temp;
	}
	startedUp = false;
}

MultiServer::~MultiServer() { CloseConnection(); }

bool MultiServer::Send(char *data, int size, int sock) {
	bool r;
	mtx.lock();
	r = (startedUp && send(sock, data, size, MSG_WAITALL) == size);
	mtx.unlock();
	return r;
}

bool MultiServer::Receive(char *data, int size, int sock) {
	bool r;
	mtx.lock();
	r = (startedUp && recv(sock, data, size, MSG_WAITALL) == size);
	mtx.unlock();
	return r;
}