#include <global_variable.h>
#if !defined _serial_parser_h_
#define _serial_parser_h_
void parseSerial()
{
  if (msg_in.length() > 0)
  {
    String command = "";
    String cmd_value = "";
    for (byte i = 0; i < msg_in.length(); i++)
    {
      command += msg_in[i];
      if (msg_in[i] == ':')
      {
        break;
      }
    }
    byte get_cmd = 0;
    for (byte i = 0; i < (msg_in.length()); i++)
    {
      if (get_cmd == 1)
      {
        cmd_value += msg_in[i];
      }
      if (msg_in[i] == ':')
      {
        get_cmd = 1;
      }
    }
    if (command == ">setdata:")
    {
      if (cmd_value.length() > 0)
      {
        parseJsonData(cmd_value);
        // Extract individual keys
        writeString(MSTR0, JSONssid);
        writeString(MSTR1, JSONpassword);
        writeString(MSTR2, JSONbroker);
        writeString(MSTR3, JSONtopic);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP.restart();
      }
    }
    else if (command == ">settime:")
    {
      parseTimeData(cmd_value);
      // January 21, 2014 at 3am you would call:
      rtc.adjust(
          DateTime(JSONyear.toInt(),
                   JSONmonth.toInt(),
                   JSONday.toInt(),
                   JSONhour.toInt(),
                   JSONminute.toInt(),
                   JSONsecond.toInt()));
    }
    else if (command == ">reboot:")
    {
      ESP.restart();
    }
    else if (command == ">getVAR:")
    {
      String jsonResponse = "{\n";
      jsonResponse += "  \"setup_var\": {\n";
      jsonResponse += "    \"SSID\":\"" + readString(MSTR0) + "\",\n";
      jsonResponse += "    \"password\":\"" + readString(MSTR1) + "\",\n";
      jsonResponse += "    \"broker\":\"" + readString(MSTR2) + "\",\n";
      jsonResponse += "    \"topic\":\"" + readString(MSTR3) + "\"\n";
      jsonResponse += "  }\n";
      jsonResponse += "}\n";

      // Send the JSON response over Serial
      Serial.println(jsonResponse);
    }
    else
    {
      Serial.println("ERROR");
    }
  }
}
#endif