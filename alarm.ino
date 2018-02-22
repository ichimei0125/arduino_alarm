unsigned long Time=0,Sec = 0;
int Hour = 0,Min = 0;
int SetTimeFlag = 0,SetAlarmFlag = 0;
boolean SecLED = true;
int AlarmH[10],AlarmM[10],AlarmNum = -1,AlarmFlag[10];
char Music[16] = {'e','e','e','g','a','C','C','a','g','g','g','a','g','g','g','g'};

#define IR 13
#define SPKR 12

#define HOUR_H 11
#define HOUR_L 10
#define MIN_H 9
#define MIN_L 8

#define SEC 7
#define A 6
#define B 5
#define C 4
#define D 3
#define E 2
#define F 1
#define G 0

#define SETTIME A5
#define SETHOUR A4
#define SETMIN A3
#define SETALARM A2
#define ADDALARM A1
#define DELALARM A0

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<=12;){
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
    i = i + 1;
  }
  pinMode(13,INPUT);
  pinMode(A5,INPUT);
  pinMode(A4,INPUT);
  pinMode(A3,INPUT);
  pinMode(A2,INPUT);
  pinMode(A1,INPUT);
  pinMode(A0,INPUT); 
  for(int j=0; j<=9;){
    AlarmFlag[j] = 0;
    j = j + 1; 
  }
}

void display_time(int h, int m){
  int hl = h % 10;
  int hh = (h - hl) / 10;
  int ml = m % 10;
  int mh = (m - ml) / 10;  
  for(int i=MIN_L; i<=HOUR_H;){
    switch (i){
      case MIN_L:
      digitalWrite(MIN_L,HIGH);
      digitalWrite(MIN_H,LOW);
      digitalWrite(HOUR_L,LOW);
      digitalWrite(HOUR_H,LOW);
      num_to_led(ml);
      break;
      case MIN_H:
      digitalWrite(MIN_L,LOW);
      digitalWrite(MIN_H,HIGH);
      digitalWrite(HOUR_L,LOW);
      digitalWrite(HOUR_H,LOW);
      num_to_led(mh);
      break;
      case HOUR_L:
      digitalWrite(MIN_L,LOW);
      digitalWrite(MIN_H,LOW);
      digitalWrite(HOUR_L,HIGH);
      digitalWrite(HOUR_H,LOW);
      num_to_led(hl);
      break;
      case HOUR_H:
      digitalWrite(MIN_L,LOW);
      digitalWrite(MIN_H,LOW);
      digitalWrite(HOUR_L,LOW);
      digitalWrite(HOUR_H,HIGH);
      num_to_led(hh);
      break;
      default:
      break;
    }
    delay(5);  // You will only see "8" without delay();
    i = i + 1;
  }
}

void display_real_time(){
  unsigned long timeTmp = millis();
  if(timeTmp - Time >= 1000){    //  second    
    Sec = Sec + (timeTmp - Time);    
    Time = timeTmp;  
    digitalWrite(SEC,SecLED);  // flash the ":"
    SecLED = !SecLED;
    if(Sec >= 60000){  // min
      Sec = Sec - 60000;      
      Min = Min + 1;
      if(Min == 60){  //hour
        Min = 0;
        Hour = Hour + 1;
        if(Hour == 24){
          Hour = 0;
        }
      }
    }
  }
  if(timeTmp < Time){  // Incase of millis() go back to zero after 50 days.
    Time = (4294967295 - 1) - Time;
  }  
  display_time(Hour,Min);
}

void set_time_button(){
  if(digitalRead(SETTIME) == LOW){
      SetTimeFlag = 0;
  }
  if(digitalRead(SETHOUR) == LOW){
    delay(200);
    Hour = Hour + 1;
    if(Hour == 24){
      Hour = 0;
    }
  }
  if(digitalRead(SETMIN) == LOW){
    delay(200);
    Min = Min + 1;
    if(Min == 60){
      Min = 0;
    }
  }
}

void set_time(){
  while(digitalRead(SETTIME) == HIGH && SetTimeFlag ==1){
    Sec = 0;  // stop counting second 
    for(int i=SEC; i<=HOUR_H;){
      digitalWrite(i,LOW);
      i = i + 1;
      set_time_button();
    }
    delay(150);            
    set_time_button();    
    for(int a=0; a<=20;){
      display_time(Hour,Min);
      a = a + 1;
      set_time_button();
    }    
  }  
  delay(200);
  SetTimeFlag = 0;
  Time = millis();  // set second to zero
  Sec = millis() - Time;
}

