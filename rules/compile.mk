
#################################################################
# COMPILER - name of compiler
# CFLAGS - compilation flags
# DEFINE - options for preprocessor. e.g. -D_MY_OPTION_
# INCLUDES - paths to headers
#################################################################

COMPILER := g++

%.o: %.cpp
	$(COMPILER) -c -MD $(CFLAGS) $(DEFINE) $(addprefix -I, $(INCLUDES)) -o $@ $<
	
%.o: %.c
	$(COMPILER) -c -MD $(CFLAGS) $(DEFINE) $(addprefix -I, $(INCLUDES)) -o $@ $<

