include ../Makefile.inc

TARGET := ../lib/TreatmentPlanTest.dll

all: $(TARGET)

$(TARGET): *.cs
	$(cc-command) *.cs -pkg:nunit -r:../lib/$(SHARED_OBJ) -out:$(TARGET)

clean:
	$(RM) $(TARGET)

tests: $(TARGET)
	nunit-console $(TARGET)
