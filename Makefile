all: build unpack

IMAGE=pushermaps

build:
	$(CC) -o $(IMAGE) $(IMAGE).c

unpack:
	./$(IMAGE)

clean:
	-rm $(IMAGE)

