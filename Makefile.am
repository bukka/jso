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
	@echo "Checking code formatting..."
	@FORMATTING_ISSUES=0; \
	for dir in src tests/unit tests/integration; do \
		if [ -d "$$dir" ]; then \
			echo "Checking $$dir/"; \
			for file in $$(find $$dir -name '*.c' -or -name '*.h' 2>/dev/null); do \
				if ! $(CLANG_FORMAT) --dry-run --Werror "$$file" >/dev/null 2>&1; then \
					echo "❌ Formatting issues in: $$file"; \
					FORMATTING_ISSUES=1; \
				fi; \
			done; \
		fi; \
	done; \
	if [ $$FORMATTING_ISSUES -eq 1 ]; then \
		echo ""; \
		echo "💡 To fix formatting issues, run: make format"; \
		exit 1; \
	else \
		echo "✅ All files are properly formatted!"; \
	fi