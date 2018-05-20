IR Remote with ESP8266 (Wemos D1 mini)

TODOS:
* turn on monitor bias light in the morning
* add a webpage to be able to control the light and AC from the computer 
* connect temprature sensor and backend to handle that data


Motivation: to be able to control my AC and lights when I'm away from home.
For example: to cool my room if I'm coming home on a hot summer day or to turn on the light automatically on a certain time.

# Read the AC remote codes
Parts: 
* Arduino Uno
* IR decoder TSOP38238 

1. I used [this](https://www.analysir.com/blog/2014/03/19/air-conditioners-problems-recording-long-infrared-remote-control-signals-arduino/) to read the codes of the AC's remote because some ACs have a special long code.  (included in this repo)
2. To read other codes I used [this](https://github.com/z3t0/Arduino-IRremote) with the Arduino .

I read the codes to raw form with (1), like for example the code to turn off my AC: 

```
Raw: (139) 8964, -4412, 660, -1624, 656, -536, 656, -540, 656, -540, 648, -1632, 660, -1624, 660, -532, 660, -536, 688, -1592, 688, -1596, 652, -540, 684, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -512, 684, -512, 688, -508, 688, -504, 688, -1596, 684, -1596, 684, -512, 680, -512, 692, -504, 688, -504, 688, -508, 684, -1596, 688, -508, 684, -1600, 680, -512, 680, -516, 688, -1592, 688, -508, 684, -19800, 688, -508, 684, -512, 680, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -512, 680, -516, 676, -516, 688, -508, 684, -512, 680, -1600, 684, -512, 680, -512, 688, -508, 684, -508, 688, -508, 684, -508, 684, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -512, 680, 
```

Then I needed to remove the signs, so I used this in the browser to clean the codes:

```javascript
let x = [8964, -4412, 660, -1624, 656, -536, 656, -540, 656, -540, 648, -1632, 660, -1624, 660, -532, 660, -536, 688, -1592, 688, -1596, 652, -540, 684, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -512, 684, -512, 688, -508, 688, -504, 688, -1596, 684, -1596, 684, -512, 680, -512, 692, -504, 688, -504, 688, -508, 684, -1596, 688, -508, 684, -1600, 680, -512, 680, -516, 688, -1592, 688, -508, 684, -19800, 688, -508, 684, -512, 680, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -512, 680, -516, 676, -516, 688, -508, 684, -512, 680, -1600, 684, -512, 680, -512, 688, -508, 684, -508, 688, -508, 684, -508, 684, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -516, 688, -504, 688, -508, 684, -508, 684, -512, 680, -512, 680];
let y = [];
for(let i of x){ y.push(Math.abs(i))}
JSON.stringify(y);
```

Using the above raw codes we can use the function `sendRaw` from IRRemote to send the signal to the IR leds. I use raw codes because I find them to work more reliably. 

I recommend making a shield for the Arduino if you have a lot of codes to decode instead of wiring up a breadboard every time.

![img](https://i.imgur.com/cy1pZi0.jpg)



# Send IR Signals 

Parts: 

* Wemos D1 mini
* 950nm IR leds, I used TSAL6200, IR333C and some cheap chinese ones, don't use 850nm leds 
* NPN 2N3904
* Resistors 
* Solder and prototype board/perf board
* Blynk app 

Code is included. I used Blynk to connect to the Wemos from my phone.

In Blynk make virtual pins and use them like in the code.

There's also an option in Blynk to switch a virtual pin based on location. 



This is the diagram to make the shield that will sit on the Wemos with the IR leds.

![img](https://i.imgur.com/PwBHFYo.png)

The calculation for the resistor values was:

R = (Vcc - Vled)/I = (5 - 1.4)/0.2 = 18 Ohms

collector current/Hfe = 0.1/30 = 3.3 mA

(3.3 - 0.7)/.0033 = 788 Ohms but to keep the transistor saturated I used a lower value. 

I needed the transistor driver circuit to amplify the signal because I have several leds and the AC is far from the leds, it might work for you without the transistor circuit though.

This is how it looks soldered. 

![img](https://i.imgur.com/tAC3zU7.jpg)

![img](https://i.imgur.com/ZABONEF.jpg)



That's the TO-92 case diagram for the transistor:

![img](https://i.imgur.com/KKn0yhk.png)



This is how you connect it to the leds: 

![img](https://i.imgur.com/dC13auG.jpg)



~~I'm still waiting for the sockets to arrive. I ordered these:~~

https://www.aliexpress.com/item/20-sets-2pin-2-54mm-Pitch-10cm-26AWG-Wire-Pin-Header-Housing-Terminal-Connector-Wire-Connectors/32800965681.html?spm=a2g0s.9042311.0.0.BGoQSD

This is how it looks with everthing connected: 

![img](https://i.imgur.com/5qhm5xV.jpg)

Notes:

To check if the IR led is working, view it with your phone's camera when it's transmitting. 

This circuit is easy to extend to work with more leds. 

#### IFTTT:
I used this: https://community.blynk.cc/t/how-to-integrate-blynk-and-ifttt-google-assistant/16107 to add voice control and location triggering with google asistant and IFTTT. 

In IFTTT, the if part is google assitant and location, in the then part:

url: http://{ping blynk-cloud.com}/{project key}/pin/V2
and the rest: 
![img](https://i.imgur.com/PVK1lbt.png)