void set_alarm(){
  digitalWrite(SEC,LOW);
  boolean AFlash = true;
  digitalWrite(HOUR_H,AFlash);
  while(digitalRead(SETALARM) == HIGH && SetAlarmFlag == 1){
    num_to_led(AlarmNum);
    AFlash = !AFlash;
    delay(150);
    digitalWrite(HOUR_H,AFlash);
    if(digitalRead(ADDALARM) == LOW){
      delay(100);
      AlarmNum = AlarmNum + 1;
      if(AlarmNum > 9){
        AlarmNum = 9;
      }
    }
    if(digitalRead(DELALARM) == LOW){
      delay(100);
      AlarmH[AlarmNum] = 0;  // clear alarm data
      AlarmM[AlarmNum] = 0;
      AlarmNum = AlarmNum - 1;      
      if(AlarmNum < 0){
        AlarmNum = -1;
      }
    }
    
    if(digitalRead(SETTIME) == LOW){  // show all alarms
      if(AlarmNum == -1){
        for(int a=0; a<=2;){
          digitalWrite(HOUR_H,HIGH);
          num_to_led(-1);
          delay(200);
          digitalWrite(HOUR_H,LOW);
          delay(200);
          a = a + 1;
        }
      }
      else{  
        for(int i=0; i<=AlarmNum;){
          for(int b=0; b<=2;){
            digitalWrite(HOUR_H,HIGH);
            num_to_led(i);
            delay(200);
            digitalWrite(HOUR_H,LOW);
            delay(200);     
            b = b + 1;   
          }
            for(int j=0; j<=50;){
              display_time(AlarmH[i],AlarmM[i]);
              j = j + 1;
            }
          i = i + 1;
        }
      }
      SetAlarmFlag = 0;
    }
    
    if(digitalRead(SETHOUR) == LOW || digitalRead(SETMIN) == LOW){
      if(AlarmNum < 0){
        AlarmNum = 0;
      }
      delay(200);
      while(digitalRead(ADDALARM) == HIGH && digitalRead(DELALARM) == HIGH){
        if(digitalRead(SETHOUR) == LOW){
          delay(200);
          AlarmH[AlarmNum] =  AlarmH[AlarmNum] + 1;
          if(AlarmH[AlarmNum] == 24){
            AlarmH[AlarmNum] = 0;
          }
        }
        if(digitalRead(SETMIN) == LOW){
          delay(200);
          AlarmM[AlarmNum] = AlarmM[AlarmNum] + 1;
          if(AlarmM[AlarmNum] == 60){
            AlarmM[AlarmNum] = 0;
          }
        }
        display_time(AlarmH[AlarmNum],AlarmM[AlarmNum]);
      }
    }    
  }
  delay(200);
  SetAlarmFlag = 0;
}

float scale(char s){
  float hz;
  switch (s){
    case 'c':
    hz = 261.6;
    break;
    case 'd':
    hz = 293.7;
    break;
    case 'e':
    hz = 329.6;
    break;
    case 'f':
    hz = 349.2;
    break;
    case 'g':
    hz = 392.0;
    break;
    case 'a':
    hz = 440.0;
    break;
    case 'b':
    hz = 493.9;
    break;
    case 'C':
    hz = 523.3;
    break;
    default:
    break;
  }
  return (1000000 / hz) / 2;
}

void speaker(){
  digitalWrite(SEC,HIGH);
  int flag = 0; int t = 0;
  while( flag == 0){  
    for(int j=MIN_L; j<=HOUR_H;){
      digitalWrite(j,LOW);
      j = j + 1;
    }
  // speaker
    for(int i=0; i<=16;){
        for(int k=0; k<=130;){
          digitalWrite(SPKR,HIGH);
          delayMicroseconds(scale(Music[i]));
          digitalWrite(SPKR,LOW);
          delayMicroseconds(scale(Music[i]));
          k = k + 1; 
          if(digitalRead(IR) == HIGH){
            flag = 1;
          }          
        }        
     i = i + 1;
    }
     t = t + 1;  // time out 
     if(t>=15){
        flag = 1;
     }
   } 
}

void loop() {
  // put your main code here, to run repeatedly:
  display_real_time();

  // set time
  if(digitalRead(SETTIME) == LOW && SetTimeFlag == 0){
    delay(200);  // make sure you have loosed your hand.
    SetTimeFlag = 1;
    set_time();
  }

  // set alarm
  if(digitalRead(SETALARM) == LOW && SetAlarmFlag == 0){
    delay(200);
    SetAlarmFlag = 1;
    set_alarm();
  }

  // alarm
  if(AlarmNum >= 0 && AlarmNum <=9){
    for(int i=0; i<=AlarmNum;){
      if(AlarmH[i] == Hour && AlarmM[i] == Min && AlarmFlag[i] == 0){
        AlarmFlag[i] = 1;
        speaker();
      }
     if(AlarmH[i] != Hour || AlarmM[i] != Min){
       AlarmFlag[i] = 0;
     }
      i = i + 1;
    }
  }
}

void num_to_led (int num){
  switch (num){
    case 0:
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,LOW);
    digitalWrite(1,LOW);
    digitalWrite(G,HIGH);
    break;
    case 1:
    digitalWrite(A,HIGH);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,HIGH);
    digitalWrite(E,HIGH);
    digitalWrite(1,HIGH);
    digitalWrite(G,HIGH);
    break;
    case 2:
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,HIGH);
    digitalWrite(D,LOW);
    digitalWrite(E,LOW);
    digitalWrite(1,HIGH);
    digitalWrite(G,LOW);
    break;
    case 3:
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,HIGH);
    digitalWrite(1,HIGH);
    digitalWrite(G,LOW);
    break;
    case 4:
    digitalWrite(A,HIGH);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,HIGH);
    digitalWrite(E,HIGH);
    digitalWrite(1,LOW);
    digitalWrite(G,LOW);
    break;
    case 5:
    digitalWrite(A,LOW);
    digitalWrite(B,HIGH);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,HIGH);
    digitalWrite(1,LOW);
    digitalWrite(G,LOW);
    break;
    case 6:
    digitalWrite(A,LOW);
    digitalWrite(B,HIGH);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,LOW);
    digitalWrite(1,LOW);
    digitalWrite(G,LOW);
    break;
    case 7:
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,HIGH);
    digitalWrite(E,HIGH);
    digitalWrite(1,HIGH);
    digitalWrite(G,HIGH);
    break;
    case 8:
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,LOW);
    digitalWrite(1,LOW);
    digitalWrite(G,LOW);
    break;
    case 9:
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,HIGH);
    digitalWrite(1,LOW);
    digitalWrite(G,LOW);
    break;
    default:
    digitalWrite(A,HIGH);
    digitalWrite(B,HIGH);
    digitalWrite(C,HIGH);
    digitalWrite(D,HIGH);
    digitalWrite(E,HIGH);
    digitalWrite(1,HIGH);
    digitalWrite(G,LOW);
    break;
  }
}

