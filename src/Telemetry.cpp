#include "Telemetry.h"

Telemetry::Telemetry(){}

String Telemetry::fetchLoraData(bool &isReceive, HardwareSerial &Serial)
{
    String data = "";
	while(Serial.available() > 0) data += (char)Serial.read();
	if(data != "") isReceive = true;
	return data;
}

bool Telemetry::timeOutCounter(bool reset)
{
    bool isTimeout;
    byte timeOut = 0;
    timeOut = reset ? 0 : timeOut;
    isTimeout = timeOut > 5 ?  true :  false;
    timeOut++;
    return isTimeout;
}

void Telemetry::parsingFromGCS(String receiveGCS)
{   
    String finalResult[receiveGCS.length() + 1]; 
    char tempReadGCS  [receiveGCS.length() + 1];
    byte Counter, wordCounter = 0;

    strcpy(tempReadGCS, receiveGCS.c_str());
    for (Counter = 0, wordCounter = 0; Counter < receiveGCS.length(); Counter++)
    {
        if (tempReadGCS[Counter] != ',') 
        {
            finalResult[wordCounter] += tempReadGCS[Counter];
            finalResult[wordCounter].trim();
        }
        else wordCounter++;
    }

    listCommand(finalResult[0], finalResult[1], finalResult[2],                      //* LeftSpeed, RightSpeed, Conveyer1, 
                finalResult[3], finalResult[4], finalResult[5], finalResult[6]);     //* leftSignal, RightSignal, CameraAngle, Conveyer2
}

void Telemetry::listCommand(String leftSpeed, String rightSpeed, String Conveyer,
                            String leftTurn, String rightTurn, String cameraAngle, String conveyerStop)
{
    GCS.leftAnalog      = leftSpeed.toInt();
    GCS.rightAnalog     = rightSpeed.toInt();
    GCS.conveyer        = Conveyer == "N" ? true : false;
    GCS.signalLeft      = leftTurn == "N" ? true : false;
    GCS.signalRight     = rightTurn == "N" ? true : false;
    GCS.cameraAngle     = cameraAngle.toInt();
    GCS.conveyerStop    = conveyerStop == "N" ? true : false;
}

void Telemetry::collectControlData(ControlData &data)
{
    data.leftTurn = false;
    data.rightTurn = false;
    data.leftJoystick    = GCS.leftAnalog;
    data.rightJoystick   = GCS.rightAnalog;
    data.conveyer     = GCS.conveyer;
    data.leftTurn     = GCS.leftAnalog  > (GCS.rightAnalog + 500) ? true : false;
    data.rightTurn    = GCS.rightAnalog > (GCS.leftAnalog + 500)  ? true : false;
    data.cameraAngle  = GCS.cameraAngle;
    data.conveyerStop = GCS.conveyerStop;
}


String Telemetry::constructMessage(float batteryPercentage, bool conveyer, 
                                   bool leftTurn, bool rightTurn, bool trash)
{
    char buffer[64];
    String iConveyer, iLeft, iRight, iTrash;
    iConveyer = conveyer? "N" : "F";
    iLeft = leftTurn ? "N" : "F";
    iRight = rightTurn ? "N" : "F";
    iTrash = trash ? "N" : "F";
    snprintf(buffer, sizeof(buffer),
            "%0.1f,"
            "%s,%s,"
            "%s",
            batteryPercentage,
            iLeft.c_str(), iRight.c_str(),
            iTrash.c_str()
    );
    return buffer;
}

