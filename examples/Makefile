CC := g++
CC2 := gcc
INCLUDEFLAGS := -I../include -I../THIRD_PARTY/ubx_library -I../THIRD_PARTY/ft4222h 
LINKFLAGS := -L../lib -L../THIRD_PARTY/ubx_library -lubx -lm -lsi5395_reg -lft4222 -Wl,-rpath,/usr/local/lib 
CFLAGS:= -Wall
TARGETS := \
	ubx_config \
	ubx_monitor \
	ubx_set_baudrate \
	syncx5104_toggle_si5395_outputs \
	si5395_prog \
	syncbus_prog 

all: $(TARGETS)

ubx_config: ubx_config.c
	$(CC2) $(INCLUDEFLAGS) $^ $(LINKFLAGS) -o $@

ubx_monitor: ubx_monitor.c
	$(CC2) $(INCLUDEFLAGS) $^ $(LINKFLAGS) -o $@

ubx_set_baudrate: ubx_set_baudrate.c
	$(CC2) $(INCLUDEFLAGS) $^ $(LINKFLAGS) -o $@

syncx5104_toggle_si5395_outputs: syncx5104_toggle_si5395_outputs.c
	$(CC2) $(INCLUDEFLAGS) $^ $(LINKFLAGS) -o $@	

si5395_prog: si5395_prog.cpp
	$(CC) $(INCLUDEFLAGS) $^ $(LINKFLAGS) -o $@	

syncbus_prog: syncbus_prog.cpp
	$(CC) $(INCLUDEFLAGS) $^ $(LINKFLAGS) -o $@	

clean:
	rm -rf $(TARGETS) configure.log

