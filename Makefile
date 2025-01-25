CONTENT_DIR = content
WEB_ROOT = web_root
MD2HTML = src/md2html/md2html
MD_INPUT_FILES = $(shell find $(CONTENT_DIR) -name "*.md")
HTML_OUTPUT_FILES = $(patsubst $(CONTENT_DIR)/%.md,$(WEB_ROOT)/%/index.html,$(MD_INPUT_FILES))

# Default target
all: $(HTML_OUTPUT_FILES)

# Build a website at web_root
$(MD2HTML):
	$(MAKE) -C $(dir $(MD2HTML))

$(WEB_ROOT)/%/index.html: $(CONTENT_DIR)/%.md $(MD2HTML)
	@mkdir -p $(dir $@)
	$(MD2HTML) $< > $@

webclean:
	rm -rf $(WEB_ROOT)

clean: | webclean
	$(MAKE) -C $(dir $(MD2HTML)) clean

.PHONY: all clean webclean
