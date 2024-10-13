SUBDIRS = src tests/unit

format:
	find src -name '*.c' -or -name '*.h' | xargs clang-format -i
	find tests/integration -name '*.c' -or -name '*.h' | xargs clang-format -i
	find tests/unit -name '*.c' -or -name '*.h' | xargs clang-format -i
