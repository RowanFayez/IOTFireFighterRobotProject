import 'package:flutter/material.dart';

class SensorData with ChangeNotifier {
  double _flame = 0.0;
  double _smoke = 0.0;
  String _state = '';

  double get flame => _flame;
  double get smoke => _smoke;
  String get state => _state;

  void updateFlame(double value) {
    _flame = value;
    notifyListeners();
  }

  void updateSmoke(double value) {
    _smoke = value;
    notifyListeners();
  }

  void updateState(String value) {
    _state = value;
    notifyListeners();
  }

  void setStateMessage(String message) {}

  void setSmokeMessage(String message) {}

  void setFlameMessage(String message) {}
}