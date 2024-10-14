import 'package:flutter/material.dart';

class SensorData with ChangeNotifier {
  String _flame = 'L:0 M:0 R:0'; // Change to store the entire flame status
  String _state = '';

  String get flame => _flame;
  String get state => _state;

  // Update the entire flame status (L:M:R)
  void updateFlame(String value) {
    _flame = value;
    notifyListeners(); // Notify UI to update
  }

  void updateState(String value) {
    _state = value;
    notifyListeners(); // Notify UI to update
  }
}
