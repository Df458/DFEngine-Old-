PKGCONFIG=pkg-config
OS=GNU/Linux
CXXFLAGS=-I../src -std=c++11 -O3 -pipe -g -pg -Wall -Wno-literal-suffix -Wno-unused-variable -pedantic-errors `$(PKGCONFIG) --cflags glew glfw3 freetype2 lua5.1 bullet openal`
WINFLAGS=-Wl,-subsystem,windows -static-libgcc -static-libstdc++ -I/usr/i686-w64-mingw32/include/freetype2 -I/usr/i686-w64-mingw32/include/freetype2/freetype -DWINDOWS
LINUXFLAGS=
CPPLIBS=-L. -Wl,-rpath -Wl,./lib
WINLIBS=`$(PKGCONFIG) --libs glew bullet openal` -Wl,-Bstatic `$(PKGCONFIG) --static --libs gl glfw3 libpng zlib freetype2 lua5.1`
LINUXLIBS=`$(PKGCONFIG) --libs --static glew glfw3 freetype2 lua5.1 bullet openal libpng zlib`
SRCPATH=src/
OBJPATH=obj/
ENGINESRCS:=$(wildcard *.cpp)
ENGINEOBJS:=$(patsubst $(SRCPATH)%.cpp,$(OBJPATH)%.o,$(wildcard $(SRCPATH)*.cpp))
ENGINEDEPS:=$(patsubst $(SRCPATH)%.cpp,$(OBJPATH)%.depend,$(wildcard $(SRCPATH)*.cpp))
ENGINEAPP=DFEngine
ifeq ($(OS), GNU/Linux)
OSFLAGS=$(LINUXFLAGS)
OSLIBS=$(LINUXLIBS)
else ifeq ($(OS), Windows)
OSFLAGS=$(WINFLAGS)
OSLIBS=$(WINLIBS)
CXX=i686-w64-mingw32-g++
PKGCONFIG=i686-w64-mingw32-pkg-config
ENGINEAPP := $(ENGINEAPP).exe
else
$(error Unknown OS selected for output)
endif

$(OBJPATH)%.depend: $(SRCPATH)%.cpp
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CXX) -M $(CXXFLAGS) $(OSFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)%.o: $(SRCPATH)%.cpp
	@echo -e "Building \e[1;35m$<\e[0m..."
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(OSFLAGS)

all: $(ENGINEDEPS) $(ENGINESRCS) $(ENGINEOBJS)
	@echo -e "\e[0;33mBuilding main application for \e[1;35m$(OS)\e[0;33m systems...\e[0m"
	@$(CXX) -o $(ENGINEAPP) $(ENGINEOBJS) $(CXXFLAGS) $(OSFLAGS) $(CPPLIBS) $(OSLIBS)
	@echo -e "\e[0;32mBuild completed.\e[0m"

release:
	@cp -r data bin
	@cp $(ENGINEAPP) bin

-include $(patsubst $(SRCPATH)%.cpp,$(OBJPATH)%.depend,$(wildcard $(SRCPATH)*.cpp))
$(shell   mkdir -p obj)

.PHONY: clean
clean:
	@echo -e "\e[0;31mCleaning up...\e[0m"
	@$(RM) $(ENGINEOBJS)
	@$(RM) $(OBJPATH)*.depend
	@$(RM) $(OBJPATH)*.depend.*
	@$(RM) $(ENGINEAPP)
