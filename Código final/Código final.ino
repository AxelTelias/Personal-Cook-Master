//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////INCLUIDO Y CONFIGURADO DE LIBRERÍA////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SD.h>               // Libreria para controlar la SD.
#include <SPI.h>              // Libreria para establecer conexion SPI para el adaptador micro SD.
#include <Wire.h>             // Libreria para establecer comuncicacion entre el adaptador I2C y el arduino.
#include <LiquidCrystal_I2C.h>// Libreria para el display lcd con I2C.
////////////////////////////////////////CONFIGURACIÓN DEL DISPLAY/////////////////////////////////////////
LiquidCrystal_I2C lcd (0x27,16,2); // DIR, E, RW, RS, D4, D5, D6, D7.
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////DEFINIDO DE LA FUNCIÓN QUE CUMPLIRÁN LOS PINES//////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SELECCIONA EL PIN A FUNCUIONAR COMO SLAVE SELECT/////////////////////////////
#define SSpin 10              // Define el Slave Select.
///////////////////SELECCIONA LOS PINES A FUNCIONAR COMO LOS  DETERMINADOS PULSADORES/////////////////////
#define ATRAS 5               // Define el pulsador "ATRAS" en el pin 5D.
#define ADELANTE 4            // Define el pulsador "ADELANTE" en el pin 4D.
#define CONFIRMAR 3           // Define el pulsador "CONFIRMAR" en el pin 3D.
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////INICIALIZACIÓN DE VARIABLES////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////OBJETO PARA CONTROLAR EL COMPORTAMIENTO DE LA SD/////////////////////////////
File archivo;                 // Crea objeto archivo.
/////////////////////////////////VARIABLE PARA IDENTIFICAR LA ETAPA ACTUAL////////////////////////////////
int contador_adelante_atras = 1;
int etapa = 1;                // Mantiene el conteo de veces que se pulse "CONFIRMAR" para avanzar a la siguiente etapa.
/////////////////////////VARIABLES PARA ALMACENAR LAS LECTURAS LECTURAS DE LA SD//////////////////////////
char caracter;                // Variable utilizada para leer caracteres de la micro SD (de a uno a la vez).
String lectura;               // Cadena de caracteres que contendrá la lectura realizada en la función leer_tarjeta .
///////////////////////////////////VARIABLES PARA MOSTRAR EN EL DISPLAY///////////////////////////////////
String linea1;                // Cadena que se imprimirá en la primera línea del display.
String linea2;                // Cadena que se imprimirá en la segunda línea del display.
/////////////////////////////////VARIABLE PARA ALMACENAR EL NÚMERO TOTAL//////////////////////////////////
String numero_maximo;         // Registra el número total de recetas, ingredientes y pasos.
/////////////////////////////VARIABLE PARA NUMERAR EL NOMBRE DE LOS ARCHIVOS//////////////////////////////
int numero_archivo;           // Variable utilizada para concatenar direcciones que requieran numeración.
////////////////////////////////VARIABLES PARA CONCATENAR LAS DIRECCIONES/////////////////////////////////
/////////////////////////////////////////////NOMBRE DE RECETA/////////////////////////////////////////////
//////////////////////////////////////////R(numero_archivo)/N.txt/////////////////////////////////////////
String direccion_nombre_inicio; // Permite concatenar la direccion del nombre de la receta (parte inicial).
String direccion_nombre_final; // Permite concatenar la direccion del nombre de la receta (parte final).
///////////////////////////////////////////////INGREDIENTE////////////////////////////////////////////////
//////////////////////////////////////////R(numero_receta)/I(numero_archivo).txt/////////////////////////////////////////
String numero_receta;

/////////////////////////////VARIABLE PARA GUARDAR LAS DIRECCIONES DE ARCHIVOS////////////////////////////
String direccion;             // Donde se guardará la dirección del archivo para llamar desde la SD.


