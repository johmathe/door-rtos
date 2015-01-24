
# Makefile
# Door OS. Pooyan, Johan, Gohulan & Patrick 

CC=m68k-elf-gcc
CXX=m68k-elf-g++
CFLAGS= -Wall -m5200 -pipe -nostdlib -I./include -I.
LD=m68k-elf-gcc
AS=m68k-elf-as
AR=m68k-elf-ar
ARFLAGS=
OBJCPY=m68k-elf-objcopy
ASM=boot/start.s
LDFLAGS = -Tboot/mcf5307.ld -Wl,-Map=rtos.map

# Note, GCC builds things in order, it's important to put yhe
#  ASM first, so that it is located at the beginning of our program.
ARCH=m68k-coldfire5307

TESTING_OBJS = \
testing/rtxeval_mid1.o \
testing/rtxeval_mid2.o \
testing/rtxeval_mid3.o \
testing/rtxeval_mid4.o 

THUMB_SRC = \
kernel/init.c \
kernel/string.c \
kernel/queue.c \
kernel/swi.c \
kernel/task.c \
kernel/rtos.c \
kernel/primitives.c \
kernel/stdio.c \
kernel/memory_queue.c \
kernel/msg_queue.c \
kernel/timer_handler.c \
kernel/uart_handler.c \
process/kcd_process.c \
process/timer_i_process.c \
process/uart_i_process.c \
process/crt_process.c \
process/set_priority_process.c \
process/wall_clock_process.c \
process/null_process.c \
process/process_a1.c \
process/process_b1.c \
process/process_c1.c \
process/process_a2.c \
process/process_b2.c \
process/process_c2.c \
process/process_a3.c

THUMB_OBJ = $(THUMB_SRC:.c=.o)
THUMB_TESTING = $(THUMB_SRC:.c=_t.o)
THUMB_ASM = $(THUMB_SRC:.c=.s)


#all: boot/start.o $(THUMB_OBJ) $(THUMB_ASM) rtos.s19

all-test: boot/start.o $(THUMB_TESTING) rtos_autotest.s19
all: boot/start.o $(THUMB_OBJ) rtos.s19



rtos.s19: boot/start.o $(THUMB_OBJ) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o rtos.o boot/start.o $(ASM) $(THUMB_OBJ)  
	$(OBJCPY) --output-format=srec rtos.o  $@ 

rtos_autotest.s19: boot/start.o $(THUMB_TESTING)
	$(CC) $(CFLAGS) $(LDFLAGS) -o rtos_autotest.o boot/start.o $(THUMB_TESTING) $(TESTING_OBJS)
	$(OBJCPY) --output-format=srec rtos_autotest.o  $@ 




$(THUMB_ASM): %.s : %.c include/*.h
	$(CC) $(CFLAGS) -S $< -o $@ 
$(THUMB_OBJ) : %.o : %.c include/*.h
	$(CC) $(CFLAGS) -c $< -o $@
$(THUMB_TESTING) : %_t.o : %.c include/*.h
	$(CC) $(CFLAGS) -D_RTX_EVAL_ -c $< -o $@


boot/start.o: $(ASM)
	$(CC) $(CFLAGS) -c $< -o $@



upload: rtos.s19
	scp $< jmathe@eceunix:
	ssh jmathe@eceunix /opt/gcc-mcf5307/bin/cfcp /home/jmathe/rtos.s19 e354g03.s19 

autotest_upload: rtos_autotest.s19
	scp $< jmathe@eceunix:
	ssh jmathe@eceunix /opt/gcc-mcf5307/bin/cfcp /home/jmathe/rtos_autotest.s19 e354g03.s19



test: upload
	utils/test_rt.exp

testtest : autotest_upload
	utils/test_rt.exp
clean:
	rm -f *.s19 kernel/*.s *.o *.map arch/$(ARCH)/*.o  arch/$(ARCH)/*.s kernel/*.o process/*.s process/*.o
