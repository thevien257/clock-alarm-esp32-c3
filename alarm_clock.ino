#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <Nextion.h>

#define BUZZER 7
#define MAX_ALARMS 5  // tối đa 5 báo thức

// Create serial for Nextion
SoftwareSerial nextionSerial(18, 19);  // RX, TX

// Declare Nextion objects (page0 elements)

// Page 0
NexText tTime = NexText(0, 1, "tTime");
NexText tDate = NexText(0, 4, "tDate");
NexText tAlarm = NexText(0, 3, "tAlarm");
NexText tTemp = NexText(0, 5, "tTemp");
NexButton bAlarm = NexButton(0, 6, "bAlarm");
NexButton bDate = NexButton(0, 7, "bDate");
NexButton bTime = NexButton(0, 5, "bTime");

// Page 1
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

// Complete touch event list
NexTouch *nex_listen_list[] = {
  // Page 0 buttons
  &bAlarm,
  &bDate,
  &bTime,

  // Page 1 buttons
  &bPlusHour,
  &bPlusMin,
  &bPlusSec,
  &bMinusHour,
  &bMinusMin,
  &bMinusSec,
  &bHome,
  &bSave,

  // Null terminator (required)
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

// Global variables for time editing
int editHour = 0;
int editMin = 0;
int editSec = 0;
bool timeEditMode = false;

struct Alarm {
  int hour;
  int minute;
  bool active;
  DateTime nextTrigger;  // ngày giờ reo tiếp theo
  bool repeatDays[7];    // lặp theo thứ nào (0=CN … 6=Th7)
};

class ClockAlarm {
private:
  RTC_DS1307 rtc;
  int buzzerPin;
  Alarm alarms[MAX_ALARMS];
  int alarmCount;

public:
  // Biến lưu thời gian hiện tại
  int curDay, curMonth, curYear;
  int curHour, curMinute, curSecond;

  // Chuỗi định dạng
  String timeString = "";   // "15:16:20"
  String dateString = "";   // "Sunday, 21/09/2025"
  String alarmString = "";  // thông tin báo thức tiếp theo

  ClockAlarm(int buzzer) {
    buzzerPin = buzzer;
    alarmCount = 0;
    curDay = curMonth = curYear = 0;
    curHour = curMinute = curSecond = 0;
  }

  void begin(int sda = 4, int scl = 5) {
    Wire.begin(sda, scl);
    pinMode(buzzerPin, OUTPUT);

    if (!rtc.begin()) {
      Serial.println("❌ Không tìm thấy DS1307!");
      while (1)
        ;
    }
    if (!rtc.isrunning()) {
      Serial.println("⚠️ DS1307 chưa chạy, cần set thời gian.");
    }

    Serial.println("👉 Gõ 'S' để chỉnh thời gian thủ công.");
    Serial.println("👉 Gõ 'A' để thêm báo thức.");
    Serial.println("👉 Gõ 'L' để liệt kê báo thức.");
    Serial.println("👉 Gõ 'E' để chỉnh sửa báo thức.");
    Serial.println("👉 Gõ 'D' để xóa báo thức.");
  }

  void showTime() {
    DateTime now = rtc.now();

    // Lưu biến
    curDay = now.day();
    curMonth = now.month();
    curYear = now.year();
    curHour = now.hour();
    curMinute = now.minute();
    curSecond = now.second();

    timeString = "";
    dateString = "";
    alarmString = "";

    // Format giờ HH:MM:SS
    char bufTime[9];
    sprintf(bufTime, "%02d:%02d:%02d", curHour, curMinute, curSecond);
    timeString = String(bufTime);

    // Format ngày kèm thứ: Sunday, 21/09/2025
    char bufDate[40];
    sprintf(bufDate, "%s, %02d/%02d/%04d",
            dayOfWeekFull(now.dayOfTheWeek()),
            curDay, curMonth, curYear);
    dateString = String(bufDate);

    updateAlarmString();

    // Debug
    Serial.print("timeString = ");
    Serial.println(timeString);
    Serial.print("dateString = ");
    Serial.println(dateString);
    Serial.print("alarmString = ");
    Serial.println(alarmString);
  }

  void setTimeFromEdit(int hour, int minute, int second) {
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, second));
    Serial.printf("✅ Time set to %02d:%02d:%02d\n", hour, minute, second);
  }

  DateTime getCurrentTime() {
    return rtc.now();
  }

  void setTimeManual() {
    int year, month, day, hour, minute, second;

    Serial.println("=== Nhập thời gian thủ công ===");
    Serial.print("Nhập năm (vd: 2025): ");
    year = readIntFromSerial();
    Serial.print("Nhập tháng (1-12): ");
    month = readIntFromSerial();
    Serial.print("Nhập ngày (1-31): ");
    day = readIntFromSerial();
    Serial.print("Nhập giờ (0-23): ");
    hour = readIntFromSerial();
    Serial.print("Nhập phút (0-59): ");
    minute = readIntFromSerial();
    Serial.print("Nhập giây (0-59): ");
    second = readIntFromSerial();

    if (year < 2000 || month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
      Serial.println("❌ Sai định dạng thời gian. Không lưu.");
      return;
    }

    rtc.adjust(DateTime(year, month, day, hour, minute, second));
    Serial.println("✅ Đã set thời gian mới cho DS1307!");
  }

  void addAlarm() {
    if (alarmCount >= MAX_ALARMS) {
      Serial.println("❌ Danh sách báo thức đã đầy!");
      return;
    }

    Serial.println("=== Nhập thời gian báo thức mới ===");
    int h, m;
    Serial.print("Nhập giờ (0-23): ");
    h = readIntFromSerial();
    Serial.print("Nhập phút (0-59): ");
    m = readIntFromSerial();

    if (h < 0 || h > 23 || m < 0 || m > 59) {
      Serial.println("❌ Sai định dạng báo thức. Không lưu.");
      return;
    }

    DateTime now = rtc.now();
    DateTime candidate(now.year(), now.month(), now.day(), h, m, 0);

    if (candidate <= now) {
      candidate = candidate + TimeSpan(1, 0, 0, 0);
    }

    Alarm a;
    a.hour = h;
    a.minute = m;
    a.active = true;
    a.nextTrigger = candidate;
    for (int i = 0; i < 7; i++) a.repeatDays[i] = false;

    alarms[alarmCount] = a;
    Serial.printf("✅ Báo thức #%d đặt lúc %02d:%02d vào ngày %02d/%02d/%04d\n",
                  alarmCount, h, m,
                  candidate.day(), candidate.month(), candidate.year());
    alarmCount++;

    setRepeatDays(alarmCount - 1);
  }

  void listAlarms() {
    Serial.println("=== Danh sách báo thức ===");
    if (alarmCount == 0) {
      Serial.println("⚠️ Chưa có báo thức nào.");
    } else {
      for (int i = 0; i < alarmCount; i++) {
        Serial.printf("[%d] %02d:%02d | %s | ",
                      i, alarms[i].hour, alarms[i].minute,
                      alarms[i].active ? "On" : "Off");
        bool hasRepeat = false;
        for (int d = 0; d < 7; d++) {
          if (alarms[i].repeatDays[d]) {
            if (!hasRepeat) {
              Serial.print("Repeat: ");
              hasRepeat = true;
            }
            Serial.printf("%s ", dayOfWeekFull(d));
          }
        }
        if (!hasRepeat) {
          Serial.printf("Once: %02d/%02d/%04d",
                        alarms[i].nextTrigger.day(),
                        alarms[i].nextTrigger.month(),
                        alarms[i].nextTrigger.year());
        }
        Serial.println();
      }
    }
  }

  void editAlarm() {
    listAlarms();
    if (alarmCount == 0) return;

    Serial.print("Chọn số báo thức cần chỉnh: ");
    int idx = readIntFromSerial();
    if (idx < 0 || idx >= alarmCount) {
      Serial.println("❌ Không hợp lệ.");
      return;
    }

    Serial.printf("=== Sửa báo thức #%d ===\n", idx);
    Serial.print("Nhập giờ (0-23): ");
    int h = readIntFromSerial();
    Serial.print("Nhập phút (0-59): ");
    int m = readIntFromSerial();

    if (h < 0 || h > 23 || m < 0 || m > 59) {
      Serial.println("❌ Sai định dạng báo thức. Không sửa.");
      return;
    }

    DateTime now = rtc.now();
    DateTime candidate(now.year(), now.month(), now.day(), h, m, 0);
    if (candidate <= now) {
      candidate = candidate + TimeSpan(1, 0, 0, 0);
    }

    alarms[idx].hour = h;
    alarms[idx].minute = m;
    alarms[idx].active = true;
    alarms[idx].nextTrigger = candidate;
    for (int i = 0; i < 7; i++) alarms[idx].repeatDays[i] = false;

    Serial.printf("✅ Đã chỉnh báo thức #%d thành %02d:%02d\n", idx, h, m);
    setRepeatDays(idx);
  }

  void deleteAlarm() {
    listAlarms();
    if (alarmCount == 0) return;

    Serial.print("Chọn số báo thức cần xóa: ");
    int idx = readIntFromSerial();
    if (idx < 0 || idx >= alarmCount) {
      Serial.println("❌ Không hợp lệ.");
      return;
    }
    for (int i = idx; i < alarmCount - 1; i++) alarms[i] = alarms[i + 1];
    alarmCount--;
    Serial.printf("✅ Đã xóa báo thức #%d\n", idx);
  }

  void checkAlarms() {
    DateTime now = rtc.now();
    for (int i = 0; i < alarmCount; i++) {
      if (alarms[i].active && now.hour() == alarms[i].nextTrigger.hour() && now.minute() == alarms[i].nextTrigger.minute() && now.second() == 0 && now.day() == alarms[i].nextTrigger.day() && now.month() == alarms[i].nextTrigger.month() && now.year() == alarms[i].nextTrigger.year()) {
        Serial.printf("🔔 Báo thức #%d kêu lúc %02d:%02d!!!\n",
                      i, alarms[i].hour, alarms[i].minute);
        digitalWrite(buzzerPin, LOW);
        delay(3000);
        digitalWrite(buzzerPin, HIGH);

        if (hasRepeat(i)) {
          alarms[i].nextTrigger = getNextRepeat(i, now);
        } else {
          alarms[i].active = false;
        }
      }
    }
  }

