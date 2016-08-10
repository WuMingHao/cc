dir := ../../src/zlib
sourcedir := $(dir)
OBJECT := $(dir)


source := $(wildcard $(sourcedir)/*.c)
obj := $(patsubst %.c,%.o,$(source))
obj := $(notdir $(obj))
Link := $(patsubst %,$(OBJECT)/%,$(obj))

CC := gcc
Flag := -Os
main := $(dir)/libzlib.a

$(main) : $(obj)
	ar rcs $(main) $(Link)
$(obj) : %.o : $(sourcedir)/%.c 
	$(CC) -c $< -o $(OBJECT)/$@

.PHONY : clean                                                                                                                                                         
clean :
	-rm -rf $(OBJECT)/*.o
	-rm -f $(main)
