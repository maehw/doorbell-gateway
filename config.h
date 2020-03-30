#define WIFI_SSID           "MyWifiSsid"
#define WIFI_PASSWORD       "MyWifiPassword"
#define MQTT_SERVER         "MyLocalIp"
#define MQTT_PORT           1883
#define MQTT_TOPIC          "Test"
#define MQTT_MSG_CONNECT    "Connected"
#define MQTT_MSG_DOORBELL   "Ding-dong"
#define ANALOG_INPUT_PIN    A0


/* Sensor loop delay to busy wait and currently get a sample interval of approx. 3 ms, i.e. a sampling frequency of 333 Hz. */
#define SENSOR_LOOP_DELAY       (3u)

#define SENSOR_OFFSET_VOLTAGE   (3835.0f)

#define SENSOR_THRESHOLD        (40.0f)

#define SENSOR_CNT_LIMIT        (20)
