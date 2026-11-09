#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Maestro
// Inicialización de la pantalla LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definición de la máquina de estados del Maestro
enum EstadoJuego {
    INACTIVO,
    JUGANDO,
    VICTORIA,
    DERROTA
};

EstadoJuego estadoActual = INACTIVO;

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

void loop() {
    switch (estadoActual) {
        case INACTIVO:
            analogWrite(PIN_LED_VERDE, 0);
            
            // Verificamos solicitud de inicio (código 255) desde el Esclavo
            if (Serial.available() > 0) {
                uint8_t datoRecibido = Serial.read();
                if (datoRecibido == 255) {
                    estadoActual = JUGANDO;
                    
                    // Sorteo de la ubicación del tesoro en el rango [12, 100]
                    randomSeed(analogRead(A2)); // Lectura de pin analógico libre para semilla
                    tesoroX = random(12, 101);
                    tesoroY = random(12, 101);
                    
                    tiempoInicio = millis();
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Juego Iniciado");
                }
            }
            break;
          }
}