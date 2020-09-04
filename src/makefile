IDIR =../../SDL2-2.0.12/mingw/include
CC=emcc
CFLAGS = -s -I$(IDIR)

ODIR=obj
LDIR =../../SDL2-2.0.12/mingw/lib

LIBS=-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file ..\assets

SOURCES = main.cpp resource_manager.cpp utility.cpp
SOURCES += ./scene/game.cpp ./scene/menu.cpp
SOURCES += ./sprite/sprite_group.cpp ./sprite/bullet.cpp
SOURCES += ./3rd_party/imgui/imgui_impl_sdl.cpp ./3rd_party/imgui/imgui_sdl.cpp 
SOURCES += ./3rd_party/imgui/imgui.cpp ./3rd_party/imgui/imgui_draw.cpp ./3rd_party/imgui/imgui_widgets.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

$(ODIR)/%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: ./3rd_party/imgui/%.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: ./sprite/%.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: ./scene/%.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

olc_code_jam_2020: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -o ../html/$@.html $(LIBS) 

.PHONY: clean

clean:
	rm -f $(ODIR)\*.o *~ core $(INCDIR)\*~ 
	
cool:
	echo "stuff works?"
	echo "$(OBJS)"