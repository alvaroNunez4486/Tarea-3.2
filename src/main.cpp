#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Maestro
// Inicialización de la pantalla LCD I2C (Dirección 0x27, 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Variables de posición y juego
int valorX = 0;
int valorY = 0;
int tesoroX = 0;
int tesoroY = 0;

unsigned long tiempoInicio = 0;
const unsigned long TIEMPO_PARTIDA = 60000; // 60 segundos de partida
const int PIN_LED_VERDE = 9;               // Pin PWM para el temporizador en el Maestro

void setup() {
    Serial.begin(9600); // Sincronización UART
    
    lcd.init();
    lcd.backlight();
    
    pinMode(PIN_LED_VERDE, OUTPUT);
    analogWrite(PIN_LED_VERDE, 0); // Apagado en estado INACTIVO
    
    lcd.setCursor(0, 0);
    lcd.print("Inactivo: Pulse");
}
