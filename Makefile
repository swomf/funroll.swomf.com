#
# website
#

WEBSITE_NAME = funroll
WEBSITE_URL = https://funroll.swomf.com
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

$(WEB_ROOT)/%/index.html: $(CONTENT_DIR)/%.md $(MD2HTML) $(WEB_ROOT)/opengraph-preview.webp
	@mkdir -p $(dir $@)
	$(MD2HTML) $< --full-html \
		--html-title="$(basename $(notdir $<)) | $(WEBSITE_NAME)" \
		--html-url="$(WEBSITE_URL)" \
		> $@

$(WEB_ROOT)/opengraph-preview.webp:
	cp -f assets/og/opengraph-preview.webp $(WEB_ROOT)/opengraph-preview.webp

webclean:
	rm -rf $(WEB_ROOT)

clean: | webclean
	$(MAKE) -C $(dir $(MD2HTML)) clean

#
# misc
#
fmt:
	find . -name '*.c' \
		-o -name '*.h' \
		-o -name '*.cpp' \
		-o -name '*.hpp' | xargs clang-format -i

.PHONY: all clean webclean fmt
