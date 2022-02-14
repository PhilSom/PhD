// Generating Interrupts 
const int PinA = 2;   // pin A motor 1
const int Pin2A = 19; // pin A motor 2
const int Pin3A = 20; // pin A motor 3
const int Pin4A =18;  // pin A motor 4

// Reading Signal
const int PinB = 3;  // Pin B motor 1  
const int Pin2B = 5; // Pin B motor 2 
const int Pin3B = 7; // Pin B motor 3
const int Pin4B = 9; // Pin B motor 4


// Last Rotary Value
int lastCount = 0;  // motor 1
int lastCount2 = 0; // motor 2
int lastCount3 = 0; // motor 3
int lastCount4 = 0; // motor 4

//Angle in Deg
float anglecalc = 0;  // Motor1
float anglecalc2 = 0; // Motor2
float anglecalc3 = 0; // Motor3
float anglecalc4 = 0; // Motor4

//Angle in Rad
float anglecalcrad = 0;  // Motor1
float anglecalc2rad = 0; // Motor2
float anglecalc3rad = 0; // Motor3
float anglecalc4rad = 0; // Motor4

//Angle difference
float anglecorrection14 =0;  // Motor 1 and 4
float anglecorrection23 =0;  // Motor 2 and 3

//Voltage difference
float voltcorr14 =0;  // Motor 1 and 4
float voltcorr23 =0;  // Motor 2 and 3

// Updating of the Interrupt Routine
volatile int virtualPosition = 0;  // Motor 1
volatile int virtualPosition2 = 0; // Motor 2
volatile int virtualPosition3 = 0; // Motor 3
volatile int virtualPosition4 = 0; // Motor 4

// Voltage Bridge Multiplier 
float voltbridgemult1 = 5;
float voltbridgemult2 = 5;
float voltbridgemult3 = 5;
float voltbridgemult4 = 5;

// Measure Voltage Motor 1
int readPin=A3; 
int readVal;    
float Vges=0;   

// Measure Voltage Motor 2
int readPin2=A4; // Motor 2
int readVal2;    // Motor 2
float Vges2=0;   // Motor 2

// Measure Voltage Motor 3
int readPin3=A5; // Motor 4
int readVal3;    // Motor 4
float Vges3=0;   // Motor 4

// Measure Voltage Motor 4
int readPin4=A6;  // Motor 4
int readVal4;     // Motor 4
float Vges4=0;    // Motor 4

// Debouncing Time of Encoder
long debouncingTime = 0.1;

// Constants for Scale Factor of ACS712 (Current Sensor)
const double scale_factor = 0.185; // 5A

// Measure Current Motor 3
float Vout3 = 0;
float Current3 = 0;
const double vRef3 = 5.00;
const double resConvert3 = 1023.;
float resADC3 = vRef3/resConvert3;
float zeroPoint3 = vRef3/2;

// Measure Current Motor 4
float Vout4 = 0;
float Current4 = 0;
const double vRef4 = 5.00;
const double resConvert4 = 1023.;
float resADC4 = vRef4/resConvert4;
float zeroPoint4 = vRef4/2;

// Calculate Power Raw Data
float calcpower14 =0;
float calcpower23 =0;


// Time in s
float timet = 0;

// Dimensionless Time
float timetau = 0;

// Average Angle
float anglemean14 =0;  // Motor 1 and 4
float anglemean23 =0;  // Motor 2 and 3


// ------------------------------------------------------------------
// INTERRUPT     ROUTINE
// ------------------------------------------------------------------

// Motor 1
void isr ()  {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  // If interrupts come faster than 0.1ms, it is assume it is a bounce 
  //can be ignored
  if (interruptTime - lastInterruptTime > debouncingTime) {
    if (digitalRead(PinB) == LOW) 
    {
      virtualPosition++ ; 
    }
    else {
      virtualPosition-- ; 
    }
    
    // Restrict value from -1000 to +1000
    virtualPosition = min(1000, max(-1000, virtualPosition));
    
    
  }
  // Keep track of time of last interrupt
  lastInterruptTime = interruptTime;
}

