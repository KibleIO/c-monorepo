.PHONY: protobuf_http_api_example themis rana_core_utils

protobuf_http_api_example:
	make -f protobuf_http_api_example.mk

protobuf_http_api_example_clean:
	make -f protobuf_http_api_example.mk clean

protobuf_http_api_example_run:
	make -f protobuf_http_api_example.mk run

themis:
	make -f themis.mk

themis_clean:
	make -f themis.mk clean

themis_run:
	make -f themis.mk run

rana_core_utils:
	make -f rana_core_utils.mk

rana_core_utils_clean:
	make -f rana_core_utils.mk clean

dockerimage-arm-publish:
	docker build --platform linux/arm64 --file Dockerfile.core -t kible/coreutils:arm .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils:arm

dockerimage-x86-publish:
	docker build --platform linux/amd64 --file Dockerfile.core -t kible/coreutils:x86 .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils:x86