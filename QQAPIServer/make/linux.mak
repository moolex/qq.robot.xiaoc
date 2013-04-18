# Makefile for MyQQ

CC = gcc
LD = gcc

DEBUGDEF =# -g
CFLAGS = $(DEBUGDEF) -c -Wall -O2 -s
LDFLAGS = -lpthread -s

OBJ_PATH = obj

TARGET   = $(OBJ_PATH)/myqq

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
	myqq.c \
	util.c \
	crc32.c \
	qqconn.c

C_OBJFILES = $(patsubst %.c,$(OBJ_PATH)/%.o,$(notdir $(C_SOURCES)))

vpath %.c $(dir $(C_SOURCES))

target: clean mkdir $(TARGET) copyfile

copyfile:
	cp $(TARGET) ../bin/myqq

mkdir:
	mkdir -p $(OBJ_PATH)
	
$(C_OBJFILES): $(OBJ_PATH)/%.o : %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $<  

$(TARGET): $(C_OBJFILES)
	$(LD) $(LDFLAGS)$(LFLAGS) -o $@ $^ 

clean:  
	-rm -rf $(OBJ_PATH)  