// Motor 2
void isr2 ()  {
  static unsigned long lastInterruptTime2 = 0;
  unsigned long interruptTime2 = millis();
  
  // If interrupts come faster than 0.1ms, it is assume it is a bounce 
  //can be ignored
  if (interruptTime2 - lastInterruptTime2 > debouncingTime) {
    if (digitalRead(Pin2B) == LOW)
    {
      virtualPosition2 ++ ; 
    }
    else {
      virtualPosition2 -- ; 
    }
    
    // Restrict value from -1000 to +1000
    virtualPosition2 = min(1000, max(-1000, virtualPosition2));
    
    
  }
  // Keep track of time of last interrupt
  lastInterruptTime2 = interruptTime2;
}

// Motor 3
void isr3 ()  {
  static unsigned long lastInterruptTime3 = 0;
  unsigned long interruptTime3 = millis();
  
  // If interrupts come faster than 0.1ms, it is assume it is a bounce 
  //can be ignored
  if (interruptTime3 - lastInterruptTime3 > debouncingTime) {
    if (digitalRead(Pin3B) == LOW)
    {
      virtualPosition3 -- ; 
    }
    else {
      virtualPosition3 ++ ; 
    }
    
    //     Restrict value from -1000 to +1000
    virtualPosition3 = min(1000, max(-1000, virtualPosition3));
    
    
  }
  //   Keep track of when we were here last (no more than every 1ms)
  lastInterruptTime3 = interruptTime3;
}

// Motor 4
void isr4 ()  {
  static unsigned long lastInterruptTime4 = 0;
  unsigned long interruptTime4 = millis();
  
  // If interrupts come faster than 0.1ms, it is assume it is a bounce 
  //can be ignored
  if (interruptTime4 - lastInterruptTime4 > debouncingTime) {
    if (digitalRead(Pin4B) == LOW)
    {
      virtualPosition4 -- ; // Could be -5 or -10
    }
    else {
      virtualPosition4 ++ ; // Could be +5 or +10
    }
    
    //     Restrict value from -1000 to +1000
    virtualPosition4 = min(1000, max(-1000, virtualPosition4));
    
    
  }
  //   Keep track of time of last interrupt
  lastInterruptTime4 = interruptTime4;
}


// ------------------------------------------------------------------
// SETUP    
// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200); //Baud Rate

  // Motor 1
  // Rotary pulses
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  // Read voltage
  pinMode(readPin,INPUT);

  //Motor 2
  // Rotary pulses
  pinMode(Pin2A, INPUT);
  pinMode(Pin2B, INPUT);
  // Read voltage
  pinMode(readPin2,INPUT);
  
  //Motor 3
  // Rotary pulses
  pinMode(Pin3A, INPUT);
  pinMode(Pin3B, INPUT);
  // Read voltage
  pinMode(readPin3,INPUT);
  
  //Motor 4
  // Rotary pulses
  pinMode(Pin4A, INPUT);
  pinMode(Pin4B, INPUT);
  // Read voltage
  pinMode(readPin4,INPUT);
  
  // Attach the routine to service the interrupts
  //Motor 1
  attachInterrupt(digitalPinToInterrupt(PinA), isr, RISING); 
  
  //Motor2
  attachInterrupt(digitalPinToInterrupt(Pin2A), isr2, RISING);
  
  //Motor3
  attachInterrupt(digitalPinToInterrupt(Pin3A), isr3, RISING);
  
  //Motor4
  attachInterrupt(digitalPinToInterrupt(Pin4A), isr4, RISING);
}

