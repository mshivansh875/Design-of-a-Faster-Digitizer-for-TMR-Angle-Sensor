#include<math.h>
#include<LiquidCrystal.h>

#define A 3
#define B 4
#define C 5

#define Dp 2

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en , d4, d5, d6, d7);

long volatile int prev_time = 0,curr_time = 0;

float Td1 = 0, Td2 = 0, sum_Td1 = 0, sum_Td2 = 0;
bool mode = HIGH;

float prev_t=0,curr_t=0;

int count1 = 0,count2 = 0;

bool change_flag = false;

int Avg_cnt = 2;

float angle_in_radian = 0;

bool tan_flag = false;

bool switch_c_val = false;

bool cos_sign = 0, sin_sign = 0;

int Quadrant = 0;

float angle = 0;
float Angle_array[200];
int i = 0, j = 0;
float Angle_sum = 0, Angle_mean = 0, sd = 0, sd_sum = 0,snr = 0,Angle_square_sum =0;

int N = 200;

void setup() {

  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(Dp,INPUT);
  attachInterrupt(digitalPinToInterrupt(Dp),mode_change,CHANGE);
  digitalWrite(A,HIGH);
  digitalWrite(C,digitalRead(Dp));
  Serial.begin(9600);

  lcd.begin(16,2);
}

void loop() 
{
  if(change_flag)
  {
    curr_time = micros();
    if((curr_time - prev_time) >= 2000)
    { 
      switch_c_val = digitalRead(Dp);
      digitalWrite(C,switch_c_val);
      digitalWrite(A,HIGH);
      change_flag = false;
      prev_t = micros();

      if(!mode) cos_sign = !digitalRead(Dp);
      else 
      {
        sin_sign = !digitalRead(Dp);
        if(cos_sign == 1 && sin_sign == 1) Quadrant = 1;
        else if(cos_sign == 0 && sin_sign == 1) Quadrant = 2;
        else if(cos_sign == 0 && sin_sign ==0) Quadrant = 3;
        else Quadrant = 4;
      }
    }
  }
 if(tan_flag)
 {
    Serial.println("The value of the angle in degrees is = ");
    if(Quadrant == 1) angle = (180* atan2(Td2,Td1))/3.14159265;
    else if(Quadrant == 2) angle = 180-(180* atan2(Td2,Td1))/3.14159265;
    else if(Quadrant == 3) angle = 180+(180* atan2(Td2,Td1))/3.14159265;
    else if(Quadrant == 4) angle = 360-(180* atan2(Td2,Td1))/3.14159265;
    Serial.println(angle , 4);
    lcd.clear();
    lcd.print("Angle in degree");
    lcd.setCursor(0,1);
    lcd.print("= ");
    lcd.print(angle,4);
    Angle_array[i] = angle;
    Angle_sum += angle;
    Serial.print(angle,2);
    if(i==N-1)
    {
      Angle_mean = Angle_sum / N;
      Angle_sum = 0;
      i= 0;
      for(int j = 0;j<N;j++)
      {
        sd_sum += pow(Angle_array[j]-Angle_mean,2);
        Angle_square_sum += pow(Angle_array[j],2);
      }
      sd = sqrt(sd_sum/(N-1));
      snr = 10*log10(Angle_square_sum/sd_sum);
      Serial.println("The value of the angle in degrees is = ");
      Serial.println(Angle_mean , 4);
      Serial.println("The standard deviation of the above 10 angle samples is = ");
      Serial.println(sd,6);
      Serial.println("The signal to noise ratio of the above 10 angle samples is = ");
      Serial.println(snr,4);
      Serial.println("The effective number of bits = ");
      Serial.println((snr-1.76)/6.02,4);
      for (int i = 0; i < 200; i++) {
        Serial.print("arr[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(Angle_array[i],4);
      }
      lcd.clear();
      lcd.print("SD     SNR  ENoB");
      lcd.setCursor(0,1);
      lcd.print(sd,4);
      lcd.print(" ");
      lcd.print(snr,1);
      lcd.print(" ");
      lcd.print((snr-1.76)/6.02,2);
      sd_sum = 0;
      Angle_square_sum = 0;
    }
    else i++;
    tan_flag = false;
  }
}

void mode_change()
{
  if(!change_flag)
  {
  mode = !mode;
  curr_t = micros();
  digitalWrite(B,mode);
  digitalWrite(A,LOW);
  prev_time = micros();
  change_flag = true;
  if(j >1 && j < Avg_cnt*N+1)
  {
  
  if(mode)
  { 
    if(count1 >= 0 && count1 <= Avg_cnt-1)
    sum_Td1 = sum_Td1 + curr_t - prev_t;
    count1 = count1 +1;
    if(count1 == Avg_cnt )
    {
      Td1 = sum_Td1 / Avg_cnt;
      count1 = 0;
      sum_Td1 = 0;
    }
  }
  else
  {
    if(count2 >= 0 && count2<= Avg_cnt-1)
    sum_Td2 = sum_Td2 + curr_t - prev_t;
    count2 = count2 +1;
    if(count2 == Avg_cnt)
    {
      Td2 = sum_Td2 / Avg_cnt;
      count2 = 0;
      sum_Td2 = 0;
      tan_flag = true;
    }
  }

  }
  j++;
  if (j == Avg_cnt*N+4) j = 0;
  
 }

}
