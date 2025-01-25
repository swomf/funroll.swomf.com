CONTENT_DIR = content
WEB_ROOT = web_root
MD_INPUT_FILES = $(shell find $(CONTENT_DIR) -name "*.md")
HTML_OUTPUT_FILES = $(patsubst $(CONTENT_DIR)/%.md,$(WEB_ROOT)/%/index.html,$(MD_INPUT_FILES))

# Default target
all: $(HTML_OUTPUT_FILES)

# Build a website at web_root
$(WEB_ROOT):
	mkdir -p $(WEB_ROOT)

$(WEB_ROOT)/%/index.html: $(CONTENT_DIR)/%.md | $(WEB_ROOT)
	@mkdir -p $(dir $@)
	md2html $< > $@

# Clean target to remove generated files
clean:
	rm -rf $(WEB_ROOT)

.PHONY: all clean

