TEST_DIRS = $(dir $(wildcard */))

.PHONY: all $(TEST_DIRS)
all: $(TEST_DIRS)

$(TEST_DIRS):
	make -C $@

.PHONY: clean
clean:
	$(foreach DIR, $(TEST_DIRS), make -C $(DIR) clean)
