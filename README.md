# Quern Flora
Observe your flora

## Project
* `user_drivers` - driver for spi and for adc - written by my self, because those 
    available in github/Interner were bit complex in a debug matter. 
* `probing` - example app how to use driver lib. user_drivers expose C-API which can be executed 
    directly from pythons cript, which is really nice. Future plans are to wrap channels values [0, 1023],
    into real world values.
* `observatory` - flask app and workers, nothing special. Goal is to have a few nice charts
    to track visualy environment,and filling/feeding flowers. Workes will read probes values,
    and store it sqlite3 database.


## SPI prerequsits
SPI has to be enabled in /boot/config.txt

## Notes about mcp3008

Start Header:
  * start bit 
  * bit nr. 2 - SGL/DIFF 
        - single-ended
        - differential input mode
  * 3 bits length channel [0,7]
  * pause bit A

Read: 
  * pause bit B
  * 10bit value MSB - so just read it as is (of course with shifting)

## Future plans
If I had more time, I'd implement maybe better UI and more modern solutions. Let's see.

## Author
Kajetan Brzuszczak
 