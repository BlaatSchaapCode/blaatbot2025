# geblaat

Welcome to `geblaat`, a modular chat framework.


## Components

### Core
  
  The core of `geblaat` has as major component the `PluginLoader`. This will load plugins, which provide all the functionality offered by `geblaat`.
  

### Client-to-Server protocol C2SProtocol

The `geblaat` chat framework has defined a `C2SProtocol` class which implements a Client-to-Server protocol. Initially the IRC protocol is implemented, but the design is intended to allow for other protocols, such as XMPP and Matrix to be implemented within the framework.

### Connection

A protocol like IRC requires a connection. This is once done in an implementation of the Connection class. As such, a plain TCP or an encrypted TLS connection can be implemented, independant of the protocol implementation. Initially there is a TCP and a LibreTLS implementation. Implementations using other TLS libraries can be made, as well as a WebSocket implementation.

### Client

The third component of the core is an implementation of the Client class. In the initial release this will be the BotClient, implementing Bot functionality, but is intended to allow for other implementations, allowing for the implementation of an IRC client, hence the name.

## BlaatBot2025

The initial client implementation for the `geblaat` chat framework is the BotModule, BlaatBot2025.

Once again, this is a modular design. The BotModule offers to bot commands, all commands are loadable modules