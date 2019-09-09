#include "HX711.h" //HX711로드셀 엠프 관련함수 호출

#define calibration_factor -7050.0 // 로드셀 스케일 값 선언

#define DOUT  3 //엠프 데이터 아웃 핀 넘버 선언
#define CLK  2  //엠프 클락 핀 넘버 
HX711 scale(DOUT, CLK); //엠프 핀 선언 

#define Mortor_DIR 5               // 모터 방향
#define Mortor_PWM 4                 // 모터 속도 출력
#define piezo 6

int mortor_time = 4500;
int mortor_time2 = mortor_time + 400;
float weight = 0;
int grade = 1;

/*
 * 변경할 부분
 */
const int WEIGHT_LIMIT_UP = 8;            // 기어가 올라가는 무게
const int WEIGHT_LIMIT_DOWN = 7;          // 기어가 내력는 무게
const int COUNT_LIMIT = 25                // 수집 카운트
////////////////////////////////////////////////////////////////


void mortorRun(int dir,int value,int delay_time,int type){
    int val = 0;
    if(dir == 1){
      val = 1000;
    }
     digitalWrite(piezo,HIGH);
     analogWrite(Mortor_DIR,val);
     analogWrite(Mortor_PWM, value);
     if(type == 1){
      delay(delay_time/2);
      digitalWrite(piezo,LOW);
      delay(delay_time/2);
     }else if(type == 2){
      delay(delay_time/6);
      digitalWrite(piezo,LOW);
      delay(delay_time/6);
      digitalWrite(piezo,HIGH);
      delay(delay_time/6);
      digitalWrite(piezo,LOW);
      delay(delay_time/6);
      digitalWrite(piezo,HIGH);
      delay(delay_time/6);
      digitalWrite(piezo,LOW);
      delay(delay_time/6);
     }
     
     analogWrite(Mortor_PWM, 0);
     Serial.println("------");
     Serial.println(grade);
     Serial.println("------");
}


void setup() {
  pinMode(piezo, OUTPUT);
  Serial.begin(9600);  // 시리얼 통신 개방
  Serial.println("HX711 scale TEST");  
  scale.set_scale(calibration_factor);  //스케일 지정 
  scale.tare();  //스케일 설정
  Serial.println("Readings:");
}
int total = 0;
double total_dgree = 0;
int before_dgree = 0;
int count = 0;
boolean first_flag = false;
void loop() {
  if(count > COUNT_LIMIT){
    int power = total / 30;
  
    
    if(!first_flag){
      first_flag = true;
    }else {
      Serial.println("==========");
      Serial.print(power);
      Serial.println("==========");
      count = 0;
      total = 0;
        if(power > WEIGHT_LIMIT_UP && grade < 7){
        grade = grade + 1;
        Serial.println("무게 업");
        Serial.println("기어 업");
        mortorRun(1,10000,mortor_time,1);
        Serial.println("기어 정지");
      }else if(power < WEIGHT_LIMIT_DOWN && grade > 1){
        grade = grade - 1;
        Serial.println("무게 다운");
        Serial.println("기어 다운");
        mortorRun(0,10000, mortor_time2,1);
        Serial.println("기어 정지");
      }
      
      
      
      if(grade < 0 ){
        grade = 1;
      }
    }
   total_dgree = 0;
  }
  

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);  //무제 출력 
  weight = scale.get_units() * 0.45359237 / 7;
  Serial.print(" lbs"); //단위
  Serial.print(" | "); //단위
  Serial.print(weight, 1); //단위
  Serial.print(" kg "); //단위
  Serial.print("||");
  Serial.print(total_dgree);
  Serial.print("||");
  Serial.println(); 
  
 
  if (Serial.available()) {
    int ch = Serial.parseInt();
    if(ch == 2){
      grade = grade + 1;
       Serial.println("기어 업");
       mortorRun(1,10000,mortor_time,2);
       Serial.println("기어 정지");
    }else if(ch == 1){
      grade = grade - 1;
      Serial.println("기어 다운");
      mortorRun(0,10000, mortor_time2,2);
      Serial.println("기어 정지");
    }
  }
  //delay(1);
  Serial.print(count);
  count++;
  total += weight;
}
