PROTOBUF_LIB=/home/gzy3g3/.conan/data/protobuf/3.21.12/_/_/package/2dbf65f76c0469903ce48756c39d50cd4e721678/lib/libprotobuf.a
PROTOBUF_OBJDIR=/home/gzy3g3/projects/uspace/ultifi/build-x86_64-release-build_tests/up-cpp/CMakeFiles/up-cpp.dir/__/up-core-api/

GOOGLE_PROTOBUF_HDRDIR=/home/gzy3g3/.conan/data/protobuf/3.21.12/_/_/package/2dbf65f76c0469903ce48756c39d50cd4e721678/include
PROTOBUF_HDRDIR=/home/gzy3g3/projects/uspace/ultifi/build-x86_64-release-build_tests/up-core-api

PROTOBUF_OBJS=$(addprefix $(PROTOBUF_OBJDIR), file.pb.cc.o  uattributes.pb.cc.o  umessage.pb.cc.o  upayload.pb.cc.o  uprotocol_options.pb.cc.o  uri.pb.cc.o  ustatus.pb.cc.o  uuid.pb.cc.o)

CFLAGS= -g --std=c++17 -I$(PROTOBUF_HDRDIR) -I$(GOOGLE_PROTOBUF_HDRDIR)


demo : demo.o UpCoreApi.o $(PROTOBUF_OBJS)
	g++ $(CFLAGS) -o demo demo.cpp UpCoreApi.cpp $(PROTOBUF_OBJS) $(PROTOBUF_LIB)

.cpp.o:
	g++ $(CFLAGS) -c $<

.cc.o:
	g++ $(CFLAGS) -c $<
