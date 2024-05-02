CFLAGS= --std=c++17 -Ic_include -Iup-core-api -Iinclude
VPATH= src up-core-api
PROTO_LIB=/home/gzy3g3/.conan/data/protobuf/3.21.12/_/_/package/2dbf65f76c0469903ce48756c39d50cd4e721678/lib/libprotobuf.a

demo : demo.o UpCoreApi.o upayload.pb.o umessage.pb.o uattributes.pb.o uri.pb.o ustatus.pb.o uuid.pb.o uprotocol_options.pb.o
	g++ $(CFLAGS) -o x *.o $(PROTO_LIB)
	# g++ $(CFLAGS) -o x *.o `pkg-config --libs protobuf`

.cpp.o:
	g++ $(CFLAGS) -c $<

.cc.o:
	g++ $(CFLAGS) -c $<
