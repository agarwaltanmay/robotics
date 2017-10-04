 #define RIGHTMOTOR1 20
#define RIGHTMOTOR2 31
#define LEFTMOTOR1 27
#define LEFTMOTOR2 34
#define RIGHTMOTORPWM 9
#define LEFTMOTORPWM 4
#define constbase 220
#define constmaxi 250
#define constmini 80
#define constKP 30
#define constKI 0.1
#define constKD 10

double P,I,I1,D,preverror;

int thres[8]={
  500,500,600,950,700,500,500,500};
int fl,fr;

void setup()
{
  pinMode(RIGHTMOTOR1,OUTPUT);
  pinMode(RIGHTMOTOR2,OUTPUT);
  pinMode(LEFTMOTOR1,OUTPUT);
  pinMode(LEFTMOTOR2,OUTPUT);
  pinMode(0,INPUT);
  pinMode(1,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  Serial.begin(9600);
  I=0;
  I1=0;
  preverror=0;  
  fl=fr=0;
}

int s[8],s1[8],i;

void readsensor()
{
  for(i=0;i<8;i++)
  {
    s1[i]=analogRead(i);
    if(s1[i]>thres[i]) s[i]=0;
    else s[i]=1;  
  }

  Serial.print(s[0]);
  Serial.print(s[1]);
  Serial.print(s[2]);
  Serial.print(s[3]);
  Serial.print(s[4]);
  Serial.print(s[5]);
  Serial.print(s[6]);
  Serial.print(s[7]);
  Serial.println("  ");

}

void readsensorthr()
{
  for(i=0;i<8;i++)
  {
    s1[i]=analogRead(i);
//    if(s1[i]>thres[i]) s[i]=0;
  //  else s[i]=1;  
  }

  Serial.print(s1[0]);
  Serial.print("  ");
  Serial.print(s1[1]);
  Serial.print("  ");
  Serial.print(s1[2]);
  Serial.print("  ");
  Serial.print(s1[3]);
  Serial.print("  ");
  Serial.print(s1[4]);
  Serial.print("  ");
  Serial.print(s1[5]);
  Serial.print("  ");
  Serial.print(s1[6]);
  Serial.print("  ");
  Serial.print(s1[7]);
  Serial.println("  ");

}

int wmean,n,correction;

int calcerror()
{
  readsensorthr();
  if((s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7])!=0)
  {
    wmean=(((s[0])+(2*s[1])+(3*s[2])+(4*s[3])+(5*s[4])+(6*s[5])+(7*s[6])+(8*s[7]))/(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]));
    n=(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]);
    if(n==8) return (-200); //junction
    else
        return(4-wmean);
  }
  else return (-198); //fully white- off track

}

int error;


int dopid()
{
  error=calcerror();

  //Serial.print(error);
  //Serial.print("  ");
 
  if(error==(-200)) return 0; //junction, ignore and go straight
  if(error==(-198)) error=preverror;//fully white- off track
  P=constKP*error;
  I+=error*constKI;
  D=constKD*(error-preverror);
  preverror=error;
  correction=P+I+D;
 if(constbase+correction>constmaxi) correction=constmaxi-constbase;
 // so that the PWM duty cycle never exceeds constmaxi/255 *100%
 if(constbase-correction<constmini) correction=constbase-constmini;
 // so that the PWM duty cycle never goes below constmini/255 *100%
  return correction;
}


int corr=0;

void loop()
{ 
  corr=dopid();
  Serial.println(corr);

  if(n>=4 && n!=8) // 90 degree turn
  {
    if(wmean>4)
    {   
      digitalWrite(RIGHTMOTOR1,HIGH);
      digitalWrite(RIGHTMOTOR2,LOW);
      digitalWrite(LEFTMOTOR1,LOW);
      digitalWrite(LEFTMOTOR2,HIGH);
      analogWrite(RIGHTMOTORPWM,250);
      analogWrite(LEFTMOTORPWM,250);
    }
   else
    {
      digitalWrite(RIGHTMOTOR1,LOW);
      digitalWrite(RIGHTMOTOR2,HIGH);
     digitalWrite(LEFTMOTOR1,HIGH);
     digitalWrite(LEFTMOTOR2,LOW);
      analogWrite(RIGHTMOTORPWM,250);
      analogWrite(LEFTMOTORPWM,250);
    }

  }

  else 
  { 
    digitalWrite(RIGHTMOTOR1,HIGH);
    digitalWrite(RIGHTMOTOR2,LOW);
    digitalWrite(LEFTMOTOR1,HIGH);
   digitalWrite(LEFTMOTOR2,LOW);
    analogWrite(RIGHTMOTORPWM,(constbase+corr));
    analogWrite(LEFTMOTORPWM,(constbase-corr));       
  }

}



