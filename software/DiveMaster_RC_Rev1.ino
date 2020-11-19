// DiveMaster-RC by DB3JHF in 2020
//
// Rev.1:
// - Corrected Timer fuction for emergency tank drain
// - If once triggered LED status for WIS or Bat.-Low. now remains (sporadic errors included)
//
// Rev.0:
// - Initial release
//
// -----------------------------------------------------------------------------------------------------
// Creative Common License by-nc-sa
// Designed for https://www.thingiverse.com/thing:4560120 (Subnautica Cyclops Submarine by Robert Cook)

#include <Adafruit_NeoPixel.h>

// --Define I/O Pins------------------------------------------------------------------------------------

int LED_PIN1 = 3;       // Output for LEDs PL9823 (Submarine Status)
int pixels = 1;         // number of LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixels, LED_PIN1, NEO_GRB + NEO_KHZ800);

int PWM_PIN1 = 4;       // RC PWM Input for Dive Control
int DTF_PIN1 = 5;       // Switch DIVE TANK FULL
int WIS_PIN1 = 9;       // Input for water ingress sensor
int DIVE_PIN1 = 7;      // Output for Dive Relay
int SURF_PIN1 = 8;      // Output for Surface Relay
int rcCommando = 0;     // First dive commando was given (Flag for emergency tank draining)
int rcCount = 0;
int rcStack = 1;        // Status of the RC-Connection >0 = not connected, 0 = connected
int pwmStack = 0;
int dtfStatus = 1;      // Flag for Dive Tank Full (1= ok, 0= full)
int batStatus = 1;      // Flag for Battery Status (1= ok, 0= low)
int wisStatus = 1;      // Flag for Water Ingress Status (1= ok, 0= water detected)
int drainActive = 0;    // Flag for Auto Trim (drain trim tank), 0 = inactive, 1 = active
int autoSurface = 0;    // Flag for Auto-Surface (drain trim tank), 0 = inactive, 1 = active
int tankEmpty = 0;      // Flag for Auto-Surface completed
long autoTrimTimer = 0; // intitiating trim timer

// Voltage measurement
int analogPin = A0;     // A0 is input (ADC) for probing Battery DC Voltage
float vout = 0.0;
float vin = 0.0;
int values = 0;


// --User defined adjustments / parameters--------------------------------------------------------------
int debugLevel = 1;             // Show Debug Information 0=off, 1=on
int rcSamples = 8;              // Number of samples for RC-Quality
int ledBrightness = 20;         // Brightness of Status-LEDs, Value 1 to 255
float batThreshold = 8.0;       // Battery low threshold (in Volts), for 9,6V NiMH should be 8,0V
float R1 = 18000.0;             // Real value of resistor R1 (18K)
float R2 = 10000.0;             // Real value of resistor R2 (10K)
long autoTrimTimeout = 30000;   // Time to drain the trim tank completely (pump shut-off time) in ms (1000 ms = 1s)

// --Configure start parameters-------------------------------------------------------------------------
void setup() {
  pinMode(PWM_PIN1, INPUT);         // Dive Control
  pinMode(DTF_PIN1, INPUT_PULLUP);  // Dive Tank Full (DTF)
  pinMode(WIS_PIN1, INPUT_PULLUP);  // Water Ingress Sensor
  pinMode(DIVE_PIN1, OUTPUT);       // Dive Relay
  pinMode(SURF_PIN1, OUTPUT);       // Surface Relay
  pinMode(LED_PIN1, OUTPUT);        // Neopixel LEDs
  
  Serial.begin(115200);

// Initialize LEDs

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(ledBrightness);

}


// --The Main Loop-----------------------------------------------------------------------------------

void loop() {
  
// get PWM samples  (PWM1 is Dive Control, PWM2 is RC-XMTR connection)
  int pwm1 = pulseIn(PWM_PIN1, HIGH); //Read PWM1 Pulse 2nd time
  
rcCount = 0;
rcStack = 0;
int pwmStack=pwm1;

// repeat
while(rcCount < rcSamples) {
  int pwm1 = pulseIn(PWM_PIN1, HIGH); 

if (pwm1 < 1000 || pwm1 > 2100) {
  rcStack = rcStack+1;
 }
  pwmStack = pwmStack + pwm1;
  rcCount++;
 }

  
 if (debugLevel=1){
  Serial.print("PWM Signal: ");
  Serial.print(pwm1);
  Serial.println();
  Serial.print("rcStack is: ");
  Serial.print(rcStack);
  Serial.println();
  
}

// --Switch command for dive/surface relays--

digitalRead(DTF_PIN1);  //probe divetank full switch
dtfStatus = digitalRead(DTF_PIN1);
 if (debugLevel=1){
  Serial.print("Dive Tank Full Switch: ");
  Serial.print(dtfStatus);
  Serial.println();
  Serial.print("drainActive Status is: ");
  Serial.print(drainActive);
  Serial.println();
  
}

if ((pwm1 > 1600) && (rcStack == 0) && (dtfStatus == 0) && (drainActive !=1) && (autoSurface !=1)) {
  digitalWrite(SURF_PIN1, LOW);
  digitalWrite(DIVE_PIN1, HIGH);
  rcCommando = 1; // RC was already active -> LoS confirmation. Flag for tank draining in case of LoS
} 
else if ((pwm1 < 1400) && (rcStack == 0) && (tankEmpty !=1)) {
   digitalWrite(SURF_PIN1, HIGH);
  digitalWrite(DIVE_PIN1, LOW);
 
}
else if((drainActive !=1) && (autoSurface !=1)) {
  digitalWrite(DIVE_PIN1, LOW);
  digitalWrite(SURF_PIN1, LOW);
}

ledMessage();     // update Status and activate LEDs
batCheck();       // update Battery Voltage
autoDrain();      // Tank draining in case of LoS
fullDrain();      // Tank draining (auto-surface) in case of emergency (water ingress or battery empty)
checkFullDrain(); // display timer status
wisCheck();       // Check Status of water ingress sensor  
      
}

