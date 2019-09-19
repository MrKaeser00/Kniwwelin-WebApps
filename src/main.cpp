#include <Kniwwelino.h>

void setup()
{
  //Initialize the Kniwwelino Board
  Kniwwelino.begin("main", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
}

void loop()
{
  char conn = (Kniwwelino.isConnected() == true ? '1' : '0');
  char str[2];
  str[0] = conn;
  str[1] = '\0';
  if (Kniwwelino.isConnected())
  {
    Kniwwelino.MATRIXwrite(Kniwwelino.getIP(), 1, false);
  }

  Kniwwelino.RGBsetBrightness((int)250);
  Kniwwelino.MATRIXwriteOnce(str);

  if (Kniwwelino.BUTTONAclicked())
  {
    for (int count = 0; count < 10; count++)
    {
      Kniwwelino.RGBsetColorEffect(String("A1FF46"), RGB_ON, -1);
      Kniwwelino.sleep((unsigned long)(5 * 1000));
      Kniwwelino.RGBsetColorEffect(String("FF0000"), RGB_BLINK, -1);
      Kniwwelino.sleep((unsigned long)(5 * 1000));
      Kniwwelino.loop(); // do background stuff...
    }
  }
  if (Kniwwelino.BUTTONBclicked())
  {
    for (int count2 = 0; count2 < 10; count2++)
    {
      Kniwwelino.RGBsetColorEffect(String("3366FF"), RGB_FLASH, -1);
      Kniwwelino.sleep((unsigned long)(5 * 1000));
      Kniwwelino.RGBsetColorEffect(String("CC33CC"), RGB_GLOW, -1);
      Kniwwelino.sleep((unsigned long)(5 * 1000));
      Kniwwelino.loop(); // do background stuff...
    }
  }
  if (Kniwwelino.BUTTONABclicked())
  {
    Kniwwelino.RGBclear();
  }

  Kniwwelino.loop(); // do background stuff...
}