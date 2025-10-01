#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <PS2Keyboard.h>

///////////variables////////////
#define lcd_col 20
#define lcd_row 4
#define blank "                    "
#define msg "msg:                "
#define you "you:                "

int primes[4] = {2, 3, 5, 7};
bool prime2_assign = false;
bool prime1_assign = false;

bool scan_done = false;
int i = 0;
int menu = 1;
int node_count = 1;

int prime1 = 3;
int prime2 = 7;

int tx_lcd_col = 4;
int tx_lcd_row = 2;
int temp_tx_col = tx_lcd_col;
int temp_tx_row = tx_lcd_row;

int rx_lcd_col = 4;
int rx_lcd_row = 0;
int temp_rx_col = rx_lcd_col;
int temp_rx_row = rx_lcd_row;

int name_lcd_col = 0;
int name_lcd_row = 1;
int temp_name_col = name_lcd_col;
int temp_name_row = name_lcd_row;

int clear_lcd = 1;
int data_available = 0;

char c;
char buff[50];
String nodesnames[5];

String rcv_data = "";
String keypad_data = "";

String indata = "";
String myname = "";

////////////pins/////////////
const int DataPin = 3;
const int IRQpin = 2;
const int espRST = 7;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(10, 11, A0, A1, A2, A3);
SoftwareSerial esp(8, 9);
PS2Keyboard keyboard;

