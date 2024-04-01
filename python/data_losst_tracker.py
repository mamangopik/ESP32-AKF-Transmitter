import json
import paho.mqtt.client as mqtt
import csv
from datetime import datetime

# Global variable to store the last received 'id_data'
last_received_id_data = -1
last_RSSI=-1
last_batt_voltage = 0.0
qos = 0
last_connection_count = 0

# CSV file name
csv_file = "mqtt_data_log.csv"

def log_to_csv(timestamp, event, message,RSSI,vbatt,qos):
    with open(csv_file, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([timestamp, event, message,RSSI,vbatt])

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribe to the updated topic where your MQTT publisher is sending messages
    client.subscribe("/shms/node_3/accelerometer")

def on_message(client, userdata, msg):
    global last_received_id_data
    global last_RSSI
    global last_batt_voltage
    global qos
    global last_connection_count

    # Decode the JSON message
    try:
        data = json.loads(msg.payload.decode('utf-8'))
    except json.JSONDecodeError:
        print("Error decoding JSON data")
        return
    if 'signal_strength' in data:
        last_RSSI = data['signal_strength']
    if 'battery_voltage' in data:
        last_batt_voltage = data['battery_voltage']
    if 'qos' in data:
        qos = data['battery_voltage']
    # Check if 'id_data' field is present
    if 'id_data' in data:
        received_id_data = data['id_data']

        # Check for overflow condition
        if received_id_data < last_received_id_data:
            # Handle the overflow condition
            print(f"Received message with id_data: {received_id_data} (Overflow)")
            log_to_csv(datetime.now(), "Overflow", f"Received message with id_data: {received_id_data}",last_RSSI,last_batt_voltage)
        elif received_id_data == last_received_id_data:
            # Handle duplicated 'id_data' values
            print(f"Duplicate message with id_data: {received_id_data}")
            log_to_csv(datetime.now(), "Duplicate", f"Received message with id_data: {received_id_data}",last_RSSI,last_batt_voltage)
        elif received_id_data == last_received_id_data + 1:
            # Normal incremental case
            print(f"Received message with id_data: {received_id_data}")
        else:
            # Potential data loss
            print(f"Potential data loss. Expected id_data: {last_received_id_data + 1}, Received id_data: {received_id_data}")
            log_to_csv(datetime.now(), "Data Loss", f"Expected id_data: {last_received_id_data + 1}, Received id_data: {received_id_data}",last_RSSI,last_batt_voltage)
            # Additional actions for handling data loss can be implemented here
        last_received_id_data = received_id_data

# Create an MQTT client instance
client = mqtt.Client()

# Set up the callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker (replace "192.168.1.235" with the actual address)
client.connect("192.168.1.235", 1883, 60)

# Loop to maintain the connection and process incoming messages
client.loop_forever()
