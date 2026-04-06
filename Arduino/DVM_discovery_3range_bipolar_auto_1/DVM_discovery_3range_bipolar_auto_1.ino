/*
  DVM1 3-Range Bipolar Input

  Read ADC and convert to voltage at input.
  Display Vinput at PC screen
  Change Vrange on Button press
*/

int Vrange = 0;     // 0 - 4V Range, 1 - 20V Range
float VrangeMax1V;
float VrangeMax4V;


int ADCword = 0;    // ADC value (counts)
float Vinput = 0;   // input voltage (V)
float Vref = 5.0;   // reference voltage

char strRange[20];  // range name

// Rdivider
float R1 = 825000;         
float R2 = 200000;            

// Amplifier
float R5 = 10000;
float R6 = 30100;      

// Scale and Shift network
float R7 = 10000;
float R8 = 10000;
float Kscale = R8/(R7+R8);
float Vshift = Vref*R7/(R7+R8);

// Gain
float Kdiv = 1;
float Kamp = 1;
float Ktot = 1;

// switch control
const int SW1 = 11;           // sw1 control
const int SW2 = 12;           // sw2 control
const int SW3 = 13;           // sw2 control
const int buttonPin = 4;     // pushbutton input


// the setup routine runs once *****************************
void setup() { 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // configure digital pins
  pinMode(SW1, OUTPUT);               // set as output
  pinMode(SW2, OUTPUT);               // set as output
  pinMode(SW3, OUTPUT);               // set as output
  pinMode(buttonPin, INPUT_PULLUP);   // set as input

  // default standard range (no gains)
  Vrange = 0;   

  // divider direct in
  digitalWrite(SW1, LOW);   // sw1 ON
  digitalWrite(SW2, HIGH);  // sw2 OFF
  // Amplifier unity gain
  digitalWrite(SW3, LOW);  // sw3 ON


}



// the loop routine runs continuously **********************
void loop() {

  // read analog input from ADC at pin A0
  ADCword = analogRead(A0);
  
  // calc scale and shift
  float Kscale = R8/(R7+R8);
  float Vshift = Vref*R7/(R7+R8);

  // calc total scaling
  if (Vrange == 0) {  // Standard
    Kdiv = 1;
    Kamp = 1;
    Ktot = Kdiv * Kamp * Kscale;
  }
  else if (Vrange == 1) {  // High Range
    Kdiv = R2/(R1+R2);
    Kamp = 1;
    Ktot = Kdiv * Kamp * Kscale;
  }
  else if (Vrange == 2) {  // Lo Range
    Kdiv = 1;
    Kamp = (R6/R5+1);
    Ktot = Kdiv * Kamp * Kscale;   
  }

  // convert ADC counts to input voltage
  // Vinput = ADCvalue * Vref/2^N * 1/Kdiv
  Vref = 5.0;
  Vinput = (ADCword*(Vref/1024) - Vshift)*1/(Ktot);


  // auto range
  VrangeMax1V = 1.15;
  VrangeMax4V = 4.6; 

  if (abs(Vinput) < VrangeMax1V) {  
    Vrange = 2;    // 1V Range
  }
  else if (abs(Vinput) >= VrangeMax1V && abs(Vinput) < VrangeMax4V) {
    Vrange = 0;   // 4V Range
  }
  else {
    Vrange = 1;  // 20V Range
  }
  
    // set Vrange switches at R Divider and Amp
  if (Vrange == 0) { // Standard Range
    // sw to straight in
    digitalWrite(SW1, LOW);   // sw1 ON
    digitalWrite(SW2, HIGH);  // sw2 OFF
    // Amplifier unity gain
    digitalWrite(SW3, LOW);  // sw3 ON
    // name range
    sprintf(strRange, "4V Range");
  }

  if (Vrange == 1) { // High Voltage
    // sw to divider tap
    digitalWrite(SW1, HIGH);  // sw1 OFF
    digitalWrite(SW2, LOW);   // sw2 ON
    // Amplifier unity gain
    digitalWrite(SW3, LOW);  // sw2 ON
    // name range
    sprintf(strRange, "20V Range");
  }

  if (Vrange == 2) {  // Low Voltage
    // sw to straight in
    digitalWrite(SW1, LOW);   // sw1 ON
    digitalWrite(SW2, HIGH);  // sw2 OFF
    // Amplifier Gain
    digitalWrite(SW3, HIGH);  // sw3 OFF
    // name range
    sprintf(strRange, "1V Range");
  }

  // time delay (ms) for approx 4 readings per second
  delay(250); 

  // print out the values to computer as ASCII
 
  Serial.print("Vinput,ADCword,Vrange,strRange:\t");
  if(ADCword > (1023 - 51)){     // check overange 95% to max
        Serial.print("OverRng");
  }
  else if (ADCword < 51){  // check under range 5% of max
        Serial.print("UnderRng");
  }
  else {
        Serial.print(Vinput,3);
  }
  Serial.print("\t"); 
  Serial.print(ADCword);
  Serial.print("\t"); 
  Serial.print(Vrange);
  Serial.print("\t");
  Serial.println(strRange);
}
