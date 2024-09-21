#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Replace with your network credentials
const char* ssid = "Tomj";
const char* password = "12345678";

// MQTT Broker details
const char* mqtt_broker = "broker.emqx.io";
const char* servoOneTopic = "emqx/esp32/control_servo_one";
const char* servoTwoTopic = "emqx/esp32/control_servo_two";
const char* servoThreeTopic = "emqx/esp32/control_servo_three";
const char* servoFourTopic = "emqx/esp32/control_servo_four";
const char* mqtt_username = "emqx";
const char* mqtt_password = "public";
const int mqtt_port = 1883;

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Initialize servos
Servo servoOne;
Servo servoTwo;
Servo servoThree;
Servo servoFour;

void setup() {
    // Start the Serial Monitor
    Serial.begin(115200);

    // Attach servos to appropriate pins 
    servoOne.attach(4);
    servoTwo.attach(5);
    servoThree.attach(19);
    servoFour.attach(21);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Set MQTT server and callback
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    // Connect to the MQTT broker
    connectToMQTT();
}

void connectToMQTT() {
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            client.subscribe(servoOneTopic);
            client.subscribe(servoTwoTopic);
            client.subscribe(servoThreeTopic);
            client.subscribe(servoFourTopic);
            Serial.println("Subscribed to topics:");
            Serial.println(servoOneTopic);
            Serial.println(servoTwoTopic);
            Serial.println(servoThreeTopic);
            Serial.println(servoFourTopic);
        } else {
            Serial.print("Failed with state ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    Serial.println("-----------------------");

    int angle = message.toInt();
    Serial.print(topic);
    Serial.print(" : ");
    Serial.println(angle);

    // Control the servo based on the topic
    if (strcmp(topic, servoOneTopic) == 0) {
        servoOne.write(angle);
    } else if (strcmp(topic, servoTwoTopic) == 0) {
        servoTwo.write(angle);
    } else if (strcmp(topic, servoThreeTopic) == 0) {
        servoThree.write(angle);
    } else if (strcmp(topic, servoFourTopic) == 0) {
        servoFour.write(angle);
    }
}

void loop() {
    if (!client.connected()) {
        connectToMQTT();
    }
    client.loop();
}
