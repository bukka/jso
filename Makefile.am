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

format-check:
	@UNFORMATTED=$$(find src tests/integration tests/unit -name '*.[ch]' | xargs $(CLANG_FORMAT) -style=file -output-replacements-xml | grep -c '<replacement ' || true); \
	if [ "$$UNFORMATTED" -ne 0 ]; then \
	  echo "$$UNFORMATTED files not properly formatted. Run 'make format' to fix."; \
	  exit 1; \
	fi
