SRCDIR = src
BINDIR = bin
NAME = vxque
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)
CFLAGS = -g -O0 -pedantic-errors -std=c11 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
LDLIBDIR = -L/usr/lib64/mysql
LDFLAGS = -lm -lmysqlclient

all: $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDLIBDIR) $(LDFLAGS) -o $(BINDIR)/$@

clean:
	$(RM) -rf $(SRCDIR)/*.o
	$(RM) $(BINDIR)/$(NAME)

cleandist: clean