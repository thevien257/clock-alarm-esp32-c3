#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <Nextion.h>

#define BUZZER 7
#define AT24C32_ADDR 0x50

// Create serial for Nextion
SoftwareSerial nextionSerial(18, 19);  // RX, TX

// Page 0
NexText tTime = NexText(0, 1, "tTime");
NexText tDate = NexText(0, 4, "tDate");
NexText tAlarm = NexText(0, 3, "tAlarm");
NexText tTemp = NexText(0, 5, "tTemp");
NexButton bAlarm = NexButton(0, 6, "bAlarm");
NexButton bDate = NexButton(0, 7, "bDate");
NexButton bTime = NexButton(0, 5, "bTime");
NexButton bStop = NexButton(0, 9, "bStop");

// Page 1 - Time Setting
NexText tHour = NexText(1, 1, "tHour");
NexText tMin = NexText(1, 3, "tMin");
NexText tSec = NexText(1, 5, "tSec");
NexButton bPlusHour = NexButton(1, 6, "bPlusHour");
NexButton bPlusMin = NexButton(1, 7, "bPlusMin");
NexButton bPlusSec = NexButton(1, 8, "bPlusSec");
NexButton bMinusHour = NexButton(1, 9, "bMinusHour");
NexButton bMinusMin = NexButton(1, 10, "bMinusMin");
NexButton bMinusSec = NexButton(1, 11, "bMinusSec");
NexButton bSave = NexButton(1, 13, "bSave");
NexButton bHome = NexButton(1, 12, "bHome");
NexText tStop = NexText(1, 8, "tStop");

// Page 2 - Date Setting
NexText tDay = NexText(2, 1, "tDay");
NexText tMonth = NexText(2, 3, "tMonth");
NexText tYear = NexText(2, 5, "tYear");
NexButton bPlusDay = NexButton(2, 6, "bPlusDay");
NexButton bPlusMonth = NexButton(2, 7, "bPlusMonth");
NexButton bPlusYear = NexButton(2, 8, "bPlusYear");
NexButton bMinusDay = NexButton(2, 11, "bMinusDay");
NexButton bMinusMonth = NexButton(2, 10, "bMinusMonth");
NexButton bMinusYear = NexButton(2, 9, "bMinusYear");
NexButton bSavePage2 = NexButton(2, 12, "bSave");
NexButton bHomePage2 = NexButton(2, 13, "bHome");

// Page 3 - Alarm List
NexButton bHomePage3 = NexButton(3, 14, "bHome");
NexButton bAdd = NexButton(3, 1, "bAdd");
NexButton b5 = NexButton(3, 12, "b5");
NexButton b6 = NexButton(3, 13, "b6");
NexButton b0 = NexButton(3, 7, "b0");
NexButton b1 = NexButton(3, 8, "b1");
NexButton b2 = NexButton(3, 9, "b2");
NexButton b3 = NexButton(3, 10, "b3");
NexButton b4 = NexButton(3, 11, "b4");
NexText t0 = NexText(3, 2, "t0");
NexText t1 = NexText(3, 3, "t1");
NexText t2 = NexText(3, 4, "t2");
NexText t3 = NexText(3, 5, "t3");
NexText t4 = NexText(3, 6, "t4");

// Page 4 - Add Alarm
NexText tHourPage4 = NexText(4, 1, "tHour");
NexText tMinPage4 = NexText(4, 3, "tMin");
NexButton bSavePage4 = NexButton(4, 22, "bSave");
NexButton bHomePage4 = NexButton(4, 23, "bHome");
NexButton bPlusHourPage4 = NexButton(4, 4, "bPlusHour");
NexButton bPlusMinPage4 = NexButton(4, 5, "bPlusMin");
NexButton bMinusHourPage4 = NexButton(4, 6, "bMinusHour");
NexButton bMinusMinPage4 = NexButton(4, 7, "bMinusMin");
NexCheckbox c1 = NexCheckbox(4, 8, "c1");   // Sunday
NexCheckbox c2 = NexCheckbox(4, 10, "c2");  // Monday
NexCheckbox c3 = NexCheckbox(4, 12, "c3");  // Tuesday
NexCheckbox c4 = NexCheckbox(4, 14, "c4");  // Wednesday
NexCheckbox c5 = NexCheckbox(4, 16, "c5");  // Thursday
NexCheckbox c6 = NexCheckbox(4, 18, "c6");  // Friday
NexCheckbox c7 = NexCheckbox(4, 20, "c7");  // Saturday

// Complete touch event list
NexTouch *nex_listen_list[] = {
  &bAlarm, &bDate, &bTime,
  &bPlusHour, &bPlusMin, &bPlusSec, &bMinusHour, &bMinusMin, &bMinusSec, &bHome, &bSave, &bStop,
  &bPlusDay, &bPlusMonth, &bPlusYear, &bMinusDay, &bMinusMonth, &bMinusYear, &bSavePage2, &bHomePage2,
  &bHomePage3, &bAdd, &b0, &b1, &b2, &b3, &b4, &b5, &b6,
  &bSavePage4, &bHomePage4, &bPlusHourPage4, &bPlusMinPage4, &bMinusHourPage4, &bMinusMinPage4,
  &c1, &c2, &c3, &c4, &c5, &c6, &c7,
  NULL
};

