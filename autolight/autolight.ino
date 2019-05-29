#define PIN_IN_MOOVER 5
#define PIN_IN_PHOTO A3
#define PIN_OUT 7
//PROD: 25
#define DARK_UNDER 50
#define STATUS_DARK 4
#define STATUS_MOVE 3
//TEST: 3 || PROD: 8
#define MAX_SHININGS 4

//TEST: 5 || PROD: 7200
#define MOVEMENT_TRIES 7200

long minutes(int minutes) {
  log("calculator", "calculating minutes...");
  return 60l * 1000 * minutes;
}

long seconds(int seconds) {
  log("calculator", "calculating seconds...");
  return 1000l * seconds;
}

long TIME_ON;
long DARK_TIME;
long MOVEMENT_TIME;
int tryTimes = 0;
long timesOn = 0;
String message;

void setup()
{
  Serial.begin(9600);  //Begin serial communcation

  //TEST: seconds(10); | PROD: minutes(30);
  TIME_ON = minutes(30);
  //TEST: seconds(4); | PROD: minutes(10);
  DARK_TIME = minutes(10);
  MOVEMENT_TIME = seconds(1);

  message = "max-shinings: ";
  message.concat(MAX_SHININGS);
  log("setup", message);

  pinMode(PIN_IN_MOOVER, INPUT);
  pinMode(PIN_IN_PHOTO, INPUT);
  pinMode(PIN_OUT, OUTPUT);
  pinMode(STATUS_DARK, OUTPUT);
  pinMode(STATUS_MOVE, OUTPUT);
  log("setup", "finfished setup");
}

void loop() {
  Serial.println("------------------------------------------------------");
  if (isDark()) {
    if (isMovement() && notMaxTries(tryTimes) && notMaxShinings(timesOn)) {
      timesOn++;
      on();
      delay(TIME_ON);
    } else {
      off();
      delay(MOVEMENT_TIME);
      if (tryTimes < MOVEMENT_TRIES)
        tryTimes++;
    }
  } else {
    off();
    delay(DARK_TIME);
    tryTimes = 0;
    timesOn = 0;
  }
  isDark();
  isMovement();
}

int notMaxShinings(int shinings) {
  message = "";
  message.concat(shinings);
  message.concat(" < ");
  message.concat(MAX_SHININGS);
  message.concat("?");
  log("max-shinings-checker", message);
  return shinings < MAX_SHININGS;
}

int notMaxTries(int triedTimes) {
  message = "";
  message.concat(triedTimes);
  message.concat(" < ");
  message.concat(MOVEMENT_TRIES);
  message.concat("?");
  log("max-try-checker", message);
  if (tryTimes == MOVEMENT_TRIES) {
    log("info", "Light is off until darkness is over.");
  }
  return tryTimes < MOVEMENT_TRIES;
}

int movementAvailable;

int isMovement() {
  movementAvailable = digitalRead(PIN_IN_MOOVER);
  log("movement-checker", movementAvailable ? "Movement detected." : "No movement detected.");
  digitalWrite(STATUS_MOVE, movementAvailable);
  return movementAvailable;
}

int photoValue;
int dark;

int isDark() {
  photoValue = analogRead(PIN_IN_PHOTO);
  dark = photoValue < DARK_UNDER;
  log("darkness-checker", dark ? "It's dark" : "It's not dark");
  digitalWrite(STATUS_DARK, dark);
  return dark;
}

int wasOn = false;

void on() {
  if (!wasOn) {
    wasOn = true;
    log("light", "setting light on");
    digitalWrite(PIN_OUT, HIGH);
  } else {
    log("light", "light is on already");
  }
}

void off() {
  if (wasOn) {
    wasOn = false;
    log("light", "setting light off");
    digitalWrite(PIN_OUT, LOW);
  } else {
    log("light", "light is off already");
  }
}

String notification;

void log(String sender, String message) {
  notification = "[";
  sender.toUpperCase();
  notification.concat(sender);
  notification.concat("]: ");
  notification.concat(message);
  Serial.println(notification);
}
