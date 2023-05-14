docker-build-proto-x86:
	docker run --platform linux/amd64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:x86 make build_proto_unix

docker-build-proto-arm:
	docker run --platform linux/arm64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:arm make build_proto_unix

build_proto_unix:
	mkdir -p src/kible.io/gen
	protoc --proto_path=proto-monorepo/idl/kible.io --cpp_out=./src/kible.io/gen --grpc_out=./src/kible.io/gen --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin proto-monorepo/idl/kible.io/*/*.proto
	rename 's/\.cc$$/.cpp/' src/kible.io/gen/*/*.cc

build_proto:
	mkdir -p src/kible.io/gen
	protoc --proto_path=proto-monorepo/idl/kible.io --cpp_out=./src/kible.io/gen --grpc_out=./src/kible.io/gen --plugin=protoc-gen-grpc=/opt/homebrew/bin/grpc_cpp_plugin proto-monorepo/idl/kible.io/*/*.proto
	rename 's/\.cc$$/.cpp/' src/kible.io/gen/*/*.cc

clean:
	rm -rf src/kible.io/gen/*

docker-build-clip-arm:
	docker run --platform linux/arm64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:arm ./install_clip.sh

docker-build-clip-x86:
	docker run --platform linux/amd64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:x86 ./install_clip.sh