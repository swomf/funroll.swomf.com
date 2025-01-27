# Default target
all: web

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

web: $(HTML_OUTPUT_FILES) $(WEB_ROOT)/opengraph-preview.webp $(WEB_ROOT)/sitemap.xml $(WEB_ROOT)/monospace.css

# Build a website at web_root
$(MD2HTML):
	$(MAKE) -C $(dir $(MD2HTML))

$(WEB_ROOT):
	mkdir -p $(WEB_ROOT)

$(WEB_ROOT)/%/index.html: $(CONTENT_DIR)/%.md $(MD2HTML)
	@mkdir -p $(dir $@)
	$(MD2HTML) $< --full-html \
		--html-title="$(basename $(notdir $<)) | $(WEBSITE_NAME)" \
		--html-css="/monospace.css" \
		--html-url="$(WEBSITE_URL)" \
		> $@

$(WEB_ROOT)/opengraph-preview.webp: $(WEB_ROOT)
	cp -f assets/og/opengraph-preview.webp $(WEB_ROOT)/opengraph-preview.webp

$(WEB_ROOT)/monospace.css: $(WEB_ROOT)
	cp -f src/css/monospace.css $(WEB_ROOT)/monospace.css

$(WEB_ROOT)/sitemap.xml: $(WEB_ROOT) $(HTML_OUTPUT_FILES)
	echo "<urlset>" > $@
	find $(WEB_ROOT) | awk -v url=$(WEBSITE_URL) -v webrootdir=$(WEB_ROOT) \
		'/index\.html$$/{sub(/index\.html$$/, ""); sub( webrootdir, ""); print "<url><loc>" url $$0 "</loc></url>"}' >> $@
	echo "</urlset>" >> $@

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

.PHONY: all web clean webclean fmt
