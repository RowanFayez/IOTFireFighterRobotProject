import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'MQTT_service.dart';

class HomeScreen extends StatelessWidget {
  final String userName;

  const HomeScreen({super.key, required this.userName});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Home'),
        actions: [
          IconButton(
            icon: Icon(Icons.logout),
            onPressed: () async {
              SharedPreferences prefs = await SharedPreferences.getInstance();
              await prefs.remove('isLoggedIn');
              Navigator.pushReplacementNamed(context, '/signin');
            },
          ),
        ],
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: Column(
              children: [
                Text(
                  'Welcome to the future of firefighting, $userName!',
                  style: TextStyle(fontSize: 24),
                ),
                const SizedBox(height: 24),
              ],
            ),
          ),
          Expanded(
            child: SmartDevicesScreen(userName: userName),
          ),
        ],
      ),
    );
  }
}

class SmartDevicesScreen extends StatefulWidget {
  final String userName;

  const SmartDevicesScreen({super.key, required this.userName});

  @override
  State<SmartDevicesScreen> createState() => _SmartDevicesScreenState();
}

class _SmartDevicesScreenState extends State<SmartDevicesScreen> {
  bool isWaterPumpOn = false;
  double servoAngle = 90; // زاوية السيرفو
  late MQTTClientWrapper mqttClientWrapper;

  @override
  void initState() {
    super.initState();
    mqttClientWrapper = MQTTClientWrapper();
    mqttClientWrapper.prepareMqttClient();
  }

  void _publishWaterPumpState(bool isOn) {
    final topic = 'home/waterpump';
    final message = isOn ? 'on' : 'off';
    mqttClientWrapper.publishMessage(topic, message);
  }

  void _publishServoAngle() {
    final topic = 'home/servo';
    mqttClientWrapper.publishMessage(topic, servoAngle.toString());
  }

  void _onWaterPumpToggle(bool isOn) {
    setState(() {
      isWaterPumpOn = isOn;
    });
    _publishWaterPumpState(isOn);
  }

  void _onServoAngleChange(double angle) {
    setState(() {
      servoAngle = angle;
    });
    _publishServoAngle();
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const SizedBox(height: 20),
          const Text(
            'Smart Devices',
            style: TextStyle(
              fontSize: 20,
              color: Colors.grey,
            ),
          ),
          const SizedBox(height: 20),
          Expanded(
            child: Column(
              children: [
                SmartDeviceControl(
                  label: 'Water Pump',
                  isOn: isWaterPumpOn,
                  onStateChanged: _onWaterPumpToggle,
                ),
                const SizedBox(height: 20),
                ServoControl(
                  angle: servoAngle,
                  onAngleChanged: _onServoAngleChange,
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class MQTTClientWrapper {
  void prepareMqttClient() {}
  
  void publishMessage(String topic, String message) {}
}

class SmartDeviceControl extends StatelessWidget {
  final String label;
  final bool isOn;
  final Function(bool) onStateChanged;

  const SmartDeviceControl({
    super.key,
    required this.label,
    required this.isOn,
    required this.onStateChanged,
  });

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        padding: const EdgeInsets.all(20.0),
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(15),
          color: isOn ? Colors.redAccent : Colors.grey[300],
          boxShadow: [
            BoxShadow(
              color: Colors.black.withOpacity(0.3),
              blurRadius: 10.0,
              offset: Offset(0, 5),
            ),
          ],
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(
              isOn ? Icons.fire_extinguisher : Icons.fire_extinguisher_outlined,
              size: 60,
              color: Colors.white,
            ),
            const SizedBox(height: 15),
            Text(
              label,
              style: TextStyle(
                fontSize: 22,
                fontWeight: FontWeight.bold,
                color: Colors.white,
              ),
            ),
            const SizedBox(height: 10),
            Switch(
              value: isOn,
              onChanged: onStateChanged,
              activeColor: Colors.yellow,
              inactiveTrackColor: Colors.grey[400],
              inactiveThumbColor: Colors.white,
            ),
            Text(
              isOn ? "On" : "Off",
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
                color: Colors.white,
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class ServoControl extends StatelessWidget {
  final double angle;
  final Function(double) onAngleChanged;

  const ServoControl({
    super.key,
    required this.angle,
    required this.onAngleChanged,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Text(
          'Servo Control',
          style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
        ),
        Slider(
          value: angle,
          min: 0,
          max: 180,
          divisions: 180,
          label: angle.round().toString(),
          activeColor: Colors.yellow,
          inactiveColor: Colors.grey,
          onChanged: (value) {
            onAngleChanged(value);
          },
        ),
        Padding(
          padding: const EdgeInsets.all(16.0),
          child: Text('Current Angle: ${angle.round()}'),
        ),
      ],
    );
  }
}
