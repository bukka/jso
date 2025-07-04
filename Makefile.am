SUBDIRS = src tests/unit tests/integration
CLANG_FORMAT ?= clang-format

check-unit:
	$(MAKE) -C tests/unit check

check-integration:
	$(MAKE) -C tests/integration check

format:
	find src -name '*.c' -or -name '*.h' | \
		grep -v -E '(\.tab\.|_scanner\.c|_scanner_defs\.h)' | \
		xargs $(CLANG_FORMAT) -i
	find tests/integration -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) -i
	find tests/unit -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) -i

format-check:
	find src -name '*.c' -or -name '*.h' | \
		grep -v -E '(\.tab\.|_scanner\.c|_scanner_defs\.h)' | \
		xargs $(CLANG_FORMAT) --dry-run --Werror
	find tests/integration -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) --dry-run --Werror
	find tests/unit -name '*.c' -or -name '*.h' | xargs $(CLANG_FORMAT) --dry-run --Werror

.PHONY: check-unit check-integration format format-check
