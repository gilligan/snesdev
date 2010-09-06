SUBDIRS = color  color2  hdma  hello_world  hires  mode7  sprites

.PHONY:subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do\
	    $(MAKE) clean -C $$dir; \
	done
