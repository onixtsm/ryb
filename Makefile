ROOT_DIR:=.
#
# when compiling empty library:
# CFLAGS:=-I. -Iplatform/ -Ilibrary/ -Iexternal/ -lm -O0 -g3 -ggdb

LIBRARIES_SOURCES:=$(wildcard ${ROOT_DIR}/library/*.c)
LIBRARIES_HEADERS:=$(wildcard ${ROOT_DIR}/library/*.h)
EXTERNAL_SOURCES:=$(wildcard ${ROOT_DIR}/external/*.c)
EXTERNAL_HEADERS:=$(wildcard ${ROOT_DIR}/external/*.h)
LIBRARIES_OBJECTS:=$(LIBRARIES_SOURCES:.c=.o) $(EXTERNAL_SOURCES:.c=.o)

SUDO?=sudo

BUILD_DIR:=${ROOT_DIR}/build
LIB_DIR:=${BUILD_DIR}/lib
LIB_PYNQ:=${LIB_DIR}/libpynq.a
LIB_SCPI:=external/scpi-parser/libscpi/dist/
OBJ_DIR:=${BUILD_DIR}/obj

OBJECTS_LIBRARIES:=$(foreach obj,$(LIBRARIES_OBJECTS),${BUILD_DIR}/$(obj))

DIRS:=$(foreach obj,$(LIBRARIES_OBJECTS),${BUILD_DIR}/$(dir $(obj))/)


SRC_DIR:=${ROOT_DIR}/src

SOURCES:=$(wildcard ${SRC_DIR}/*.c)
SOURCES_H:=$(wildcard ${SRC_DIR}/*.h)
SOURCES_OBJ:=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

D_FILES:=$(OBJECTS_LIBRARIES:.o=.d) $(LIBRARIES:.o=.d)

CFLAGS:=-I. -I${ROOT_DIR}/platform/ -I${ROOT_DIR}/library/ -I${ROOT_DIR}/external/ -lm -O0 -g3 -ggdb -Wextra -Wall
LDFLAGS:= -lm -ggdb -g3 -I. -I${SRC_DIR}

nopynq:=0
x86_64:=x86_64
ifeq ($(shell uname -m), $(x86_64))
	ifeq ($(nopynq), 0)
		CC=arm-linux-gnueabihf-gcc
	endif
endif

all: ${LIB_PYNQ} ${LIB_SCPI} ${BUILD_DIR}/main

${BUILD_DIR}/%.d: %.c | ${DIRS}
	${CC} -c -MT"${OBJ_DIR}/$*.o" -MM  -o $@ $^ ${CFLAGS}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c | ${DIRS}
	@mkdir -p $(@D)  # Create the directory if it doesn't exist
	${CC} -c -o $@ $< ${CFLAGS} ${MYVARS}

${BUILD_DIR}/main: ${SOURCES_OBJ} ${LIB_PYNQ} ${EXTERNAL_LIBS}
	$(VERBOSE)${CC} -o $@ $^ ${LDFLAGS} ${MYWARS}
ifeq ($(nopynq), 0)
		$(VERBOSE)${SUDO} setcap cap_sys_rawio+ep ./${@}
endif

# first time 
install:
	@# these warnings don't get displayed before the .d are made due to wrong(?) dependencies
	@echo 'WARNING: if compilation never finishes, then stop it with ^C'
	@echo 'WARNING: you may have missed the -DD flag when unzipping'
	@echo 'WARNING: remove this directory and unzip again with '\''unzip -DD -q '$(RELEASE_BASENAME)'.zip'
	@# alternatively: run find . -exec touch {} \;' in the current directory
	@ [ -f external/scpi-parser/README.md ] || echo "Error: the scpi parser is missing"
	$(MAKE) all applications
	@# always copy latest version
	cp applications/read-version/main bin/compatibility-check
ifeq ($(nopynq), 0)
	sudo setcap cap_sys_rawio+ep bin/compatibility-check
	cp applications/pin-indexing-tool/main bin/pin-indexing-tool
	sudo setcap cap_sys_rawio+ep bin/pin-indexing-tool
endif

${LIB_SCPI}:
	$(MAKE) -C ${ROOT_DIR}/external/scpi-parser/libscpi/

ifneq (clean,$(MAKECMDGOALS))
-include ${D_FILES}
endif

${BUILD_DIR}/%.d: %.c | ${DIRS}
	${CC} -c -MT"${BUILD_DIR}/$*.o" -MM  -o $@ $^ ${CFLAGS}

${BUILD_DIR}/%.o: %.c | ${DIRS}
	${CC} -c -o $@ $< ${CFLAGS}

${LIB_PYNQ}: ${OBJECTS_LIBRARIES} | ${LIB_DIR}
	@mkdir -p build/lib
	$(AR) rcs $@ $?

%/:
	mkdir -p ${@}

indent:
	clang-format -i $^ ${SOURCES} ${SOURCES_H}


indent-library: ${LIBRARIES_SOURCES} ${LIBRARIES_HEADERS}
	clang-format -i $^

doc: ${BUILD_DIR}/html

${BUILD_DIR}/html: documentation/ryb.doxy ${LIBRARIES_SOURCES} ${LIBRARIES_HEADERS}
	sed -e 's/^PROJECT_NUMBER         =.*/PROJECT_NUMBER         = \"('$(LABEL)'\")/' documentation/ryb.doxy > documentation/ryb.doxy.new
	doxygen documentation/ryb.doxy.new
	rm documentation/ryb.doxy.new
	${MAKE} -C ${BUILD_DIR}/latex/


${LIB_DIR}:
		@mkdir -p $@
# can only clean both library and applications
clean: 
	rm -rf ${OBJ_DIR} ${BUILD_DIR}/main

realclean:
	rm -rf ${BUILD_DIR}

run:
	@${BUILD_DIR}/main


.PHONY: indent indent-library indent-applications doc clean release install doc realclean run
