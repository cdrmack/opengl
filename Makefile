EXE = opengl
SOURCES = main.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CPPFLAGS = `pkg-config --cflags glew glfw3`
CFLAGS = -std=c23 -g -Wall -Werror
LDFLAGS = `pkg-config --libs glew glfw3`

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo $(EXE) build complete

$(EXE): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(EXE) $(OBJS)