void setup() {
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////INICIALIZACIÓN DE COMPONENTES///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////INICALIZACIONES DE LOS PULSADORES/////////////////////////////////////
  pinMode(CONFIRMAR, INPUT);  // Inicializa el pulsador "COMFIRMAR" como entrada.
  pinMode(ADELANTE, INPUT);   // Inicializa el pulsador "ADELANTE" como entrada.
  pinMode(ATRAS, INPUT);      // Inicializa el pulsador "ATRAS" como entrada.
  //attachInterrupt(digitalPinToInterrupt(CONFIRMAR), ISR_CONFIRMAR, RISING);
  //attachInterrupt(digitalPinToInterrupt(ADELANTE), ISR_ADELANTE, RISING);
  //attachInterrupt(digitalPinToInterrupt(ATRAS), ISR_ATRAS, RISING);
/////////////////////////INICALIZACIÓN Y PREPARO PARA FUNCIONAMIENTO DE DISPLAY///////////////////////////
  lcd.init();                 // Inicializa el display.
  lcd.backlight();            // Enciende la luz tracera del display.
  lcd.clear();                // Limpia la pantalla.
  Serial.begin(9600);         // Establece la comunicacion serie con el display.
//////////////////////////////////////////INICALIZACIÓN DE LA SD//////////////////////////////////////////
  SD.begin(10);               // Inicializa la tarjeta SD.
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////COMPORTAMIENTO DEL DISPOSITIVO//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  switch(etapa){
    case 1:
      Serial.println("Bienvenida");
      bienvenida();             // Etapa de introdicción a Personal Cook Master.
    case 2:
      Serial.println("Selección de receta");
      seleccion_receta();         // Etapa de elección de la receta.
    case 3:
      Serial.println("Repaso de ingredientes");
      repaso_ingredientes();      // Etapa de repaso de ingredientes necesarios para cocinar la receta seleccionada.
    case 4:
      Serial.println("Procedimientos");
      procedimientos();           // Etapa de los procedimientos de la mreceta seleccionada.
    case 5:
      Serial.println("Final del Programa");
      final();             // Etapa de finalización de Personal Cook Master.
  }
} 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////FUNCIONES DE LAS ETAPAS DE RECETA////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////FUNCIÓN DE LA ETAPA DE BIENVENIDA/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void bienvenida(){
  do{
    linea1 = "Bienvenido a";
    linea2 = "";
    mostrar_display();
    delay(2000);
    linea1 = "Personal Cook";
    linea2 = "Master";
    mostrar_display();
    delay(2000);
    linea1 = "Presione";
    linea2 = "CONFIRMAR para";
    mostrar_display();
    delay(2000);
    linea1 = "continuar";
    linea2 = "";
    mostrar_display();
    delay(2000);    
  }while(digitalRead(CONFIRMAR) == LOW);
  pulsar_confirmar();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////FUNCIÓN DE LA ETAPA DE SELECCIÓN DE RECETA////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void seleccion_receta(){
  direccion = "ND.txt";
  buscar_numero_maximo();
  direccion_nombre_inicio = "R";
  numero_archivo = 1;
  direccion_nombre_final = "/N.txt";
  proceso_seleccion();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////FUNCIÓN DE LA ETAPA DE REPASO DE INGREDIENTES///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void repaso_ingredientes(){
  numero_receta = numero_archivo;
  direccion = "R" + String(numero_receta) + "/NI.txt";
  buscar_numero_maximo();
  direccion_nombre_inicio += String(numero_receta) + "/I";
  numero_archivo = 1;
  direccion_nombre_final = ".txt";
  proceso_seleccion();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////FUNCIÓN DE LA ETAPA DE PROCEDIMIENTOS///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void procedimientos(){
  direccion = "R" + String(numero_receta) + "/NP.txt";
  buscar_numero_maximo();
  direccion_nombre_inicio = "R" + String(numero_receta) + "/P";
  numero_archivo = 1;
  direccion_nombre_final = ".txt";
  proceso_seleccion();
}

void final(){
  linea1 = "Y la comida ya";
  linea2 = "esta lista!!";
  mostrar_display();
  delay(2000);
  linea1 = "Presione";
  linea2 = "CONFIRMAR para";
  mostrar_display();
  delay(2000);
  linea1 = "regresar";
  linea2 = "";
  mostrar_display();
  delay(2000);
  pulsar_confirmar();
  etapa = 1;
}


/*void ISR_CONFIRMAR(){
  etapa++;
}
void ISR_ADELANTE(){
  contador_adelante_atras++;
  verificar_numero_seleccion();
}

void ISR_ATRAS(){
  contador_adelante_atras--;
  verificar_numero_seleccion();
}*/

