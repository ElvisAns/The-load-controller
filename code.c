 #include  <TimerOne.h>
volatile int i = 0;
volatile boolean zero_cross = 0;
int AC_pin = 4;

int dim = 0;
volatile unsigned long currentmicros, ledMillis;


int freqStep = 75;


typedef unsigned char uchar;

const int ledPin = 13;
const int led_on_off = 7;
int DIO = 7;
int CLK = 10;
int STB = 5; 
int button = 9;
int sensorValue = 0;
int x = 0, fliker = 0x01;
unsigned long nowTime, nowSec;

long totalTime;
int state = 1;

int minutes = 58, heures = 5;



int font(int Num)
{
  switch (Num) {

    case 0: //zero
      return 0x7B ;
      break;
    case 1: // 1
      return 0x30 ;
      break;
    case 2: // 2
      return 0xE9  ;
      break;
    case 3: // 3
      return 0xF8 ;;
      break;
    case 4: // 4
      return 0xB2 ;
      break;
    case 5: // 5
      return 0xDA;
      break;
    case 6: // 6
      return 0xDB ;
      break;
    case 7: // 7
      return  0x70 ;
      break;
    case 8: // 8
      return 0xFB ;
      break;
    case 9 : // 9
      return 0xFA  ;
      break;
    default:
      return 0 ;
      break;
  }
}

int displayx(int minutes, int heures)
{
  int minutes_dec = 0, minutes_un = 0, hr_dec = 0, hr_un = 0;
  minutes_un = minutes % 10;
  minutes_dec = (minutes / 10) % 6;
  hr_un = heures % 10;
  hr_dec = (heures / 10) % 3;

  int tab[4] = {minutes_un, minutes_dec, hr_un, hr_dec};

  for (int j = 0; j < 4 ; j++) {
    send_8bit(font(tab[j]));
    send_8bit(font(tab[j]));
  }

  if (millis() - nowTime >= 1000) {
    nowTime = millis();

  }
  send_8bit(fliker);
  send_8bit(fliker);


  send_8bit(0x00);
  send_8bit(0x00);

  send_8bit(0x00);
  send_8bit(0x00);
}
int displayy(int total_value)
{
  int minutes_dec = 0, minutes_un = 0, hr_dec = 0, hr_un = 0;
  minutes_un = total_value % 10;
  minutes_dec = (total_value / 10) % 10;
  hr_un = (total_value / 100) % 2;
  hr_dec = 0;

  int tab[4] = {minutes_un, minutes_dec, hr_un, hr_dec};

  for (int j = 0; j < 4 ; j++) {
    send_8bit(font(tab[j]));
    send_8bit(font(tab[j]));
  }

  send_8bit(0x00);
  send_8bit(0x00);

  send_8bit(0x00);
  send_8bit(0x00);

  send_8bit(0x00);
  send_8bit(0x00);
}


void send_8bit(uchar dat)
{
  uchar i ;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(CLK, 0);
    if (dat & 0x01)
      digitalWrite(DIO, 1);
    else
      digitalWrite(DIO, 0);

    digitalWrite(CLK, 1);
    dat = dat >> 1;
  }
}

void command(uchar com)
{
  digitalWrite(STB, 1);
  delayMicroseconds(1);
  digitalWrite(STB, 0);
  send_8bit(com);
}

void display(String type)
{
  uchar i;
  command(0x03);
  command(0x40);
  command(0xc0);

  if (type == "normal")
    displayx(minutes, heures);

  command(0x8f);
  digitalWrite(STB, 1);
}



void setup()
{
  pinMode(led_on_off, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(STB, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(2,INPUT_PULLUP);
  
  attachInterrupt(0, zero_cross_detect, FALLING);
  Timer1.initialize(freqStep);
  digitalWrite(led_on_off, HIGH);
  pinMode(AC_pin, OUTPUT);

  while (digitalRead(button)) {

    sensorValue = analogRead(A0);
    minutes = sensorValue % 60;
    heures = (sensorValue / 60 ) % 24 ;
    display(String("normal"));
    delay(500);
  }
  nowTime = millis(); 
  nowSec = millis();
}
int flag1;
int sec = 0;


void zero_cross_detect() {

  zero_cross = true;
  i = 0;
  digitalWrite(AC_pin, LOW);
}

void dim_check() {
  if (zero_cross == true) {
    if (i >= dim) {
      digitalWrite(AC_pin, HIGH);
      i = 0;
      if ((micros() - currentmicros) >= 100) {
        currentmicros = micros();
        digitalWrite(AC_pin, LOW);
        zero_cross = false;
      }
    }
    else {
      i++;
    }
  }
}


void loop()
{
  int analogvalue=analogRead(A0);
  dim = map(analogvalue,0,1023,64,0);

  display(String("normal"));
  if (millis() - nowSec >= 500 & state == 1) {

    nowSec = millis();
    x++;
    if (x == 1) {
      fliker = 0x01;
    }
    else {
      fliker = 0x00;
      x = 0;
      sec++;
      if (sec == 60) {
        if (heures >= 0) {
          minutes--;
          if (flag1 == 1) {
            heures--;
            flag1 = 0;
          }
          if (minutes == 00) {
            flag1 = 1;
            minutes = 60 ;
          }
        }
      }
      sec = sec % 60;
    }
  }

  if (state == 1) {
    Timer1.attachInterrupt(dim_check, freqStep);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
  else {
    Timer1.detachInterrupt();
    digitalWrite(ledPin, HIGH);
  }


  if (heures == 0) {
    if (minutes == 60) {
      state = 0;
    }
  }

}

