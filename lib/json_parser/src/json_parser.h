#include <global_variable.h>
#if !defined _json_parser_h_
#define _json_parser_h_

// Extract individual values by searching for keys
String ssid, password, broker, topic;
String year, month, day, hour, minute, second;

void parseJsonData(const String &jsonData)
{
  // Find the start and end of the JSON object
  int start = jsonData.indexOf('{');
  int end = jsonData.lastIndexOf('}');

  if (start == -1 || end == -1 || end <= start)
  {
    // JSON object not found or invalid
    Serial.println("Invalid JSON data");
    return;
  }

  String jsonContent = jsonData.substring(start, end + 1);

  int ssidStart = jsonContent.indexOf("\"ssid\"");
  if (ssidStart != -1)
  {
    ssidStart = jsonContent.indexOf('"', ssidStart + 6); // Start after "ssid": "
    int ssidEnd = jsonContent.indexOf('"', ssidStart + 1);
    JSONssid = jsonContent.substring(ssidStart + 1, ssidEnd);
  }

  int passwordStart = jsonContent.indexOf("\"password\"");
  if (passwordStart != -1)
  {
    passwordStart = jsonContent.indexOf('"', passwordStart + 10); // Start after "password": "
    int passwordEnd = jsonContent.indexOf('"', passwordStart + 1);
    JSONpassword = jsonContent.substring(passwordStart + 1, passwordEnd);
  }

  int brokerStart = jsonContent.indexOf("\"broker\"");
  if (brokerStart != -1)
  {
    brokerStart = jsonContent.indexOf('"', brokerStart + 8); // Start after "broker": "
    int brokerEnd = jsonContent.indexOf('"', brokerStart + 1);
    JSONbroker = jsonContent.substring(brokerStart + 1, brokerEnd);
  }

  int topicStart = jsonContent.indexOf("\"topic\"");
  if (topicStart != -1)
  {
    topicStart = jsonContent.indexOf('"', topicStart + 7); // Start after "topic": "
    int topicEnd = jsonContent.indexOf('"', topicStart + 1);
    JSONtopic = jsonContent.substring(topicStart + 1, topicEnd);
  }
}

void parseTimeData(const String &jsonData)
{
  // Find the start and end of the JSON object
  int start = jsonData.indexOf('{');
  int end = jsonData.lastIndexOf('}');

  if (start == -1 || end == -1 || end <= start)
  {
    // JSON object not found or invalid
    Serial.println("Invalid JSON data");
    return;
  }

  String jsonContent = jsonData.substring(start, end + 1);

  int yearStart = jsonContent.indexOf("\"year\"");
  if (yearStart != -1)
  {
    yearStart = jsonContent.indexOf('"', yearStart + 6); // Start after "year": "
    int yearEnd = jsonContent.indexOf('"', yearStart + 1);
    JSONyear = jsonContent.substring(yearStart + 1, yearEnd);
  }

  int monthStart = jsonContent.indexOf("\"month\"");
  if (monthStart != -1)
  {
    monthStart = jsonContent.indexOf('"', monthStart + 7); // Start after "month": "
    int monthEnd = jsonContent.indexOf('"', monthStart + 1);
    JSONmonth = jsonContent.substring(monthStart + 1, monthEnd);
  }

  int dayStart = jsonContent.indexOf("\"day\"");
  if (dayStart != -1)
  {
    dayStart = jsonContent.indexOf('"', dayStart + 6); // Start after "day": "
    int dayEnd = jsonContent.indexOf('"', dayStart + 1);
    JSONday = jsonContent.substring(dayStart + 1, dayEnd);
  }

  int hourStart = jsonContent.indexOf("\"hour\"");
  if (hourStart != -1)
  {
    hourStart = jsonContent.indexOf('"', hourStart + 7); // Start after "hour": "
    int hourEnd = jsonContent.indexOf('"', hourStart + 1);
    JSONhour = jsonContent.substring(hourStart + 1, hourEnd);
  }

  int minuteStart = jsonContent.indexOf("\"minute\"");
  if (minuteStart != -1)
  {
    minuteStart = jsonContent.indexOf('"', minuteStart + 9); // Start after "minute": "
    int minuteEnd = jsonContent.indexOf('"', minuteStart + 1);
    JSONminute = jsonContent.substring(minuteStart + 1, minuteEnd);
  }

  int secondStart = jsonContent.indexOf("\"second\"");
  if (secondStart != -1)
  {
    secondStart = jsonContent.indexOf('"', secondStart + 9); // Start after "second": "
    int secondEnd = jsonContent.indexOf('"', secondStart + 1);
    JSONsecond = jsonContent.substring(secondStart + 1, secondEnd);
  }
}
#endif