void setup()
{
  Serial.begin(9600);
  esp.begin(115200);
  pinMode(espRST , OUTPUT);
  keyboard.begin(DataPin, IRQpin);
  lcd.begin(lcd_col, lcd_row);
  lcd.clear();
  lcd.print(F("  Data Transmission")); // Print a message to the LCD.
  lcd.setCursor(0, 1);              // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.print(F("    through WIFI"));
  lcd.setCursor(0, 2);
  lcd.print("       using");
  lcd.setCursor(0, 3);
  lcd.print(F("   RSA encryption"));
  digitalWrite(espRST, HIGH);
  delay(500);
  digitalWrite(espRST, LOW);
  delay(100);
  digitalWrite(espRST, HIGH);
  delay(3000);
  esp.flush();

  lcd.clear();
  lcd.setCursor( 0 , 0);
  lcd.print(F("Enter 2 prime numbers:"));
  lcd.setCursor( 0 , 1);
  lcd.print(F("between 2 and 9 "));
  i = 0;
  while (!prime2_assign)
  {
    if (keyboard.available())
    {
      c = char(keyboard.read());
      if (c != PS2_ENTER && c != PS2_BACKSPACE)
      {
        myname += c;
        lcd.setCursor( temp_name_col , temp_name_row);
        lcd.print(c);
        temp_name_col++;
      }
      if ( temp_name_col >= lcd_col)
      {
        temp_name_row++;
        temp_name_col = name_lcd_col;
        if ( temp_name_row >= lcd_row )
        {
          lcd.setCursor( temp_name_col , temp_name_row);
          lcd.print(c);
          temp_name_row = name_lcd_row;
          temp_name_col = name_lcd_col;
        }
      }
      if (c == PS2_ENTER)
      {
        if (!prime1_assign)
        {
          prime1 = myname.toInt();
          for (int pp = 0; pp <= 4 ; pp++)
          {
            if (prime1 == primes[pp])
            {
              prime1_assign = true;
              break;
            }
          }
        }
        else
        {
          prime2 = myname.toInt();
          for (int pp = 0; pp <= 4 ; pp++)
          {
            if (prime2 == primes[pp])
            {
              prime2_assign = true;
              break;
            }
          }
        }
        temp_name_row = name_lcd_row;
        temp_name_col = name_lcd_col;
        break;
      }
      if (c == PS2_BACKSPACE)
      {
        if (temp_name_col > name_lcd_col)
        {
          temp_name_col--;
          lcd.setCursor( temp_name_col , temp_name_row);
          lcd.print(" ");
          lcd.setCursor( temp_name_col , temp_name_row);
          i = i - 1;
          myname[i] = 0;
        }
        else if ( temp_name_row > name_lcd_row)
        {
          temp_name_row = name_lcd_row;
          temp_name_col = lcd_col - 1;
          lcd.setCursor( temp_name_col , temp_name_row);
          lcd.print(" ");
          lcd.setCursor( temp_name_col , temp_name_row);
          i = i - 1;
          myname[i] = 0;
        }
      }
      else
      {
        i++;
      }
    }
  }

  lcd.clear();
  lcd.setCursor( 0 , 0);
  lcd.print(F("->Search for device"));
  lcd.setCursor( 0 , 1);
  lcd.print(F("Wait for connection"));
  while (1)
  {
    if (keyboard.available())
    {
      c = char(keyboard.read());
      if (c == PS2_UPARROW)
      {
        lcd.clear();
        lcd.setCursor( 0 , 0);
        lcd.print(F("->Search for device"));
        lcd.setCursor( 0 , 1);
        lcd.print(F("Wait for connection"));
        menu = 1;
      }
      else if (c == PS2_DOWNARROW)
      {
        lcd.clear();
        lcd.setCursor( 0 , 0);
        lcd.print(F("Search for device"));
        lcd.setCursor( 0 , 1);
        lcd.print(F("->Wait for connectn"));
        menu = 2;
      }
      else if (c == PS2_ENTER)
      {
        break;
      }
    }
  }
  lcd.clear();
  lcd.setCursor( 0 , 0);
  lcd.print(F("Enter your name:"));
  i = 0;
  while (1)
  {
    if (keyboard.available())
    {
      c = char(keyboard.read());
      if (c != PS2_ENTER && c != PS2_BACKSPACE)
      {
        myname += c;
        lcd.setCursor( temp_name_col , temp_name_row);
        lcd.print(c);
        temp_name_col++;
      }
      if ( temp_name_col >= lcd_col)
      {
        temp_name_row++;
        temp_name_col = name_lcd_col;
        if ( temp_name_row >= lcd_row )
        {
          lcd.setCursor( temp_name_col , temp_name_row);
          lcd.print(c);
          temp_name_row = name_lcd_row;
          temp_name_col = name_lcd_col;
        }
      }
      if (c == PS2_ENTER)
      {
        temp_name_row = name_lcd_row;
        temp_name_col = name_lcd_col;
        break;
      }
      if (c == PS2_BACKSPACE)
      {
        if (temp_name_col > name_lcd_col)
        {
          temp_name_col--;
          lcd.setCursor( temp_name_col , temp_name_row);
          lcd.print(" ");
          lcd.setCursor( temp_name_col , temp_name_row);
          i = i - 1;
          myname[i] = 0;
        }
        else if ( temp_name_row > name_lcd_row)
        {
          temp_name_row = name_lcd_row;
          temp_name_col = lcd_col - 1;
          lcd.setCursor( temp_name_col , temp_name_row);
          lcd.print(" ");
          lcd.setCursor( temp_name_col , temp_name_row);
          i = i - 1;
          myname[i] = 0;
        }
      }
      else
      {
        i++;
      }
    }
  }
  if (menu == 1)
  {
    esp.println("scan," + myname);
    lcd.clear();
    indata = "";
    int clm = 1;
    while (!scan_done)
    {
      indata = get_data();
      if (indata != "")
      {
        Serial.print(indata);
        if (indata.indexOf("scan") >= 0)
        {
          node_count = 1;
          lcd.clear();
          lcd.print(F("Scaning for devices"));
          while (1)
          {
            indata = get_data();
            if (indata != "")
            {
              Serial.print(indata);
              if (indata.indexOf('$') >= 0)
              {
                if (clm <= 3)
                {
                  indata.toCharArray(buff, indata.length() + 1);
                  sscanf(buff , "$%[^$]$", buff);
                  lcd.setCursor( 0 , clm);
                  lcd.print(String(clm) + ")");
                  nodesnames[clm - 1] = String(buff);
                  lcd.print(nodesnames[clm - 1]);
                  clm++;
                }
                else
                {
                  break;
                }
              }
              else if (indata.indexOf("end") >= 0)
              {
                break;
              }
              else if (indata.indexOf("alone") >= 0)
              {
                lcd.clear();
                lcd.setCursor( 0 , 1);
                lcd.print(F("  No device found"));
                while (1);
              }
            }
          }
          scan_done = true;
        }
      }
    }
    delay(2000);
    lcd.clear();
    lcd.print(F("Press entr to connct"));
    for (int i = 1; i <= node_count; i++)
    {
      lcd.setCursor( 0 , i);
      if ( i == 1)
      {
        lcd.print("->");
      }
      lcd.print(String(i) + ")");
      lcd.print(nodesnames[i - 1]);
    }
    int arrow = 1;
    while (1)
    {
      if (keyboard.available())
      {
        c = char(keyboard.read());
        if (c == PS2_UPARROW)
        {
          if (arrow < node_count)
          {
            arrow--;
          }
          else
          {
            arrow = 1;
          }
          for (int i = 1; i <= node_count; i++)
          {
            lcd.setCursor( 0 , arrow);
            if ( arrow == i)
            {
              lcd.print("->");
            }
            lcd.print(String(i) + ")");
            lcd.print(nodesnames[i - 1]);
            lcd.print("    ");
          }
        }
        else if (c == PS2_DOWNARROW)
        {
          if (arrow < node_count)
          {
            arrow++;
          }
          else
          {
            arrow = 1;
          }
          for (int i = 1; i <= node_count; i++)
          {
            lcd.setCursor( 0 , arrow);
            if ( arrow == i)
            {
              lcd.print("->");
            }
            lcd.print(String(i) + ")");
            lcd.print(nodesnames[i - 1]);
            lcd.print("    ");
          }
        }
        else if (c == PS2_ENTER)
        {
          lcd.clear();
          lcd.print(F("Connecting to"));
          lcd.setCursor( 0 , 1);
          lcd.print(nodesnames[arrow - 1]);
          esp.println("$" + nodesnames[arrow - 1] + "$");
        }
      }
      else
      {
        indata = get_data();
        if (indata != "")
        {
          if (indata.indexOf("@begins@") >= 0)
          {
            break;
          }
        }
      }
    }
  }
  if (menu == 2)
  {
    esp.println("wait," + myname);
    lcd.clear();
    indata = "";
    while (!scan_done)
    {
      indata = get_data();
      if (indata != "")
      {
        indata.trim();
        Serial.println(indata);
        if (indata.indexOf("wait") >= 0)
        {
          lcd.clear();
          lcd.print(F("Waiting for "));
          lcd.setCursor( 0 , 1);
          lcd.print(F("connection"));
        }
        else if (indata.indexOf("req") >= 0)
        {
          int index1 = indata.indexOf(',');
          String nodename = indata.substring(index1 + 1);
          lcd.clear();
          lcd.print(F("Request from"));
          lcd.setCursor( 0 , 1);
          lcd.print(nodename);
          lcd.setCursor( 0 , 2);
          lcd.print(F("Press Entr to accept"));
          while (1)
          {
            if (keyboard.available())
            {
              c = char(keyboard.read());
              if (c == PS2_ENTER)
              {
                lcd.clear();
                esp.println("y");
                break;
              }
            }
          }
        }
        else if (indata.indexOf("@begins@") >= 0)
        {
          scan_done = true;
          break;
        }
      }
    }
  }
  lcd.clear();
  lcd.print(F("Connected"));
  delay(2000);
  lcd.clear();
  lcd.setCursor( 0 , 0);
  lcd.print(msg);
  lcd.setCursor( 0 , 1);
  lcd.print(blank);
  lcd.setCursor( 0 , 2);
  lcd.print(you);
  lcd.setCursor( 0 , 3);
  lcd.print(blank);
}
void loop()
{
  if (keyboard.available())
  {
    if (clear_lcd == 1)
    {
      lcd.setCursor( 0 , 2);
      lcd.print(you);
      lcd.setCursor( 0 , 3);
      lcd.print(blank);
      clear_lcd = 0;
    }
    c = char(keyboard.read());
    if (c != PS2_ENTER && c != PS2_BACKSPACE)
    {
      keypad_data += c;
      lcd.setCursor( temp_tx_col , temp_tx_row);
      lcd.print(c);
      temp_tx_col++;
    }
    if ( temp_tx_col >= lcd_col)
    {
      temp_tx_row++;
      temp_tx_col = tx_lcd_col;
      if ( temp_tx_row >= lcd_row )
      {
        lcd.setCursor( temp_tx_col , temp_tx_row);
        lcd.print(c);
        temp_tx_row = tx_lcd_row;
        temp_tx_col = tx_lcd_col;
      }
    }
    if (c == PS2_ENTER)
    {
      keypad_data.trim();
      lcd.setCursor( 0 , 2);
      lcd.print(F("you: sending msg... "));
      lcd.setCursor( 0 , 3);
      lcd.print(blank);
      if (senddata(keypad_data, true))
      {
        lcd.setCursor( 0 , 2);
        lcd.print(F("you: msg sent      "));
        lcd.setCursor( 0 , 3);
        lcd.print(blank);
      }
      else
      {
        lcd.setCursor( 0 , 2);
        lcd.print(F("you:failed to send  "));
        lcd.setCursor( 0 , 3);
        lcd.print(blank);
      }
      clear_lcd = 1;
      keypad_data = "";
      temp_tx_row = tx_lcd_row;
      temp_tx_col = tx_lcd_col;
    }
    if (c == PS2_BACKSPACE)
    {
      if (temp_tx_col > tx_lcd_col)
      {
        temp_tx_col--;
        lcd.setCursor( temp_tx_col , temp_tx_row);
        lcd.print(" ");
        lcd.setCursor( temp_tx_col , temp_tx_row);
        keypad_data.remove(keypad_data.length() - 1);
      }
      else if ( temp_tx_row > tx_lcd_row)
      {
        temp_tx_row = tx_lcd_row;
        temp_tx_col = lcd_col - 1;
        lcd.setCursor( temp_tx_col , temp_tx_row);
        lcd.print(" ");
        lcd.setCursor( temp_tx_col , temp_tx_row);
        keypad_data.remove(keypad_data.length() - 1);
      }
    }
  }
  else if (esp.available() > 0)
  {
    indata = esp.readString();
    indata.trim();
    indata = indata.substring(indata.indexOf('*') + 1, indata.indexOf('#'));
    if (indata.indexOf("ack") < 0)
    {
      Serial.println("rx : " + indata);
      indata.trim();
      data_available = 0;
      lcd.setCursor( 0 , 0);
      lcd.print(msg);
      lcd.setCursor( 0 , 1);
      lcd.print(blank);
      temp_rx_row = rx_lcd_row;
      temp_rx_col = rx_lcd_col;
      for (int j = 0 ; j < indata.length() ; j++ )
      {
        lcd.setCursor( temp_rx_col , temp_rx_row);
        lcd.print(indata[j]);
        temp_rx_col++;
        if ( temp_rx_col >= lcd_col)
        {
          temp_rx_row++;
          temp_rx_col = rx_lcd_col;
          if ( temp_rx_row >= rx_lcd_col)
          {
            lcd.setCursor( 0 , 0);
            lcd.print(msg);
            lcd.setCursor( 0 , 1);
            lcd.print(blank);
            temp_rx_row = rx_lcd_row;
            temp_rx_col = rx_lcd_col;
          }
        }
      }
      delay(100);
      //    Serial.println("sending ack");
      senddata("ack", false);
      rcv_data = "";
    }
  }
}

bool senddata(String data, bool ack)
{
  long t1 = millis();
  int first = 1;
  int resend = 0;
  data += '#';
  data = '*' + data;
  Serial.println("tx : " + data);
  if (ack)
  {
    //    Serial.println("encrypted msg : " + data);
    while (resend <= 4)
    {
      if (((millis() - t1) > 2000) || first == 1)
      {
        first = 0;
        resend++;
        esp.println(data);
        t1 = millis();
      }
      if (esp.available() >= 0)
      {
        indata = esp.readString();
        indata.trim();
        indata = indata.substring(indata.indexOf('*') + 1, indata.indexOf('#'));
        Serial.println("ack : " + indata);
        if (indata == "ack")
        {
          return true;
        }
      }
    }
    return false;
  }
  else
  {
    esp.println(data);
  }
  return true;
}

String get_data()
{
  if (esp.available() >= 0)
  {
    return (esp.readString());
  }
  else
  {
    return "";
  }
}