private:
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
      sprintf(buf, "%02d:%02d %02d/%02d",
              alarms[idx].hour, alarms[idx].minute,
              nextAlarm.day(), nextAlarm.month());
      alarmString = String(buf);
    } else {
      alarmString = "No Active";
    }
  }

  void setRepeatDays(int idx) {
    Serial.println("👉 Chọn ngày trong tuần để lặp (0=Sun … 6=Sat), nhập -1 để kết thúc.");
    while (true) {
      Serial.print("Day: ");
      int d = readIntFromSerial();
      if (d == -1) break;
      if (d >= 0 && d <= 6) {
        alarms[idx].repeatDays[d] = true;
        Serial.printf("   ✔ Repeat on %s\n", dayOfWeekFull(d));
      }
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
        return DateTime(candidate.year(), candidate.month(), candidate.day(),
                        alarms[idx].hour, alarms[idx].minute, 0);
      }
    }
    return after + TimeSpan(1, 0, 0, 0);
  }

  int readIntFromSerial() {
    while (!Serial.available())
      ;
    String s = Serial.readStringUntil('\n');
    s.trim();
    return s.toInt();
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
};

// ================== Dùng class ==================
ClockAlarm myClock(BUZZER);

void setup() {
  Serial.begin(9600);
  myClock.begin();
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);

  nextionSerial.begin(9600);
  nexInit();

  // Page 0 button attachments
  bAlarm.attachPop(bAlarmPop, &bAlarm);
  bDate.attachPop(bDatePop, &bDate);
  bTime.attachPop(bTimePop, &bTime);

  // Page 1 button attachments
  bPlusHour.attachPop(bPlusHourPop, &bPlusHour);
  bPlusMin.attachPop(bPlusMinPop, &bPlusMin);
  bPlusSec.attachPop(bPlusSecPop, &bPlusSec);
  bMinusHour.attachPop(bMinusHourPop, &bMinusHour);
  bMinusMin.attachPop(bMinusMinPop, &bMinusMin);
  bMinusSec.attachPop(bMinusSecPop, &bMinusSec);
  bSave.attachPop(bSavePop, &bSave);
  bHome.attachPop(bHomePop, &bHome);
}


