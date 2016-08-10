outdir := ../release

source := $(wildcard $(sourcedir)/*.cpp)
obj := $(patsubst %.cpp,%.o,$(source))
obj := $(notdir $(obj))
Link := $(patsubst %,$(OBJECT)/%,$(obj))

CC := g++
libsdir :=  -L../release
libs:= -lflvpulish
main := $(outdir)/test

$(main) : test.cpp
	$(CC) ${libsdir} ${libs} test.cpp -o ${main}

.PHONY : clean                                                                                                                                                         
clean :
	-rm -f $(main)
