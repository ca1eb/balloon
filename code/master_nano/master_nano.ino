/* #include<avr/wdt.h> */
#define ADDRESS 1
#define HEADER 0
#define ARG1 2
#define ARG2 3
#define CRC8_1 4
#define CRC8_2 5
#define FOOTER 6
#define PACKET_LENGTH 6
#define POLY 0x8408
#define SWITCHING_DELAY 40 

#define RX_EN 3

uint8_t checksum_hash(uint8_t *data, int packet_length);
uint16_t crc16(uint8_t *data_p, unsigned short length);
void extractPackets();
void actOnPacket(char,char,char);
void addActOnBuff(char);
void flushActOnBuff(void);
void rx_enable(void);
void tx_enable(void);

using namespace std;
char actOnBuff[300];
int nextActBuffIndex = 0;

//int eEPromAddy = 0;
void setup() {
  pinMode(RX_EN,OUTPUT);

  /* wdt_enable(WDTO_500MS); */
  /* Serial.begin(9600); */
  Serial.begin(9600);
  /* while(Serial.available()<=0); */
  /* writeByCommand(Serial.read()); */
  writeByCommand('e');
    /*if (file == NULL)
    {
        return NULL; //could not open file
    }*/
  /* wdt_reset(); */
  /* extractPackets(); */
}
void loop(){
  delay(500);
  writeByCommand('2');
}

void extractPackets()
{
    uint8_t data[6];
    size_t n = 0;
    char c;
    int crc;
    int i = 0;
    bool isCorrupt;
    bool isPacket;
    bool done=0;
    int nextStart = 0;
    char address = 'n';

    while(address!='e')
    {
      /* wdt_reset(); */
        if(Serial.available()>0)
        {
            c = Serial.read();
            if(i == sizeof(data))//If the last byte of a possible packet is reached
            {
                crc = crc16(data, 4);
            
                //if checksum of the entire possible packet is passed
                if(data[4]==(uint8_t) (crc & 0xFF)&& data[5] == (uint8_t) ((crc >> 8) & 0xFF))
                {
                    done = 1;
                }
                //checksum of packet does not pass
                else
                {
                    done = 0;
                    //If there was a startbyte in middle of what was a possible packet
                    if(nextStart!=0)
                    {
                        int j;
                        for(j = nextStart;j < sizeof(data);j++)
                        {
                            data[j-nextStart] = data[j];
                        }
                        i = j - nextStart;
                        nextStart = 0;
                    }
                }
            }
        
            if(c=='@')
            {
                if(!done)
                {
                    if(i>=sizeof(data))
                    {
                        i = 0;
                        nextStart = 0;
                    }
                    else if(nextStart==0)
                        nextStart = i;   
                }
                else
                {
                    i = 0;
                    nextStart = 0;
                    /*for(int i=0;i<6;i++)
                    {
                      writeToStorage(data[i]);
                    }*/
                    actOnPacket(data[1],data[2],data[3]);
                    done = 0;
                    address = data[1];
                }
            }   
            
            if(!done)
            {
                if(i<sizeof(data))data[i]=c;
                i++;
            }        
        }
    }
    return;
}

/*void writeToStorage(char c)
{
  if(eEPromAddy<200)
    EEPROM.write(eEPromAddy++, c);
}
*/

void writeByCommand(char address)
{
    char dataOne;
    char dataTwo;
    switch(address)
    {
        case '1':
            dataOne = 'a';
            dataTwo = 'b';
            writeData(address,dataOne,dataTwo);
            break;
        case '2':
            dataOne = 'c';
            dataTwo = 'd';
            writeData(address,dataOne,dataTwo);
            break;
        case '3':
            dataOne = 'e';
            dataTwo = 'f';
            writeData(address,dataOne,dataTwo);
            break;
        case '4':
            dataOne = 'g';
            dataTwo = 'h';
            writeData(address,dataOne,dataTwo);
            break;
        case '5':
            writeData('5','a','b');
            break;
        case '6':
            writeData('6','c','d');
            break;
        case '7':
            writeData('7','e','f');
            break;
        case '8':
            writeData('8','g','h');
            break;
        case 'e':
            dataOne = 'n';
            dataTwo = 'd';

            tx_enable();
            writeData(address,dataOne,dataTwo);

            // don't have the slave set up yet, just send a command and don't wait for response
            /* rx_enable(); */
            /* extractPackets(); */
            break;
    }
    //writeData('e','n','d');    
    return;
}

