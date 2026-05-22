CC := gcc
DEBUG_FLAGS:=-g
INCLUDE_FLAGS := -I../include -I../THIRD_PARTY/ubx_library -I../THIRD_PARTY/ft4222h
LINK_FLAGS := -L../lib -lubx -lm -L../THIRD_PARTY/ubx_library -L/usr/local/lib -lft4222
OPTIMIZE_FLAGS:=-O2
WARNING_FLAGS:=-Wall
CFLAGS:=$(DEBUG_FLAGS) $(INCLUDE_FLAGS) $(LINK_FLAGS) $(OPTIMIZE_FLAGS) $(WARNING_FLAGS)
LIB_FILES := \
	si5395_reg.o

all: si5395_reg.a 

si5395_reg.a: $(LIB_FILES)
	ar -cr libsi5395_reg.a $(LIB_FILES)

si5395_reg.o: si5395_reg.c
	$(CC) $(CFLAGS) -c si5395_reg.c

clean:
	rm -rf *.o *.a

