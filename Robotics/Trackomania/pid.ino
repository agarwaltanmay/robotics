#define RIGHTMOTOR1 22
#define RIGHTMOTOR2 41
#define LEFTMOTOR1 23
#define LEFTMOTOR2 43
#define RIGHTMOTORPWM 6
#define LEFTMOTORPWM 7
#define constbase 220
#define constmaxi 250
#define constmini 80
#define constKP 30
#define constKI 0.1
#define constKD 10

double P,I,I1,D,preverror;

int thres[7]={
  500,500,600,950,700,500,500};
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
  Serial.begin(9600);
  I=0;
  I1=0;
  preverror=0;  
  fl=fr=0;
}

int s[7],s1[7],i;

void readsensor()
{
  for(i=0;i<7;i++)
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
  Serial.println("  ");

}

int wmean,n,correction;

int calcerror()
{
  readsensor();
  if((s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6])!=0)
  {
    wmean=(((s[0])+(2*s[1])+(3*s[2])+(4*s[3])+(5*s[4])+(6*s[5])+(7*s[6]))/(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]));
    n=(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]);
    if(n==7) return (-200); //junction
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

  if(n>=4 && n!=7) // 90 degree turn
  {
    if(wmean>3.5)
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