void writeData(char address, char dataOne, char dataTwo)
{
      uint8_t data[6];
    
    data[HEADER] = '@';
    data[ADDRESS] = address;
    data[ARG1] = dataOne;
    data[ARG2] = dataTwo;
    
    int crc = crc16(data, 4);
    data[4] = (uint8_t) (crc & 0xFF);
    data[5] = (uint8_t) ((crc >> 8) & 0xFF);
    
    /* Serial.print("\nYo this is what I'm sending back to master: "); */
    int i;
    for (i=0;i < (sizeof (data) /sizeof (data[0]));i++) 
    {
          Serial.print((char)data[i]);
          /* Serial.print((char)data[i]);    */
      //fprintf(yo, "%c",(char)data[i]);
        //printf("%d\n",data[i]);
    }
    /* Serial.println(""); */
    if(address=='e')
    {
      Serial.print('@');
    }

}

void actOnPacket(char address, char dataOne, char dataTwo)
{
  /* wdt_reset(); */
    switch(address)
    {
        case '1':
            //printf("Sensor 1 data: %d mph, %d degrees\n",(int)dataOne,(int)dataTwo);
            /* Serial.print("Sensor 1 data: "); */
            /* Serial.print((int)dataOne); */
            /* Serial.print(" mph, "); */
            /* Serial.print((int)dataTwo); */
            /* Serial.print(" degrees\n"); */
            break;
        case '2':
            //printf("Sensor 2 data: %d K\n",(int)dataOne);
            /* Serial.print("Sensor 2 data: "); */
            /* Serial.print((int)dataOne); */
            /* Serial.print(" K\n"); */
            break;
        case '3':
            //printf("Sensor 3 data: %d N\n",(int)dataOne);
            /* Serial.print("Sensor 3 data: "); */
            /* Serial.print((int)dataOne); */
            /* Serial.print("N\n"); */
            break;
        case '4':
            //pri ntf("Sensor 4 data: %d m\n",(int) dataOne);
            /* Serial.print("Sensor 4 data: "); */
            /* Serial.print((int)dataOne); */
            /* Serial.print(" m\n"); */
            break;
        case '5':
        case '6':
        case '7':
        case '8':
            addActOnBuff(address-4);
            break;
        case 'e':
            /* if(Serial.available()>0) */
            /*   addActOnBuff(Serial.read()); */
            delay(SWITCHING_DELAY);
            flushActOnBuff();
            break;
    }
}

void addActOnBuff(char address)
{
  /* Serial.println("\nADDED SOMETHING TO BUFFER"); */
  if(nextActBuffIndex<sizeof(actOnBuff))
    actOnBuff[nextActBuffIndex++] = address;
  return;
}

void flushActOnBuff()
{
  for(int i=0; i<nextActBuffIndex&&i<sizeof(actOnBuff);i++)
  {
    writeByCommand(actOnBuff[i]);
    /* wdt_reset(); */
  }
  nextActBuffIndex = 0;
  writeByCommand('e');//send end packet
  return;
}

uint16_t crc16(uint8_t *data_p, unsigned short length)
{
      unsigned char i;
      unsigned int data;
      unsigned int crc = 0xffff;
 
      if (length == 0)
            return (~crc);
 
      do
      {
            for (i=0, data=(unsigned int)0xff & *data_p++;
                 i < 8; 
                 i++, data >>= 1)
            {
                  if ((crc & 0x0001) ^ (data & 0x0001))
                        crc = (crc >> 1) ^ POLY;
                  else  crc >>= 1;
            }
      } while (--length);
 
      crc = ~crc;
      data = crc;
      crc = (crc << 8) | (data >> 8 & 0xff);
 
      return (crc);
}

void tx_enable(){
  digitalWrite(RX_EN,LOW);
  delay(SWITCHING_DELAY);
}

void rx_enable(){
  digitalWrite(RX_EN,HIGH);
  delay(SWITCHING_DELAY);
}

uint8_t checksum_hash(uint8_t *data) {

  uint16_t total = 0; 
  uint8_t packet_length = sizeof(data) / sizeof(uint8_t);

  int i = 0;
  for (i = 0; i < packet_length; i++) {

    if (i != 1 && i != 2) {

      total += data[i];
    
    }
  
  }

  uint8_t low = total & 0xff;
  uint8_t checksum = 0xff;
  checksum = checksum - low;

  return checksum;

} 

