SUBDIRS = src tests/unit tests/integration

check-unit:
	$(MAKE) -C tests/unit check

check-integration:
	$(MAKE) -C tests/integration check


format:
	find src -name '*.c' -or -name '*.h' | xargs clang-format -i
	find tests/integration -name '*.c' -or -name '*.h' | xargs clang-format -i
	find tests/unit -name '*.c' -or -name '*.h' | xargs clang-format -i