// Forward declarations
void bAlarmPop(void *ptr);
void bDatePop(void *ptr);
void bTimePop(void *ptr);
void bPlusHourPop(void *ptr);
void bPlusMinPop(void *ptr);
void bPlusSecPop(void *ptr);
void bMinusHourPop(void *ptr);
void bMinusMinPop(void *ptr);
void bMinusSecPop(void *ptr);
void bSavePop(void *ptr);
void bHomePop(void *ptr);
void bStopPop(void *ptr);
void bPlusDayPop(void *ptr);
void bPlusMonthPop(void *ptr);
void bPlusYearPop(void *ptr);
void bMinusDayPop(void *ptr);
void bMinusMonthPop(void *ptr);
void bMinusYearPop(void *ptr);
void bSavePage2Pop(void *ptr);
void bHomePage2Pop(void *ptr);
void bHomePage3Pop(void *ptr);
void bAddPop(void *ptr);
void b0Pop(void *ptr);
void b1Pop(void *ptr);
void b2Pop(void *ptr);
void b3Pop(void *ptr);
void b4Pop(void *ptr);
void b5Pop(void *ptr);
void b6Pop(void *ptr);
void bSavePage4Pop(void *ptr);
void bHomePage4Pop(void *ptr);
void bPlusHourPage4Pop(void *ptr);
void bPlusMinPage4Pop(void *ptr);
void bMinusHourPage4Pop(void *ptr);
void bMinusMinPage4Pop(void *ptr);
void c1Pop(void *ptr);
void c2Pop(void *ptr);
void c3Pop(void *ptr);
void c4Pop(void *ptr);
void c5Pop(void *ptr);
void c6Pop(void *ptr);
void c7Pop(void *ptr);

// Global variables
int editHour = 0, editMin = 0, editSec = 0;
bool timeEditMode = false;
int editDay = 1, editMonth = 1, editYear = 2025;
bool dateEditMode = false;
int editAlarmHour = 0, editAlarmMin = 0;
bool alarmEditMode = false;
bool editAlarmRepeatDays[7] = { false };
int alarmListOffset = 0;
int currentRingingAlarm = -1;

void sendCommand(String command) {
  nextionSerial.print(command);
  nextionSerial.write(0xff);
  nextionSerial.write(0xff);
  nextionSerial.write(0xff);
  Serial.println("Sent command: " + command);
}

struct Alarm {
  int hour, minute;
  bool active;
  DateTime nextTrigger;
  bool repeatDays[7];
};

class ClockAlarm {
private:
  RTC_DS1307 rtc;
  int buzzerPin;
  Alarm *alarms = nullptr;
  int alarmCount = 0;
  int alarmCapacity = 0;

  DateTime getNextValidTrigger(int hour, int minute, bool repeatDays[7], DateTime after) {
    // First check if today's alarm time hasn't passed and today is a valid repeat day
    DateTime todayAlarm(after.year(), after.month(), after.day(), hour, minute, 0);
    if (todayAlarm > after && repeatDays[after.dayOfTheWeek()]) {
      return todayAlarm;
    }

    // Otherwise, find the next valid repeat day
    for (int add = 1; add <= 7; add++) {
      DateTime candidate = after + TimeSpan(add, 0, 0, 0);
      if (repeatDays[candidate.dayOfTheWeek()]) {
        return DateTime(candidate.year(), candidate.month(), candidate.day(), hour, minute, 0);
      }
    }

    // Fallback (should never reach here if repeatDays has at least one true value)
    return after + TimeSpan(1, 0, 0, 0);
  }

public:
  int curDay, curMonth, curYear, curHour, curMinute, curSecond;
  String timeString = "", dateString = "", alarmString = "";

  ClockAlarm(int buzzer)
    : buzzerPin(buzzer), alarmCount(0) {
    curDay = curMonth = curYear = curHour = curMinute = curSecond = 0;
    alarms = new Alarm[alarmCapacity];
  }

  ~ClockAlarm() {
    delete[] alarms;
  }

  void begin(int sda = 4, int scl = 5) {
    Wire.begin(sda, scl);
    pinMode(buzzerPin, OUTPUT);
    if (!rtc.begin()) {
      Serial.println("RTC not found!");
      while (1)
        ;
    }
    if (!rtc.isrunning()) {
      Serial.println("RTC not running, need to set time.");
    }
    loadAlarmsFromEEPROM();
    Serial.println("Clock system initialized");
  }

  // Add this as a public method in ClockAlarm class
  bool isOnceOnlyAlarm(int index) {
    if (index < 0 || index >= alarmCount) return false;
    for (int d = 0; d < 7; d++) {
      if (alarms[index].repeatDays[d]) return false;  // Has repeat days
    }
    return true;  // No repeat days = once only
  }

