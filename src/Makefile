CC	= gcc
DS_INSTALL_DIR	= /opt/fd/lib/freeDiameter
CFLAGS	= -g -Wall -I/opt/local/include -I/opt/ats/include -I/opt/fd/include -I../include -mcx16 -fPIC -c
DS_LDFLAGS=-g -L/opt/fd/lib -lfdcore -lfdproto -lpthread -shared

SRC_DS	= ta_serv.c test_app.c ta_dict.c ta_cli_dummy.c

OBJS_DS	= $(SRC_DS:.c=.o)

DS_EXE	= ds.fdx

all: $(DS_EXE)

$(DS_EXE): $(OBJS_DS)
	$(CC) $(DS_LDFLAGS) $(OBJS_DS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o *.so *.fdx

install_app:
	install_name_tool -change libfdproto.6.dylib /opt/fd/lib/libfdproto.6.dylib /opt/fd/lib/libfdcore.6.dylib

	install_name_tool -change libfdcore.6.dylib /opt/fd/lib/libfdcore.6.dylib $(DS_EXE)
	install_name_tool -change libfdproto.6.dylib /opt/fd/lib/libfdproto.6.dylib $(DS_EXE)

install:
	cp $(DS_EXE) $(DS_INSTALL_DIR)/$(DS_EXE)
	chmod 0755 $(DS_INSTALL_DIR)/$(DS_EXE)

uninstall:
	rm $(DS_INSTALL_DIR)/$(DS_EXE)
