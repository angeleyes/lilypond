
MF_FILES := $(call src-wildcard,*.mf)
EXTRA_DIST_FILES += $(MF_FILES)
MF_TFM_FILES = $(addprefix $(outdir)/, $(FONT_FILES:.mf=.tfm))
MF_DVI_FILES = $(addprefix $(outdir)/, $(FONT_FILES:.mf=.dvi))
MF_LOG_FILES = $(addprefix $(outdir)/, $(FONT_FILES:.mf=.log))
DVI_FILES += $(MF_DVI_FILES)
TFM_FILES += $(MF_TFM_FILES)