  void showTime() {
    DateTime now = rtc.now();
    curDay = now.day();
    curMonth = now.month();
    curYear = now.year();
    curHour = now.hour();
    curMinute = now.minute();
    curSecond = now.second();

    char bufTime[9], bufDate[40];
    sprintf(bufTime, "%02d:%02d:%02d", curHour, curMinute, curSecond);
    timeString = String(bufTime);

    sprintf(bufDate, "%s, %02d/%02d/%04d", dayOfWeekFull(now.dayOfTheWeek()), curDay, curMonth, curYear);
    dateString = String(bufDate);

    updateAlarmString();
  }

  void setTimeFromEdit(int hour, int minute, int second) {
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, second));
    Serial.printf("Time set to %02d:%02d:%02d\n", hour, minute, second);
  }

  void setDateFromEdit(int day, int month, int year) {
    DateTime now = rtc.now();
    rtc.adjust(DateTime(year, month, day, now.hour(), now.minute(), now.second()));
    Serial.printf("Date set to %02d/%02d/%04d\n", day, month, year);
  }

  DateTime getCurrentTime() {
    return rtc.now();
  }

  int getDaysInMonth(int month, int year) {
    switch (month) {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12: return 31;
      case 4:
      case 6:
      case 9:
      case 11: return 30;
      case 2:
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) return 29;
        return 28;
      default: return 31;
    }
  }

  int getAlarmCount() {
    return alarmCount;
  }

  Alarm *getAlarm(int index) {
    return (index >= 0 && index < alarmCount) ? &alarms[index] : nullptr;
  }

  void addAlarmFromGUI(int hour, int minute, bool repeatDays[7]) {
    if (alarmCount >= alarmCapacity) expandAlarms();

    DateTime now = rtc.now();
    DateTime candidate;

    // Check if any repeat days are selected
    bool hasAnyRepeat = false;
    for (int i = 0; i < 7; i++) {
      if (repeatDays[i]) {
        hasAnyRepeat = true;
        break;
      }
    }

    if (hasAnyRepeat) {
      // For repeating alarms, find the next valid day
      candidate = getNextValidTrigger(hour, minute, repeatDays, now);
    } else {
      // For one-time alarms, use the old logic
      candidate = DateTime(now.year(), now.month(), now.day(), hour, minute, 0);
      if (candidate <= now) candidate = candidate + TimeSpan(1, 0, 0, 0);
    }

    Alarm a;
    a.hour = hour;
    a.minute = minute;
    a.active = true;
    a.nextTrigger = candidate;
    for (int i = 0; i < 7; i++) a.repeatDays[i] = repeatDays[i];

    alarms[alarmCount] = a;
    Serial.printf("Alarm #%d set for %02d:%02d, next trigger: %02d/%02d/%04d %02d:%02d\n",
                  alarmCount, hour, minute,
                  candidate.day(), candidate.month(), candidate.year(),
                  candidate.hour(), candidate.minute());
    alarmCount++;
    saveAlarmsToEEPROM();
  }

  void deleteAlarmByIndex(int index) {
    if (index < 0 || index >= alarmCount) return;
    for (int i = index; i < alarmCount - 1; i++) alarms[i] = alarms[i + 1];
    alarmCount--;
    shrinkAlarms();
    saveAlarmsToEEPROM();
    Serial.printf("Deleted alarm at index %d\n", index);
  }

  void checkAlarms() {
    DateTime now = rtc.now();
    for (int i = 0; i < alarmCount; i++) {
      if (alarms[i].active && now.hour() == alarms[i].nextTrigger.hour() && now.minute() == alarms[i].nextTrigger.minute() && now.second() == 0 && now.day() == alarms[i].nextTrigger.day() && now.month() == alarms[i].nextTrigger.month() && now.year() == alarms[i].nextTrigger.year()) {
        Serial.printf("ALARM #%d: %02d:%02d!!!\n", i, alarms[i].hour, alarms[i].minute);
        tStop.setText("PRESS TO STOP");
        digitalWrite(buzzerPin, HIGH);
        currentRingingAlarm = i;  // Track which alarm is ringing

        if (hasRepeat(i)) {
          alarms[i].nextTrigger = getNextRepeat(i, now);
        } else {
          alarms[i].active = false;
        }
        break;  // Only handle one alarm at a time
      }
    }
  }