void pulsar_confirmar(){
  if(digitalRead(CONFIRMAR) == HIGH){
    while(digitalRead(CONFIRMAR) == HIGH){
      delay(500);
    }
    Serial.println("CONFIRMAR");
    etapa++;  
    while(digitalRead(CONFIRMAR) == LOW){
      delay(500);
      break;
    }  
  }
}

void pulsar_adelante(){
  if(digitalRead(ADELANTE) == HIGH){
    while(digitalRead(ADELANTE) == HIGH){
      delay(500);
    }
    Serial.println("ADELANTE");
    numero_archivo++;
    verificar_numero_seleccion();
    while(digitalRead(ADELANTE) == LOW && digitalRead(CONFIRMAR) == LOW){
      delay(500);
      break;
    }
  }
}

void pulsar_atras(){
  if(digitalRead(ATRAS) == HIGH){
    while(digitalRead(ATRAS) == HIGH){
      delay(500);
    }
    Serial.println("ATRAS");
    numero_archivo = numero_archivo - 3;
    verificar_numero_seleccion();
    while(digitalRead(ATRAS) == LOW){
      delay(500);
      break;
    }
  }
}



void mostrar_display(){
  Serial.println(linea1);
  Serial.println(linea2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(linea1);
  lcd.setCursor(0,1);
  lcd.print(linea2);
}

void scroll(){
  while (digitalRead(CONFIRMAR) == LOW && digitalRead(ADELANTE) == LOW && digitalRead(ATRAS) == LOW){
    lcd.scrollDisplayLeft();
    delay(500);
  }
}



void leer_tarjeta(){
  archivo = SD.open(direccion);
  while(archivo.available()){
    caracter = archivo.read();
    lectura += caracter;
  }
  archivo.close();  
}

void buscar_numero_maximo(){
  leer_tarjeta();
  numero_maximo = lectura;
  lectura = "";
}
void proceso_seleccion(){
  do{
    if(numero_archivo % 2 == 0){
      numero_archivo--;
    }
    direccion = direccion_nombre_inicio+String(numero_archivo)+direccion_nombre_final;
    leer_tarjeta();
    linea1 = lectura;
    lectura = "";
    numero_archivo ++;
    direccion = direccion_nombre_inicio + String(numero_archivo) + direccion_nombre_final;
    leer_tarjeta();
    linea2 = lectura;
    lectura = "";
    mostrar_display();
    while(digitalRead(CONFIRMAR) == LOW && digitalRead(ADELANTE) == LOW && digitalRead(ATRAS) == LOW){
      delay(500);
    }
    pulsar_adelante();
    pulsar_atras();
  }while(digitalRead(CONFIRMAR) == LOW);
  pulsar_confirmar();
}
    
void verificar_numero_seleccion(){

  if(numero_archivo == -1){
    numero_archivo = numero_maximo.toInt() - 1;
  }
  do{
    if(numero_archivo == numero_maximo.toInt() + 1){
      if(etapa == 2){
        linea1 = "Llegaste al";
        linea2 = "final de la";
        mostrar_display();
        delay(2000);
        linea1 = "etapa";
        linea2 = "";
        mostrar_display();
        delay(2000);        
        linea1 = "Pulsa ADELANTE O";
        linea2 = "ATRAS";
        mostrar_display();
        delay(2000); 
      }else{
        linea1 = "Llegaste al";
        linea2 = "final de la";                          
        mostrar_display();
        delay(2000);
        linea1 = "etapa";
        linea2 = "";
        mostrar_display();
        delay(2000);        
        linea1 = "Pulsa CONFIRMAR";
        linea2 = "para continuar";
        mostrar_display();
        delay(2000);     
      }
      pulsar_adelante();
      pulsar_atras();
    }
  }while(numero_archivo == numero_maximo.toInt() + 1 && digitalRead(CONFIRMAR) == LOW);
  if(numero_archivo == numero_maximo.toInt() + 2){
    numero_archivo = 1; 
  }
  if(numero_archivo % 2 == 0){
    numero_archivo ++; 
  }
  //delay(500);
}
