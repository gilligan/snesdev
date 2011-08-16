SUBDIRS = color  color2  hdma  hello_serial hello_world hello_link hires  mode7  sprites beep 

.PHONY:subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do\
	    $(MAKE) clean -C $$dir; \
	done
