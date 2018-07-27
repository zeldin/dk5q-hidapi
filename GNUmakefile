
HIDAPI_IMPL = hidapi-hidraw

HIDAPI_CFLAGS := $(shell pkg-config --cflags $(HIDAPI_IMPL))
HIDAPI_LIBS := $(shell pkg-config --libs $(HIDAPI_IMPL))

CFLAGS = -O2 $(HIDAPI_CFLAGS) $(EXTRA_CFLAGS)

OBJS = dk5q.o


all : 5q-hidapi-test rainbow

clean :
	rm -f 5q-hidapi-test rainbow $(OBJS)

5q-hidapi-test : main.o $(OBJS)
	$(CC) -o $@ $^ $(HIDAPI_LIBS)

rainbow : rainbow.o $(OBJS)
	$(CC) -o $@ $^ $(HIDAPI_LIBS) -lm

