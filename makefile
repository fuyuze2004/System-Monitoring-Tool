CC = gcc
CFLAGS = -Wall -Werror -std=c99

TARGET = monitorTool

SRCS = main.c mem_tool.c cpu_tool.c cores_tool.c signal_handler.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGET)