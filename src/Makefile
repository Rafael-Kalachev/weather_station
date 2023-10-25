PROJECT = test
BUILD_DIR = bin

CFILES = my-project/my-project.c 

# TODO - you will need to edit these two lines!
DEVICE=stm32f429zi
OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here. 
INCLUDE_DIR = libopencm3/include my-project
INCLUDES += $(patsubst %,-I%, . $(INCLUDE_DIR))
OPENCM3_DIR= libopencm3
VPATH = $(INCLUDE_DIR)

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
