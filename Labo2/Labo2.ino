#include <PID_v1.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <util/delay.h>
#include <arduino-timer.h>

auto timer = timer_create_default(); //Timer para no usar delay

//Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
//Copiado de la librería, apunta a los pines que necesita
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int sensorPin = A0;
int buttonUp = 6;
int buttonDown = 7;
int relePin = 8;
int salidaPWM = 9;

int buttonUpState = 0;
int lastButtonUpState = 0;
int buttonDownState = 0;
int lastButtonDownState = 0;

//Setpoint es a donde queremos llegar
//Input es la variable que medimos
//Output es la corrección hacia el actuador
double Setpoint, Input, Output; // Respectivamente, SP, PV, Salida
//Estos K se van toqueteando para tunear la corrección
//Hay una libreria de autotuning, pero por lo pronto veamos con los default que son 2, 5, 1
double Kp = 2.0, Ki = 5.0, Kd = 1.0;

//Constructor del PID
PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);

//Funciones
void SPcontrol()
{
  //Chequea el estado de los botones para ajustar el SP
  //Sólo altera SP al apretar el botón (Button Press)
  buttonUpState = digitalRead(buttonUp);
  buttonDownState = digitalRead(buttonDown);

  if(buttonUpState != lastButtonUpState)
  {
    if(buttonUpState == HIGH)
    {
      Setpoint += 5;
      if(Setpoint > 100)
      {
        Setpoint = 100;
      }
    }
  }

  if(buttonDownState != lastButtonDownState)
  {
    if(buttonDownState == HIGH)
    {
      Setpoint -= 5;
      if(Setpoint < 0)
      {
        Setpoint = 0;
      }
    }
  }

  lastButtonUpState = buttonUpState;
  lastButtonDownState = buttonDownState;
}

bool printPV(void *)
{
  Serial.print(Input);
  return true; //true mantiene el timer activo
}

void setup()
{
  //Código PID

  //Le avisamos a Input de donde vamos a leer
  Input = analogRead(0);
  //Inicializamos Setmode. El valor acá es arbitrario.
  Setpoint = 50;
  //Inicializamos el objeto PID  
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255); // PWM va de 0 a 255, asi que queremos que el OUTPUT vaya entre esos

  //Código de display
  //Esto siempre es así por las columnas y filas del display 
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Control de nivel"); //Justo tiene 16 caracteres, entra centrado en el display
  //Setea el cursor a la columna 0, linea 1 (La de arriba es la 0, la de abajo es la 1)
  lcd.setCursor(0, 1);

  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  pinMode(relePin, OUTPUT);
  //Faltaria el de PWM

  //Código Serial
  Serial.begin(9600);
  timer.every(500, printPV); //Cada 500 ms hacete esa funcion
}

void loop() 
{
  //Código HMI
  //Todos los loops chequea cambios en los botones para ajustar el SP entre 0% y 100%
  //Sólo altera SP al apretar el botón (Button Press)
  SPcontrol();  
  
  //Código PID
  Input = analogRead(0);
  myPID.Compute();
  analogWrite(9,Output);

  if(Input > 70) //PV > 70%. Hay que hacer (Input*100/256)?
  {
    digitalWrite(relePin, HIGH); //Abro la válvula
  }
  else
  {
    digitalWrite(relePin, LOW); //Cierro la válvula
  }

  //Código LCD
  //Imprime en el display
  lcd.print(" PV " + String(Input) + "% " + "SP " + String(Setpoint) + "%"); 
  
  //Con esto vamos siempre a sobreescribir la segunda linea
  //que tiene el SP y el PV en % de nivel
  lcd.setCursor(0, 1);

  //Código Serial
  timer.tick();
  //Serial.print(Input);
  //_delay_ms(500); // Esto le mete un delay a todo
  //Esto comentado es reemplazado por el timer.every() en el setup y el timer.tick() acá
}
