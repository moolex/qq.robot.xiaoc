# Makefile for LibQQ

CC = gcc
LD = gcc

DEBUGDEF =# -g
CFLAGS = $(DEBUGDEF) -c -fPIC -Wall -O2 -s
LDFLAGS = -lpthread -shared -s

OBJ_PATH = obj

TARGET   = $(OBJ_PATH)/libqq.so

C_SOURCES = \
	qqsocket.c \
	qqcrypt.c \
	md5.c \
	debug.c \
	qqclient.c \
	memory.c \
	config.c \
	packetmgr.c \
	qqpacket.c \
	prot_login.c \
	protocol.c \
	prot_misc.c \
	prot_im.c \
	prot_user.c \
	list.c \
	buddy.c \
	group.c qun.c \
	prot_group.c \
	prot_qun.c \
	prot_buddy.c \
	loop.c \
	utf8.c \
	libqq.c \
	util.c \
	crc32.c \
	qqconn.c

C_OBJFILES = $(patsubst %.c,$(OBJ_PATH)/%.o,$(notdir $(C_SOURCES)))

vpath %.c $(dir $(C_SOURCES))

target: clean mkdir $(TARGET) copyfile

copyfile:
	cp $(TARGET) ../bin/libqq.so

mkdir:
	mkdir -p $(OBJ_PATH)
	
$(C_OBJFILES): $(OBJ_PATH)/%.o : %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $<  

$(TARGET): $(C_OBJFILES)
	$(LD) $(LDFLAGS)$(LFLAGS) -o $@ $^ 

clean:  
	-rm -rf $(OBJ_PATH)  