// End void loop

void ledMessage (){
// --Status LEDs-----------------------------------------------------------------------------------
// Possible LED messages (in ascending order of relevance):
// red      -> sos , los & bat -> Rescue Beacon
// blue     -> wtr : water ingress
// orange   -> bat : Battery's low
// yellow   -> los : loss of signal (radio)
// green    -> ok  : everything's fine


//sos
if ((rcStack != 0) && (batStatus == 0)) {
  //autoSurface=1;  // set Flag to active
  sos(); // activate emergency beacon
  }

// wtr
else if (wisStatus == 0) {
  fullDrainTimer(); // set millis for drain timer
  if (debugLevel=1){
  Serial.print("Status Message: Water Ingress Detected!");
  Serial.println(); 
   }
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();
  
 }

// bat
else if (batStatus == 0) {
  fullDrainTimer(); // set millis for drain timer
  if (debugLevel=1){
  Serial.print("Status Message: Battery low!");
  Serial.println(); 
   }
  strip.setPixelColor(0, 165, 255, 0);
  strip.show();
  
 }
  
// los
else if ((rcStack != 0) && (batStatus != 0)) {
  drainActive=1;  // set Flag to active
   if (debugLevel=1){
  Serial.print("Status Message: Loss of Signal");
  Serial.println(); 
   }
  strip.setPixelColor(0, 255, 255, 0);
  strip.show();
  
 }
 
// ok 
else if ((tankEmpty !=1) && (autoSurface !=1)){  // status ok, if no emergency has been detected before
  drainActive=0;  // set Flag to inactive
  if (debugLevel=1){
  Serial.print("Status Message: Everything ok");
  Serial.println(); 
   }
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();
 }
}

void sos(){  // generate emergency beacon signal
  strip.setPixelColor(0, 0, 255, 0);
  strip.show();
   if (debugLevel=1){
  Serial.print("Status Message: SOS");
  Serial.println(); 
   }
  }

 void batCheck(){
  values = analogRead(analogPin);  // read ADC input value
  vout = (values * 5.0) / 1024.0;  // calculate ADC input voltage  
  vin = vout / (R2 / (R1 + R2));   // calculate battery voltage

  if (vin < batThreshold){
  batStatus =0;
  if (debugLevel=1){
  Serial.print("Battery Status is low.");
  Serial.println(); 
   }
  }
  else {
  batStatus =1;
  if (debugLevel=1){
  Serial.print("Battery Status is good.");
  Serial.println(); 
   }
  }
  
  if (debugLevel=1){
  Serial.print("Battery Voltage is:");
  Serial.print(vin);
  Serial.println(); 
   }
  }

  void autoDrain(){ // Drain trim tank in case of loss-of-signal
    if((rcCommando ==1) && (drainActive ==1) && (autoSurface !=1)){
      
       digitalWrite(DIVE_PIN1, LOW);
       digitalWrite(SURF_PIN1, HIGH);
       
       Serial.print("LoS tank draining active");
       Serial.println(); 
    
   }      
  }
 

  void fullDrain(){ // Drain trim tank in case of water ingress or emergency
    if((autoSurface ==1) && (tankEmpty !=1)){
      
      digitalWrite(DIVE_PIN1, LOW);
       digitalWrite(SURF_PIN1, HIGH);
       if (millis() > autoTrimTimeout + autoTrimTimer ) {
       
       digitalWrite(DIVE_PIN1, LOW);
       digitalWrite(SURF_PIN1, LOW);
       tankEmpty= 1; // tank is empty, no need for another autoSurface
       }      
  }
}

 void wisCheck(){
  digitalRead(WIS_PIN1);  //probe pin of water ingress sensor
  wisStatus = digitalRead(WIS_PIN1);
  if (debugLevel=1){
  Serial.print("Water Ingress Sensor Status is:");
  Serial.print(wisStatus);
  Serial.println(); 
   }
  }

  void checkFullDrain(){
     if ((debugLevel=1) && (autoSurface==1) && (tankEmpty !=1)){
       Serial.print("Emergency tank draining active - auto surface");
       Serial.println();
       Serial.print("autoTrimTimer is: ");
       Serial.print((millis() - (autoTrimTimer)) / (1000));
       Serial.print(" sec");
       Serial.println();
   } 
  }

    void fullDrainTimer(){
       if (autoSurface !=1){
        autoTrimTimer = millis(); // set current millis for timer start
        autoSurface=1;  // set Flag to active
         }
  }
