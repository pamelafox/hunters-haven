# Hunter's Haven

An LED backlit sign with animations controlled by the Amazon Alexa Skills API and a Particle Photon.

## Demo!

Here's a video of the sign at night, with me issuing commands to it via Alexa, like "Ask Hunter's Haven to make a rainbow", "Ask Hunter's Haven to make a fire", and "Ask Hunter's Haven to turn off".

youtube here

## The Hardware

The sign is constructed from:
 
  * Laser-cut wood front, cut from birchwood ply at TechShop SF
  * Border pieces sawed from scrap wood to create the frame
  * (http://amzn.to/2nCLuiF)[Particle Photon]
  * (http://amzn.to/2n0h8Ix)[Programmable RGB LED Strip]
  * (http://amzn.to/2n0gOd6)[DC Power Female to Male Splitter], to connect Photon and LEDs both to power
  * (http://amzn.to/2mGj3jf)[DC Barrel Female Jack to Micro-USB B], to connect Photon to splitter
  * (http://amzn.to/2mGuJCt)[12V 2A DC power adapter]
  * (http://amzn.to/2nCUpAK)[Extension cable for the power adapter], long enough to reach nearest outlet
  
## The Photon Code

In our setup, the Photon is powered on all the time, but it does not turn on any animations until Alexa tells it to.

In the Photon `setup()` function, it publishes 3 functions to the world using (https://docs.particle.io/reference/firmware/photon/#particle-function-)[`Particle.function()`]: `alexaOn()`, `alexaOff()`, and `alexaAnim()`. Each of those functions changes the value of a global variable, and those variables are inspected by the `loop()` function.

When `TURNED_ON` is true, the `loop()` function uses a `switch` to call functions that send different animations to the LEDs - color spectrums (rainbow/green) and fire (which comes from (https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino)[Mark Kriegsman's example]. 

## The Alexa Code

In order to make Alexa respond to commands, we needed to create an "Alexa Skill" (or, we needed to teach Alexa a new skill, if you'd like to think of it more anthropomorphically). Each Alexa Skill consists of "intents", which are commands that Alexa understands. An intent can have "slots", which are ways of modifying the command. 

Our Alexa Skill has 3 intents: `SignAnimation`, `SignOn`, `SignOff`. The `SignAnimation` has a slot for animation type, which can be `rainbow`, `fire`, or `green`. The `config` folder contains `intent_schema.json` which describes that in JSON, and . The `config` folder also contains `sample_utterances.txt`, which contains sentences that Alexa should map to this skill. Both of those get copied/pasted into the Alexa Skill Interaction Model tab in the Amazon Developer Console.

The `lambda` folder contains the two files that are zipped and uploaded to the AWS lambda service, which basically runs standalone functions in the cloud. `AlexaSkill.js` is the official Alexa JS SDK, and `index.js` is the code specific to our Alexa Skill. That code registers three intent handlers for `SignAnimation`, `SignOn`, and `SignOff`, and each of those handlers calls the published Particle function, and then tells Alexa how to respond.

Note: We learned how to code an Alexa Skill thanks to (https://github.com/rlisle/alexaParticleBridge)[rlisle's great repo]. Read the instructions there as well, to figure out how to upload to Lambda and get set up on Alexa.
