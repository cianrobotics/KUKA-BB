//si funciona
//se retiro los serial prints y serial.begin
#include <Servo.h>

// Parámetros geométricos del brazo (cm)
const float a1 = 2.0;
const float a2 = 10.7;
const float a3 = 16.0;
const float d1 = 9.5;

// Límites articulares (grados)
const int THETA1_MIN = -90;
const int THETA1_MAX = 90;
const int THETA2_MIN = 0;
const int THETA2_MAX = 160;
const int THETA3_MIN = -140;
const int THETA3_MAX = 40;

// Pines de los servos
const int PIN_THETA1 = 3;
const int PIN_THETA2 = 5;
const int PIN_THETA3 = 6;
const int PIN_ACTUATOR = 9;

Servo servoBase;    // θ1
Servo servoHombro;  // θ2
Servo servoCode;    // θ3
Servo servoPinza;   // θ4

// Calcula la cinemática inversa para una configuración de codo (signo de sen_alpha)
// Devuelve true si los ángulos están dentro de los límites, false en caso contrario
bool inverseKinematics(float xd, float yd, float zd, float &theta1, float &theta2, float &theta3, int signo) {
    // Primer ángulo
    theta1 = atan2(yd, xd) * 180.0 / PI;
    if (theta1 < THETA1_MIN || theta1 > THETA1_MAX) return false;

    // Coordenadas en el plano del brazo
    float r = sqrt(xd * xd + yd * yd) - a1;
    float h = sqrt(r * r + (zd - d1) * (zd - d1));

    // Evitar valores fuera del alcance (singularidad)
    if (h > (a2 + a3) || h < fabs(a2 - a3)) return false;

    // Ángulo interno entre a2 y a3 (alfa)
    float cos_alpha = (a2 * a2 + a3 * a3 - h * h) / (2 * a2 * a3);
    float sen_alpha = signo * sqrt(1 - cos_alpha * cos_alpha);
    float alpha = atan2(sen_alpha, cos_alpha);

    // Tercer ángulo
    float theta3_rad = PI - alpha;
    theta3 = theta3_rad * 180.0 / PI;
    // Ajuste al rango [-180,180] por si acaso
    if (theta3 > 180) theta3 -= 360;
    if (theta3 < THETA3_MIN || theta3 > THETA3_MAX) return false;

    // Segundo ángulo
    float phi = atan2(zd - d1, r);
    float beta = atan2(a3 * sin(theta3_rad), a2 + a3 * cos(theta3_rad));
    theta2 = (phi - beta) * 180.0 / PI;
    if (theta2 < THETA2_MIN || theta2 > THETA2_MAX) return false;

    return true;
}

void setup() {
    servoBase.attach(PIN_THETA1);
    servoHombro.attach(PIN_THETA2);
    servoCode.attach(PIN_THETA3);
    servoPinza.attach(PIN_ACTUATOR);
}

void loop() {
    static int toggleState = 0;     // Estado de la pinza (0=cerrado, 1=abierto)
    static float i = 14.0;          // Variable auxiliar para la trayectoria

    float xd, yd = 0.0, zd = 5.0;   // Coordenada Y fija en 0, Z constante a 5 cm

    // Generación de la trayectoria: X va de 14 a 25 y luego regresa a 10
    if (i <= 25.0) {
        xd = i;
    } else {
        xd = 50.0 - i;
    }

    // Toggle de la pinza cuando i alcanza 25 (con tolerancia para float)
    if (fabs(i - 25.0) < 0.1) {
        if (toggleState == 0) {
            servoPinza.write(180);  // Abrir
            toggleState = 1;
        } else {
            servoPinza.write(0);    // Cerrar
            toggleState = 0;
        }
    }

    // Mostrar coordenadas objetivo

    // Calcular cinemática inversa, primero probando configuración codo abajo (sen_alpha positivo)
    float t1, t2, t3;
    bool valido = inverseKinematics(xd, yd, zd, t1, t2, t3, 1);

    // Si no es válida, probar configuración codo arriba (sen_alpha negativo)
    if (!valido) {
        valido = inverseKinematics(xd, yd, zd, t1, t2, t3, -1);
    }

    if (!valido) {
        // No hay solución válida dentro de los límites
        Serial.println(" -> Posición inalcanzable");
    } else {
        // Escribir los ángulos en los servos, aplicando las conversiones necesarias
        int servo1_ang = constrain(t1 + 90, 0, 180);
        servoBase.write(servo1_ang);

        int servo2_ang = constrain(t2, 0, 180);
        servoHombro.write(servo2_ang);

        // Mapeo de θ3: el servo espera 0-180, donde 0 corresponde a θ3 = 40°, y 180 a θ3 = -140°
        int servo3_ang = constrain(40 - t3, 0, 180);
        servoCode.write(servo3_ang);

        // Mostrar ángulos por el puerto serie
    
    }

    // Actualizar la variable de trayectoria
    i += 0.5;
    if (i > 40.0) i = 14.0;  // Reiniciar el ciclo

    delay(50);  // Pequeña pausa entre iteraciones
}