# firefly
attiny based 'learn to solder' kit that will synchronize blinking with other nearby units and has capacitive touch wings for changing modes

I started off just wanting to build a simple led 'learn to solder' badge, but, well, that's not very interesting is it?

Schematic and BOM - https://easyeda.com/chriscook8/firefly-14b9a966c0084510be2fbed9457bff87

![Firefly PCB](/img/firefly-PCB.png?raw=true "Firefly PCB")

## Inspriation
* https://tinkerlog.com/howto/synchronizing-firefly-how-to/
* http://www.instructables.com/id/Synchronizing-Fireflies/
* https://github.com/tinkerlog/fireflies

## Couldn't have done it without
* https://github.com/SpenceKonde/ATTinyCore -- This is the core used with the Ardiuno IDE to work with the ATtiny45
* https://www.marcelpost.com/wiki/index.php/ATtiny85_ADC --  for when I couldn't get analog readings to work, remove the abstraction
* https://github.com/cpldcpu/TinyTouchLib -- single pin captouch / qTouch
* http://tuomasnylund.fi/drupal6/content/capacitive-touch-sensing-avr-and-single-adc-pin -- how qTouch works
* https://www.sparkfun.com/tutorials/329 -- fun led fades
