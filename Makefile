
CC := gcc
LD := gcc
LDFLAGS := -T script.ld
CFLAGS := -O2 -I$(PWD)

TARGET := jumplable
OBJS := main.o arch/text.o arch/jump_label.o linux/jump_label.o

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)
