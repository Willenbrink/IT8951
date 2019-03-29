SRCS=AsciiLib.c bmp.c bcm.c IT.c cmd.c interface.c
CC=gcc
TARGET=IT8951

$(TARGET):$(SRCS)
	$(CC) -g -fPIC -shared -Wall $(SRCS) -o $(TARGET) -lbcm2835

clean:
	rm -f $(TARGET)
