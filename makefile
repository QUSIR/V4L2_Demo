Target=main
Object=main.c V4L.c FB.c
CC=arm-linux-gcc
Compile=$(CC)
main:
	$(Compile) $(Object) -o $(Target)
	cp main /mnt/data
clean:
	rm -f main 
