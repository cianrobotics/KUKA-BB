===========================================
BRAZO ROBÓTICO DE 3 GRADOS DE LIBERTAD
===========================================

DESCRIPCIÓN GENERAL
-------------------
Este código controla un brazo robótico de 3 grados de libertad (más una pinza)
utilizando servomotores. Implementa cinemática inversa para posicionar el efector
final en coordenadas cartesianas (x, y, z) y ejecuta una trayectoria de prueba
automática.

PARÁMETROS GEOMÉTRICOS DEL BRAZO (en cm)
----------------------------------------
a1 = 2.0    (offset del hombro)
a2 = 10.7   (longitud del eslabón 2 - brazo)
a3 = 16.0   (longitud del eslabón 3 - antebrazo)
d1 = 9.5    (altura de la base)

LÍMITES ARTICULARES (en grados)
-------------------------------
θ1 (base):      -90° a +90°   (giro horizontal)
θ2 (hombro):    0° a 160°     (elevación)
θ3 (codo):     -140° a +40°   (extensión del codo)

CONEXIONES DE SERVOS (pines Arduino)
------------------------------------
Pin 3  → Servo Base (θ1)
Pin 5  → Servo Hombro (θ2)
Pin 6  → Servo Codo (θ3)
Pin 9  → Servo Pinza (actuador)

FUNCIONAMIENTO
--------------
1. El brazo ejecuta una trayectoria automática de prueba:
   - La coordenada X se mueve entre 14cm y 25cm
   - La coordenada Y se mantiene fija en 0cm
   - La coordenada Z se mantiene fija en 5cm

2. En cada punto extremo (X=25cm), la pinza cambia de estado:
   - Abre (180°) cuando llega a X=25cm
   - Cierra (0°) cuando regresa a X=25cm

3. Para cada posición objetivo:
   - Se calcula la cinemática inversa (primero con configuración "codo abajo")
   - Si no es válida, se prueba con configuración "codo arriba"
   - Si es alcanzable, se mueven los servos a las posiciones calculadas

FUNCIONES IMPORTANTES
---------------------
inverseKinematics(xd, yd, zd, theta1, theta2, theta3, signo)
   Calcula los ángulos necesarios para alcanzar la posición (xd, yd, zd)
   Parámetros:
     - xd, yd, zd: coordenadas objetivo en cm
     - theta1, theta2, theta3: variables donde se devuelven los ángulos
     - signo: +1 para configuración "codo abajo", -1 para "codo arriba"
   Retorna: true si la posición es alcanzable, false en caso contrario

CONVERSIONES DE SERVOS
----------------------
Servo Base:   ángulo_servo = θ1 + 90°   (mapea -90°..+90° a 0..180)
Servo Hombro: ángulo_servo = θ2          (directo, 0°..160°)
Servo Codo:   ángulo_servo = 40° - θ3    (mapea 40°..-140° a 0..180)
Servo Pinza:  0° = cerrado, 180° = abierto

CICLO DE EJECUCIÓN
------------------
- La trayectoria se actualiza cada 50ms (paso de 0.5cm en X)
- Cuando X supera 40cm, se reinicia a 14cm
- El ciclo se repite indefinidamente

NOTAS DE IMPLEMENTACIÓN
-----------------------
- La comunicación serie (Serial.print/Serial.begin) ha sido desactivada
  para evitar conflictos con los servos
- Se utiliza la biblioteca Servo.h estándar de Arduino
- Los ángulos se limitan automáticamente dentro de rangos seguros con constrain()
- El código detecta automáticamente posiciones inalcanzables

MODIFICACIONES POSIBLES
-----------------------
- Cambiar la trayectoria modificando los valores en loop()
- Ajustar los parámetros geométricos según las dimensiones reales del brazo
- Modificar los límites articulares según las limitaciones mecánicas
- Rehabilitar la comunicación serie si es necesario (descomentar Serial.begin)

VERSIÓN
-------
Código funcional - Comunicación serie desactivada
