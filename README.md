# door-rtos
Real time operating system written a looong time ago for fun
This is the ece354 rtos

Two ways to test this os :
1. Classical
	-> ensure that the file kernel/init.c is the same that init/standard_init.c
	-> type make clean and make test

2. Automated testing
	-> ensure that the file kernel/init.c is the same that init/irene_init.c
	-> type make clean and make testtest

Enjoy !

-
Some features:
	-> Accessing (dequeueing and enqueuing) a queue is a constant complexity. But
           the set_priority is more complex. We assume here that accessing a
           queue happens more frequently that a  set_priority
	-> The registers are saved on the local stack of the process, which is more
           efficient.
	-> We run users processes in user mode.
	-> Multi arch : all the assembly code is in  arch/ and called by preprocessor.
