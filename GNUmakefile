
HIDAPI_IMPL = hidapi-hidraw

HIDAPI_CFLAGS := $(shell pkg-config --cflags $(HIDAPI_IMPL))
HIDAPI_LIBS := $(shell pkg-config --libs $(HIDAPI_IMPL))

CFLAGS = -O2 $(HIDAPI_CFLAGS) $(EXTRA_CFLAGS)

OBJS = main.o dk5q.o


all : 5q-hidapi-test

clean :
	rm -f 5q-hidapi-test $(OBJS)

5q-hidapi-test : $(OBJS)
	$(CC) -o $@ $^ $(HIDAPI_LIBS)

