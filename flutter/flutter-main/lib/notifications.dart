import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class NotificationsScreen extends StatefulWidget {
  @override
  _NotificationsScreenState createState() => _NotificationsScreenState();
}

class _NotificationsScreenState extends State<NotificationsScreen> {
  MqttServerClient? client;
  String fireNotification = 'No fire detected';
  String location = 'No location available';

  @override
  void initState() {
    super.initState();
    connectToMqtt();
  }

  Future<void> connectToMqtt() async {
    // Set up the MQTT client
    client = MqttServerClient('broker.hivemq.com', '');
    client!.port = 1883;
    client!.logging(on: true);
    client!.keepAlivePeriod = 20;
    client!.onDisconnected = _onDisconnected;
    client!.onConnected = _onConnected;
    client!.onSubscribed = _onSubscribed;

    final connMessage = MqttConnectMessage()
        .withClientIdentifier('fire_fighter_app')
        .keepAliveFor(20)
        .startClean()
        .withWillTopic('willtopic')
        .withWillMessage('Client disconnected')
        .withWillQos(MqttQos.atLeastOnce);

    client!.connectionMessage = connMessage;

    try {
      await client!.connect();
    } catch (e) {
      print('MQTT connection error: $e');
      _disconnect();
    }

    if (client!.connectionStatus!.state == MqttConnectionState.connected) {
      print('Connected to HiveMQ');
      client!.subscribe('firefighter/notification', MqttQos.atMostOnce);
    } else {
      print('Failed to connect');
      _disconnect();
    }

    client!.updates!
        .listen((List<MqttReceivedMessage<MqttMessage?>>? messages) {
      final MqttPublishMessage recMessage =
          messages![0].payload as MqttPublishMessage;
      final String payload =
          MqttPublishPayload.bytesToStringAsString(recMessage.payload.message);

      // Process the received message and update UI
      setState(() {
        if (payload.contains('fire detected')) {
          fireNotification = 'Fire Detected!';
          location = payload.contains('location:')
              ? payload.split('location:')[1]
              : 'Unknown location';
        } else {
          fireNotification = 'No fire detected';
          location = 'No location available';
        }
      });
    });
  }

  void _onConnected() {
    print('Connected to the MQTT broker');
  }

  void _onDisconnected() {
    print('Disconnected from the MQTT broker');
  }

  void _onSubscribed(String topic) {
    print('Subscribed to topic: $topic');
  }

  void _disconnect() {
    client?.disconnect();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Notifications'),
        backgroundColor: const Color.fromARGB(255, 219, 161, 161),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                fireNotification,
                style: TextStyle(
                  fontSize: 24,
                  fontWeight: FontWeight.bold,
                  color: fireNotification == 'Fire Detected!'
                      ? Colors.red
                      : const Color.fromARGB(255, 0, 0, 0),
                ),
              ),
              const SizedBox(height: 20),
              Text(
                'Location: $location',
                style: const TextStyle(fontSize: 18),
              ),
            ],
          ),
        ),
      ),
    );
  }

  @override
  void dispose() {
    _disconnect();
    super.dispose();
  }
}
