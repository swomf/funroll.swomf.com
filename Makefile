# Default target
all: tangle

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
COPY_INPUT_FILES = assets/butterfly/butterfly-with-text.webp \
									 $(CONTENT_DIR)/index.html $(CONTENT_DIR)/typing-funroll.js \
									 assets/butterfly/butterfly.svg $(CONTENT_DIR)/404.html
# Unfortunately this is the cleanest way to make a non-redundant copy rule
COPY_OUTPUT_FILES =
define CREATE_COPY_RULE
$$(WEB_ROOT)/$$(notdir $(1)): $(1)
	cp -uf $$^ $$@
COPY_OUTPUT_FILES += $$(WEB_ROOT)/$$(notdir $(1))
endef
$(foreach file,$(COPY_INPUT_FILES),$(eval $(call CREATE_COPY_RULE,$(file))))

# Build a website at web_root
web: $(HTML_OUTPUT_FILES) $(TOP_COPY_OUTPUT_FILES) \
	$(WEB_ROOT) $(WEB_ROOT)/sitemap.xml $(COPY_OUTPUT_FILES) \
	$(WEB_ROOT)/main.css

$(WEB_ROOT):
	[ -d $@ ] || mkdir $@

$(WEB_ROOT)/%/index.html: $(CONTENT_DIR)/%.md $(MD2HTML)
	@mkdir -p $(@D)
	$(MD2HTML) $< --full-html \
		--html-title="$(basename $(notdir $<)) | $(WEBSITE_NAME)" \
		--html-css="/main.css" \
		--html-url="$(WEBSITE_URL)" \
		--ftables \
		--stat \
		> $@

$(WEB_ROOT)/sitemap.xml: $(HTML_OUTPUT_FILES) $(COPY_OUTPUT_FILES) | $(WEB_ROOT)
	echo "<urlset>" > $@
	find $(WEB_ROOT) -not -name '404.html' | awk -v url=$(WEBSITE_URL) -v webrootdir=$(WEB_ROOT) \
		'/index\.html$$/{sub(/index\.html$$/, ""); sub( webrootdir, ""); print "<url><loc>" url $$0 "</loc></url>"}' >> $@
	echo "</urlset>" >> $@

$(WEB_ROOT)/main.css: $(wildcard ./src/css/*.css)
	cat $^ > $@

webclean:
	$(RM) -r $(WEB_ROOT)

######################
### gentoo_install ###
######################

# the files in gentoo_install to copy into /

tangle: gentoo_install
include src/tangle/tangle.mk # for TANGLE

JOBS := $(if $(filter -j%, $(MAKEFLAGS)),$(subst -j,,$(filter -j%, $(MAKEFLAGS))),1)
# NOTE: src/tangle/tangle is responsible for running mkdir -p here.
#       therefore EVERYTHING is rebuilt anyway
gentoo_install: $(MD_INPUT_FILES) $(TANGLE)
	$(RM) -r $@
	echo $(MD_INPUT_FILES) | xargs -n1 -P $(JOBS) $(TANGLE)

# If the user isn't root, only show a dry run.
ifneq ($(shell id -u), 0)
INSTALL_CMD := echo dryrun: install
else
INSTALL_CMD := install
endif

# FIXME: This is a glorious hack.
#        In `make sync`, $(PORTAGE_OUTPUT_FILES) *cannot* be defined
#        until the gentoo_install directory exists.
#        Therefore, in order to have Makefile attempt redefinition of
#        $(PORTAGE_OUTPUT_FILES) after the gentoo_install target is reached,
#        we have Makefile enter *its own directory* to run the second step of sync.
#        NORMALLY, we are able to redefine variables,
#           https://stackoverflow.com/questions/2711963/change-makefile-variable-value-inside-the-target-body
#        but, if the variable is expressed as a dependency, it won't be reevaluated.
PORTAGE_OUTPUT_FILES = $(patsubst gentoo_install/%,/%,$(shell find gentoo_install -type f 2>/dev/null))
sync: gentoo_install
	$(MAKE) _sync2_INTERNAL
	@echo Configuration synced.
_sync2_INTERNAL: $(PORTAGE_OUTPUT_FILES)

diff: gentoo_install
	./src/scripts/diff

# Relies on $(PORTAGE_OUTPUT_FILES) to be correctly defined
/%: gentoo_install/%
	@# Multiple printfs for one message will lead to race conditions
	@# when JOBS >= 1, so we repeat ourselves quite a bit
	@if [ -f "$@" ]; then \
		diff_output="$$(diff --color=always $@ $<)"; \
		if [ -z "$${diff_output}" ]; then \
			printf "\033[1;32m      Syncing\033[0m %s \033[1;32m->\033[0m %s\n" "$<" "$@"; \
		else \
			printf "\033[1;32m      Syncing\033[0m %s \033[1;32m->\033[0m %s\n%s\n" "$<" "$@" "$${diff_output}"; \
		fi \
	else \
    printf "\033[1;32m      Syncing\033[0m %s \033[1;32m->\033[0m %s \033[1;32m(new)\033[0m\n" "$<" "$@"; \
	fi
	@case "$@" in \
		*.sh | *.start) \
	    $(INSTALL_CMD) -Dm755 "$<" "$@" \
			;; \
		*) \
	    $(INSTALL_CMD) -Dm644 "$<" "$@" \
			;; \
	esac

############
### misc ###
############

# use 2 colons so it can be additively redefined in any included .mk files
clean:: webclean
	$(RM) $(MD2HTML) $(TANGLE)
	$(RM) -r gentoo_install

help:
	@echo Run `make` and `make sync` to tangle your Markdown files and install.
	@echo Run `make web` to turn your Markdown files into HTML.
	@echo Run `make clean` to clean everything, or `make webclean` to only clean the web_root directory (since `make web` is not stateless)

.PHONY: all web webclean tangle diff sync _sync2_INTERNAL help
