
TINYSTM_VERSION=1.0.3
TINYSTM_DIR=tinySTM-$(TINYSTM_VERSION)
TINYSTM_TARBALL=$(TINYSTM_DIR).tgz
TINYSTM_URL=http://tmware.org/sites/tmware.org/files/tinySTM/$(TINYSTM_TARBALL)
TINYSTM_LIB=libitm.so

####

BUILD_DIR=build
DIST_DIR=dist

all: tinystm

dirs: lib $(BUILD_DIR) $(DIST_DIR)
lib:
	mkdir -p lib

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

tinystm: $(BUILD_DIR)/.tinystm

$(BUILD_DIR)/.tinystm: dirs $(DIST_DIR)/$(TINYSTM_TARBALL) $(BUILD_DIR)/$(TINYSTM_DIR) lib/$(TINYSTM_LIB)
	touch $@

lib/$(TINYSTM_LIB): 
	make -C $(BUILD_DIR)/$(TINYSTM_DIR) abi-gcc
	cp $(BUILD_DIR)/$(TINYSTM_DIR)/abi/gcc/$(TINYSTM_LIB) $@
	ln -s $(TINYSTM_LIB) $@.0

$(DIST_DIR)/$(TINYSTM_TARBALL):
	cd $(DIST_DIR); wget -c $(TINYSTM_URL)

$(BUILD_DIR)/$(TINYSTM_DIR):
	tar xzvf $(DIST_DIR)/$(TINYSTM_TARBALL) -C $(BUILD_DIR)

distclean: clean
	rm -rf $(DIST_DIR)

clean:
	rm -rf lib
	rm -rf $(BUILD_DIR)

