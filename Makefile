# Default target
all: web tangle

include nonrec.mk

###############
### website ###
###############

WEBSITE_NAME = funroll
WEBSITE_URL = https://funroll.swomf.com
CONTENT_DIR = content
WEB_ROOT = web_root
include src/md2html/md2html.mk # for MD2HTML
MD_INPUT_FILES = $(shell find $(CONTENT_DIR) -name "*.md")
HTML_OUTPUT_FILES = $(patsubst $(CONTENT_DIR)/%.md,$(WEB_ROOT)/%/index.html,$(MD_INPUT_FILES))

# Build a website at web_root
web: $(HTML_OUTPUT_FILES) $(TOP_COPY_OUTPUT_FILES) \
	$(WEB_ROOT)/sitemap.xml $(WEB_ROOT)/opengraph-preview.webp $(WEB_ROOT)/monospace.css $(WEB_ROOT)/index.html

# A bit repetitive but simple
$(WEB_ROOT):
	mkdir -p $@
$(WEB_ROOT)/opengraph-preview.webp: assets/og/opengraph-preview.webp | $(WEB_ROOT)
	cp -f $< $@
$(WEB_ROOT)/monospace.css: src/css/monospace.css | $(WEB_ROOT)
	cp -f $< $@
$(WEB_ROOT)/index.html: $(CONTENT_DIR)/index.html | $(WEB_ROOT)
	cp -f $< $@

$(WEB_ROOT)/%/index.html: $(CONTENT_DIR)/%.md $(MD2HTML)
	@mkdir -p $(@D)
	$(MD2HTML) $< --full-html \
		--html-title="$(basename $(notdir $<)) | $(WEBSITE_NAME)" \
		--html-css="/monospace.css" \
		--html-url="$(WEBSITE_URL)" \
		--stat \
		> $@

$(WEB_ROOT)/sitemap.xml: $(HTML_OUTPUT_FILES) | $(WEB_ROOT)
	echo "<urlset>" > $@
	find $(WEB_ROOT) | awk -v url=$(WEBSITE_URL) -v webrootdir=$(WEB_ROOT) \
		'/index\.html$$/{sub(/index\.html$$/, ""); sub( webrootdir, ""); print "<url><loc>" url $$0 "</loc></url>"}' >> $@
	echo "</urlset>" >> $@

webclean:
	$(RM) -r $(WEB_ROOT)

######################
### gentoo_install ###
######################

tangle: gentoo_install
include src/tangle/tangle.mk # for TANGLE

# TODO Should multithreading use -j, use nproc, or not be used at all?
gentoo_install: $(MD_INPUT_FILES) $(TANGLE)
	$(RM) -r gentoo_install
	echo $(MD_INPUT_FILES) | xargs -n1 -P $(shell nproc) $(TANGLE)

############
### misc ###
############

clean:: webclean
	$(RM) $(MD2HTML) $(TANGLE)
	$(RM) -r gentoo_install

.PHONY: all web webclean
