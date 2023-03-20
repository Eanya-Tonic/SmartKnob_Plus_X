#include "hal/hal.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include "hal/EEPROM_utils.h"
#include "hal/EEPROM_utils.h"

// 任务句柄
TaskHandle_t WiFiHandle;

// 强制门户配网
const char *AP_NAME = "SmartKnob"; // wifi名字
// 暂时存储wifi账号密码
String GET_SSID;
String GET_PASSWORD;
// 配网页面代码
const char *page_html = "\
<!DOCTYPE html>\r\n\
<html lang='en'>\r\n\
<head>\r\n\
<meta charset='UTF-8'>\r\n\
<meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n\
<title>为您的设备连接互联网</title>\r\n\
</head>\r\n\
<body>\r\n\
<h3>请在下方输入可用的Wi-Fi网络信息(不支持5Ghz网络), 然后点击保存</h3>\r\n\
<form name='input' action='/' method='POST'>\r\n\
      wifi名称: <br>\r\n\
      <input type='text' name='ssid'><br>\r\n\
      wifi密码:<br>\r\n\
      <input type='text' name='password'><br>\r\n\
      <input type='submit' value='保存'>\r\n\
  </form>\r\n\
</body>\r\n\
</html>\r\n\
";
void connectNewWifi();
const byte DNS_PORT = 53;       // DNS端口号
IPAddress apIP(192, 168, 4, 1); // esp32-AP-IP地址
DNSServer dnsServer;            // 创建dnsServer实例
WebServer server(80);           // 创建WebServer

WiFiUDP Udp;                      // 设置UDP实例
uint16_t udp_port = 1122;         // 存储需要监听的端口号
char incomingPacket[255];         // 存储Udp客户端发过来的数据
static bool UDPConnected = false; // UDP连接状态

bool getTime(bool recheck);

String GET_MQTT_IP;
String GET_MQTT_PORT;
String GET_MQTT_USER;
String GET_MQTT_PASSWORD;

void handleRoot()
{ // 访问主页回调函数
  server.send(200, "text/html", page_html);
}

void handleRootPost()
{ // Post回调函数
  Serial.println("handleRootPost");
  if (server.hasArg("ssid"))
  { // 判断是否有账号参数
    Serial.print("got ssid:");
    GET_SSID = server.arg("ssid"); // 将账号参数拷贝到sta_ssid中
    Serial.println(GET_SSID);
    vTaskDelay(1000);
  }
  else
  { // 没有参数
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); // 返回错误页面
    return;
  }
  // 密码与账号同理
  if (server.hasArg("password"))
  {
    Serial.print("got password:");
    GET_PASSWORD = server.arg("password");
    Serial.println(GET_PASSWORD);
    vTaskDelay(1000);
  }
  else
  {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  server.send(200, "text/html", "<meta charset='UTF-8'>保存成功"); // 返回保存成功页面
  // 连接wifi
  const char *ssid = GET_SSID.c_str();
  const char *password = GET_PASSWORD.c_str();
  WiFi.begin(ssid, password);
  vTaskDelay(500);
}

void wifi_loop(void *parameter)
{
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(5);
    server.handleClient();
    dnsServer.processNextRequest();
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    SISSD_NUM = GET_SSID.length();
    PASSWORD_NUM = GET_PASSWORD.length();
    EEPROM.write(SISSD_ADDR, SISSD_NUM);
    set_String(SISSD_NUM, SISSD_ADDR + 1, GET_SSID);
    EEPROM.write(PASSWORD_ADDR, PASSWORD_NUM);
    set_String(PASSWORD_NUM, PASSWORD_ADDR + 1, GET_PASSWORD);
    Serial.print("本地IP: ");
    Serial.println(WiFi.localIP());
    while (!getTime(1))
    {
      vTaskDelay(1000);
    }
    Udp.begin(udp_port); // 启动UDP监听端口
    vTaskDelay(200);
    WiFi.enableAP(false);
    vTaskDelete(WiFiHandle);
  }
}

void initBasic(void)
{                               // 初始化基础
  WiFi.hostname("Smart-ESP32"); // 设置设备名
}

void initSoftAP(void)
{ // 初始化AP模式
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if (WiFi.softAP(AP_NAME))
  {
    Serial.println("ESP32 SoftAP is right");
  }
}

