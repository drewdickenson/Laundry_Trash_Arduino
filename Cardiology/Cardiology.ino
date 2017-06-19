//    Drew Dickenson
//    May 2, 2014
//    Dirty and Clean Utility Room Push Button Email --> Emergin --> Alpha Pager.
//
//    *** THIS IS THE Cardiology Version
//
//    Version 2.0 -- Adjustment for Hiplink.  Added 10 second delay after HELO in mail send. 1-24-17
//    Version 1.07 -- Added all locations
//    Version 1.06 -- This Version
//    Version 1.05 -- Test Version
//    Version 1.04 -- Added bodies to the emails since the emergin server was not sending subjects.
//    Version 1.02 -- Added proper recipients and 5 Nursery as a location.
//    Version 1.01 -- 5th Floor Production Pilot Version
//
//    This program allows for a 6 button system per unit at the hospital to press a button and notify someone that
//    something needs to be done, or notify a supervisor that the action has been completed.
//
//    It also allows to hook up 3 sets of these 6 button configurations per each Arduino Mega 2560
//
//    needTrashPickedUp (a,b,or c) sends a page to the environmental service tech and their supervisor via Emergin letting them know there is trash on the unit to be picked up
//    trashHasBeenPickedUp (a, b, or c) sends a message to the environmental services supervisor that the tech has been there and emptied the trash.
//
//    needLaundryPickedUp does teh same as the trash above except with Laundry employees.
//    laundryHasBeenPickedUp (a,b, or c) does the same as trashHasBeenPickedUp except for Laundry employees.
//
//    freshLinensNeeded (a, b, or c) lets the laundry department know that a unit needs fresh linens
//    freshLinenesStocked (a, b, or c) lets the laundry supervisor know that the linens requested have been delivered.
//
//
//
//    This program is free to use and edit to your hearts desire, just throw some little shout out to me in the comments :)
//
//    This program was based loosely on the project found here:   https://github.com/crmchenry3/Arduino_SMTP_Email/blob/master/BlinkMail-Youtube-Version.ino
//    Here's the link to the youtube video as well: http://www.youtube.com/watch?v=ROr5mh32gyo


#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <SPI.h>


const int needTrashPickedUpPinA = 24;
const int trashHasBeenPickedUpPinA = 25;
const int needLaundryPickedUpPinA = 26;
const int laundryHasBeenPickedUpPinA = 27;
const int freshLinensNeededPinA = 30;
const int freshLinensStockedPinA = 31;

const int needTrashPickedUpPinB = 34;
const int trashHasBeenPickedUpPinB = 35;
const int needLaundryPickedUpPinB = 36;
const int laundryHasBeenPickedUpPinB = 37;
const int freshLinensNeededPinB = 40;
const int freshLinensStockedPinB = 41;

/*
const int needTrashPickedUpPinC = 44;
const int trashHasBeenPickedUpPinC = 45;
const int needLaundryPickedUpPinC = 46;
const int laundryHasBeenPickedUpPinC = 47;
const int freshLinensNeededPinC = 18;
const int freshLinensStockedPinC = 19;
*/

// LED Pins for Pod A or North
const int dirtyUtilityGreenA =  22;      // Green LED
const int dirtyUtilityRedA =  23;      // Red LED
const int cleanUtilityGreenA = 28; //Red LED
const int cleanUtilityRedA = 29;  //Green LED

// LED Pins for Pod B or South
const int dirtyUtilityGreenB =  32;      // Green LED
const int dirtyUtilityRedB =  33;      // Red LED
const int cleanUtilityGreenB = 38; //Red LED
const int cleanUtilityRedB = 39;  //Green LED
/*
// LED Pins for Pod C
const int dirtyUtilityGreenC =  42;      // Green LED
const int dirtyUtilityRedC =  43;      // Red LED
const int cleanUtilityGreenC = 16; //Red LED
const int cleanUtilityRedC = 17;  //Green LED
*/
const int WAIT = 300;       // delay time

//Variables
int needTrashPickedUpButtonStateA = 0;
int trashHasBeenPickedUpButtonStateA = 0;
int needLaundryPickedUpButtonStateA = 0;
int laundryHasBeenPickedUpButtonStateA = 0;
int needFreshLinensButtonStateA = 0;
int freshLinensDeliveredButtonStateA = 0;

