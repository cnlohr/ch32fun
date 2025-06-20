This adc_polled example sets up the ADC of the CH32X035 chip to read the
Vrefint input of the ADC unit.  It sets up a single conversion and 
converts twice a second.  Note: This input doesn't work on versions of
the chip with a 0 as the 5th digit of the 9 digit batch number as they
do not have a worning input on AD channels 3, 7, 11, and 15--see Footnote
in the DS for the pin table.  Page 18 of the Version 1.7 of the CH32X035
DS.
