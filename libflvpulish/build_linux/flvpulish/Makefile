dir := ../../src/flvpulish
sourcedir := $(dir)
OBJECT := $(dir)


source := $(wildcard $(sourcedir)/*.cpp)
obj := $(patsubst %.cpp,%.o,$(source))
obj := $(notdir $(obj))
Link := $(patsubst %,$(OBJECT)/%,$(obj))

CC := g++
Flag := -Os -lpthread -Wl,
libsdir :=  -L../../src/zlib -L../../src/rtmp
headdir := -I../../src/zlib -I../../src -I../../include
libs:= -lrtmp -lzlib
main := $(dir)/libflvpulish.so

$(main) : $(obj)
	$(CC) -shared ${Flag} $(Link) ${libsdir} ${libs} -o ${main}
$(obj) : %.o : $(sourcedir)/%.cpp
	$(CC) ${headdir} -c $< -o $(OBJECT)/$@

.PHONY : clean                                                                                                                                                         
clean :
	-rm -rf $(OBJECT)/*.o
	-rm -f $(main)
