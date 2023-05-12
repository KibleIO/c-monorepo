docker-build-proto:
	docker run --platform linux/amd64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:xubuntux86 make build_proto_unix

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