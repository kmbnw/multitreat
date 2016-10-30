CC := mcs

TARGET := TreatmentPlan
NUNIT := /usr/lib/cli/nunit.framework-2.6.3

all: $(TARGET).dll

$(TARGET).dll: $(TARGET).cs
	$(CC) $(TARGET).cs -pkg:nunit -out:$(TARGET).dll -target:library

clean:
	$(RM) $(TARGET).dll
