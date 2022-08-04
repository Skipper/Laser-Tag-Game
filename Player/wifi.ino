#include "hardware.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <cstring>

#include <iostream>
#include <string>
#include <sstream> // this will allow you to use stringstream in your program
#include <cstdlib>

using namespace std;

/*
 * CONSTANTES
 */

//Variable para seleccionar el ejercicio a ejecutar
int ejercicio = 5;
int peticion = 0;
char ssid[] = "MOVISTARWIFI3932";                       // Network SSID (name)
char key[] = "jyma1698";                                // Network key
//char ssid[] = "TP-LINK_C062";                         // Network SSID (name)
//char key[] = "77817570";                              // Network key
int status = WL_IDLE_STATUS;                            
int sensorValue;
float luminosity;
int restar = 0;

char packetBuffer[255];
string mensajeString = ""; 
int posicionInicial = 10;
int posicionFinal = 2;

//seteamos UDP
WiFiUDP Udp;
unsigned int localPort = 8080; //4210
IPAddress ip(192, 168, 86, 141); 

// Identificado unico del usuario
char ID [] = "00"; 
char macAddress[30];
char PLAYER [30]; // ID + MAC;
char REGISTRADO [] = "REGISTRADO"; 
char LIVES [30];// 02 
char START [30];  // 02
char REDUCE [30];  // 02
char CHECKGAME [30]; // 02 
const char GAMEOVER [] = "GAME OVER"; 
const char LOSER [] = "LOSER"; 

int registrado = 0; 
// 0 = Usuario no registrado
// 1 = Usuario registrado, solicitar vidas
int vidas = 0;
int gameStatus = 0;
// 0 = Registro de usuarios
// 1 = Juego en curso
// 2 = Fin del juego

void setup() {

  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command

  
  Serial.begin(9600);
  Serial.print("\nDiagnóstico...\n");
  WiFi.printDiag(Serial);
  Serial.println();
  Serial.printf("MAC Address = %s\n", WiFi.softAPmacAddress().c_str());

  string macAddressString = WiFi.softAPmacAddress().c_str();
  strcpy(macAddress, macAddressString.c_str()); // convertir string a char
        strcpy(PLAYER, ID); // En PLAYER pasamos el valor de ID
        strcat(PLAYER, macAddress); // A PLAYER le sumamos la MAC

  // ejercicio Final
  
  //Configuramos la fotocelula como input y los leds como outputs
  pinMode(photocell, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);

  //Apagamos los leds en caso de que alguno haya quedado encendido
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);
  digitalWrite(led5, HIGH);
  digitalWrite(led6, HIGH);
  digitalWrite(led7, HIGH);

  //Iniciamos el monitor serie
//  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  //Intentamos realizar la conexion wifi
  while ( status != WL_CONNECTED) {

    digitalWrite(ledRed, HIGH);
    Serial.print("Intentando conectar a la red: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, key);

    // Esperamos 10 segundos a que se conecte
    delay(10000);
  }

  //Si se conectó
  Serial.println("Se ha establecido la conexión");

  //Nos conectamos al servidor
  Udp.begin(localPort);
}

