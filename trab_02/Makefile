    CC     = gcc -std=c11 -g
    CFLAGS = 
    LFLAGS =

      PROG = iccTrab02
      OBJS = utils.o \
             SistemasLineares.o \
             $(PROG).o

.PHONY: limpa faxina clean distclean purge all

%.o: %.c %.h utils.h
	$(CC) -c $(CFLAGS) $<

$(PROG):  $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

doc:configura.doxygen $(files)
	doxygen configura.doxygen

clean:
	@rm -f *~ *.bak

purge:   clean
	@rm -f *.o core a.out
	@rm -f $(PROG)
