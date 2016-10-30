include ../Makefile.inc

TARGET := ../$(LIBDIR)$(SHARED_OBJ)

all: $(TARGET)

$(TARGET): *.cs
	mkdir -p ../$(LIBDIR)
	$(cc-command) *.cs -out:$(TARGET)

clean:
	$(RM) $(TARGET)
