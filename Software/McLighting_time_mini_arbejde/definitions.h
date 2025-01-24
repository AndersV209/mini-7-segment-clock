// Neopixel
#define PIN 3          // PIN where neopixel / WS2811 strip is attached
#define NUMLEDS 86      // Number of leds in the strip


#define HOSTNAME "7segmentclock_mini_work_big"     // Friedly hostname
#define AP_NAME "ConfigAP-7SClock_mini_work_big"   // Friedly Autoconfig ap name 
#define websocketport 81             //Port for websocket 

//#define debugging   //define this to activate debuging 

// ***************************************************************************
// Global variables / definitions
// ***************************************************************************
//#define Serial Serial  // Set debug output port

// List of all color modes
enum MODE { OFF, ALL, RAINBOW};

MODE mode = ALL;   // Standard mode that is active when software starts

int brightness = 255;       // Global variable for storing the brightness (255 == 100%)

int rainbow_swipe = 0;      // variable to hold number from 0-255 for the rainbow function 
int rainbow_delay = 50;     // varaibale to hold delay for rainbow color change 
int analogvalue = 0;        // variable to hold value read from ldr resistor. 

bool exit_func = false;     // Global helper variable to get out of the color modes when mode changes

struct ledstate             // Data structure to store a state of a single led
{
   uint8_t red = 0;
   uint8_t green = 0;
   uint8_t blue = 255; //40;
};

typedef struct ledstate LEDState;   // Define the datatype LEDState
LEDState main_color;                // Store the "main color" of the strip used in single color modes 


//clock part 
const bool analogEnable = false;  //enable night mode 
const int analogInPin = A0;  // Analog input pin that the LDR is attached to

const int lightvalue = 200;  // value for light to dim 

//array to hold the pixels for each digit in order of 3 pixels for segment a-a-a-b-b-b-c-c-c-d-d-d-e-e-e-f-f-f-g-g-g
int digit1[21] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
int digit2[21] = {21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41};
int digit3[21] = {42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62};
int digit4[21] = {63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83};
//array for hold pixels for the colon 
int colon[2] = {84,85};

//Outgoing value for colors 
int Red = 0;    
int Green = 0;
int Blue = 0;

int nighttime_count = 0;    //Variable to hold how many seconds light have been under "lightvalue"

int j = 0; int i = 0; int h = 0;
int g = 0; int k = 0; int l = 0;

bool led_state_change = true;      //state to check if led need to be updated. 

//Clock settings
/* ******** NTP Server Settings ******** */
/*   (Set to your time server of choice) */
//IPAddress timeServer(132, 163, 97, 1);  // time-a-wwv.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
bool time_is_set = false;

/* Set this to the offset (in seconds) to your local time
   This example is GMT + 1 (Denmark) */
const long timeZoneOffset = 3600L; 

//Denmark summer vinter time
TimeChangeRule myDST = {"Vin", Last, Sun, Oct, 3, 0};    //winter time
TimeChangeRule mySTD = {"Som", Last, Sun, Mar, 2, -60};     //sommer time
Timezone myTZ(myDST, mySTD);

//If TimeChangeRules are already stored in EEPROM, comment out the three
//lines above and uncomment the line below.
//Timezone myTZ(100);       //assumes rules stored at EEPROM address 100

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
time_t utc, local;

/* Syncs to NTP server every 15 seconds for testing,
   set to 1 hour or more to be reasonable. Value is in seconds */
unsigned int ntpSyncTime = 3600; //Seconds
const unsigned int ntpSyncTime_not_set = 300;

//NTP recieve 
/* ALTER THESE VARIABLES AT YOUR OWN RISK */
unsigned int localPort = 8888;             // local port to listen for UDP packets
const int NTP_PACKET_SIZE= 48;             // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];        // Buffer to hold incoming and outgoing packets                          
WiFiUDP udp;                               // A UDP instance to let us send and receive packets over UDP
unsigned long ntpLastUpdate = 0;           // Keeps track of how long ago we updated the NTP server
//time_t prevDisplay = 0;                    // Check last time clock displayed (Not in Production)

//initilizing variables for clock
int hours = 0;
int minutes = 0;
int seconds = 0;

//end clock setup

#define NBREG   9

//pauser
typedef struct {
  const int week_day;
  const int start_hour;
  const int start_minut;
  const int stop_hour;
  const int stop_minut;
  const int duration;
} workbreak_struct;


//volatile
workbreak_struct workbreak[NBREG] = {
  {1, 9, 0, 9, 20, 20},    // mandag  fra  9:00 til  9:20
  {1, 12, 0, 12, 20, 20},  // mandag  fra 12:00 til 12:20
  {2, 9, 0, 9, 20, 20},    // tirsdag fra  9:00 til  9:20
  {2, 12, 0, 12, 20, 20},  // tirsdag fra 12:00 til 12:20
  {3, 9, 0, 9, 20, 20},    // onsdag  fra  9:00 til  9:20
  {3, 12, 0, 12, 20, 20},  // onsdag  fra 12:00 til 12:20
  {4, 9, 0, 9, 20, 20},    // torsdag fra  9:00 til  9:20
  {4, 12, 0, 12, 20, 20},  // torsdag fra 12:00 til 12:20
  {5, 9, 0, 9, 30, 30},    // fredag  fra  9:00 til  9:30
  //{6, 17, 5, 17, 10, 2},// test 


};

bool check_state[NBREG] = { false };
int last_minute = 0;
