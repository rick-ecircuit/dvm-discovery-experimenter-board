/*
  DVM1 Basic 1

  Read ADC and convert to voltage at R divider input.
  Display Vinput at PC screen
  Read Pushbuto change Vrange if pressed

*/

int ADCword = 0;      // ADC value (counts)
float Vinput = 0;     // input voltage (V)
int Vrange = 0;       // 0 - 4V Range, 1 - 20V Range
char strRange[20];    // range name

// resistor divider
float R1 = 825000;            // Rdivider
float R2 = 200000;            
float Kdiv = 1;               // gain

// digital bits
const int SW1 = 11;           // sw1 control
const int SW2 = 12;           // sw2 control
const int buttonPin = 4;     // pushbutton input

// pushbutton press
int buttonState = 1;        // current state of the button
int lastButtonState = 1;    // previous state of the button

// the setup routine runs once *****************************
void setup() { 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // configure digital pins
  pinMode(SW1, OUTPUT);               // set as output
  pinMode(SW2, OUTPUT);               // set as output
  pinMode(buttonPin, INPUT_PULLUP);   // set as input

  // set voltage range manually with variable
  Vrange = 0;    // 0 - 4V Range, 1 - 20V Range

  // set Vrange switches at R Divider
  if (Vrange == 0) { // 4V Range
    // sw to straight in
    digitalWrite(SW1, LOW); // sw1 ON
    digitalWrite(SW2, HIGH); // sw2 OFF
  }
  else { // 25V Range
    // sw to divider tap
    digitalWrite(SW1, HIGH); // sw1 OFF
    digitalWrite(SW2, LOW); // sw2 ON
  }

}



// the loop routine runs continuously **********************
void loop() {

  // read analog input from ADC at pin A0
  ADCword = analogRead(A0);
  
  // calc input divider scaling
  if (Vrange == 0) {  // 4V Range
    Kdiv = 1;
  }
  else {              // 20V Range
    Kdiv = R2/(R1+R2);
  }

  // convert ADC counts to input voltage
  // Vinput = ADCvalue * Vref/2^N * 1/Kdiv
  Vinput = ADCword * (5.0 / 1024.0) * 1/Kdiv ;

  // read Vrange Pushbutton 
  buttonState = digitalRead(buttonPin);
  
  // check for Hi to Lo transition
  if (lastButtonState == 1 && buttonState == 0) {
    // if transition, then toggle voltage range
    if (Vrange == 0) {
      Vrange = 1; 
    }
    else {
      Vrange = 0; 
    }
  }

  // update last state
  lastButtonState = buttonState;
  
    // set Vrange switches at R Divider
  if (Vrange == 0) { // 4V Range
    // sw to straight in
    digitalWrite(SW1, LOW);   // sw1 ON
    digitalWrite(SW2, HIGH);  // sw2 OFF

    // name range
    sprintf(strRange, "4V Range");
  }
  else { // 20V Range
    // sw to divider tap
    digitalWrite(SW1, HIGH);  // sw1 OFF
    digitalWrite(SW2, LOW);   // sw2 ON

    // name range
    sprintf(strRange, "20V Range");
  }

  // time delay (ms) for approx 4 readings per second
  delay(250); 

  // print out the values to computer as ASCII
  Serial.print("Vinput,ADCword,buttonState,Vrange,strVrange:\t");
  if(ADCword < 1023){
    Serial.print(Vinput,3);
  }
  else{
    Serial.print("OverRng");
  }
  Serial.print("\t"); 
  Serial.print(ADCword);
  Serial.print("\t"); 
  Serial.print(buttonState);
  Serial.print("\t"); 
  Serial.print(Vrange);
  Serial.print("\t"); 
  Serial.println(strRange);
}
