all: build

tag := `git describe --always`
commit := `git describe --always`
buildtime := `date +%FT%T%z`
CURRENT_DIR=$(shell pwd)
BUILD_DIR=$(CURRENT_DIR)/build
TEST_DATA_DIR=$(CURRENT_DIR)/test_data
LINUX_TOOLCHAIN_FILE=$(CURRENT_DIR)/hbapp_public/script/linux.toolchain.cmake
ClangFormat := $(shell command -v clang-format 2> /dev/null)
DOCKER := $(shell command -v docker 2> /dev/null)
exist_build = $(shell if [ -f $(BUILD_DIR) ]; then echo "exist"; else echo "notexist"; fi;)


lint:
ifndef ClangFormat
	$(error "clang-format is not available please install clang-format")
endif
	find $(CURRENT_DIR)/plugins -name '*.h' -or -name '*.inl' -or -name '*.cc' -or -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i -style file
	find $(CURRENT_DIR)/hbapp -name '*.h' -or -name '*.inl' -or -name '*.cc' -or -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i -style file

lint_public:
ifndef ClangFormat
	$(error "clang-format is not available please install clang-format")
endif
	find $(CURRENT_DIR)/hbapp_public  -name '*.h' -or -name '*.inl' -or -name '*.cc' -or -name '*.cpp' -or -name '*.hpp' | grep -iv '$(CURRENT_DIR)/hbapp_public/appbase' | xargs clang-format -i -style file

lint_cmake:
	echo "is comming"

mkdir_build:
	if [ ! -d $(BUILD_DIR) ]; then \
        mkdir $(BUILD_DIR); \
    fi

build: mkdir_build
	cd $(BUILD_DIR) && cmake -DCMAKE_TOOLCHAIN_FILE=$(LINUX_TOOLCHAIN_FILE) -DCMAKE_BUILD_TYPE=Release  .. && make -j2

debug: mkdir_build
	cd $(BUILD_DIR) && cmake -DCMAKE_TOOLCHAIN_FILE=$(LINUX_TOOLCHAIN_FILE) -DCMAKE_BUILD_TYPE=Debug  ..  && make -j2

q: 
	cd $(BUILD_DIR) && make -j2

mkdir_test_data:
	if [ ! -d $(TEST_DATA_DIR) ]; then \
		mkdir $(TEST_DATA_DIR);  \
	fi
	if [ ! -d $(TEST_DATA_DIR)/config ]; then \
		mkdir $(TEST_DATA_DIR)/config; \
	fi
	if [ ! -d $(TEST_DATA_DIR)/config/json ]; then \
		mkdir $(TEST_DATA_DIR)/config/json; \
	fi
	if [ ! -d $(TEST_DATA_DIR)/data ]; then \
		mkdir $(TEST_DATA_DIR)/data; \
	fi

run: mkdir_test_data
	$(BUILD_DIR)/hbapp/launcher/launcher --config-dir $(TEST_DATA_DIR)/config --data-dir $(TEST_DATA_DIR)/data

test:
	cd $(BUILD_DIR) && make test 

clean:
	rm -rf build

git_check:
	! git status -s | grep '^'

.PHONY: sync server style format test build clean vendor tidy docker

# docker:
# ifndef DOCKER
# 	$(error "docker is not available please install docker")
# endif
# 	bash tools/docker/build_linux.sh