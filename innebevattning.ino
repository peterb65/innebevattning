#include <Metro.h>

#define PUMP 11
#define SECOND 1000
#define MINUTE 60 * SECOND
#define HOUR 60 * MINUTE
#define PUMP_INTERVAL 24 * 60 // 24 * 60 minutes
#define PUMP_TIME 20 * SECOND
#define SIGN_OF_LIFE_INTERVAL 10 SECOND

Metro minutes = Metro(MINUTE);
Metro pumping = Metro(PUMP_TIME);
int pumpState = LOW;
long pumpIntervalTime = PUMP_INTERVAL;
long pumpCountDown = pumpIntervalTime;
long pumpTime = PUMP_TIME;

#define CMD_CMD 0
#define CMD_ARG 1
#define CMD_OK 2
#define CMD_ERR 99

char cmd;
int intArg;
char charArg;
byte cmdState = 0;

void setup(void) {
   pinMode(PUMP, OUTPUT);
   digitalWrite(PUMP, LOW);
   Serial.begin(57600);
   Serial.println("Bevattning 2.1");
   signOfLife();
}

void signOfLife() {
  pinMode(LED_BUILTIN, OUTPUT);
  boolean state = LOW;
  for (int i=0; i<20; i++) {
    digitalWrite(LED_BUILTIN, state);
    state = !state;
    delay(25);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop(void) {
  if (minutes.check() && pumpState == LOW && pumpCountDown-- == 0) {
    Serial.println("Pump ON");
    pumping.reset();
    setPump(HIGH);
  }
  if (pumping.check() && pumpState == HIGH) {
    Serial.println("Pump OFF");
    setPump(LOW);
    pumpCountDown = pumpIntervalTime;
  }
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch > 'Z' || ch < '0') {
     return;
    } 
    switch (cmdState) {
      case CMD_CMD:
        cmd = ch;
Serial.print("Command: "); Serial.println(cmd);
        intArg = 0;
        cmdState = CMD_ARG;
        break;
      case CMD_ARG:
Serial.print("arg "); Serial.println(ch);
        if (ch == ';') {
          cmdState = CMD_OK;
          executeCommand();
        } else {
          intArg = intArg * 10 + ch;
        }
        break;
    }
  }
}

void executeCommand() {
  if (cmd == '?') {
    Serial.println("Info");
    Serial.print("Interval:");
    Serial.println(pumpIntervalTime);
    Serial.print("Pumptime:");
    Serial.println(pumpTime);
    Serial.print("Pump is ");
    Serial.println(pumpState == 0 ? "OFF" : "ON");
  }
  else if (cmd == 'I') {
    Serial.print("Set interval to ");
    Serial.println(intArg);
  }
  else if (cmd == 'T') {
    Serial.print("Set pump time to ");
    Serial.println(intArg);
  }
  else if (cmd == 'P') {
    Serial.print("Manual set pump ");
    Serial.println(intArg == 0 ? "OFF" : "ON");
  }
  cmdState = CMD_CMD;
}

void setPump(int pState) {
  pumpState = pState;
  digitalWrite(PUMP, pumpState);
  Serial.println(millis());
}

