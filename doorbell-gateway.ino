/*
 * MQTT based doorbell gateway  
 * 
 * It connects to an MQTT server in the local network.
 * It tries to detect the sound of the doorbell and then publishes 
 * a message (e.g. "ding-dong") to an arbitrary topic (e.g. "Test").
 * It currently also sends a message on (re-)connect.
 * 
 * This has beentested on a NodeMCU 1.0 ESP12-E.
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these configuration values suitable for your network.
#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi()
{
    delay(10);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to WiFi with SSID: \"");
    Serial.print(WIFI_SSID);
    Serial.println("\"");
  
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
  
    Serial.println("");
    Serial.println("WiFi connected.");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if (client.connect("RmClientWithoutSpe", "", ""))
        {
            Serial.println("connected");

            // Once connected, publish an announcement...
            client.publish(MQTT_TOPIC, MQTT_MSG_CONNECT);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT); // initialize the BUILTIN_LED pin as an output
    Serial.begin(230400); // start serial for debug output
    setup_wifi();

    client.setServer(MQTT_SERVER, MQTT_PORT);

    if (!client.connected())
    {
        reconnect();
    }
}

float countsToMillivolts(int nCounts)
{
    // For the Arduino Uno, its a 10 bit unsgined value (i.e. a range of 0..1023).
    // Its measurement range is from 0..5 volts.
    // This yields a resolution between readings of: 5 volts / 1024 units or approx. 4.9 mV per LSB
    return nCounts * 4.882813f;
}

bool detectDoorbell(float fVoltage)
{
    const float fOffset = SENSOR_OFFSET_VOLTAGE;
    const float fThreshold = SENSOR_THRESHOLD;
    static float fSignal;
    static int nCnt = 0;
    static int nCntDown = 0;
    static bool bDetected = false;
    static bool bDetectedOld = false;

    fSignal = fVoltage - fOffset;

    if( abs(fSignal) > fThreshold )
    {
        nCnt++; // increase likeliness of occurence
    }
    else
    {
        nCnt--; // decrease likeliness of occurence, but never fall below zero
        if( nCnt < 0 )
        {
            nCnt = 0;
        }
    }

    if( nCnt > SENSOR_CNT_LIMIT )
    {
        // detected
        bDetected = true;

        // keep output true for specific time
        nCntDown = 400;
    }

    if( nCntDown > 0 )
    {
        nCntDown--;
        if( nCntDown == 0 )
        {
            // no longer keep output true
            bDetected = false;
        }
    }

#if DEBUG
    Serial.print("V: ");
    Serial.print( fVoltage );
    Serial.print(", |Sig|: ");
    Serial.print( abs(fSignal) );
    Serial.print(", Count: ");
    Serial.print( nCnt );
    Serial.print(", Out: ");
    Serial.println( bDetected ? 300 : 0 );
#endif

    return bDetected;
}

void loop()
{
    static int nConversionValue = 0;
    static bool bDetected = false;
    static bool bDetectedOld = false;
    
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    nConversionValue = analogRead(ANALOG_INPUT_PIN);

    bDetected = detectDoorbell( countsToMillivolts(nConversionValue) );

    /* detect rising edge */
    if( (bDetectedOld != bDetected) && bDetected )
    {
        if( client.publish(MQTT_TOPIC, MQTT_MSG_DOORBELL) )
        {
            Serial.println("Published via MQTT.");
        }
        else
        {
            Serial.println("Unable to publish via MQTT.");
        }
    }
    bDetectedOld = bDetected;

    delay(SENSOR_LOOP_DELAY);
}


