include ../Makefile.inc

TARGET := ../$(LIBDIR)$(SHARED_OBJ)

all: $(TARGET)

$(TARGET): *.cpp *.h
	mkdir -p ../$(LIBDIR)
	$(cc-command) $(SHAREDCFLAGS) -o $(TARGET) *.cpp

clean:
	$(RM) $(TARGET)
