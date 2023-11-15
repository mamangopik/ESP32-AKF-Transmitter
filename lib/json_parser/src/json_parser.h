#include <global_variable.h>
// Extract individual values by searching for keys
String ssid, password, broker, topic;

void parseJsonData(const String &jsonData) {
  // Find the start and end of the JSON object
  int start = jsonData.indexOf('{');
  int end = jsonData.lastIndexOf('}');
  
  if (start == -1 || end == -1 || end <= start) {
    // JSON object not found or invalid
    Serial.println("Invalid JSON data");
    return;
  }

  String jsonContent = jsonData.substring(start, end + 1);

  int ssidStart = jsonContent.indexOf("\"ssid\"");
  if (ssidStart != -1) {
    ssidStart = jsonContent.indexOf('"', ssidStart + 6); // Start after "ssid": "
    int ssidEnd = jsonContent.indexOf('"', ssidStart + 1);
    JSONssid = jsonContent.substring(ssidStart + 1, ssidEnd);
  }

  int passwordStart = jsonContent.indexOf("\"password\"");
  if (passwordStart != -1) {
    passwordStart = jsonContent.indexOf('"', passwordStart + 10); // Start after "password": "
    int passwordEnd = jsonContent.indexOf('"', passwordStart + 1);
    JSONpassword = jsonContent.substring(passwordStart + 1, passwordEnd);
  }

  int brokerStart = jsonContent.indexOf("\"broker\"");
  if (brokerStart != -1) {
    brokerStart = jsonContent.indexOf('"', brokerStart + 8); // Start after "broker": "
    int brokerEnd = jsonContent.indexOf('"', brokerStart + 1);
    JSONbroker = jsonContent.substring(brokerStart + 1, brokerEnd);
  }

  int topicStart = jsonContent.indexOf("\"topic\"");
  if (topicStart != -1) {
    topicStart = jsonContent.indexOf('"', topicStart + 7); // Start after "topic": "
    int topicEnd = jsonContent.indexOf('"', topicStart + 1);
    JSONtopic = jsonContent.substring(topicStart + 1, topicEnd);
  }
}