int needTrashPickedUpButtonStateB = 0;
int trashHasBeenPickedUpButtonStateB = 0;
int needLaundryPickedUpButtonStateB = 0;
int laundryHasBeenPickedUpButtonStateB = 0;
int needFreshLinensButtonStateB = 0;
int freshLinensDeliveredButtonStateB = 0;
/*
int needTrashPickedUpButtonStateC = 0;
int trashHasBeenPickedUpButtonStateC = 0;
int needLaundryPickedUpButtonStateC = 0;
int laundryHasBeenPickedUpButtonStateC = 0;
int needFreshLinensButtonStateC = 0;
int freshLinensDeliveredButtonStateC = 0;
*/

bool dirtyUtilityErrorStatus = 0;
bool cleanUtilityErrorStatus = 0;
bool errorState = 0;

// ********************************  Arduino network information
// Your Ethernet Shield MAC address (Make sure the address is unique for each board. The first 3 octet are Intel Mfg)
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9C, 0xC3 };
// Home IP address data (if unable to use DHCP) [REMEMBER MUST CALL DIFFERENT Ethernet.begin(mac, ip, subnet, gateway) inside setupCommunications to use static IP;
//byte ip[] = { 10, 16, 80, 209 };
//byte subnet[] = { 255, 255, 252, 0 };
//byte gateway[] = { 10, 16, 80, 1 };

//*** SMTP Server IP Address
byte smtpServerIP[] = { 10, 16, 8, 160 };
String smtpServerName = "emergin";

// ********************************  Mail information Arrays

String locations[] = {"ICU_Pod_A", "ICU_Pod_B", "ICU_Pod_C", "3_East_Pod_A", "3_East_Pod_B", "3_East_Pod_C", "4_East_Pod_A", "4_East_Pod_B",
                      "4_East_Pod_C", "5_North", "5_South", "5_Nursery", "4_North", "4_South", "3_North", "3_South", "OPS", "Surgery", "ED Green Zone", "ED Red Zone", 
                      "ED Clean Supply", "MEC", "Outpatient Infusion", "Radiology", "Wound Care", "Cardiology"
                     };
String recipients[] = {"Freshloc_Pharm@phnpage.net", "ddickenson@phnpage.net", "TrashOnUnit@phnpage.net", "Laundry_Full@phnpage.net",
                       "Laundry_Emptied@phnpage.net", "Linens_Request@phnpage.net", "Linens_Delivered@phnpage.net"
                      };
String subjects[] = { "Trash Full", "Trash Emptied", "Laundry Full", "Laundry Emptied", "Linens Needed", "Linens Stocked"};
String bodies[] = {"Trash Full", "Trash Emptied", "Laundry Full", "Laundry Emptied", "Linens Needed", "Linens Stocked"};

/***********************Guide To Arrays *******************************************

locations [0]  = ICU Pod A          recipients[0] =  Freshloc_Pharm@phnpage.net        subjects[0] = Trash Full				bodies[0] = Trash Full
locations [1]  = ICU Pod B          recipients[1] =  ddickenson@phnpage.net            subjects[1] = Trash Emptied			bodies[1] = Trash Emptied
locations [2]  = ICU Pod C          recipients[2] =  TrashOnUnit@phnpage.net           subjects[2] = Laundry Full			bodies[2] = Laundry Full
locations [3]  = 3 East Pod A       recipients[3] =  Laundry_Full@phnpage.net          subjects[3] = Laundry Emptied		bodies[3] = Laundry Emptied
locations [4]  = 3 East Pod B       recipients[4] =  Laundry_Emptied@phnpage.net       subjects[4] = Linens Needed			bodies[4] = Linens Needed
locations [5]  = 3 East Pod C       recipients[5] =  Linens_Request@phnpage.net        subjects[5] = Linens Stocked			bodies[5] = Linens Stocked
locations [6]  = 4 East Pod A       recipients[6] =  Linens_Delivered@phnpage.net
locations [7]  = 4 East Pod B       recipients[7] =
locations [8]  = 4 East Pod C       recipients[8] =
locations [9]  = 5 North            recipients[9] =
locations [10] = 5 South            recipients[10] =
locations [11] = 5 Nursery          recipients[11] =
locations [12] = 4 North            recipients[12] =
locations [13] = 4 South            recipients[13] =
locations [14] = 3 North            recipients[14] =
locations [15] = 3 South            recipients[15] =
locations [16] = OPS                recipients[16] =
locations [17] = Surgery            recipients[17] =
locations [18] = ED Green Zone      recipients[18] =
locations [19] = ED Red Zone
locations [20] = ED Clean Supply
locations [21] = MEC
locations [22] = Outpatient Infusion
locations [23] = Radiology
locations [24] = Wound Care
locations [25] = Cardiology

*/

