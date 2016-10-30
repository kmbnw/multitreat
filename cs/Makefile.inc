CC := mcs
LIBDIR=lib/
SHARED_OBJ=TreatmentPlan.dll

define cc-command
$(CC) -target:library
endef