private:
  void expandAlarms() {
    int newCap = alarmCapacity + 1;
    Alarm *newAlarms = new Alarm[newCap];
    for (int i = 0; i < alarmCount; i++) newAlarms[i] = alarms[i];
    delete[] alarms;
    alarms = newAlarms;
    alarmCapacity = newCap;
    Serial.printf("Expanded alarm array to %d slots\n", alarmCapacity);
  }

  void shrinkAlarms() {
    if (alarmCount < alarmCapacity) {
      int newCap = max(alarmCount, 1);
      Alarm *newAlarms = new Alarm[newCap];
      for (int i = 0; i < alarmCount; i++) newAlarms[i] = alarms[i];
      delete[] alarms;
      alarms = newAlarms;
      alarmCapacity = newCap;
    }
  }

  void updateAlarmString() {
    if (alarmCount == 0) {
      alarmString = "No Alarm";
      return;
    }
    DateTime nextAlarm(2099, 12, 31, 23, 59, 59);
    int idx = -1;
    for (int i = 0; i < alarmCount; i++) {
      if (alarms[i].active && alarms[i].nextTrigger < nextAlarm) {
        nextAlarm = alarms[i].nextTrigger;
        idx = i;
      }
    }
    if (idx != -1) {
      char buf[20];
      sprintf(buf, "%02d:%02d %02d/%02d", alarms[idx].hour, alarms[idx].minute, nextAlarm.day(), nextAlarm.month());
      alarmString = String(buf);
    } else {
      alarmString = "No Active";
    }
  }

  bool hasRepeat(int idx) {
    for (int d = 0; d < 7; d++)
      if (alarms[idx].repeatDays[d]) return true;
    return false;
  }

  DateTime getNextRepeat(int idx, DateTime after) {
    for (int add = 1; add <= 7; add++) {
      DateTime candidate = after + TimeSpan(add, 0, 0, 0);
      if (alarms[idx].repeatDays[candidate.dayOfTheWeek()]) {
        return DateTime(candidate.year(), candidate.month(), candidate.day(), alarms[idx].hour, alarms[idx].minute, 0);
      }
    }
    return after + TimeSpan(1, 0, 0, 0);
  }

  const char *dayOfWeekFull(int dow) {
    switch (dow) {
      case 0: return "Sunday";
      case 1: return "Monday";
      case 2: return "Tuesday";
      case 3: return "Wednesday";
      case 4: return "Thursday";
      case 5: return "Friday";
      case 6: return "Saturday";
      default: return "Unknown";
    }
  }

  void eepromWriteBytes(int eeaddress, byte *data, int size) {
    const int pageSize = 32;
    int written = 0;
    while (written < size) {
      int toWrite = min(pageSize - (eeaddress % pageSize), size - written);
      Wire.beginTransmission(AT24C32_ADDR);
      Wire.write((eeaddress >> 8) & 0xFF);
      Wire.write(eeaddress & 0xFF);
      for (int i = 0; i < toWrite; i++) Wire.write(data[written + i]);
      Wire.endTransmission();
      delay(5);
      eeaddress += toWrite;
      written += toWrite;
    }
  }

  void eepromReadBytes(int eeaddress, byte *buffer, int size) {
    int readed = 0;
    while (readed < size) {
      int toRead = min(32, size - readed);
      Wire.beginTransmission(AT24C32_ADDR);
      Wire.write((eeaddress >> 8) & 0xFF);
      Wire.write(eeaddress & 0xFF);
      Wire.endTransmission();
      Wire.requestFrom(AT24C32_ADDR, toRead);
      for (int i = 0; i < toRead && Wire.available(); i++) {
        buffer[readed++] = Wire.read();
        eeaddress++;
      }
    }
  }

  void saveAlarmsToEEPROM() {
    eepromWriteBytes(0, (byte *)&alarmCount, sizeof(alarmCount));
    if (alarmCount > 0) eepromWriteBytes(4, (byte *)alarms, sizeof(Alarm) * alarmCount);
  }

  void loadAlarmsFromEEPROM() {
    eepromReadBytes(0, (byte *)&alarmCount, sizeof(alarmCount));
    if (alarmCount < 0) alarmCount = 0;
    if (alarmCount > alarmCapacity) {
      delete[] alarms;
      alarmCapacity = alarmCount;
      alarms = new Alarm[alarmCapacity];
    }
    if (alarmCount > 0) eepromReadBytes(4, (byte *)alarms, sizeof(Alarm) * alarmCount);
  }
};

ClockAlarm myClock(BUZZER);

