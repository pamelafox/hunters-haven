/**
 * Particle.io account information
 * Replace these with your particle.io device id and access token
 */
var deviceId = "";
var accessToken = "";

/**
 * Update skillName and invocationName to match the values
 * that you specify in the Alexa Skill Kit.
 * These are only used in responses from Alexa.
 */
var skillName = "Hunters Haven"
var invocationName = "Hunter's Haven";

/**
 * App ID for the skill
 * Update and use this if/when you publish your skill publicly.
 * It's ok to leave this undefined until then.
 */
var APP_ID = undefined; //replace with "amzn1.echo-sdk-ams.app.[your-unique-value-here]";

/**
 * The AlexaSkill prototype and helper functions
 * Particle is a child of AlexaSkill.
 */
var http = require('https');
var AlexaSkill = require('./AlexaSkill');
var Particle = function () {
  AlexaSkill.call(this, APP_ID);
};

// Extend AlexaSkill
Particle.prototype = Object.create(AlexaSkill.prototype);
Particle.prototype.constructor = Particle;

Particle.prototype.eventHandlers.onSessionStarted = function (sessionStartedRequest, session) {
  console.log(invocationName + "onSessionStarted requestId: " + sessionStartedRequest.requestId
         + ", sessionId: " + session.sessionId);
     // any initialization logic goes here
};

Particle.prototype.eventHandlers.onLaunch = function (launchRequest, session, response) {
  console.log(invocationName + " onLaunch requestId: " + launchRequest.requestId + ", sessionId: " + session.sessionId);
  var speechOutput = "Welcome to " + skillName + ", you can tell me to turn on or off";
  var repromptText = "You can tell me to turn on or off";
  response.ask(speechOutput, repromptText);
};

Particle.prototype.eventHandlers.onSessionEnded = function (sessionEndedRequest, session) {
  console.log(skillName + " onSessionEnded requestId: " + sessionEndedRequest.requestId
           + ", sessionId: " + session.sessionId);
  // any cleanup logic goes here
};

Particle.prototype.intentHandlers = {
  // Register custom intent handlers.
  // This simple skill only uses one, but more can be added.
  "SignAnimation": function (intent, session, response) {
    var requestURI = "/v1/devices/" + deviceId + "/alexaAnim";
     
    var animationSlot = intent.slots.animation;
    var animation = animationSlot ? intent.slots.animation.value : "";
    var speakText = "";

    // Verify that a command was specified.
    // We can extend this to prompt the user,
    // but let's keep this simple for now.
    if(animation.length > 0) {

      var postData = "args=" + animation;
      console.log("Post data = " + postData);

      makeParticleRequest(requestURI, postData, function(resp){
         response.tell("As you wish master!");
        });
    } else {
      response.tell("I don't know what to make it do!");
    }
  }, // ParticleIntent
   
  "SignOn": function (intent, session, response) {
    var requestURI = "/v1/devices/" + deviceId + "/alexaOn";
    makeParticleRequest(requestURI, "", function(resp){
         response.tell("Baby light my fire." );
        });
  },
  "SignOff": function (intent, session, response) {
    var requestURI = "/v1/devices/" + deviceId + "/alexaOff";
    makeParticleRequest(requestURI, "", function(resp){
         response.tell("I'm  too hot to handle!" );
        });
  },
  "AMAZON.HelpIntent": function (intent, session, response) {
    response.ask("You can tell " + invocationName + " to turn on or off, or to make a rainbow or fire.");
  } // HelpIntent
};

// Create the handler that responds to the Alexa Request.
exports.handler = function (event, context) {
  var particleSkill = new Particle();
  particleSkill.execute(event, context);
};

function makeParticleRequest(requestURI, postData, callback){
 	var options = {
 		hostname: "api.particle.io",
 		port: 443,
 		path: requestURI,
 		method: 'POST',
 		headers: {
 			'Content-Type': 'application/x-www-form-urlencoded',
      'Authorization': 'Bearer ' + accessToken,
 			'Accept': '*.*'
 		}
 	};

 	var req = http.request(options, function(res) {
 		console.log('STATUS: ' + res.statusCode);
 		console.log('HEADERS: ' + JSON.stringify(res.headers));

 		var body = "";

 		res.setEncoding('utf8');
 		res.on('data', function (chunk) {
 			console.log('BODY: ' + chunk);
 			body += chunk;
 		});

 		res.on('end', function () {
       callback(body);
    });
 	});

 	req.on('error', function(e) {
 		console.log('problem with request: ' + e.message);
 	});

 	// write data to request body
 	req.write(postData.toString());
 	req.end();
}
