
####################################################################
# APP_NAME - name for application
# LINK - name of linker
# OBJECTS - paths to object files
# LFLAGS - flags for linker
# LIBS - paths to other libraries, e.g. /usr/local/lib
# L_LIBS - short names of libraries for linker, e.g. -lstdc++
####################################################################

LINK := g++
MOVER := mv
BIN_PATH := ../bin

.PHONY: clean 
clean:
	rm -vf *.o
	rm -vf *.d

.PHONY: move_to_bin
move_to_bin:
	@mkdir -p $(BIN_PATH)
	$(MOVER) -f $(APP_NAME) $(BIN_PATH)


$(APP_NAME): $(OBJECTS)
	$(LINK) $^ $(LFLAGS) -o $@ $(addprefix -L,$(LIBS)) $(L_LIBS)