void setup() {
  Serial.begin(9600);
  myClock.begin();
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  nextionSerial.begin(9600);
  nexInit();

  // Attach all button callbacks
  bAlarm.attachPush(bAlarmPop, &bAlarm);
  bDate.attachPush(bDatePop, &bDate);
  bTime.attachPush(bTimePop, &bTime);
  bPlusHour.attachPush(bPlusHourPop, &bPlusHour);
  bPlusMin.attachPush(bPlusMinPop, &bPlusMin);
  bPlusSec.attachPush(bPlusSecPop, &bPlusSec);
  bMinusHour.attachPush(bMinusHourPop, &bMinusHour);
  bMinusMin.attachPush(bMinusMinPop, &bMinusMin);
  bMinusSec.attachPush(bMinusSecPop, &bMinusSec);
  bSave.attachPush(bSavePop, &bSave);
  bHome.attachPush(bHomePop, &bHome);
  bStop.attachPush(bStopPop, &bStop);

  bPlusDay.attachPush(bPlusDayPop, &bPlusDay);
  bPlusMonth.attachPush(bPlusMonthPop, &bPlusMonth);
  bPlusYear.attachPush(bPlusYearPop, &bPlusYear);
  bMinusDay.attachPush(bMinusDayPop, &bMinusDay);
  bMinusMonth.attachPush(bMinusMonthPop, &bMinusMonth);
  bMinusYear.attachPush(bMinusYearPop, &bMinusYear);
  bSavePage2.attachPush(bSavePage2Pop, &bSavePage2);
  bHomePage2.attachPush(bHomePage2Pop, &bHomePage2);
  bHomePage3.attachPush(bHomePage3Pop, &bHomePage3);
  bAdd.attachPush(bAddPop, &bAdd);
  b0.attachPush(b0Pop, &b0);
  b1.attachPush(b1Pop, &b1);
  b2.attachPush(b2Pop, &b2);
  b3.attachPush(b3Pop, &b3);
  b4.attachPush(b4Pop, &b4);
  b5.attachPush(b5Pop, &b5);
  b6.attachPush(b6Pop, &b6);
  bSavePage4.attachPush(bSavePage4Pop, &bSavePage4);
  bHomePage4.attachPush(bHomePage4Pop, &bHomePage4);
  bPlusHourPage4.attachPush(bPlusHourPage4Pop, &bPlusHourPage4);
  bPlusMinPage4.attachPush(bPlusMinPage4Pop, &bPlusMinPage4);
  bMinusHourPage4.attachPush(bMinusHourPage4Pop, &bMinusHourPage4);
  bMinusMinPage4.attachPush(bMinusMinPage4Pop, &bMinusMinPage4);
  c1.attachPush(c1Pop, &c1);
  c2.attachPush(c2Pop, &c2);
  c3.attachPush(c3Pop, &c3);
  c4.attachPush(c4Pop, &c4);
  c5.attachPush(c5Pop, &c5);
  c6.attachPush(c6Pop, &c6);
  c7.attachPush(c7Pop, &c7);
}

void loop() {
  static unsigned long lastShowTime = 0;
  unsigned long nowMs = millis();

  if (!timeEditMode && !dateEditMode && !alarmEditMode) {
    if (nowMs - lastShowTime >= 1000) {
      lastShowTime = nowMs;
      myClock.showTime();
      tTime.setText(myClock.timeString.c_str());
      tDate.setText(myClock.dateString.c_str());
      // tAlarm.setText(myClock.alarmString.c_str());
      myClock.checkAlarms();
    }
  }
  nexLoop(nex_listen_list);
}

// Helper Functions
void updateTimeDisplay() {
  char buffer[10];
  sprintf(buffer, "%02d", editHour);
  tHour.setText(buffer);
  sprintf(buffer, "%02d", editMin);
  tMin.setText(buffer);
  sprintf(buffer, "%02d", editSec);
  tSec.setText(buffer);
}

void updateDateDisplay() {
  char buffer[10];
  sprintf(buffer, "%02d", editDay);
  tDay.setText(buffer);
  sprintf(buffer, "%02d", editMonth);
  tMonth.setText(buffer);
  sprintf(buffer, "%04d", editYear);
  tYear.setText(buffer);
}

void updateAlarmDisplay() {
  char buffer[10];
  sprintf(buffer, "%02d", editAlarmHour);
  tHourPage4.setText(buffer);
  sprintf(buffer, "%02d", editAlarmMin);
  tMinPage4.setText(buffer);
  c1.setValue(editAlarmRepeatDays[0] ? 1 : 0);
  c2.setValue(editAlarmRepeatDays[1] ? 1 : 0);
  c3.setValue(editAlarmRepeatDays[2] ? 1 : 0);
  c4.setValue(editAlarmRepeatDays[3] ? 1 : 0);
  c5.setValue(editAlarmRepeatDays[4] ? 1 : 0);
  c6.setValue(editAlarmRepeatDays[5] ? 1 : 0);
  c7.setValue(editAlarmRepeatDays[6] ? 1 : 0);
}

void updateAlarmListDisplay() {
  int alarmCount = myClock.getAlarmCount();
  t0.setText("");
  t1.setText("");
  t2.setText("");
  t3.setText("");
  t4.setText("");

  for (int i = 0; i < 5; i++) {
    int alarmIndex = alarmListOffset + i;
    if (alarmIndex < alarmCount) {
      Alarm *alarm = myClock.getAlarm(alarmIndex);
      if (alarm != nullptr) {
        char buffer[50];
        String repeatStr = "";
        bool hasRepeat = false;
        for (int d = 0; d < 7; d++) {
          if (alarm->repeatDays[d]) {
            if (hasRepeat) repeatStr += ",";
            const char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
            repeatStr += days[d];
            hasRepeat = true;
          }
        }
        if (!hasRepeat) repeatStr = "Once";

        sprintf(buffer, "%02d:%02d - %s", alarm->hour, alarm->minute, repeatStr.c_str());

        switch (i) {
          case 0: t0.setText(buffer); break;
          case 1: t1.setText(buffer); break;
          case 2: t2.setText(buffer); break;
          case 3: t3.setText(buffer); break;
          case 4: t4.setText(buffer); break;
        }
      }
    }
  }
  Serial.printf("Displaying alarms %d-%d of %d total\n", alarmListOffset, min(alarmListOffset + 4, alarmCount - 1), alarmCount);
}

