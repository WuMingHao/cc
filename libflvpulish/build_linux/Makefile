flvpulish := ../src/flvpulish/libflvpulish.so
rtmp      := ../src/rtmp/librtmp.a
zlib      := ../src/zlib/libzlib.a


${flvpulish} : clean  ${rtmp}
	make -C flvpulish;mkdir -p release;cp ../src/flvpulish/lib*.so release
${rtmp} : ${zlib} 
	make -C rtmp
${zlib} :
	make -C zlib

.PHONY : test                                                                                                                                                        
test : ${flvpulish}
	make -C test

.PHONY : clean                                                                                                                                                         
clean :
	rm -f release/*
	make -C flvpulish clean
	make -C rtmp clean
	make -C rtmp clean
	make -C test clean
