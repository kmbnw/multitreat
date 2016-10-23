include ../Makefile.inc

TARGET := $(BINDIR)/multitreat_test

all: $(TARGET)

$(TARGET):
	mkdir -p $(BINDIR)
	$(cc-command) -I ../src -L ../$(LIBDIR) -o $(TARGET) *.cpp $(LIBS) -lcppunit

clean:
	$(RM) $(TARGET)

tests: all
	LD_LIBRARY_PATH=../$(LIBDIR) ./$(TARGET)
