SUBDIRS := ./commons
SUBDIRS += $(shell find . -maxdepth 1 -regextype posix-extended -regex './[0-9]{2}.+' -type d | sort)

.PHONY : all test check $(SUBDIRS)
all : $(SUBDIRS)
test : all
check : test

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY : clean
clean : $(SUBDIRS)