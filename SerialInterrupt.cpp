const int lengthMsgs = 11;
const int time_out = 999;
const int baudrate = 9600;
bool commanderReady = false;
int m_speedx, m_speedy, m_speedz;
int m_transx ,m_transy ,m_transz;
int m_rotx , m_roty , m_rotz;
int timing_step;
int speedx, speedy, speedz;
int transx, transy, transz;
float rotx, roty, rotz;
int m_protocol, m_time;
int neutral = 512;
int neutral_protocol = 127;
float float_divider_point = 10.0;
bool iswaitng = false;
int JR_PROTOCOL;
char chahe_commander[50];
char commander[50];
int decoded_data[10];
int index = 0;


void setup(){
  SerialUSB.begin();
  Serial2.begin(baudrate);
  Serial2.attachInterrupt(serialInterrupt);
  pinMode(BOARD_LED_PIN, OUTPUT);  //toggleLED_Pin_Out
}

void loop(){

  if (commanderReady){
    decoder_commander();
    
    SerialUSB.print("speedx : ");
    SerialUSB.println(speedx);
    SerialUSB.print("JR Protocol : ");
    SerialUSB.println(JR_PROTOCOL);
    SerialUSB.print("rotx : ");
    SerialUSB.println(rotx);
    SerialUSB.print("transz : ");
    SerialUSB.println(transz);

  }

}

/*
=========================================================================
 Serial interrupt communication decoding program
 =========================================================================
 */
void serialInterrupt(byte buffer){
  //SerialUSB.print(char (buffer));
  if (char (buffer) != '\n' ){ 
    chahe_commander[index] = char (buffer);
    index++;
  }
  else{
    commanderReady = true;
    for (int k = 0; k<index; k++){
      commander[k]=chahe_commander[k];
    }
    index=0; 
  }
}



/*
Decoding system JRProtocol V1
send packet format = xxxyyyzzztttpppuuurrreeewwwnnnjjj\n
 
 xxx = speedx
 yyy = speedy
 zzz = speedz
 
 ttt = translationx (encoding * 10)
 ppp = translationy (encoding * 10)
 uuu = translationz (encoding * 10)
 
 rrr = rotationx
 eee = rotationy
 www = rotationz
 nnn = step time
 jjj = jr command robot protocol
 
 jr protocol :
 SERVO_DISABLE   = 12;
 SERVO_ENABLE    = 14;
 
 WALKMODE_TRIPOD = 18;
 WALKMODE_WAVE   = 20;
 WALKMODE_RIPPLE = 22;
 
 WALK_STOP       = 28;
 WALK_PLAY       = 30;
 
 
 */
void decoder_commander(){
  
  int index_colom;
  int byte1,byte2,byte3;
  int counter_buff = 0;
  for (index_colom=0; index_colom<lengthMsgs; index_colom++){
    for (int k = index_colom; k<(3+index_colom); k++){
      counter_buff++;
      if ((k-index_colom) == 0){
        byte1 = int (commander[counter_buff-1] - '0')*100;
      }
      if ((k-index_colom) == 1){
        byte2 = int (commander[counter_buff-1] - '0')*10;
      }
      if ((k-index_colom) == 2){
        byte3 = int (commander[counter_buff-1] - '0');
      }
    }
    decoded_data[index_colom] = byte1 + byte2 + byte3; 
  }


  if (abs(decoded_data[0]>=time_out)){
    //SerialUSB.println("Data is not Match");
    //Serial data was corrupted.
     iswaitng = true;
  }
  else
  {
    m_speedx = decoded_data[0];
    m_speedy = decoded_data[1]; 
    m_speedz = decoded_data[2];

    m_transx = decoded_data[3];
    m_transy = decoded_data[4]; 
    m_transz = decoded_data[5]; 

    m_rotx = decoded_data[6];
    m_roty = decoded_data[7];
    m_rotz = decoded_data[8];
    m_time = decoded_data[9];
    m_protocol = decoded_data[10];

    timing_step = m_time - neutral_protocol ;
    JR_PROTOCOL =  m_protocol - neutral_protocol ;

    speedx = m_speedx - neutral;
    speedy = m_speedy - neutral;
    speedz = m_speedz - neutral;

    transx = m_transx - neutral;
    transy = m_transy - neutral;
    transz = m_transz - neutral;

    rotx = float((m_rotx - neutral))/float_divider_point;
    roty = float((m_roty - neutral))/float_divider_point;
    rotz = float((JR_PROTOCOL - neutral))/float_divider_point;
  }

}


