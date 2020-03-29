// Test setup:
//   Arduino Nano pin asssignment:
//     Analog input: A0
//     LED: 13 (on board LED)

#define LED_PIN               (13)

/* Mapping of the analog input pin, define alias here */
#define ANALOG_INPUT_PIN      (A0)

/* Mapping of the digital output pin for real-time supervision, define alias here */
#define RT_SUPERVISION_PIN    (A3)

/* Sensor loop delay to busy wait and currently get a sample interval of approx. 2.5 ms,
 * i.e. a sampling frequency of 400 Hz.
 */
#define SENSOR_LOOP_DELAY     (2u)

void setup()
{
    Serial.begin(230400); // start serial for debug output

#ifdef RT_SUPERVISION_PIN
    pinMode(RT_SUPERVISION_PIN, OUTPUT);
#endif

    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    static int nConversionValue = 0;
    static int nCnt = 0;
    static int nCntDown = 0;
    static int nOutput = 0;
    static float fVoltage = 0.0f;
    static float fSignal = 0.0f;
    //const float fOffset = 2710.0f; // threshold for connected logic analyzer
    const float fOffset = 2755.0f;
    const float fThreshold = 40.0f;

#ifdef RT_SUPERVISION_PIN
    digitalWrite(RT_SUPERVISION_PIN, HIGH);
#endif

    nConversionValue = analogRead(ANALOG_INPUT_PIN);
    fVoltage = countsToMillivolts(nConversionValue);
    fSignal = fVoltage - fOffset;

//    Serial.print("Conversion value: ");
//    Serial.println(nConversionValue);

    if( abs(fSignal) > fThreshold )
    {
        nCnt++;
    }
    else
    {
        nCnt--;
        if( nCnt < 0 )
        {
            nCnt = 0;
        }
    }

    if( nCnt > 20 )
    {
        // switch on
        nOutput = 100; // arbitrary value for plot scaling
        nCntDown = 400;
        digitalWrite(LED_PIN, HIGH);
    }

    if( nCntDown > 0 )
    {
        nCntDown--;
        if( nCntDown == 0 )
        {
            // switch off
            nOutput = 0;
            digitalWrite(LED_PIN, LOW);
        }
    }

    //Serial.print("Sig: ");
    //Serial.print( fSignal );
    Serial.print(", |Sig|: ");
    Serial.print( abs(fSignal) );
    Serial.print(", Count: ");
    Serial.print( nCnt );
    Serial.print(", Out: ");
    Serial.println( nOutput );

#ifdef RT_SUPERVISION_PIN
    digitalWrite(RT_SUPERVISION_PIN, LOW);
#endif

    /* Wait until next cyclic measurement is made */
    delay(SENSOR_LOOP_DELAY);
}

float countsToMillivolts(int nCounts)
{
    // For the Arduino Uno, its a 10 bit unsgined value (i.e. a range of 0..1023).
    // Its measurement range is from 0..5 volts.
    // This yields a resolution between readings of: 5 volts / 1024 units or approx. 4.9 mV per LSB
    return nCounts * 4.882813f;
}