// ********************************  Arduino network connection
String ServerResponse = "";
EthernetClient client;

void setup()
{
  Serial.begin(115200);
  Serial.println("Cardiology\n");
  Serial.println("Good Morning!  Let me get some stuff setup for you\n");
  Serial.println("Setting up I/O Pin Modes...");


  // Setup I/O Pin Modes for all buttons / LED
  Serial.println("Setting up Pod A / North Pins");
  pinMode (needTrashPickedUpPinA, INPUT);
  pinMode (trashHasBeenPickedUpPinA, INPUT);
  pinMode (needLaundryPickedUpPinA, INPUT);
  pinMode (laundryHasBeenPickedUpPinA, INPUT);
  pinMode (freshLinensNeededPinA, INPUT);
  pinMode (freshLinensStockedPinA, INPUT);

  Serial.println("Setting up Pod B / South Pins");
  pinMode (needTrashPickedUpPinB, INPUT);
  pinMode (trashHasBeenPickedUpPinB, INPUT);
  pinMode (needLaundryPickedUpPinB, INPUT);
  pinMode (laundryHasBeenPickedUpPinB, INPUT);
  pinMode (freshLinensNeededPinB, INPUT);
  pinMode (freshLinensStockedPinB, INPUT);
/*
  Serial.println("Setting up Pod C Pins");
  pinMode (needTrashPickedUpPinC, INPUT);
  pinMode (trashHasBeenPickedUpPinC, INPUT);
  pinMode (needLaundryPickedUpPinC, INPUT);
  pinMode (laundryHasBeenPickedUpPinC, INPUT);
  pinMode (freshLinensNeededPinC, INPUT);
  pinMode (freshLinensStockedPinC, INPUT);
*/




  // Debug shit for pins that are acting up and going high when the board resets...
  //
  // First we read all the pins and put the state in a variable that we can then go back and
  // read the variable to see if they are HIGH or LOW --DD 5/2/2014
  Serial.println("Starting irritating debugging shit...\n");
  //Pod A / North Debug Shit...
  Serial.println("Checking Pod A / North Pins for anything set HIGH...");
  needTrashPickedUpButtonStateA = digitalRead(needTrashPickedUpPinA);
  trashHasBeenPickedUpButtonStateA = digitalRead(trashHasBeenPickedUpPinA);
  needLaundryPickedUpButtonStateA = digitalRead(needLaundryPickedUpPinA);
  laundryHasBeenPickedUpButtonStateA = digitalRead(laundryHasBeenPickedUpPinA);
  needFreshLinensButtonStateA = digitalRead(freshLinensNeededPinA);
  freshLinensDeliveredButtonStateA = digitalRead(freshLinensStockedPinA);
  if (needTrashPickedUpButtonStateA == 1)
    Serial.println("Pin 24 is High");
  else if (trashHasBeenPickedUpButtonStateA == 1)
    Serial.println("Pin 25 is High");
  else if (needLaundryPickedUpButtonStateA == 1)
    Serial.println("Pin 26 is High");
  else if (laundryHasBeenPickedUpButtonStateA == 1)
    Serial.println("Pin 27 is High");
  else if (needFreshLinensButtonStateA == 1)
    Serial.println("Pin 30 is High");
  else if (freshLinensDeliveredButtonStateA == 1)
    Serial.println("Pin 31 is High");

  //Pod B / South Debug Shit...
  Serial.println("Checking Pod B / South Pins for anything set HIGH...");
  needTrashPickedUpButtonStateB = digitalRead(needTrashPickedUpPinB);
  trashHasBeenPickedUpButtonStateB = digitalRead(trashHasBeenPickedUpPinB);
  needLaundryPickedUpButtonStateB = digitalRead(needLaundryPickedUpPinB);
  laundryHasBeenPickedUpButtonStateB = digitalRead(laundryHasBeenPickedUpPinB);
  needFreshLinensButtonStateB = digitalRead(freshLinensNeededPinB);
  freshLinensDeliveredButtonStateB = digitalRead(freshLinensStockedPinB);
  if (needTrashPickedUpButtonStateB == 1)
    Serial.println("Pin 34 is High");
  else if (trashHasBeenPickedUpButtonStateB == 1)
    Serial.println("Pin 35 is High");
  else if (needLaundryPickedUpButtonStateB == 1)
    Serial.println("Pin 36 is High");
  else if (laundryHasBeenPickedUpButtonStateB == 1)
    Serial.println("Pin 37 is High");
  else if (needFreshLinensButtonStateB == 1)
    Serial.println("Pin 40 is High");
  else if (freshLinensDeliveredButtonStateB == 1)
    Serial.println("Pin 41 is High");

  /*
  //Pod C Debug Shit...
  Serial.println("Checking Pod C (if applicable) Pins for anything set HIGH...");
  needTrashPickedUpButtonStateC = digitalRead(needTrashPickedUpPinC);
  trashHasBeenPickedUpButtonStateC = digitalRead(trashHasBeenPickedUpPinC);
  needLaundryPickedUpButtonStateC = digitalRead(needLaundryPickedUpPinC);
  laundryHasBeenPickedUpButtonStateC = digitalRead(laundryHasBeenPickedUpPinC);
  needFreshLinensButtonStateC = digitalRead(freshLinensNeededPinC);
  freshLinensDeliveredButtonStateC = digitalRead(freshLinensStockedPinC);
  if(needTrashPickedUpButtonStateC == 1)
  Serial.println("Pin 44 is High");
  else if(trashHasBeenPickedUpButtonStateC == 1)
  Serial.println("Pin 45 is High");
  else if(needLaundryPickedUpButtonStateC == 1)
  Serial.println("Pin 46 is High");
  else if(laundryHasBeenPickedUpButtonStateC == 1)
  Serial.println("Pin 47 is High");
  else if(needFreshLinensButtonStateC == 1)
  Serial.println("Pin 18 is High");
  else if(freshLinensDeliveredButtonStateC == 1)
  Serial.println("Pin 19 is High");
  */

  pinMode (dirtyUtilityGreenA, OUTPUT);      // Green LED
  pinMode (dirtyUtilityRedA, OUTPUT);      // Red LED
  pinMode (cleanUtilityGreenA, OUTPUT);
  pinMode (cleanUtilityRedA, OUTPUT);

  pinMode (dirtyUtilityGreenB, OUTPUT);      // Green LED
  pinMode (dirtyUtilityRedB, OUTPUT);      // Red LED
  pinMode (cleanUtilityGreenB, OUTPUT);
  pinMode (cleanUtilityRedB, OUTPUT);
/*  
  pinMode (dirtyUtilityGreenC, OUTPUT);      // Green LED
  pinMode (dirtyUtilityRedC, OUTPUT);      // Red LED
  pinMode (cleanUtilityGreenC, OUTPUT);
  pinMode (cleanUtilityRedC, OUTPUT);
  */
  
  // Call Communications Init Function


  setupCommunications();

  digitalWrite(dirtyUtilityGreenA, HIGH); //  Alternate GREEN RED GREEN RED to
  digitalWrite(dirtyUtilityGreenB, HIGH); //  indicate that the communication
  //digitalWrite(dirtyUtilityGreenC, HIGH); //  setup routine has finished running
  delay(100);                             //  and the loop is about to start.
  digitalWrite(dirtyUtilityGreenA, LOW);  //
  digitalWrite(dirtyUtilityGreenB, LOW);  //  NOTE: No input will be allowed
  //digitalWrite(dirtyUtilityGreenC, LOW);  //  until this has happened.
  digitalWrite(dirtyUtilityRedA, HIGH);
  digitalWrite(dirtyUtilityRedB, HIGH);
  //digitalWrite(dirtyUtilityRedC, HIGH);
  delay(100);
  digitalWrite(dirtyUtilityRedA, LOW);
  digitalWrite(dirtyUtilityRedB, LOW);
  //digitalWrite(dirtyUtilityRedC, LOW);
  digitalWrite(dirtyUtilityGreenA, HIGH);
  digitalWrite(dirtyUtilityGreenB, HIGH);
  //digitalWrite(dirtyUtilityGreenC, HIGH);
  delay(100);
  digitalWrite(dirtyUtilityGreenA, LOW);
  digitalWrite(dirtyUtilityGreenB, LOW);
  //digitalWrite(dirtyUtilityGreenC, LOW);
  digitalWrite(dirtyUtilityRedA, HIGH);
  digitalWrite(dirtyUtilityRedB, HIGH);
  //digitalWrite(dirtyUtilityRedC, HIGH);
  delay(100);
  digitalWrite(dirtyUtilityRedA, LOW);
  digitalWrite(dirtyUtilityRedB, LOW);
  //digitalWrite(dirtyUtilityRedC, LOW);

  digitalWrite(cleanUtilityGreenA, HIGH); //  Alternate GREEN RED GREEN RED to
  digitalWrite(cleanUtilityGreenB, HIGH); //  indicate that the communication
  //digitalWrite(cleanUtilityGreenC, HIGH); //  setup routine has finished running
  delay(100);                             //  and the loop is about to start.
  digitalWrite(cleanUtilityGreenA, LOW);  //
  digitalWrite(cleanUtilityGreenB, LOW);  //  NOTE: No input will be allowed
  //digitalWrite(cleanUtilityGreenC, LOW);  //  until this has happened.
  digitalWrite(cleanUtilityRedA, HIGH);
  digitalWrite(cleanUtilityRedB, HIGH);
  //digitalWrite(cleanUtilityRedC, HIGH);
  delay(100);
  digitalWrite(cleanUtilityRedA, LOW);
  digitalWrite(cleanUtilityRedB, LOW);
  //digitalWrite(cleanUtilityRedC, LOW);
  digitalWrite(cleanUtilityGreenA, HIGH);
  digitalWrite(cleanUtilityGreenB, HIGH);
  //digitalWrite(cleanUtilityGreenC, HIGH);
  delay(100);
  digitalWrite(cleanUtilityGreenA, LOW);
  digitalWrite(cleanUtilityGreenB, LOW);
  //digitalWrite(cleanUtilityGreenC, LOW);
  digitalWrite(cleanUtilityRedA, HIGH);
  digitalWrite(cleanUtilityRedB, HIGH);
  //digitalWrite(cleanUtilityRedC, HIGH);
  delay(100);
  digitalWrite(cleanUtilityRedA, LOW);
  digitalWrite(cleanUtilityRedB, LOW);
  //digitalWrite(cleanUtilityRedC, LOW);

  Serial.println("********* Device Startup Complete *********");
}

