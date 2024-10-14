import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'mqtt_service.dart';
import 'sensor_data.dart';

class AnalyticScreen extends StatefulWidget {
  @override
  _AnalyticScreenState createState() => _AnalyticScreenState();
}

class _AnalyticScreenState extends State<AnalyticScreen> {
  late MQTTService mqttService; // Declare late variable

  @override
  void initState() {
    super.initState();
    final sensorData = Provider.of<SensorData>(context, listen: false);
    mqttService = MQTTService(sensorData); // Correctly initialize
    mqttService.connect(context);
  }

  Widget _buildDataCard(String title, String value) {
    return Card(
      elevation: 3,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(12),
      ),
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Text(
              title,
              style: TextStyle(
                fontSize: 16,
                color: Colors.black54,
              ),
            ),
            SizedBox(height: 10),
            Text(
              value,
              style: TextStyle(
                fontSize: 22,
                fontWeight: FontWeight.bold,
                color: Colors.black87,
              ),
            ),
          ],
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: const Color.fromARGB(255, 217, 185, 183),
        title: Text('Robot Analytics'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Center(
          child: Consumer<SensorData>(
            builder: (context, sensorData, child) {
              return GridView.count(
                shrinkWrap: true,
                crossAxisCount: 2, // Display 2 cards per row
                crossAxisSpacing: 16.0,
                mainAxisSpacing: 16.0,
                children: [
                  _buildDataCard(
                      'Flame Sensor Status', sensorData.flame.toString()),
                  _buildDataCard('Robot State', sensorData.state),
                ],
              );
            },
          ),
        ),
      ),
    );
  }
}