void loop() {

  //Chequeamos qué ejercicio se está intentando ejecutar
  switch (ejercicio)  {
    
      case 5:
        
        // ejercicio Final

        while ( WiFi.status() == WL_CONNECTED && gameStatus < 2) {

          peticion++;
          //SI REGISTRADO = 0 -> REGISTRAR
          //SI REGISTRADO = 1 -> SOLICITAR VIDAS
          
          //SI VIDAS Y JUEGO COMENZADO -> JUGAR(ESPERAR DISPAROS PARA INTERACTUAR CON RPI, VERIFICAR SI NO TERMINO EL JUEGO)
          //Encendemos led verde y apagamos el rojo para indicar que está conectado
          if (gameStatus < 1) {
            digitalWrite(ledRed, LOW);
            digitalWrite(ledBlue, LOW);
            digitalWrite(ledGreen, HIGH);  
          }
          
          /* ENVÍO DE MENSAJES AL RASPBERRY PI */

          //Mensaje a enviar al servidor
          char mensaje[10];

          // Juego aun no iniciado
          if (gameStatus == 0) {

            // Solicitar registro
            if (registrado == 0 && peticion == 1){
                //strcpy(mensaje, "PLAYER 2");                                       
                strcpy(mensaje, PLAYER);        //                                
            }
            // Usuario registrado, solicitar vidas
            if (registrado == 1){
                //strcpy(mensaje, LIVES);                        
                strcpy(mensaje, LIVES);                        
            }

            // Me asignaste vidas, me quedo esperando a que arranque el juego
            if (vidas > 0){
                // Solicitar empezar juego
                strcpy(mensaje, START);                        
                
            }

            
          } else if (gameStatus == 1) {

            //checkGame(); // funcion

            // Valor max 1023 (poca luz) minimo 0 (mucha luz)
            // Luminosidad: (1023*100)/(1023 - valor leido)
            sensorValue = analogRead(photocell);
            Serial.print("Valor devuelto por el sensor: ");
            Serial.println(sensorValue);
            luminosity = ((1023-sensorValue)*100)/(1023);
            //Tambien se puede hacer con map de la siguiente forma
            //ilum = map(sensorValue,0,1023,100,0);
            
            //Utilizamos una cadena para meter lo medido en ella
            char cadena[255];
  
            //Tomamos solo un decimal de la cadena
            //sprintf(cadena,"%.1f",luminosity);
            
            if (sensorValue <= 400) {
              if (vidas > 0){
                restar=1;
              //}
              // Si usuario recibe luz directa, resta 1 vida
              strcpy(mensaje, REDUCE); 
              Serial.print("Vidas restantes: ");
              Serial.println(vidas);  
              }
             delay(3000);                     
            } else {
              // Si usuario NO recibe luz directa, el juego continua en curso
              strcpy(mensaje, CHECKGAME);
            }

          // El juego ha terminado
          } else if (gameStatus == 2) {
            Serial.print("Fin MensajeString: ");
            cout << mensajeString <<endl;          
          }

          
          


          Serial.print("Mensaje enviado: ");
          Serial.println(mensaje);
              
          // Si hay algun paquete lo leemos
          Udp.beginPacket(ip, localPort);
  
          //Enviamos el mensaje
          Udp.write(mensaje);  
          Udp.endPacket();
          //delay(5000);
          delay(1500);
          
          int packetSize = Udp.parsePacket();
        
          if (packetSize) {

            Serial.print("Paquete recibido de tamaño: ");
            Serial.println(packetSize);
            Serial.print("De ");
            IPAddress remoteIp = Udp.remoteIP();
            Serial.print(remoteIp);
            Serial.print(", puerto ");
            Serial.println(Udp.remotePort());
            
            // Leemos el paquete dentro de packetBufffer
            
            int len = Udp.read(packetBuffer, 255);
        
            if (len > 0) {
              packetBuffer[len] = 0;
            }
        
            Serial.print("Contenido: ");
            Serial.println(packetBuffer);

            mensajeString = string(packetBuffer);

            Serial.print("MensajeString: ");
            cout << mensajeString <<endl;


            
            // Si raspberry responde "Registrado" el estado del usuario cambia. userStatus
            if (mensajeString.substr(0, 10) == "REGISTRADO"){
              //10 + 2 = 12
              string mensajeSubString = mensajeString.substr(posicionInicial, posicionFinal);
              //cout<<mensajeSubString<<endl; ID extraido del buffer recibido (REGISTRADO01)

              strcpy(ID, mensajeSubString.c_str()); // convertir string a char
              //Serial.print(ID); // recibiria XX

              registrado = 1;
              Serial.println("Jugador Registrado!");
              
              // Asignando el ID a los Char
              strcpy(LIVES, ID); 
              strcat(LIVES, "VIDAS"); 

              strcpy(START, ID); 
              strcat(START, "START"); 
              
              strcpy(REDUCE, ID); 
              strcat(REDUCE, "RESTAR"); 

              strcpy(CHECKGAME, ID); 
              strcat(CHECKGAME, "CHECK GAME"); 

            }
            
            else if ((registrado == 1) && (vidas == 0) && (gameStatus == 0)) {
              // Asignación de vidas por parte del Raspberry Pi
              Serial.print("La cantidad de vidas es: ");        
              cout<< mensajeString <<endl;
              
              //vidas = mensajeString;  // convertir string a int
              //vidas = int(packetBuffer);  
              //vidas = boost::lexical_cast<int>(packetBuffer);
              // Conveción del packetBuffer a int  
              sscanf(packetBuffer, "%d", &vidas);
              //vidas = std::stoi(mensajeString);
              Serial.print("VIDAS: ");        
              Serial.println(vidas);        

            // Si Raspberry Pi responde START, los jugadores son notificados para que inicie el juego
            } else if (mensajeString == "START") {
              gameStatus = 1;
              Serial.println("El juego ha iniciado.");   
              delay(500);
              digitalWrite(ledRed, LOW);
              digitalWrite(ledGreen, LOW);
              digitalWrite(ledBlue, HIGH);
              // reemplazar por metodo encenderLed                   
            } else if (mensajeString  == GAMEOVER) {
                
                gameStatus = 2;

                if (vidas == 1) {
                  Serial.println("Has ganado la partida");                                      
                  Serial.print("Aun tenias ");                                      
                  Serial.print(vidas);                                      
                  Serial.println(" vida.");                                      
                } else if (vidas >= 2) {
                  Serial.println("Has ganado la partida");                                      
                  Serial.print("Aun tenias ");                                      
                  Serial.print(vidas);                                      
                  Serial.println(" vidas.");                                      
                } else {
                  Serial.println("Has perdido la partida ");    
                  Serial.print("Tienes ");                                      
                  Serial.print(vidas);                                      
                  Serial.println(" vidas.");    

                   // pondrán sus leds a verde y aquel jugador que ha perdido, su led parpadeará con una cadencia de un segundo en azul.
                }
                
                Serial.println("El juego ha finalizado.");                      
              
            }else if (mensajeString  == "OK") {
              if (restar == 1) {
                vidas--;
                restar = 0;
              }
            } else if (mensajeString  == LOSER) {
                
                gameStatus = 2;

                vidas--;

                Serial.println("Has perdido la partida ");    
                Serial.print("Tienes ");                                      
                Serial.print(vidas);                                      
                Serial.println(" vidas.");    

                Serial.println("El juego ha finalizado.");                      
              
            }

            //Enviamos una respuesta, a la IP y puerto desde la que recibimos el paquete
            //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            //Udp.write(mensaje);
            //Udp.endPacket();
          }
          Serial.println("________________________________________________________________________");                                      
          //delay(5000);

        } // End While

      break;
      
  } // End switch

  while (1){
    // pondrán sus leds a verde y aquel jugador que ha perdido, su led parpadeará con una cadencia de un segundo en azul.
    if (vidas > 0) {
      delay(500);
      digitalWrite(ledBlue, LOW);
      digitalWrite(ledRed, LOW);
      digitalWrite(ledGreen, HIGH);
    } else {
      delay(500);
      digitalWrite(ledBlue, LOW);
      delay(500);
      digitalWrite(ledRed, LOW);
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledBlue, HIGH);      
    }
    
  }
  
  //Chequeamos la conexión una vez cada 10 segundos
  delay(10000);
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
}