void loop()
{

  if (errorState == 1)
  {
    digitalWrite(dirtyUtilityRedA, HIGH); // If there is a serious error state
    digitalWrite(dirtyUtilityRedB, HIGH); // such as no network access turn
    //digitalWrite(dirtyUtilityRedC, HIGH); // all RED led on so that someone
    digitalWrite(cleanUtilityRedA, HIGH); // will alert the Communications
    digitalWrite(cleanUtilityRedB, HIGH); // department of the error.
    //digitalWrite(cleanUtilityRedC, HIGH);
  }
  else if (errorState == 0)
  {
    digitalWrite(dirtyUtilityRedA, LOW); // If there is a serious error state
    digitalWrite(dirtyUtilityRedB, LOW); // such as no network access turn
    //digitalWrite(dirtyUtilityRedC, LOW); // all RED led on so that someone
    digitalWrite(cleanUtilityRedA, LOW); // will alert the Communications
    digitalWrite(cleanUtilityRedB, LOW); // department of the error.
   // digitalWrite(cleanUtilityRedC, LOW);
  }
  // Set the buttone state to a variable every iteration of the loop to see if a button is pressed you can look in the variable and see if it's a 1 or 0;
  needTrashPickedUpButtonStateA = digitalRead(needTrashPickedUpPinA);
  trashHasBeenPickedUpButtonStateA = digitalRead(trashHasBeenPickedUpPinA);
  needLaundryPickedUpButtonStateA = digitalRead(needLaundryPickedUpPinA);
  laundryHasBeenPickedUpButtonStateA = digitalRead(laundryHasBeenPickedUpPinA);
  needFreshLinensButtonStateA = digitalRead(freshLinensNeededPinA);
  freshLinensDeliveredButtonStateA = digitalRead(freshLinensStockedPinA);

  needTrashPickedUpButtonStateB = digitalRead(needTrashPickedUpPinB);
  trashHasBeenPickedUpButtonStateB = digitalRead(trashHasBeenPickedUpPinB);
  needLaundryPickedUpButtonStateB = digitalRead(needLaundryPickedUpPinB);
  laundryHasBeenPickedUpButtonStateB = digitalRead(laundryHasBeenPickedUpPinB);
  needFreshLinensButtonStateB = digitalRead(freshLinensNeededPinB);
  freshLinensDeliveredButtonStateB = digitalRead(freshLinensStockedPinB);
  /*
  needTrashPickedUpButtonStateC = digitalRead(needTrashPickedUpPinC);
  trashHasBeenPickedUpButtonStateC = digitalRead(trashHasBeenPickedUpPinC);
  needLaundryPickedUpButtonStateC = digitalRead(needLaundryPickedUpPinC);
  laundryHasBeenPickedUpButtonStateC = digitalRead(laundryHasBeenPickedUpPinC);
  needFreshLinensButtonStateC = digitalRead(freshLinensNeededPinC);
  freshLinensDeliveredButtonStateC = digitalRead(freshLinensStockedPinC);
  */

  if (needTrashPickedUpButtonStateA == 1)
    Serial.println("Pin 24 is High");
  else if (trashHasBeenPickedUpButtonStateA == 1)
    Serial.println("Pin 25 is High");
  else if (needLaundryPickedUpButtonStateA == 1)
    Serial.println("Pin 26 is High");
  else if (laundryHasBeenPickedUpButtonStateA == 1)
    Serial.println("Pin 27 is High");
  else if (needFreshLinensButtonStateA == 1)
    Serial.println("Pin 30 is High");
  else if (freshLinensDeliveredButtonStateA == 1)
    Serial.println("Pin 31 is High");
  else if (needTrashPickedUpButtonStateB == 1)
    Serial.println("Pin 34 is High");
  else if (trashHasBeenPickedUpButtonStateB == 1)
    Serial.println("Pin 35 is High");
  else if (needLaundryPickedUpButtonStateB == 1)
    Serial.println("Pin 36 is High");
  else if (laundryHasBeenPickedUpButtonStateB == 1)
    Serial.println("Pin 37 is High");
  else if (needFreshLinensButtonStateB == 1)
    Serial.println("Pin 40 is High");
  else if (freshLinensDeliveredButtonStateB == 1)
    Serial.println("Pin 41 is High");
  /*
  else if(needTrashPickedUpButtonStateC == 1)
    Serial.println("Pin 44 is High");
  else if(trashHasBeenPickedUpButtonStateC == 1)
    Serial.println("Pin 45 is High");
  else if(needLaundryPickedUpButtonStateC == 1)
    Serial.println("Pin 46 is High");
  else if(laundryHasBeenPickedUpButtonStateC == 1)
    Serial.println("Pin 47 is High");
  else if(needFreshLinensButtonStateC == 1)
    Serial.println("Pin 18 is High");
  else if(freshLinensDeliveredButtonStateC == 1)
    Serial.println("Pin 19 is High");
  */

  // Trash Handling Buttons for Pod A or North

  if (needTrashPickedUpButtonStateA == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenA, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[2], locations[25], subjects[0], bodies[0] ))
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedA, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedA, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedA, LOW);
      delay(100);
      setupCommunications();
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      errorState = 0;
    }
  }
  else  if (trashHasBeenPickedUpButtonStateA == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenA, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[2], locations[25], subjects[1], bodies[1] ))
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedA, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedA, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedA, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      errorState = 0;
    }
  }
  //Laundry Handling Buttons for Pod A or North

  if (needLaundryPickedUpButtonStateA == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenA, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[3], locations[25], subjects[2], bodies[2] ))
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedA, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedA, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedA, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      errorState = 0;
    }
  }
  else if (laundryHasBeenPickedUpButtonStateA == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenA, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[4], locations[25], subjects[3], bodies[3] ))
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedA, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedA, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedA, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenA, LOW);
      delay(100);
      errorState = 0;
    }
  }
  //Clean Utility for Pod A / North

  if (needFreshLinensButtonStateA == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(cleanUtilityGreenA, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (cleanUtilityErrorStatus = sendEmail(recipients[5], locations[25], subjects[4], bodies[4] ))
    {
      digitalWrite(cleanUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityRedA, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(cleanUtilityRedA, LOW);  // blink twice.
      delay(100);
      digitalWrite(cleanUtilityRedA, HIGH);
      delay(100);
      digitalWrite(cleanUtilityRedA, LOW);
      delay(100);
      cleanUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(cleanUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenA, LOW);
      delay(100);
      digitalWrite(cleanUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenA, LOW);
      delay(100);
      errorState = 0;
    }
  }
  if (freshLinensDeliveredButtonStateA == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(cleanUtilityGreenA, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (cleanUtilityErrorStatus = sendEmail(recipients[6], locations[25], subjects[5], bodies[5] ))
    {
      digitalWrite(cleanUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityRedA, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(cleanUtilityRedA, LOW);  // blink twice.
      delay(100);
      digitalWrite(cleanUtilityRedA, HIGH);
      delay(100);
      digitalWrite(cleanUtilityRedA, LOW);
      delay(100);
      cleanUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(cleanUtilityGreenA, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenA, LOW);
      delay(100);
      digitalWrite(cleanUtilityGreenA, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenA, LOW);
      delay(100);
      errorState = 0;
    }
  }


  // ***************************************************************************
  // ********************* POD B / SOUTH ***************************************
  // ***************************************************************************



  // Trash Handling Buttons for Pod B or South

  if (needTrashPickedUpButtonStateB == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenB, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[2], locations[25], subjects[0], bodies[0] ))
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedB, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedB, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedB, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      errorState = 0;
    }
  }
  else  if (trashHasBeenPickedUpButtonStateB == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenB, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[2], locations[25], subjects[1], bodies[1] ))
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedB, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedB, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedB, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      errorState = 0;
    }
  }
  //Laundry Handling Buttons for Pod B or South

  if (needLaundryPickedUpButtonStateB == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenB, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[3], locations[25], subjects[2], bodies[2] ))
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedB, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedB, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedB, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      errorState = 0;
    }
  }
  else if (laundryHasBeenPickedUpButtonStateB == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenB, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[4], locations[25], subjects[3], bodies[3] ))
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedB, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedB, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedB, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenB, LOW);
      delay(100);
      errorState = 0;
    }
  }
  //Clean Utility for Pod B / South

  if (needFreshLinensButtonStateB == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(cleanUtilityGreenB, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (cleanUtilityErrorStatus = sendEmail(recipients[5], locations[25], subjects[4], bodies[4] ))
    {
      digitalWrite(cleanUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityRedB, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(cleanUtilityRedB, LOW);  // blink twice.
      delay(100);
      digitalWrite(cleanUtilityRedB, HIGH);
      delay(100);
      digitalWrite(cleanUtilityRedB, LOW);
      delay(100);
      cleanUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(cleanUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenB, LOW);
      delay(100);
      digitalWrite(cleanUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenB, LOW);
      delay(100);
      errorState = 0;
    }
  }
  if (freshLinensDeliveredButtonStateB == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(cleanUtilityGreenB, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (cleanUtilityErrorStatus = sendEmail(recipients[6], locations[25], subjects[5], bodies[5] ))
    {
      digitalWrite(cleanUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityRedB, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(cleanUtilityRedB, LOW);  // blink twice.
      delay(100);
      digitalWrite(cleanUtilityRedB, HIGH);
      delay(100);
      digitalWrite(cleanUtilityRedB, LOW);
      delay(100);
      cleanUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(cleanUtilityGreenB, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenB, LOW);
      delay(100);
      digitalWrite(cleanUtilityGreenB, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenB, LOW);
      delay(100);
      errorState = 0;
    }
  }


/*
  // ***************************************************************************
  // ************************** POD C ******************************************
  // ***************************************************************************


  // Trash Handling Buttons for Pod C

  if (needTrashPickedUpButtonStateC == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenC, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[2], locations[2], subjects[0], bodies[0] ))
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedC, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedC, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedC, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      errorState = 0;
    }
  }
  else  if (trashHasBeenPickedUpButtonStateC == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenC, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[2], locations[2], subjects[1], bodies[1] ))
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedC, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedC, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedC, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      errorState = 0;
    }
  }
  //Laundry Handling Buttons for Pod C

  if (needLaundryPickedUpButtonStateC == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenC, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[3], locations[2], subjects[2], bodies[2] ))
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedC, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedC, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedC, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      errorState = 0;
    }
  }
  else if (laundryHasBeenPickedUpButtonStateC == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(dirtyUtilityGreenC, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (dirtyUtilityErrorStatus = sendEmail(recipients[4], locations[2], subjects[3], bodies[3] ))
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityRedC, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(dirtyUtilityRedC, LOW);  // blink twice.
      delay(100);
      digitalWrite(dirtyUtilityRedC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityRedC, LOW);
      delay(100);
      dirtyUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(dirtyUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(dirtyUtilityGreenC, LOW);
      delay(100);
      errorState = 0;
    }
  }
  //Clean Utility for Pod C

  if (needFreshLinensButtonStateC == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(cleanUtilityGreenC, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (cleanUtilityErrorStatus = sendEmail(recipients[5], locations[2], subjects[4], bodies[4] ))
    {
      digitalWrite(cleanUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityRedC, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(cleanUtilityRedC, LOW);  // blink twice.
      delay(100);
      digitalWrite(cleanUtilityRedC, HIGH);
      delay(100);
      digitalWrite(cleanUtilityRedC, LOW);
      delay(100);
      cleanUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(cleanUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenC, LOW);
      delay(100);
      digitalWrite(cleanUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenC, LOW);
      delay(100);
      errorState = 0;
    }
  }
  if (freshLinensDeliveredButtonStateC == HIGH)
  {
    Serial.println("Trying to Send Email");
    digitalWrite(cleanUtilityGreenC, HIGH);  // Turn On LED to show "Sending"
    testConnection();

    if (cleanUtilityErrorStatus = sendEmail(recipients[6], locations[2], subjects[5], bodies[5] ))
    {
      digitalWrite(cleanUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityRedC, HIGH); // If the sendEmail() Function returns 1
      delay(100);                           // to indicate a failure the RED led will
      digitalWrite(cleanUtilityRedC, LOW);  // blink twice.
      delay(100);
      digitalWrite(cleanUtilityRedC, HIGH);
      delay(100);
      digitalWrite(cleanUtilityRedC, LOW);
      delay(100);
      cleanUtilityErrorStatus = 0;
    }
    else
    {
      digitalWrite(cleanUtilityGreenC, LOW); //Turn the GREEN led off to indicate end of sending;
      delay(WAIT);
      digitalWrite(cleanUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenC, LOW);
      delay(100);
      digitalWrite(cleanUtilityGreenC, HIGH);
      delay(100);
      digitalWrite(cleanUtilityGreenC, LOW);
      delay(100);
      errorState = 0;
    }
  }
*/


  else
  {
    //Serial.println("Turning LED off");
    //This will reset the Red Power/Ready LED
    // digitalWrite(dirtyUtilityGreenA, LOW);
    // digitalWrite(dirtyUtilityRedA, HIGH);
    // digitalWrite(cleanUtilityGreenA, LOW);
    // digitalWrite(cleanUtilityRedA, HIGH);
  }

}

void setupCommunications()
{
  Serial.println("Setting up Communication...");
  byte byteIPByte;
  //Ethernet.begin(mac, ip, subnet, gateway);
  Serial.println("Setting up IP with DHCP...");
  int i = 0;
  while ( !Ethernet.begin(mac) && i < 2)
  {
    Serial.println("\nConnection Failed.");
    Serial.println("Let me try that again...\n");
    i++;
  }

  if ( i > 1 )
  {
    errorState = 1;
  }



  delay(WAIT);

  Serial.print("\nMy IP address: ");
  for (byteIPByte = 0; byteIPByte < 4; byteIPByte++)
  {
    // Print the value of each byte of the IP address.
    Serial.print(Ethernet.localIP()[byteIPByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("***************************************");
  Serial.println();

}




int sendEmail(String Recipient, String Sender, String Subject, String Body)
{
  Serial.println("Connecting to the SMTP server ...");

  //
  //  If successful connecting to the SMTP server on
  //  port 587, then pass it the information needed
  //  to send the email message.  Otherwise, display
  //  a failure message for debugging.
  //
  if (client.connect(smtpServerIP, 25))
  {
    Serial.println("Connected to the SMTP server ...");

    ethernetOut("HELO " + smtpServerName); /* say hello*/
    delay(10000);
    ethernetOut("MAIL From:<" + Sender + "@phn-waco.org>"); /* identify sender */
    ethernetOut("RCPT To:<" + Recipient + ">"); /* identify recipient */
    ethernetOut("DATA");
    ethernetOut("To: " + Recipient); /*  recipient in message header */
    ethernetOut("From: " + Sender); /* seder name in message header */
    ethernetOut("Subject: " + Subject); /* insert subject */
    ethernetOut(""); /* empty line */
    ethernetOut(Body); /* insert body */
    ethernetOut(""); /* empty line */
    ethernetOut("."); /* end mail */
    ethernetOut("QUIT"); /* terminate connection */
    client.println();
    delay(WAIT);
    return 0;
  }
  else
  {
    Serial.println("Connection to SMTP server failed... ");
    setupCommunications();
    return 1;
  }
}

void ethernetOut(String m) {
  // Write the string to the ethernet port.
  client.println(m);
  // Display the string that was written.
  Serial.println(">>>" + m);
  // Pause in order to wait for a response.
  delay(WAIT);
  // Read and display any response.
  getResponse();
}

void getResponse() {
  if (client.available()) {
    char c = client.read();
    while (client.available()) { // Store command char by char.
      ServerResponse += c;
      c = client.read();
    }
    Serial.println("<<<" + ServerResponse);
    ServerResponse = "";
  }
}

int testConnection()
{
  if (!client.connected())
  {
    Serial.println();
    Serial.println("Port closed ... reconnecting ...");
    client.stop();
    delay(WAIT);
    return 1;
    setupCommunications();
  }
  else
  {
    return 0;
  }
}



