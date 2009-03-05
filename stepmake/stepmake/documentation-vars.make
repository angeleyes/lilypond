at-dir = $(doc-dir)/
at-ext = .in

CSS_FILES := $(call src-wildcard,*.css)
JS_FILES := $(call src-wildcard,*.js)
EXTRA_DIST_FILES += $(CSS_FILES) $(JS_FILES)