// ------------------------------------------------------------------
// MAIN LOOP    
// ------------------------------------------------------------------
void loop() {
  
  // Motor 1
  anglecalc = virtualPosition *(360. / 700. );
  anglecalcrad = virtualPosition *(2*3.14159 / 700. );
  readVal=analogRead(readPin);
  Vges=(5./1023.)*readVal*voltbridgemult1; 
  
  // Motor 2
  anglecalc2 = virtualPosition2 *(360. / 700. );
  anglecalc2rad = virtualPosition2 *(2*3.14159 / 700. );
  readVal2=analogRead(readPin2);
  Vges2=(5./1023.)*readVal2*voltbridgemult2; 
  
  // Motor 3
  anglecalc3 = virtualPosition3 *(360. / 700. );
  anglecalc3rad = virtualPosition3 *(2*3.14159 / 700. );
  readVal3=analogRead(readPin3);
  Vges3=(5./1023.)*readVal3*voltbridgemult3; 
  
  // Motor 4
  anglecalc4 = virtualPosition4 *(360. / 700. );
  anglecalc4rad = virtualPosition4 *(2*3.14159 / 700. );
  readVal4=analogRead(readPin4);
  Vges4=(5./1023.)*readVal4*voltbridgemult4; 

  // Angle Difference
  anglecorrection14 = anglecalc - anglecalc4;
  anglecorrection23 = anglecalc2 - anglecalc3;
  
  // Voltage offset
  voltcorr14 = Vges - Vges4;
  voltcorr23 = Vges2 - Vges3;
  
  // Read Current Motor 3
  Vout3 = (resADC3 * analogRead(A8));
  Current3 = (Vout3 - zeroPoint3)/ scale_factor;   
  
  // Read Current Motor 4
  Vout4 =  (resADC4 * analogRead(A9));
  Current4 = (Vout4 - zeroPoint4)/ scale_factor; 
  
  // Calculate Power
  calcpower14 = 2*voltcorr14*Current4;
  calcpower23 = 2*voltcorr23*Current3;
  
  // Time in s
  timet =  millis()*0.001; // time * 10-3
  
  //timetau
  timetau =  millis()*4.429446918*0.001; // time * omega\_0* 10-3
  
  // Average Angle 
  anglemean14 = (anglecalc + anglecalc4) / 2;
  anglemean23 = (anglecalc2 + anglecalc3) / 2;
  
  // Experimental output
  //Time scales
  Serial.print(millis());             // Time in ms
  Serial.print('\t');
  Serial.print(timet);                // Time in s
  Serial.print('\t');
  Serial.print(timetau);              // Tau
  Serial.print('\t');
  //M1 and M4
  Serial.print(anglemean14);          // Mean Angle over M1 and M4
  Serial.print('\t');
  Serial.print(anglecorrection14);    // Angle Difference over M1- M4
  Serial.print('\t');
  Serial.print(voltcorr14);           // Voltage M1 Voltage M4
  Serial.print('\t');
  Serial.print(Current4,3);           // Current 4 
  Serial.print('\t');
  Serial.print(calcpower14);          // Power calculated 
  Serial.print('\t');
  //M2 and M3
  Serial.print(anglemean23);          // Mean Angle over M2 and M3
  Serial.print('\t');
  Serial.print(anglecorrection23);    // Angle Difference over M2- M3
  Serial.print('\t');
  Serial.print(voltcorr23);           // Voltage M2 Voltage M3
  Serial.print('\t');
  Serial.print(Current3,3);           // Current 2 
  Serial.print('\t');
  Serial.print(calcpower23);          // Power calculated 
  Serial.print('\t');
  //Motor1
  Serial.print(anglecalc);            // Angle deg M1
  Serial.print('\t');
  Serial.print(anglecalcrad);         // Angle rad M1
  Serial.print('\t');
  Serial.print(Vges);                 // Voltage M1
  Serial.print('\t');
  //Motor2
  Serial.print(anglecalc2);           // Angle deg M2
  Serial.print('\t');
  Serial.print(anglecalc2rad);        // Angle rad M2
  Serial.print('\t');
  Serial.print(Vges2);                // Voltage M2
  Serial.print('\t');
  //Motor3
  Serial.print(anglecalc3);           // Angle deg M3
  Serial.print('\t');
  Serial.print(anglecalc3rad);        // Angle rad M3
  Serial.print('\t');
  Serial.print(Vges3);                // Voltage M3
  Serial.print('\t');
  Serial.print(Current3,3);           // Voltage M3
  Serial.print('\t');
  //Motor4
  Serial.print(anglecalc4);           // Angle deg M4
  Serial.print('\t');
  Serial.print(anglecalc4rad);        // Angle rad M4
  Serial.print('\t');
  Serial.print(Vges4);                // Voltage M4
  Serial.print('\t');
  Serial.println(Current4,3);         // Voltage M4
  
  delay(10);
}