// Page 0 Callbacks
void bAlarmPop(void *ptr) {
  Serial.println("Alarm button pressed!");
  alarmListOffset = 0;
  sendCommand("page AlarmList");
  updateAlarmListDisplay();
}

void bTimePop(void *ptr) {
  Serial.println("Time button pressed!");
  timeEditMode = true;
  DateTime now = myClock.getCurrentTime();
  editHour = now.hour();
  editMin = now.minute();
  editSec = now.second();
  sendCommand("page SetTime");
  updateTimeDisplay();
}

void bDatePop(void *ptr) {
  Serial.println("Date button pressed!");
  dateEditMode = true;
  DateTime now = myClock.getCurrentTime();
  editDay = now.day();
  editMonth = now.month();
  editYear = now.year();
  sendCommand("page SetDate");
  updateDateDisplay();
}

// Page 1 Time Setting Callbacks
void bPlusHourPop(void *ptr) {
  if (timeEditMode) {
    editHour++;
    if (editHour > 23) editHour = 0;
    char buffer[10];
    sprintf(buffer, "%02d", editHour);
    tHour.setText(buffer);
    Serial.printf("Hour: %02d\n", editHour);
  }
}

void bPlusMinPop(void *ptr) {
  if (timeEditMode) {
    editMin++;
    if (editMin > 59) editMin = 0;
    char buffer[10];
    sprintf(buffer, "%02d", editMin);
    tMin.setText(buffer);
    Serial.printf("Minute: %02d\n", editMin);
  }
}

void bPlusSecPop(void *ptr) {
  if (timeEditMode) {
    editSec++;
    if (editSec > 59) editSec = 0;
    char buffer[10];
    sprintf(buffer, "%02d", editSec);
    tSec.setText(buffer);
    Serial.printf("Second: %02d\n", editSec);
  }
}

void bMinusHourPop(void *ptr) {
  if (timeEditMode) {
    editHour--;
    if (editHour < 0) editHour = 23;
    char buffer[10];
    sprintf(buffer, "%02d", editHour);
    tHour.setText(buffer);
    Serial.printf("Hour: %02d\n", editHour);
  }
}

void bMinusMinPop(void *ptr) {
  if (timeEditMode) {
    editMin--;
    if (editMin < 0) editMin = 59;
    char buffer[10];
    sprintf(buffer, "%02d", editMin);
    tMin.setText(buffer);
    Serial.printf("Minute: %02d\n", editMin);
  }
}

void bMinusSecPop(void *ptr) {
  if (timeEditMode) {
    editSec--;
    if (editSec < 0) editSec = 59;
    char buffer[10];
    sprintf(buffer, "%02d", editSec);
    tSec.setText(buffer);
    Serial.printf("Second: %02d\n", editSec);
  }
}

void bSavePop(void *ptr) {
  if (timeEditMode) {
    myClock.setTimeFromEdit(editHour, editMin, editSec);
    timeEditMode = false;
    sendCommand("page Homepage");
    Serial.println("Time saved successfully");
  }
}

void bHomePop(void *ptr) {
  timeEditMode = false;
  sendCommand("page Homepage");
}

void bStopPop(void *ptr) {
  tStop.setText("");          // Clear the stop message
  digitalWrite(BUZZER, LOW);  // Turn off buzzer

  // If there's a currently ringing alarm and it's a once-only alarm, delete it
  if (currentRingingAlarm >= 0) {
    if (myClock.isOnceOnlyAlarm(currentRingingAlarm)) {
      Serial.printf("Deleting once-only alarm #%d\n", currentRingingAlarm);
      myClock.deleteAlarmByIndex(currentRingingAlarm);

      // Update the alarm list display if we're on the alarm list page
      // You might want to check if the current page is AlarmList before calling this
      updateAlarmListDisplay();
    }
    currentRingingAlarm = -1;  // Reset the tracking variable
  }

  Serial.println("Alarm stopped by user");
}

// Page 2 Date Setting Callbacks
void bPlusDayPop(void *ptr) {
  if (dateEditMode) {
    editDay++;
    int maxDays = myClock.getDaysInMonth(editMonth, editYear);
    if (editDay > maxDays) editDay = 1;
    char buffer[10];
    sprintf(buffer, "%02d", editDay);
    tDay.setText(buffer);
    Serial.printf("Day: %02d\n", editDay);
  }
}

