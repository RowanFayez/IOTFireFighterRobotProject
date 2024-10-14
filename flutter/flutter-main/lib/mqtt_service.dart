import 'dart:io';
import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:provider/provider.dart';
import 'sensor_data.dart'; // Import your SensorData class

class MQTTService {
  final String broker = '3653c25602b04b5fa79a6836417632e7.s1.eu.hivemq.cloud';
  final int port = 8883;
  final String clientIdentifier = 'FlutterClient';
  final String username = 'esraa_123';
  final String password = 'Me_br123';

  late MqttServerClient client; // Declare MQTT client
  final SensorData sensorData; // Instance of SensorData

  // Topics to publish to
  final String waterPumpTopic = 'home/waterPump';
  final String servoTopic = 'home/servo';

  // Topics to subscribe to
  final String flameTopic = 'firefighter/flame';
  final String stateTopic = 'firefighter/state';

  // Constructor
  MQTTService(this.sensorData)
      : client = MqttServerClient(
            "3653c25602b04b5fa79a6836417632e7.s1.eu.hivemq.cloud",
            'FlutterClient');

  Future<void> connect(BuildContext context) async {
    // Configure the MQTT client
    client.port = port;
    client.secure = true; // Use secure connection
    client.logging(on: true); // Enable logging for debugging
    client.keepAlivePeriod = 20; // Keep alive period in seconds
    client.onDisconnected = onDisconnected; // Set disconnect handler
    client.onConnected = onConnected; // Set connect handler
    client.onSubscribed = onSubscribed; // Set subscribe handler

    // Create the connection message
    final connMessage = MqttConnectMessage()
        .withClientIdentifier(clientIdentifier)
        .authenticateAs(username, password)
        .startClean() // Clean session
        .withWillQos(MqttQos.atMostOnce); // Last Will QoS
    client.connectionMessage = connMessage;

    try {
      // Attempt to connect to the broker
      await client.connect();
      print('Connected to MQTT broker');

      // Subscribe to topics after connection
      subscribeToTopics();
    } catch (e) {
      print('Connection failed: $e');
      client.disconnect();
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text('MQTT Connection Failed: $e'),
          backgroundColor: Colors.red,
        ),
      );
    }
  }

  void subscribeToTopics() {
    // Subscribe to the specified topics
    client.subscribe(flameTopic, MqttQos.atMostOnce);
    client.subscribe(stateTopic, MqttQos.atMostOnce);

    client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;
      final String message =
          MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      print('Received message: $message from topic: ${c[0].topic}');

      // Handle messages based on topics
      switch (c[0].topic) {
        case 'firefighter/flame':
          handleFlameMessage(message);
          break;

        case 'firefighter/state':
          sensorData.updateState(message);
          break;

        default:
          print('Unknown topic: ${c[0].topic}');
          break;
      }
    });
  }

  // Handle flame message
  void handleFlameMessage(String message) {
    try {
      // Message format: "L:1 M:1 R:1"
      List<String> parts = message.split(' '); // Split the string by spaces

      // Extract the values for L, M, R by further splitting on ':'
      String lValue = parts[0].split(':')[1]; // Extract '1' from 'L:1'
      String mValue = parts[1].split(':')[1]; // Extract '1' from 'M:1'
      String rValue = parts[2].split(':')[1]; // Extract '1' from 'R:1'

      // Create a formatted string for display (or update separate values)
      String flameStatus = "L:$lValue M:$mValue R:$rValue";

      // Update sensorData (you can either update each value separately or as a single string)
      sensorData
          .updateFlame(flameStatus); // Assuming updateFlame takes a string now

      print('Flame sensor status updated: $flameStatus');
    } catch (e) {
      print('Error parsing flame message: $e');
    }
  }

  void onConnected() {
    print('Connected to MQTT broker');
    client.subscribe(flameTopic, MqttQos.atMostOnce);
    print('Subscribed to flameTopic');
  }

  void onDisconnected() {
    print('Disconnected from MQTT broker');
  }

  void onSubscribed(String topic) {
    print('Subscribed to $topic');
  }

  void disconnect() {
    client.disconnect();
    print('Disconnected from MQTT broker');
  }
}
