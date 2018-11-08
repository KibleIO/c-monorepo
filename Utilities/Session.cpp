#ifndef SESSION_CPP_
#define SESSION_CPP_

#include "Session.h"

Session::Session() {
	connected = false;
}

Session::~Session() {
	if (connected) {
		ssh_disconnect(ssh);
        ssh_free(ssh);
	}
}

bool Session::Connect(string ip, string user, string pass, int *port) {
	int rc;

	ssh = ssh_new();
	if (ssh == NULL)
                return false;
	ssh_options_set(ssh, SSH_OPTIONS_HOST, ip.c_str());
        ssh_options_set(ssh, SSH_OPTIONS_PORT, port);

	rc = ssh_connect(ssh);
        if (rc != SSH_OK)
		return false;

	rc = ssh_userauth_password(ssh, user.c_str(), pass.c_str());
        if (rc != SSH_AUTH_SUCCESS) {
                ssh_disconnect(ssh);
                ssh_free(ssh);
		return false;
        }
	connected = true;
	username = user;
	return true;
}

string Session::runCommand(string command) {
	if (!connected) return "Not connected to SSH server";

	ssh_channel channel;
        int rc;
        char buffer[256];
        unsigned int nbytes;
        channel = ssh_channel_new(ssh);

        if (channel == NULL)
                return "SSH error";

        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
                ssh_channel_free(channel);
                return "Problem creating channel";
        }

        rc = ssh_channel_request_exec(channel, command.c_str());
        if (rc != SSH_OK) {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                return "Error running command:" + command;
        }

        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        
	/*
        while (nbytes > 0) {
                if (write(1, buffer, nbytes) != nbytes) {
                        ssh_channel_close(channel);
                        ssh_channel_free(channel);
                        return "Error reading buffer";
                }
                nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
		//not quite sure what to do here
        }
	*/

        if (nbytes < 0) {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                return "Returned empty output";
        }
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return buffer;
}

string Session::runCommand_(string command) {
	if (!connected) return "Not connected to SSH server";
	ssh_channel channel;
        int rc;
        char buffer[256];
        unsigned int nbytes;
        channel = ssh_channel_new(ssh);
        if (channel == NULL)
                return "SSH error";
        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
                ssh_channel_free(channel);
                return "Problem creating channel";
        }
        rc = ssh_channel_request_exec(channel, command.c_str());
        if (rc != SSH_OK) {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                return "Error running command:" + command;
        }
    return "Success!";
}

#endif /* SESSION_CPP_ */