void loop() {
  static unsigned long lastShowTime = 0;  // lưu thời điểm lần cuối showTime
  unsigned long nowMs = millis();         // thời gian hiện tại (ms)
  if (timeEditMode == false) {
    // Chỉ gọi showTime mỗi 1 giây
    if (nowMs - lastShowTime >= 1000) {
      lastShowTime = nowMs;

      myClock.showTime();  // đọc RTC + tạo chuỗi thời gian
                           // Cập nhật Nextion
      tTime.setText(myClock.timeString.c_str());
      tDate.setText(myClock.dateString.c_str());
      tAlarm.setText(myClock.alarmString.c_str());
    }



    // Xử lý dữ liệu nhập từ Serial (không chặn)
    // if (Serial.available()) {
    //   char c = Serial.read();
    //   while (Serial.available()) Serial.read();  // xóa phần dư
    //   if (c == 'S' || c == 's') myClock.setTimeManual();
    //   else if (c == 'A' || c == 'a') myClock.addAlarm();
    //   else if (c == 'L' || c == 'l') myClock.listAlarms();
    //   else if (c == 'E' || c == 'e') myClock.editAlarm();
    //   else if (c == 'D' || c == 'd') myClock.deleteAlarm();
    // }
    myClock.checkAlarms();  // kiểm tra báo thức
  }
  nexLoop(nex_listen_list);  // xử lý sự kiện Nextion
}


// Nextion Event Handlers
void bAlarmPop(void *ptr) {
  Serial.println("🔔 Alarm button pressed!");
  // myClock.addAlarm();
}
void bDatePop(void *ptr) {
  Serial.println("📅 Date button pressed!");
  // myClock.setTimeManual();
}
void bTimePop(void *ptr) {
  Serial.println("🕒 Time button pressed!");
  timeEditMode = true;

  // Get current time for editing using myClock object
  DateTime now = myClock.getCurrentTime();
  editHour = now.hour();
  editMin = now.minute();
  editSec = now.second();

  // Update display with current time values
  updateTimeDisplay();
}


// Time setting helper function
void updateTimeDisplay() {
  char buffer[10];

  sprintf(buffer, "%02d", editHour);
  tHour.setText(buffer);

  sprintf(buffer, "%02d", editMin);
  tMin.setText(buffer);

  sprintf(buffer, "%02d", editSec);
  tSec.setText(buffer);
}

// Time setting button callback functions
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
    // Save the edited time to RTC
    myClock.setTimeFromEdit(editHour, editMin, editSec);

    // Exit edit mode
    timeEditMode = false;

    Serial.println("✅ Time saved and returned to main page");
  }
}

void bHomePop(void *ptr) {
  timeEditMode = false;
}
