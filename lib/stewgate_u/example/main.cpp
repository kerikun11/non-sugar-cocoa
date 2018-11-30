#include <Arduino.h>

#include <stewgate_u.h>

// Put StewGate Token
const std::string token = "a7405aee08448afb86bd34e903e5548f";

// object
StewGate_U sg(token);

void setup() {
  // put your setup code here, to run once:
  WiFi.begin();
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    log_e("WiFi Error");
    vTaskDelay(portMAX_DELAY);
  }

  sg.tweet("Hello, from M5Stack!");
}

void loop() {
  // put your main code here, to run repeatedly:
}
