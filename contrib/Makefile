
TINYSTM_VERSION=1.0.3
TINYSTM_DIR=tinySTM-$(TINYSTM_VERSION)
TINYSTM_TARBALL=$(TINYSTM_DIR).tgz
TINYSTM_URL=http://tmware.org/sites/tmware.org/files/tinySTM/$(TINYSTM_TARBALL)
TINYSTM_LIB=libitmtiny.so

GCCTRUNK_SVN=svn://gcc.gnu.org/svn/gcc/trunk
GCCTRUNK_DIR=gcc-trunk
GCCTRUNK_BIN=g++

####

BUILD_DIR=build
DIST_DIR=dist
TARGET_DIR=target

all: tinystm
.PHONY: clean distclean tinystm gcc-trunk

$(BUILD_DIR):
	mkdir $@

$(DIST_DIR):
	mkdir $@

$(TARGET_DIR):
	mkdir $@ $@/lib

#
# TinySTM
#
tinystm: $(BUILD_DIR)/.tinystm

$(BUILD_DIR)/.tinystm: $(DIST_DIR) $(BUILD_DIR)  $(TARGET_DIR) $(TARGET_DIR)/lib/$(TINYSTM_LIB)
	@touch $@
	@echo "====== TinySTM: done ======"

$(TARGET_DIR)/lib/$(TINYSTM_LIB): $(BUILD_DIR)/$(TINYSTM_DIR)
	@echo "====== TinySTM: build ======"
	make -C $(BUILD_DIR)/$(TINYSTM_DIR) abi-gcc DEFINES="-DTLS -DDESIGN=WRITE_THROUGH -DCM=CM_DELAY -DCLOCK_IN_CACHE_LINE -UEPOCH_GC -UINTERNAL_STATS -DIRREVOCABLE_ENABLED"
	@echo "====== TinySTM: install ======"
	@mkdir -p $(TARGET_DIR)/lib
	@rm -rf $@.0
	cp $(BUILD_DIR)/$(TINYSTM_DIR)/abi/gcc/libitm.so $@
	ln -s $(TINYSTM_LIB) $@.0

$(BUILD_DIR)/$(TINYSTM_DIR): $(DIST_DIR)/$(TINYSTM_TARBALL) 
	@echo "====== TinySTM: unpack ======"
	@tar xzvf $(DIST_DIR)/$(TINYSTM_TARBALL) -C $(BUILD_DIR) && touch $(BUILD_DIR)/$(TINYSTM_DIR)

$(DIST_DIR)/$(TINYSTM_TARBALL): 
	cd $(DIST_DIR) && wget -c $(TINYSTM_URL) && touch $(TINYSTM_TARBALL)

#
# GCC-Trunk
#
gcc-trunk: $(BUILD_DIR)/.gcc-trunk

$(BUILD_DIR)/.gcc-trunk: $(DIST_DIR) $(BUILD_DIR)  $(TARGET_DIR) $(TARGET_DIR)/bin/${GCCTRUNK_BIN}
	touch $@

$(TARGET_DIR)/bin/${GCCTRUNK_BIN}: $(DIST_DIR)/.gcc-trunk-svnup
	@echo "====== GCC-trunk: configure ======"
	@mkdir -p $(BUILD_DIR)/$(GCCTRUNK_DIR)
	CONFIGURE="$(realpath $(DIST_DIR))/$(GCCTRUNK_DIR)/configure" PREFIX="$(realpath $(TARGET_DIR))"; \
		cd $(BUILD_DIR)/$(GCCTRUNK_DIR) ;\
		$${CONFIGURE} \
		--enable-languages=c,c++ \
		--prefix=$${PREFIX} \
		--enable-shared \
		--with-system-zlib \
		--without-included-gettext \
		--disable-nls \
		--disable-multilib \
		--enable-plugin \
		--with-tune=native \
		--disable-bootstrap \
		
	
	@echo "====== GCC-trunk: build ======"
	@cd $(BUILD_DIR)/$(GCCTRUNK_DIR) && LIBRARY_PATH="" make
	@echo "====== GCC-trunk: install ======"
	@cd $(BUILD_DIR)/$(GCCTRUNK_DIR) && LIBRARY_PATH="" make install

$(DIST_DIR)/.gcc-trunk-svnup: $(DIST_DIR)/$(GCCTRUNK_DIR)
	@echo "====== GCC-trunk: svn update ======"
	@cd $(DIST_DIR)/$(GCCTRUNK_DIR) && svn up

$(DIST_DIR)/$(GCCTRUNK_DIR):
	@echo "====== GCC-trunk: svn checkout ======"
	@cd $(DIST_DIR) && svn co $(GCCTRUNK_SVN) $(GCCTRUNK_DIR)

distclean: clean
	rm -rf $(DIST_DIR)

clean:
	rm -rf $(TARGET_DIR)
	rm -rf $(BUILD_DIR)