void initWebServer(void)
{ // 初始化WebServer
  // server.on("/",handleRoot);
  // 上面那行必须以下面这种格式去写否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);      // 设置主页回调函数
  server.onNotFound(handleRoot);             // 设置无法响应的http请求的回调函数
  server.on("/", HTTP_POST, handleRootPost); // 设置Post请求回调函数
  server.begin();                            // 启动WebServer
  Serial.println("WebServer started!");
}

void initDNS(void)
{ // 初始化DNS服务器
  if (dnsServer.start(DNS_PORT, "*", apIP))
  { // 判断将所有地址映射到esp32的ip上是否成功
    Serial.println("start dnsserver success.");
  }
  else
    Serial.println("start dnsserver failed.");
}

bool ServerFlag = false;
void connectNewWifi(void)
{
  vTaskDelay(5000);
  if (WiFi.status() != WL_CONNECTED && ServerFlag == false)
  {
    ServerFlag = true;
    // 如果5秒内没有连上，就开启Web配网 可适当调整这个时间
    initSoftAP();
    initWebServer();
    initDNS();
    xTaskCreatePinnedToCore(
        wifi_loop,
        "WiFiThread",
        2048,
        nullptr,
        5,
        &WiFiHandle,
        ESP32_RUNNING_CORE);
  }
  if (WiFi.status() == WL_CONNECTED)
  { // 如果连接上 就输出IP信息
    Serial.println("WIFI Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // 打印esp32的IP地址
    while (!getTime(1))
    {
      vTaskDelay(1000);
    }
    Udp.begin(udp_port); // 启动UDP监听端口
    if (ServerFlag)
      server.stop();
  }
}

void UDP_loop(void *parameter)
{
  if (WiFi.status() != WL_CONNECTED)
    return;
  /*接收发送过来的Udp数据*/
  int Data_length = Udp.parsePacket(); // 获取接收的数据的长度
  if (Data_length)                     // 如果有数据那么Data_length不为0，无数据Data_length为0
  {
    int len = Udp.read(incomingPacket, 255); // 读取数据，将数据保存在数组incomingPacket中
    if (len > 0)                             // 为了避免获取的数据后面乱码做的判断
    {
      incomingPacket[len] = '\0'; // 将数据后面加上结束符
    }
    if (incomingPacket[0] == '0')
    {
      /*发送确认连接信息*/
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // 准备发送数据到目标IP和目标端口
      Udp.print("connected");                            // 将数据放入发送的缓冲区
      Udp.endPacket();                                   // 向目标IP目标端口发送数据
      UDPConnected = true;
    }
    else if (incomingPacket[0] == '1')
    {
      String GET_DEVICE_NAME;
      for (int i = 1; i < len; i++)
      {
        GET_DEVICE_NAME += incomingPacket[i];
      }
      DEVICE_NAME_NUM = GET_DEVICE_NAME.length();
      EEPROM.write(DEVICE_NAME_ADDR, DEVICE_NAME_NUM);
      set_String(DEVICE_NAME_NUM, DEVICE_NAME_ADDR + 1, GET_DEVICE_NAME);
    }
    else if (incomingPacket[0] = '2')
    {
      String GET_DEVICE_USER;
      for (int i = 1; i < len; i++)
      {
        GET_DEVICE_USER += incomingPacket[i];
      }
      DEVICE_USER_NUM = GET_DEVICE_USER.length();
      EEPROM.write(DEVICE_USER_ADDR, DEVICE_USER_NUM);
      set_String(DEVICE_USER_NUM, DEVICE_USER_ADDR + 1, GET_DEVICE_USER);
    }
    else if (incomingPacket[0] = '3')
    {
      for (int i = 1; i < len; i++)
      {
        GET_MQTT_IP += incomingPacket[i];
      }
      MQTT_IP_NUM = GET_MQTT_IP.length();
      EEPROM.write(MQTT_IP_ADDR, MQTT_IP_NUM);
      set_String(MQTT_IP_NUM, MQTT_IP_ADDR + 1, GET_MQTT_IP);
    }
    else if (incomingPacket[0] = '4')
    {
      for (int i = 1; i < len; i++)
      {
        GET_MQTT_PORT += incomingPacket[i];
      }
      MQTT_PORT_NUM = GET_MQTT_PORT.length();
      EEPROM.write(MQTT_PORT_ADDR, MQTT_PORT_NUM);
      set_String(MQTT_PORT_NUM, MQTT_PORT_ADDR + 1, GET_MQTT_PORT);
    }
    else if (incomingPacket[0] = '5')
    {
      for (int i = 1; i < len; i++)
      {
        GET_MQTT_USER += incomingPacket[i];
      }
      MQTT_USER_NUM = GET_MQTT_USER.length();
      EEPROM.write(MQTT_USER_ADDR, MQTT_USER_NUM);
      set_String(MQTT_USER_NUM, MQTT_USER_ADDR + 1, GET_MQTT_USER);
    }
    else if (incomingPacket[0] = '6')
    {
      for (int i = 1; i < len; i++)
      {
        GET_MQTT_PASSWORD += incomingPacket[i];
      }
      MQTT_PASSWORD_NUM = GET_MQTT_PASSWORD.length();
      EEPROM.write(MQTT_PASSWORD_ADDR, MQTT_PASSWORD_NUM);
      set_String(MQTT_PASSWORD_NUM, MQTT_PASSWORD_ADDR + 1, GET_MQTT_PASSWORD);
    }
  }
  vTaskDelay(1);
}

static int32_t currentHour, currentMinute;
static int tmpSecond = 0;

// 获取时间
bool getTime(bool recheck)
{
  // 降低查询频率
  if (!recheck && (tmpSecond == 0 || tmpSecond == 1))
  {
    currentMinute++;
    if (currentMinute % 60 != 0)
      return true;
    else
      currentMinute--;
  }
  String URL = "http://quan.suning.com/getSysTime.do";

  // 创建 HTTPClient 对象
  HTTPClient httpClient;
  WiFiClient client; // 新添加
  httpClient.setReuse(false);
  httpClient.begin(client, URL);

  int httpCode = httpClient.GET();
  Serial.println("正在获取时间数据");

  if (httpCode == HTTP_CODE_OK)
  {
    String str = httpClient.getString();

    int indexStart = str.indexOf("\"sysTime1\":\"");
    int indexEnd = str.indexOf("\"}");

    String tmpTime = str.substring(indexStart + 12, indexEnd);
    Serial.println(tmpTime);

    currentHour = atoi(tmpTime.substring(8, 10).c_str());
    currentMinute = atoi(tmpTime.substring(10, 12).c_str());

    tmpSecond = atoi(tmpTime.substring(12, 14).c_str());

    Serial.println("获取成功");
    // 关闭ESP32与服务器连接
    httpClient.end();
    return true;
  }
  else
  {
    Serial.println("请求时间错误：");
    Serial.print(httpCode);
    // 关闭ESP32与服务器连接
    httpClient.end();
    return false;
  }
}

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);
int mqtt_last_connect_time_ = 0;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  char buf[256];
  snprintf(buf, sizeof(buf), "Received mqtt callback for topic %s, length %u", topic, length);
  Serial.printf(buf);
}

