#include "hal/EEPROM_utils.h"


void set_String(int a, int b, String str)
{
  EEPROM.write(a, str.length()); // EEPROM第a位，写入str字符串的长度
  // 把str所有数据逐个保存在EEPROM
  for (unsigned int i = 0; i < str.length(); i++)
  {
    EEPROM.write(b + i, str[i]);
  }
  EEPROM.commit();
}

// a位是字符串长度，b是起始位
String get_String(int a, int b)
{
  String data = "";
  // 从EEPROM中逐个取出每一位的值，并链接
  for (int i = 0; i < a; i++)
  {
    data += char(EEPROM.read(b + i));
  }
  return data;
}

// 清空EEPROM
void clean_EEPROM(void)
{ 
  // write a 0 to all bytes of the EEPROM
  for (int i = 0; i < 4096; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}