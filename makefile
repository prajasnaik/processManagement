all: my_cmd

my_fm: my_cmd.c 
	gcc -Wall -o my_cmd my_cmd.c

clean:
	$(RM) my_cmd