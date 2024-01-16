all:
	gcc -oFast engine.c -o engine
	./engine
	
debug:
	gcc engine.c -o engine
	./engine