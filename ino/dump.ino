const int _D0  = 2;    // D0-D7
const int _A0  = 22;   // A0-A23

const int OE  = 10;
const int CS  = 11;
const int WE  = 12;
const int RST = 13;

void setAddress(unsigned long address, int isLoROM)
{
  if(isLoROM)
  {
    unsigned long upper = address / 0x8000;
    unsigned long lower = address % 0x8000;
    address = upper * 2 * 0x8000 + lower + 0x8000;
  }

  for(int i=0; i<24; i++)
  {
    digitalWrite(_A0 + i, (address & ((unsigned long)1 << i)) ? HIGH : LOW);
  }
}

void readData()
{
  byte b = 0;

  for(int i=0; i<8; i++)
  {
    if(digitalRead(_D0 + i) == HIGH)
    {
      b |= (1 << i);
    }
  }

  Serial.write(b);
}

void setup()
{
  for(int i=0; i<8; i++)
  {
    pinMode(_D0 + i, INPUT);
  }

  for(int i=0; i<24; i++)
  {
    pinMode(_A0 + i, OUTPUT);
  }
  
  pinMode(OE, OUTPUT);
  digitalWrite(OE, LOW);
  
  pinMode(CS, OUTPUT);
  digitalWrite(CS, LOW);

  pinMode(WE, OUTPUT);
  digitalWrite(WE, HIGH);

  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);

  Serial.begin(115200);
}

void loop()
{
  if(Serial.available() < 2)
  {
    return;
  }

  byte cmd = Serial.read();
  
  if(cmd == 'R' || cmd == 'r')
  {
    while(Serial.available() < 6)
    {
      delay(1);
    }

    int isLoROM = false;
    if(cmd == 'r')
    {
      isLoROM = true;
    }

    unsigned long address, datasize;

    address  = (unsigned long)Serial.read();
    address += (unsigned long)Serial.read() << 8;
    address += (unsigned long)Serial.read() << 16;

    datasize  = (unsigned long)Serial.read();
    datasize += (unsigned long)Serial.read() << 8;
    datasize += (unsigned long)Serial.read() << 16;
    
    for(unsigned long i=0; i<datasize; i++, address++)
    {
      setAddress(address, isLoROM);
      readData();
    }
  }
  else if (cmd == 'c')
  {
    // control
    byte b = Serial.read();
    
    digitalWrite(OE,  (b & B0001) ? HIGH : LOW);
    digitalWrite(CS,  (b & B0010) ? HIGH : LOW);
    digitalWrite(WE,  (b & B0100) ? HIGH : LOW);
    digitalWrite(RST, (b & B1000) ? HIGH : LOW);
  }
}
