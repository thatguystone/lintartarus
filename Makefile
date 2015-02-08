SRC = src

PKGS = x11 libusb-1.0 >= 1.0.19

OBJECTS = \
	$(SRC)/lintartarus.o \
	$(SRC)/usb.o \
	$(SRC)/x.o

export CFLAGS = \
	-c \
	-g \
	-Wall \
	-Wextra \
	-Wcast-qual \
	-Wdeclaration-after-statement \
	-Wdisabled-optimization \
	-Wformat=2 \
	-Wmissing-prototypes \
	-Wredundant-decls \
	-Wshadow \
	-Wstrict-prototypes \
	-Wundef \
	-Wwrite-strings \
	-Werror \
	-fstack-protector \
	--param=ssp-buffer-size=4 \
	-D_FORTIFY_SOURCE=2 \
	-std=gnu99 \
	-march=native \
	-mfpmath=sse \
	-msse \
	-msse2 \
	-MMD \
	`pkg-config --cflags '$(PKGS)'`

export LDFLAGS = \
	-g \
	-rdynamic \
	`pkg-config --libs '$(PKGS)'`

all: lintartarus

clean:
	rm -f $(OBJECTS)
	rm -f $(OBJECTS:.o=.d)

lintartarus: $(OBJECTS)
	@echo LD $@
	@$(CC) $^ -o $@ $(LDFLAGS)

$(OBJECTS): %.o: %.c
	@echo CC $<
	@$(CC) $(CFLAGS) $< -o $@

ifeq (,$(findstring clean,$(MAKECMDGOALS)))
-include $(OBJECTS:.o=.d)
endif