void connectMQTT()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    return;
  }
  Serial.println("Attempting MQTT connection...");

  if (EEPROM.read(MQTT_PASSWORD_ADDR) != 0)
  {
    if (mqtt_client.connect(GET_MQTT_USER.c_str(), GET_MQTT_USER.c_str(), GET_MQTT_PASSWORD.c_str()))
    {
      Serial.printf("MQTT connected");
    }
    else
    {
      Serial.printf("MQTT failed rc=%d will try again in 5 seconds", mqtt_client.state());
    }
  }
  else
  {
    if (mqtt_client.connect(GET_MQTT_USER.c_str()))
    {
      Serial.printf("MQTT connected");
    }
    else
    {
      Serial.printf("MQTT failed rc=%d will try again in 5 seconds", mqtt_client.state());
    }
  }

  Serial.flush();
}

TaskHandle_t handleTaskMqtt;
void TaskMqttUpdate(void *pvParameters)
{

  while (1)
  {
    long now = millis();
    if (!mqtt_client.connected() && (now - mqtt_last_connect_time_) > 5000)
    {
      Serial.printf("Reconnecting MQTT");
      mqtt_last_connect_time_ = now;
      connectMQTT();
    }
    mqtt_client.loop();
    vTaskDelay(5);
  }
}

int HAL::mqtt_subscribe(const char *topic)
{
  if (EEPROM.read(MQTT_IP_ADDR) == 0)
  {
    return 0;
  }
  bool ret = mqtt_client.subscribe(topic);
  if (ret)
  {
    Serial.printf("Subscribe Error to topic:%s\n", topic);
  }
  return ret;
}

