all : flash

TARGET:=tim1_phase_shifted_pwm

# This tool requires you to also specify the specific package of the 003 you
# using.  See funconfig.h

TARGET_MCU?=CH32V003

include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

# force PD7 to be GPIO instead of NRST
setup: 
	$(MINICHLINK)/minichlink -D
