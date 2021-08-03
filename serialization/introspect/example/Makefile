all: main example_generated.h
#main: main.c example_generated.h

%_generated.h: %.fbs
	flatc --cpp  $^

