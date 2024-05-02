#!/usr/bin/env python3

from pathlib import Path
from protobuf_wrapper import ProtobufWrapper

proto_dir = Path.cwd() / '../up-core-api/uprotocol'
with ProtobufWrapper('uprotocol::v1', 'UpCoreApi') as pb:
    pb(proto_dir / 'uuid.proto')
    pb(proto_dir / 'ustatus.proto')
    pb(proto_dir / 'uri.proto')
    pb(proto_dir / 'uattributes.proto')
    pb(proto_dir / 'upayload.proto')
    pb(proto_dir / 'umessage.proto')

