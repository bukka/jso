SUBDIRS = src tests/unit tests/integration
CLANG_FORMAT ?= clang-format

check-unit:
	$(MAKE) -C tests/unit check

check-integration:
	$(MAKE) -C tests/integration check

format:
	find src -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) -i
	find tests/integration -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) -i
	find tests/unit -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) -i
