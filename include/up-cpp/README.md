# uProtocol C++ Interface

The core uProtocol interface in C++ is divided into layers corresponding to the
layers defined in the [uProtocol specifications](https://github.com/eclipse-uprotocol/up-spec).
Each layer is represented as a folder containing headers.

## L0: Datamodel

The API for interacting with uProtocol data is provided in the `datamodel/`
folder. This is unofficially "Layer Zero" of uProtocol.

`datamodel/` contains:

* `builder/` - Interfaces for building common configurations of uProtocol objects
* `serializer/` - Functions for serializing and deserializing uProtocol objects to formats _other than_ protobuf
* `validator/` - Functions for inspecting and validating the content of uProtocol objects

## L1: Transport

The abstract API in `transport/` is for sending and receiving uProtocol
messages over a transport protocol, such as Zenoh or SOME/IP.

## L2: Client

uEntities using uProtocol for communication will use the APIs in `client/`.
These provide specific communication models, such as pub/sub, RPC, and
notifications.

These headers are the primary interface for uEntities.

Client classes are implmented entirely on top of the Transport API, and should
not require customization or re-implementation on a per-transport basis.
