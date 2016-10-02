CONTIKI_PROJECT = SerialRelay
all: $(CONTIKI_PROJECT)

#UIP_CONF_IPV6=1

CFLAGS += -g

PROJECT_LIBRARIES += ./libs/lbutton.a

PROJECT_LIBRARIES +=  ./libs/lserial.a

#PROJECT_LIBRARIES +=  ./libs/lleds.a

PROJECT_LIBRARIES +=  ./libs/ltimer.a

CONTIKI = contiki
include $(CONTIKI)/Makefile.include