int HAL::mqtt_publish(const char *topic, const char *playload)
{
  if (EEPROM.read(MQTT_IP_ADDR) == 0)
  {
    return 0;
  }
  bool ret = mqtt_client.publish(topic, playload);
  if (ret)
  {
    Serial.printf("Publish Error: topic:%s, playload:%s\n", topic, playload);
  }
  return ret;
}

void HAL::mqtt_init(void)
{
  HAL::wifi_init();
  // 获取EEPROM信息
  if (EEPROM.read(MQTT_IP_ADDR) != 0)
  {
    GET_MQTT_IP = get_String(EEPROM.read(MQTT_IP_ADDR), MQTT_IP_ADDR + 1);
    GET_MQTT_PORT = get_String(EEPROM.read(MQTT_PORT_ADDR), MQTT_PORT_ADDR + 1);
    GET_MQTT_USER = get_String(EEPROM.read(MQTT_USER_ADDR), MQTT_USER_ADDR + 1);
    GET_MQTT_PASSWORD = get_String(EEPROM.read(MQTT_PASSWORD_ADDR), MQTT_PASSWORD_ADDR + 1);
    mqtt_client.setServer(GET_MQTT_IP.c_str(), GET_MQTT_PORT.toInt());
    // connectMQTT();
    mqtt_client.setCallback(mqttCallback);
    xTaskCreatePinnedToCore(
        TaskMqttUpdate,
        "MqttThread",
        2048,
        nullptr,
        2,
        &handleTaskMqtt,
        ESP32_RUNNING_CORE);
  }
}

void HAL::mqtt_deinit(void)
{
  if (EEPROM.read(MQTT_IP_ADDR) == 0)
  {
    return;
  }
  mqtt_client.disconnect();
  vTaskDelete(handleTaskMqtt);
}

void HAL::wifi_init(void)
{
  // 连接wifi
  Serial.print("正在连接WIFI\n");
  if (EEPROM.read(SISSD_ADDR) != 0)
  {
    GET_SSID = get_String(EEPROM.read(SISSD_ADDR), SISSD_ADDR + 1);
    GET_PASSWORD = get_String(EEPROM.read(PASSWORD_ADDR), PASSWORD_ADDR + 1);
    Serial.printf("SSID:%s\r\n", GET_SSID.c_str());
    Serial.printf("PSW:%s\r\n", GET_PASSWORD.c_str());
    const char *ssid = GET_SSID.c_str();
    const char *password = GET_PASSWORD.c_str();
    WiFi.begin(ssid, password);
  }
  initBasic();
  connectNewWifi();
}

bool HAL::wifi_is_connected(void)
{
  if (WiFi.status() == WL_CONNECTED)
    return true;
  else
    return false;
}

void HAL::wifi_disconnect(void)
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

void HAL::wifi_dial_update(SuperDialMotion direction)
{
  if (UDPConnected)
  {

    switch (direction)
    {
    case SUPER_DIAL_LEFT:
      /*发送信息*/
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // 准备发送数据到目标IP和目标端口
      Udp.print("LEFT");                                 // 将数据放入发送的缓冲区
      Udp.endPacket();                                   // 向目标IP目标端口发送数据
      break;
    case SUPER_DIAL_RIGHT:
      /*发送信息*/
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // 准备发送数据到目标IP和目标端口
      Udp.print("RIGHT");                                // 将数据放入发送的缓冲区
      Udp.endPacket();                                   // 向目标IP目标端口发送数据
      break;
    default:
      break;
    }
  }
}

void HAL::wifi_dial_press(void)
{
  if (UDPConnected)
  {
    /*发送信息*/
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // 准备发送数据到目标IP和目标端口
    Udp.print("PRESS");                                // 将数据放入发送的缓冲区
    Udp.endPacket();                                   // 向目标IP目标端口发送数据
  }
}

void HAL::wifi_dial_release(void)
{
  if (UDPConnected)
  {
    /*发送信息*/
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // 准备发送数据到目标IP和目标端口
    Udp.print("RELEASE");                              // 将数据放入发送的缓冲区
    Udp.endPacket();                                   // 向目标IP目标端口发送数据
  }
}

String HAL::return_IP(void)
{
  return WiFi.localIP().toString();
}

bool HAL::return_UDP(void)
{
  return UDPConnected;
}

int HAL::return_hour(void)
{
  return currentHour;
}

int HAL::return_minute(void)
{
  return currentMinute;
}

int HAL::updateTime(void)
{
  while (!getTime(0))
  {
    vTaskDelay(1000);
  }
  return tmpSecond;
}