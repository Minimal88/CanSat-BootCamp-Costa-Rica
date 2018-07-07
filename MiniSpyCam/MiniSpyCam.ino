int trig = 0;
int led = 1;

void setup() {                
  // Inicializa los pines digitales como salidas
  pinMode(led, OUTPUT);

  digitalWrite(led, HIGH);  
  
  pinMode(trig, OUTPUT);         
  digitalWrite(trig, HIGH); 
}

// Hold HIGH and trigger quick (<250ms) LOW to take a photo. Holding LOW and trigger HIGH starts/stops video recording

void loop() {
  digitalWrite(trig, LOW);   
  digitalWrite(led, LOW);
  
  delay(30000);   //Delay a modificar para cambiar a modo de foto o modo de caotura de video            

  digitalWrite(trig, HIGH);    
  digitalWrite(led, HIGH);   

  //Delay between pictures
  delay(10000);               
}

