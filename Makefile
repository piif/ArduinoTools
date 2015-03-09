# it's just a library
TODO := lib

include ${AMM_DIR}/etc/Makefile.target

$(info TARGET_PLATFORM = ${TARGET_PLATFORM})

INCLUDE_FLAGS += $(addprefix -I,${LIBRARIES_DIRS} .)

SOURCE_EXCLUDE_PATTERNS := /examples/ /tests/

include ${AMM_DIR}/etc/Makefile.main
