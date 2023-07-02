#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>

const char* ssid = "MiTelefono"; // Nombre de la red WiFi
const char* password = "cuyaregay"; // Contraseña de la red WiFi

const char* mqtt_server = "mqtt.eclipseprojects.io"; // Direccion del servidor MQTT
const int mqtt_port = 1883; // Puerto del servidor MQTT
const char* mqtt_topic = "my_topic"; // Tema MQTT al que se enviaran los mensajes

const int pinPIR = 25; // Pin del sensor de obstaculos
const int ledPIR = 0; // Pin del LED de estado

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
 Serial.begin(9600); // Inicia la comunicacion serial a 115200 baudios

 WiFi.begin(ssid, password); // Conectarse a la red WiFi utilizando las credenciales proporcionadas
 while (WiFi.status() != WL_CONNECTED) { // Esperar hasta que se establezca la conexion WiFi
  delay(500);
  Serial.print(".");
 }
 Serial.println();
 client.setServer(mqtt_server, mqtt_port); // Configurar el cliente MQTT con la direccion y el puerto del servidor MQTT

 pinMode(pinPIR, INPUT); // Establecer el pin del sensor de obstaculos como entrada
 pinMode(ledPIR, OUTPUT); // Establecer el pin del LED de estado como salida

 timeClient.begin(); // Inicializar el cliente NTP
 timeClient.setTimeOffset(-3600*5); // Ajustar el desplazamiento horario (en segundos)
}

bool need_to_send = false;

void loop() {
  

// Obtener la fecha y hora actual en formato de cadena
 
 if (!client.connected()) { // Verificar si el cliente MQTT esta conectado
  reconnect(); // Reconectar si no esta conectado
 }
 client.loop(); // Mantener la conexion MQTT activa
 while(true) {
  while (!Serial.available()) {
    }
    while(!timeClient.update()) { // Actualizar el cliente NTP y esperar la actualizacion
  timeClient.forceUpdate(); // Forzar una actualizacion si no se puede obtener la hora
 }
 formattedDate = timeClient.getFormattedTime(); //
  // Read the incoming data from Arduino Mega
  String dataString = Serial.readStringUntil('\n');

  // Process the data or perform any necessary actions
  // For example, you can send the data to the internet using WiFi or other protocols

  Serial.print("Received data: ");
  Serial.println(dataString);
  publishMessage(dataString.c_str()); // Enviar un mensaje al servidor MQTT con el texto "Objeto detectado"
    
 }
  

  //delay(100);

}

void reconnect() {
 while (!client.connected()) { // Mientras el cliente MQTT no este conectado
  Serial.print("Attempting MQTT connection...");
  if (client.connect("ESP32Client")) { // Intentar conectar al servidor MQTT
   Serial.println("connected");
   client.subscribe(mqtt_topic); // Suscribirse al tema MQTT especificado
  } else {
   Serial.print("failed, rc=");
   Serial.print(client.state()); // Mostrar el estado de la conexion MQTT
   Serial.println(" retrying in 5 seconds...");
   delay(5000); // Esperar 5 segundos antes de intentar reconectar
  }
 }
}

void publishMessage(const char* message) {
 int splitT = formattedDate.indexOf("T"); // Encontrar la posicion del caracter 'T' en la fecha y hora
 timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1); // Extraer la hora de la cadena de fecha y hora
 client.publish(mqtt_topic,timeStamp.c_str()); // Publicar la hora en el tema MQTT
 if (client.publish(mqtt_topic, message)) { // Enviar el mensaje al servidor MQTT
  Serial.println("Message sent");
 } else {
  Serial.println("Failed to send message");
 }
}
