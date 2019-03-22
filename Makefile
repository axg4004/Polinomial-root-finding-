SRCHW =  roots.c
SRCPOLY = poly.c
PROG = roots

SIM  = simple.txt
POLY  = polynomials.txt
OUT = out.txt
MEM = mem.txt

CFLG = gcc -Wall -std=c99 -pedantic -g -O0
VFLG = valgrind --tool=memcheck --leak-check=yes

.SILENT:
all: $(PROG) 
$(PROG) : $(SRCHW)
	@echo "Compiling roots.c"
	$(CFLG) $(SRCHW) $(SRCPOLY) -o $(PROG) -lm

.PHONY: roots
roots: all
	@echo "Testing roots"
	./$(PROG) -i $(POLY) -v > $(OUT)
	
.PHONY: mem
mem: all
	@echo "Running Valgrind " >> $(OUT)
	$(VFLG) ./$(PROG) -i $(POLY) > $(MEM) 2>&1
	
.PHONY: help
help:
	@echo "All makefile targets: all, roots, mem, clean, help"

.PHONY: clean 
clean:
	@echo "Removing temp file"
	rm -f $(PROG)
	rm -f $(OUT)
	rm -f $(MEM)

