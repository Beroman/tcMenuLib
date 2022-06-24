
#ifndef TCLIBRARYDEV_TCMENUECWEBSERVER_H
#define TCLIBRARYDEV_TCMENUECWEBSERVER_H

#include <Arduino.h>
#include <STM32Ethernet.h>
#include <remote/TcMenuWebSocket.h>
#include <remote/BaseRemoteComponents.h>

/**
 * This global method absolutely must be provided when you are not using the pre-packaged version of embedCONTROL-JS
 * @param server The server reference to provide the static hosting and configuration details to.
 */

class TcMenuWebSockTransport : public tcremote::AbstractWebSocketTcMenuTransport {
private:
    EthernetClient client;
public:
    TcMenuWebSockTransport() = default;

    void setClient(EthernetClient newClient) {
        client = newClient;
        setState(tcremote::WSS_UPGRADING);
    }

    bool connected() override {
        return client.connected();
    }

    int performRawRead(uint8_t* buffer, size_t bufferSize) override {
        return client.read(buffer, bufferSize);
    }

    int performRawWrite(const uint8_t* data, size_t dataSize) override {
        return client.write(data, dataSize);
    }
};

class TcMenuWebSockInitialisation : public tcremote::AbstractWebSocketTcMenuInitialisation {
private:
    EthernetServer* server;
public:
    TcMenuWebSockInitialisation(const char *expectedPath) : AbstractWebSocketTcMenuInitialisation(expectedPath) {}
    bool attemptInitialisation() override;
    bool attemptNewConnection(tcremote::BaseRemoteServerConnection* remoteConnection) override;
};

void webServerInitialise();

#endif //TCLIBRARYDEV_TCMENUECWEBSERVER_H
