include ../Makefile.inc

TARGET := TreatmentPlanTest.dll

all: $(TARGET)

$(TARGET): *.cs
	$(cc-command) *.cs -pkg:nunit -r:../lib/TreatmentPlan.dll -out:$(TARGET)

clean:
	$(RM) $(TARGET)

tests: $(TARGET)
	nunit-console $(TARGET)
