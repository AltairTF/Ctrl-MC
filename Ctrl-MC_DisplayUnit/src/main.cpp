/***  Ctrl-MC // An open source Motorcycle Controller Arduino project by KI Hestad: https://github.com/KIHestad/Ctrl-MC  ***/

// External libaries
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Common lbraries
#include "../../Ctrl-MC_Common/lib/OnBoardLed/OnBoardLed.h" // Ctrl-MC_Common/lib -> onBoard LED
#include "../../Ctrl-MC_Common/lib/SerialCommunication/SerialCommunication.h" // Ctrl-MC_Common/lib
#include "../../Ctrl-MC_Common/lib/Config/Config.h" // Ctrl-MC_Common/lib -> To be user edited to enable/disable features and configure arduino board
OnBoardLed onBoardLed;
SerialCommunication serialCommunication;
// Initiate Adafruit OLD display
Adafruit_SSD1306 display(Config::DisplaySettings::ScreenWidth, Config::DisplaySettings::ScreenHeight, &Wire, Config::DisplaySettings::ScreenAddress);
// Project includes
#include <BikeStatus.h>
BikeStatus bikeStatus;
#include <Button.h>
Button btnClutch;
Button btnIndicatorLeft;
Button btnIndicatorRight;
Button btnLightsHiLo;
Button btnStartStop;
Button btnMenuSelect;
Button btnMenuNext;
Button btnBrakeFront;
#include <ButtonHelper.h>
#include <DisplayImage.h>
#include <DisplayHelper.h>
DisplayHelper displayHelper;
#include <DisplayMenu.h>
DisplayMenu displayMenu;
#include <ButtonEvent.h>
ButtonEvent buttonEvent;
#include <Action.h>
Action action;
// Features
#include <TestButtons.h>
TestButtons testButtons;
#include <IgnitionButtonPassword.h>
IgnitionButtonPassword ignitionButtonPassword;

#include <init.h>



void setup() {
    // Serial comm
    Config config = Config();
    Serial.begin(config.serialCommSpeed);
    serialCommunication = SerialCommunication();
    serialCommunication.clearBuffer();
    // Setup initial values
    Init init = Init();
    init.run();
    // Init onboard led
    Config::DisplayUnitOutput duOutput = Config::DisplayUnitOutput();
    onBoardLed = OnBoardLed();
    onBoardLed.init(duOutput.onBoardLed);
    
    // Read bike status from relay unit
    // TODO 
}

void loop() {
    // Check for incoming serial data from relay unit
    SerialCommunication::Data serialData = SerialCommunication::Data();
    serialData = serialCommunication.read();
    if (serialData.received) 
        action.checkReceivedData(serialData);
    // Check progress for switching back to status page
    displayHelper.gotoStatusPage();
    // Depending on ignition status allow different operations
    if (bikeStatus.ignition != BikeStatusIgnition::ignOn) {
        // Ignintion is not on, meaning it is off or in password mode
        // Check for button testing
        testButtons.loopAction();
        // Check for turning on ignintion using button password
        ignitionButtonPassword.loopAction();
    }
    else {
        // Ignition is on, allow operations, inititate handshake to check that communication to relay unit is working
        action.performHandshake();
        // Check for selected menu
        displayMenu.loopAction();
        // Check handlebarbuttons
        buttonEvent.loopAction();
        // Check for runnning actions
        action.indicatorBlink();
    }

}