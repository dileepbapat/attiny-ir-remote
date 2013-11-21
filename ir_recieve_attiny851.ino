//based on http://nathan.chantrell.net/20121014/tinypcremote-an-attiny85-based-infrared-pc-remote-control/
//original code https://github.com/nathanchantrell/TinyPCRemote
//NEC format referred from http://www.sbprojects.com/knowledge/ir/nec.php
//Only works for NEC pulse with encoding.
#define IRpin_PIN PINB

#ifdef USING_ARDUINO_BOARD
#define IRpin 3
#else
#define IRpin 4
#endif

#define RESOLUTION 5 // time between IR measurements
#define NUMPULSES 34 // max IR pulse pairs to sample
#define MAXPULSE 5000 //(5000/RESOLUTION) // max IR pulse length, default 5 milliseconds

uint16_t pulses[NUMPULSES][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing

#define AV_PIN 0
#define SB_PIN 1
#define DVD_PIN 2
#define X_PIN 3

//int stateTV = 0;
int stateAV = 0;
int stateSB = 0;
int stateDVD = 0;
int stateX = 0;

void setup()
{

#ifdef USING_ARDUINO_BOARD
  //Serial.begin(9600);  
  //pinMode(11, INPUT); // Set IR pin as input
#else  
  pinMode(4, INPUT); // Make sure IR pin is set as input
#endif
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);
}

void loop() {
 unsigned long irCode=0;
 int d;
 
 while(listenForIR()!=NUMPULSES); // Wait for an IR Code  
 
 for (int i = 2; i < 34; i++) {
  irCode=irCode<<1;
  if((pulses[i][0] )>0&&(pulses[i][0])<(600/RESOLUTION)) {
   irCode|=0;
  } else {
   irCode|=1;
  }
  pulses[i][0]=0;
 }
 
 command(irCode);
}


void command(int c){
  switch(c) {
  case 0x5EA1906E:
    stateAV=1; stateSB=1; stateDVD=0; stateX=0; break;
  case 0x5EA1C03E:
    stateAV=1; stateSB=0; stateDVD=1; stateX=0; break;
  case 0x5EA1609E:
    stateAV=1; stateSB=0; stateDVD=0; stateX=1; break;
  case 0x5EA100FE:
    stateAV=1; stateSB=0; stateDVD=0; stateX=0; break;  
  default: break;
  }
  
  digitalWrite(AV_PIN,stateAV);
  digitalWrite(SB_PIN,stateSB);
  digitalWrite(DVD_PIN,stateDVD);
  digitalWrite(X_PIN,stateX);
  delay(10);
}

int listenForIR() {
/*
	pulse sample ||||||||||   |||||   || ||||  ||  || |||||
	               >5ms        3.5ms   bit pulses <400ms = 0 else 1
*/  
  currentpulse = 0;
  while (currentpulse<NUMPULSES) {
   unsigned int highpulse, lowpulse; // temporary storage timing
   highpulse = lowpulse = 0; // start out with no pulse length
  
   while (IRpin_PIN & _BV(IRpin)) { // got a high pulse
      highpulse++;
      delayMicroseconds(RESOLUTION);
      if (currentpulse == 1 && highpulse>4000/RESOLUTION) return currentpulse;
      if (currentpulse>1&&highpulse >1500/RESOLUTION) return currentpulse;
      if (((highpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
        return currentpulse;
      }
   }
   pulses[currentpulse][0] = highpulse;

   while (! (IRpin_PIN & _BV(IRpin))) { // got a low pulse
      lowpulse++;
      delayMicroseconds(RESOLUTION);
      //if (currentpulse>1&&lowpulse >1+500/RESOLUTION) return currentpulse;
      if (((lowpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
        return currentpulse;
      }
   }
   pulses[currentpulse][1] = lowpulse;
   currentpulse++;
  }
  return currentpulse;
}





