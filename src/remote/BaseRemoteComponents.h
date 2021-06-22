/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Nutricherry LTD).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

/**
 * @file BaseRemoteComponents.h contains remote components that are shared across all remote devices.
 */

#ifndef TCMENU_BASEREMOTECOMPONENTS_H
#define TCMENU_BASEREMOTECOMPONENTS_H

#include <PlatformDetermination.h>
#include "../RemoteConnector.h"

#define ALLOWED_CONNECTIONS 4

namespace tcremote {

    /**
     * The device initialiser is responsible for initially preparing the hardware communications device for use, and
     * then for creating new connections that can be used with a transport, you provide the transport to it, and the
     * transport provided MUST be of the expected type for this instance.
     */
    class DeviceInitialisation {
    protected:
        bool initialised = false;
    public:
        bool isInitialised() const { return initialised; }
        virtual bool attemptInitialisation()=0;
        virtual bool attemptNewConnection(TagValueTransport* transport)=0;
    };

    /**
     * Some communication devices don't need any initialisation, in these cases this class can be used, it is a NOP
     * implementation that always returns success.
     */
    class NoInitialisationNeeded : public DeviceInitialisation {
    private:
    public:
        bool attemptInitialisation() override {
            initialised = true;
            return true;
        }

        bool attemptNewConnection(TagValueTransport *transport) override { return true; }
    };

    /**
     * Contains the components of a single connection, which is essentially the remoteConnector and the transport, this
     * allows us to send and receive messages on that transport, connect to it, and determine if it is still connected.
     * It also allows us to establish new connections via the transport.
     */
    class RemoteServerConnection {
    private:
        TagValueRemoteConnector remoteConnector;
        TagValueTransport &remoteTransport;
        CombinedMessageProcessor messageProcessor;
        DeviceInitialisation &initialisation;
    public:
        RemoteServerConnection(TagValueTransport &transport, DeviceInitialisation& initialisation);

        TagValueRemoteConnector *connector() { return &remoteConnector; }

        TagValueTransport *transport() { return &remoteTransport; }

        CombinedMessageProcessor *messageProcessors() { return &messageProcessor; }

        void tick();
    };

    typedef RemoteServerConnection* RemoteServerConnectionPtr;

    /**
     * This is the component that allows us to manage as many connections as needed using a single instance, it
     * holds on to instances of RemoteServerConnection and services them all, it also provides the getter functions
     * for acquiring the transport or connector for a given item.
     */
    class TcMenuRemoteServer : public Executable {
        RemoteServerConnectionPtr connections[ALLOWED_CONNECTIONS];
        const ConnectorLocalInfo& appInfo;
        uint8_t remotesAdded;
    public:
        /**
         * Creates an instance of the remote server component that has no connections but is properly configured ready
         * for connections to be added. You must provide the application information. The server is started when the
         * first remote is added.
         *
         * @param appInfo the application information - uuid and name basically.
         */
        explicit TcMenuRemoteServer(const ConnectorLocalInfo& appInfo) : connections{}, appInfo(appInfo), remotesAdded(0) { }

        /**
         * Remove all current remotes
         */
        void clearRemotes() {
            remotesAdded = 0;
        }

        void exec() override;

        /**
         * Adds a connection to the managed connections, this class will ensure that it is properly initialised,
         * any authenticator on menuMgr added to it, and will call it's tick method frequently.
         * @param toAdd the connection to add
         * @return a remote number of 0xff if it fails.
         */
        uint8_t addConnection(RemoteServerConnection *toAdd);

        /**
         * @return the number of remote connections added.
         */
        uint8_t remoteCount() const { return remotesAdded; }

        /**
         * Gets the `TagValueRemoteConnector` at the given remoteNo, or nullptr if not available.
         * @param num the remote number
         * @return either a valid object or nullptr
         */
        TagValueRemoteConnector *getRemoteConnector(int num) {
            if(num >= remotesAdded) return nullptr;
            return connections[num]->connector();
        }

        /**
         * Gets the `TagValueTransport` at the given remoteNo, or nullptr if not available.
         * @param num the remote number
         * @return either a valid object or nullptr
         */
        TagValueTransport *getTransport(int num) {
            if(num >= remotesAdded) return nullptr;
            return connections[num]->transport();
        }
    };
}

#endif //TCMENU_BASEREMOTECOMPONENTS_H