void bPlusMonthPop(void *ptr) {
  if (dateEditMode) {
    editMonth++;
    if (editMonth > 12) editMonth = 1;
    int maxDays = myClock.getDaysInMonth(editMonth, editYear);
    if (editDay > maxDays) editDay = maxDays;
    char buffer[10];
    sprintf(buffer, "%02d", editMonth);
    tMonth.setText(buffer);
    sprintf(buffer, "%02d", editDay);
    tDay.setText(buffer);
    Serial.printf("Month: %02d, Day adjusted to: %02d\n", editMonth, editDay);
  }
}

void bPlusYearPop(void *ptr) {
  if (dateEditMode) {
    editYear++;
    if (editYear > 2099) editYear = 2000;
    if (editMonth == 2 && editDay == 29) {
      if (!((editYear % 4 == 0 && editYear % 100 != 0) || (editYear % 400 == 0))) {
        editDay = 28;
      }
    }
    char buffer[10];
    sprintf(buffer, "%04d", editYear);
    tYear.setText(buffer);
    sprintf(buffer, "%02d", editDay);
    tDay.setText(buffer);
    Serial.printf("Year: %04d\n", editYear);
  }
}

void bMinusDayPop(void *ptr) {
  if (dateEditMode) {
    editDay--;
    if (editDay < 1) editDay = myClock.getDaysInMonth(editMonth, editYear);
    char buffer[10];
    sprintf(buffer, "%02d", editDay);
    tDay.setText(buffer);
    Serial.printf("Day: %02d\n", editDay);
  }
}

void bMinusMonthPop(void *ptr) {
  if (dateEditMode) {
    editMonth--;
    if (editMonth < 1) editMonth = 12;
    int maxDays = myClock.getDaysInMonth(editMonth, editYear);
    if (editDay > maxDays) editDay = maxDays;
    char buffer[10];
    sprintf(buffer, "%02d", editMonth);
    tMonth.setText(buffer);
    sprintf(buffer, "%02d", editDay);
    tDay.setText(buffer);
    Serial.printf("Month: %02d, Day adjusted to: %02d\n", editMonth, editDay);
  }
}

void bMinusYearPop(void *ptr) {
  if (dateEditMode) {
    editYear--;
    if (editYear < 2000) editYear = 2099;
    if (editMonth == 2 && editDay == 29) {
      if (!((editYear % 4 == 0 && editYear % 100 != 0) || (editYear % 400 == 0))) {
        editDay = 28;
      }
    }
    char buffer[10];
    sprintf(buffer, "%04d", editYear);
    tYear.setText(buffer);
    sprintf(buffer, "%02d", editDay);
    tDay.setText(buffer);
    Serial.printf("Year: %04d\n", editYear);
  }
}

void bSavePage2Pop(void *ptr) {
  if (dateEditMode) {
    myClock.setDateFromEdit(editDay, editMonth, editYear);
    dateEditMode = false;
    sendCommand("page Homepage");
    Serial.println("Date saved successfully");
  }
}

void bHomePage2Pop(void *ptr) {
  dateEditMode = false;
  sendCommand("page Homepage");
}

// Page 3 Alarm List Callbacks
void bHomePage3Pop(void *ptr) {
  Serial.println("Home from AlarmList");
  sendCommand("page Homepage");
}

void bAddPop(void *ptr) {
  Serial.println("Add alarm pressed");
  alarmEditMode = true;
  editAlarmHour = 7;
  editAlarmMin = 0;
  for (int i = 0; i < 7; i++) editAlarmRepeatDays[i] = false;
  sendCommand("page AddAlarm");
  updateAlarmDisplay();
}

void b0Pop(void *ptr) {
  int alarmIndex = alarmListOffset + 0;
  if (alarmIndex < myClock.getAlarmCount()) {
    myClock.deleteAlarmByIndex(alarmIndex);
    updateAlarmListDisplay();
  }
}

void b1Pop(void *ptr) {
  int alarmIndex = alarmListOffset + 1;
  if (alarmIndex < myClock.getAlarmCount()) {
    myClock.deleteAlarmByIndex(alarmIndex);
    updateAlarmListDisplay();
  }
}

void b2Pop(void *ptr) {
  int alarmIndex = alarmListOffset + 2;
  if (alarmIndex < myClock.getAlarmCount()) {
    myClock.deleteAlarmByIndex(alarmIndex);
    updateAlarmListDisplay();
  }
}

void b3Pop(void *ptr) {
  int alarmIndex = alarmListOffset + 3;
  if (alarmIndex < myClock.getAlarmCount()) {
    myClock.deleteAlarmByIndex(alarmIndex);
    updateAlarmListDisplay();
  }
}

void b4Pop(void *ptr) {
  int alarmIndex = alarmListOffset + 4;
  if (alarmIndex < myClock.getAlarmCount()) {
    myClock.deleteAlarmByIndex(alarmIndex);
    updateAlarmListDisplay();
  }
}

void b5Pop(void *ptr) {
  Serial.println("Previous 5 alarms");
  if (alarmListOffset > 0) {
    alarmListOffset -= 5;
    if (alarmListOffset < 0) alarmListOffset = 0;
    updateAlarmListDisplay();
  }
}

