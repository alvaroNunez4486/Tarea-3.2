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

        case JUGANDO: {
            // Control del Temporizador (60s)
            unsigned long tiempoTranscurrido = millis() - tiempoInicio;
            if (tiempoTranscurrido >= TIEMPO_PARTIDA) {
                estadoActual = DERROTA;
                break;
            }
            
            // Atenuación progresiva del LED verde (100% a 0%)
            int brilloVerde = map(tiempoTranscurrido, 0, TIEMPO_PARTIDA, 255, 0);
            analogWrite(PIN_LED_VERDE, constrain(brilloVerde, 0, 255));

            // Lectura no bloqueante de coordenadas seriales
            while (Serial.available() > 0) {
                uint8_t dato = Serial.read();
                if (dato <= 100) {
                    valorX = dato; // Eje X (0-100)
                } else if (dato > 100 && dato <= 200) {
                    valorY = dato - 100; // Eje Y normalizado (0-100)
                }
            }

            // Cálculo de distancia absoluta por eje al tesoro
            int dx = (valorX < tesoroX) ? (tesoroX - valorX) : (valorX > tesoroX ? valorX - tesoroX : 0);
            int dy = (valorY < tesoroY) ? (tesoroY - valorY) : (valorY > tesoroY ? valorY - tesoroY : 0);

            // Determinación de niveles de proximidad (Tabla 3: 0=Frío, 1=Tibio, 2=Caliente, 3=Aquí)
            uint8_t nivelX = (dx > 10) ? 0 : (dx >= 5 ? 1 : (dx > 0 ? 2 : 3));
            uint8_t nivelY = (dy > 10) ? 0 : (dy >= 5 ? 1 : (dy > 0 ? 2 : 3));

            // Envío de retroalimentación de proximidad al esclavo
            Serial.write(nivelX);
            Serial.write(nivelY);

            // Actualización de coordenadas en pantalla LCD
            lcd.setCursor(0, 1);
            lcd.print("X:");
            lcd.print(valorX);
            lcd.print(" Y:");
            lcd.print(valorY);
            lcd.print("   ");

            // Condición de Victoria (coincidencia exacta en ambos ejes)
            if (dx == 0 && dy == 0) {
                estadoActual = VICTORIA;
            }
            break;
        }

        case VICTORIA:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("¡VICTORIA!");
            analogWrite(PIN_LED_VERDE, 255); // LED verde fijo al 100%
            delay(2000);
            estadoActual = INACTIVO;
            break;

        case DERROTA:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FIN DE TIEMPO");
            analogWrite(PIN_LED_VERDE, 0); // Apagado
            delay(2000);
            estadoActual = INACTIVO;
            break;
    }
}      