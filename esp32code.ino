#include <WiFi.h>
#include <HTTPClient.h>
#include <U8g2lib.h> 
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
const char* ssid = "wifiname";
const char* password = "pass";

const char* geminiProxyURL = "http://172.20.10.11:3000/gemini";
const int btn1 = 12; 
const int btn2 = 14;
String prompts[] = {
  "Tell me a joke", 
  "GOATED society in KIIT?"
};

void displayWrappedText(const String &msg, int startY);
String sendPromptToGemini(String prompt); 
void setup() {
  Serial.begin(115200);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "Connecting...");
  u8g2.sendBuffer();
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    u8g2.drawStr(u8g2.getStrWidth("Connecting..."), 15, ".");
    u8g2.sendBuffer();
  }
  Serial.println("\nWiFi connected.");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "WiFi connected.");
  u8g2.drawStr(0, 30, "Press a button!");
  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  if (digitalRead(btn1) == LOW) {
    handlePrompt(0);
  }
  if (digitalRead(btn2) == LOW) {
    handlePrompt(1);
  }

  delay(200); 
}
void handlePrompt(int index) {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Thinking 🤔...");
  u8g2.sendBuffer();
  Serial.print("Prompt: ");
  Serial.println(prompts[index]); 

  String reply = sendPromptToGemini(prompts[index]); 

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "🤖:"); 
  displayWrappedText(reply, 20);
  u8g2.sendBuffer();

  delay(4000);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Press a button!");
  u8g2.sendBuffer();
}
String sendPromptToGemini(String prompt) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost!");
    return "WiFi not connected.";
  }

  HTTPClient http; 
  http.begin(geminiProxyURL); 
  http.addHeader("Content-Type", "application/json");
  String body = "{\"prompt\": \"" + prompt + "\"}";
  Serial.print("Sending: ");
  Serial.println(body);

  int httpCode = http.POST(body); 

  String response;
  if (httpCode > 0) { 
    response = http.getString(); 
    Serial.print("Reply: ");
    Serial.println(response);
  } else {
    response = "HTTP Err: " + String(httpCode); 
    Serial.print("Error: ");
    Serial.println(httpCode);
  }
  http.end();
  if (response.length() > 500) response = response.substring(0, 500);
  return response;
}
void displayWrappedText(const String &msg, int startY) {
  int lineHeight = u8g2.getFontAscent() - u8g2.getFontDescent() + 2; 
  int currentY = startY;
  String currentLine = "";
  for (int i = 0; i < msg.length(); i++) {
    char c = msg.charAt(i);
    String testLine = currentLine + c;
    if (u8g2.getStrWidth(testLine.c_str()) > u8g2.getWidth() || c == '\n') {
      u8g2.drawStr(0, currentY, currentLine.c_str()); 
      currentLine = "";
      currentY += lineHeight; 
      if (currentY >= u8g2.getHeight()) break;
      if (c == '\n') continue; 
    }
    currentLine += c;
  }
  if (currentLine.length() > 0 && currentY < u8g2.getHeight()) {
    u8g2.drawStr(0, currentY, currentLine.c_str());
  }
}