void b6Pop(void *ptr) {
  Serial.println("Next 5 alarms");
  int alarmCount = myClock.getAlarmCount();
  if (alarmListOffset + 5 < alarmCount) {
    alarmListOffset += 5;
    updateAlarmListDisplay();
  }
}

// Page 4 Add Alarm Callbacks
void bPlusHourPage4Pop(void *ptr) {
  if (alarmEditMode) {
    editAlarmHour++;
    if (editAlarmHour > 23) editAlarmHour = 0;
    char buffer[10];
    sprintf(buffer, "%02d", editAlarmHour);
    tHourPage4.setText(buffer);
    Serial.printf("Alarm Hour: %02d\n", editAlarmHour);
  }
}

void bPlusMinPage4Pop(void *ptr) {
  if (alarmEditMode) {
    editAlarmMin++;
    if (editAlarmMin > 59) editAlarmMin = 0;
    char buffer[10];
    sprintf(buffer, "%02d", editAlarmMin);
    tMinPage4.setText(buffer);
    Serial.printf("Alarm Minute: %02d\n", editAlarmMin);
  }
}

void bMinusHourPage4Pop(void *ptr) {
  if (alarmEditMode) {
    editAlarmHour--;
    if (editAlarmHour < 0) editAlarmHour = 23;
    char buffer[10];
    sprintf(buffer, "%02d", editAlarmHour);
    tHourPage4.setText(buffer);
    Serial.printf("Alarm Hour: %02d\n", editAlarmHour);
  }
}

void bMinusMinPage4Pop(void *ptr) {
  if (alarmEditMode) {
    editAlarmMin--;
    if (editAlarmMin < 0) editAlarmMin = 59;
    char buffer[10];
    sprintf(buffer, "%02d", editAlarmMin);
    tMinPage4.setText(buffer);
    Serial.printf("Alarm Minute: %02d\n", editAlarmMin);
  }
}

void bSavePage4Pop(void *ptr) {
  if (alarmEditMode) {
    myClock.addAlarmFromGUI(editAlarmHour, editAlarmMin, editAlarmRepeatDays);
    alarmEditMode = false;
    sendCommand("page AlarmList");
    updateAlarmListDisplay();
    Serial.println("Alarm saved successfully");
  }
}

void bHomePage4Pop(void *ptr) {
  alarmEditMode = false;
  Serial.println("Alarm button pressed!");
  alarmListOffset = 0;
  sendCommand("page AlarmList");
  updateAlarmListDisplay();
  // sendCommand("page AlarmList");
}

// Checkbox Callbacks for Repeat Days
void c1Pop(void *ptr) {
  editAlarmRepeatDays[0] = !editAlarmRepeatDays[0];
  c1.setValue(editAlarmRepeatDays[0] ? 1 : 0);
  Serial.printf("Sunday repeat: %s\n", editAlarmRepeatDays[0] ? "ON" : "OFF");
}

void c2Pop(void *ptr) {
  editAlarmRepeatDays[1] = !editAlarmRepeatDays[1];
  c2.setValue(editAlarmRepeatDays[1] ? 1 : 0);
  Serial.printf("Monday repeat: %s\n", editAlarmRepeatDays[1] ? "ON" : "OFF");
}

void c3Pop(void *ptr) {
  editAlarmRepeatDays[2] = !editAlarmRepeatDays[2];
  c3.setValue(editAlarmRepeatDays[2] ? 1 : 0);
  Serial.printf("Tuesday repeat: %s\n", editAlarmRepeatDays[2] ? "ON" : "OFF");
}

void c4Pop(void *ptr) {
  editAlarmRepeatDays[3] = !editAlarmRepeatDays[3];
  c4.setValue(editAlarmRepeatDays[3] ? 1 : 0);
  Serial.printf("Wednesday repeat: %s\n", editAlarmRepeatDays[3] ? "ON" : "OFF");
}

void c5Pop(void *ptr) {
  editAlarmRepeatDays[4] = !editAlarmRepeatDays[4];
  c5.setValue(editAlarmRepeatDays[4] ? 1 : 0);
  Serial.printf("Thursday repeat: %s\n", editAlarmRepeatDays[4] ? "ON" : "OFF");
}

void c6Pop(void *ptr) {
  editAlarmRepeatDays[5] = !editAlarmRepeatDays[5];
  c6.setValue(editAlarmRepeatDays[5] ? 1 : 0);
  Serial.printf("Friday repeat: %s\n", editAlarmRepeatDays[5] ? "ON" : "OFF");
}

void c7Pop(void *ptr) {
  editAlarmRepeatDays[6] = !editAlarmRepeatDays[6];
  c7.setValue(editAlarmRepeatDays[6] ? 1 : 0);
  Serial.printf("Saturday repeat: %s\n", editAlarmRepeatDays[6] ? "ON" : "OFF");
}