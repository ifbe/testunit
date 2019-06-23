int ch1_value, ch1_freq, ch1_start, ch1_stop;
int ch2_value, ch2_freq, ch2_start, ch2_stop;
int ch3_value, ch3_freq, ch3_start, ch3_stop;
int ch4_value, ch4_freq, ch4_start, ch4_stop;
int ch5_value, ch5_freq, ch5_start, ch5_stop;
int ch6_value, ch6_freq, ch6_start, ch6_stop;
int val=0;
HardwareTimer t4(4);
short table[] = {
  0, 174, 342, 500, 643, 766, 866, 940, 985,
1000, 985, 940, 866, 766, 643, 500, 342, 174,
  0, -174, -342, -500, -643, -766, -866, -940, -985,
-1000, -985, -940, -866, -766, -643, -500, -342, -174};

void setup()
{
  Serial.begin(115200);

  pinMode(PB6, PWM);
  pinMode(PB7, PWM);
  pinMode(PB8, PWM);
  pinMode(PB9, PWM);
  t4.setPrescaleFactor(72);
  t4.setOverflow(1000);

  //pinMode(PB0, INPUT_ANALOG);

  pinMode(PA0, INPUT);
  pinMode(PA1, INPUT);
  pinMode(PA2, INPUT);
  pinMode(PA3, INPUT);
  delay(250);

  Timer2.attachCompare1Interrupt(handler_ch1);
  Timer2.attachCompare2Interrupt(handler_ch2);
  Timer2.attachCompare3Interrupt(handler_ch3);
  Timer2.attachCompare4Interrupt(handler_ch4);
  TIMER2_BASE->CR1 = TIMER_CR1_CEN;
  TIMER2_BASE->CR2 = 0;
  TIMER2_BASE->SMCR = 0;
  TIMER2_BASE->DIER = TIMER_DIER_CC1IE | TIMER_DIER_CC2IE | TIMER_DIER_CC3IE | TIMER_DIER_CC4IE;
  TIMER2_BASE->EGR = 0;
  TIMER2_BASE->CCMR1 = 0b100000001;//TIMER_CCMR1_CC1S_INPUT_TI1;
  TIMER2_BASE->CCMR2 = 0b100000001;
  TIMER2_BASE->CCER = TIMER_CCER_CC1E | TIMER_CCER_CC2E | TIMER_CCER_CC3E | TIMER_CCER_CC4E;
  TIMER2_BASE->PSC = 71;
  TIMER2_BASE->ARR = 0xffff;
  TIMER2_BASE->DCR = 0;
/*
  Timer3.attachCompare1Interrupt(handler_ch5);
  Timer3.attachCompare2Interrupt(handler_ch6);
  TIMER3_BASE->CR1 = TIMER_CR1_CEN;
  TIMER3_BASE->CR2 = 0;
  TIMER3_BASE->SMCR = 0;
  TIMER3_BASE->DIER = TIMER_DIER_CC1IE | TIMER_DIER_CC2IE;
  TIMER3_BASE->EGR = 0;
  TIMER3_BASE->CCMR1 = 0b100000001;//TIMER_CCMR1_CC1S_INPUT_TI1;
  TIMER3_BASE->CCMR2 = 0;
  TIMER3_BASE->CCER = TIMER_CCER_CC1E | TIMER_CCER_CC2E;
  TIMER3_BASE->PSC = 71;
  TIMER3_BASE->ARR = 0xffff;
  TIMER3_BASE->DCR = 0;*/
}

void loop()
{
  pwmWrite(PB6, table[(val+ 0)%36]/2 + 500);
  pwmWrite(PB7, table[(val+ 6)%36]/2 + 500);
  pwmWrite(PB8, table[(val+12)%36]/2 + 500);
  pwmWrite(PB9, table[(val+18)%36]/2 + 500);
  val++;
  if(val >= 36)val = 0;
  delay(100);

  //Serial.print(map(analogRead(PB0), 0, 4096, 0, 3300));
  Serial.print(ch1_value);
  Serial.print(',');
  Serial.print(ch2_value);
  Serial.print(',');
  Serial.print(ch3_value);
  Serial.print(',');
  Serial.print(ch4_value);
  Serial.print('\n');
}

void handler_ch1()
{
  if(1 & GPIOA_BASE->IDR)
  {
    ch1_freq = ch1_start;
    ch1_start = TIMER2_BASE->CCR1;
    ch1_freq = ch1_start - ch1_freq;
    if(ch1_freq < 0)ch1_freq += 0xffff;

    TIMER2_BASE->CCER |= TIMER_CCER_CC1P;
  }
  else
  {
    ch1_value = TIMER2_BASE->CCR1 - ch1_start;
    if(ch1_value < 0)ch1_value += 0xffff;

    TIMER2_BASE->CCER &= ~TIMER_CCER_CC1P;
  }
}
void handler_ch2()
{
  if(2 & GPIOA_BASE->IDR)
  {
    ch2_freq = ch2_start;
    ch2_start = TIMER2_BASE->CCR2;
    ch2_freq = ch2_start - ch2_freq;
    if(ch2_freq < 0)ch2_freq += 0xffff;

    TIMER2_BASE->CCER |= TIMER_CCER_CC2P;
  }
  else
  {
    ch2_value = TIMER2_BASE->CCR2 - ch2_start;
    if(ch2_value < 0)ch2_value += 0xffff;

    TIMER2_BASE->CCER &= ~TIMER_CCER_CC2P;
  }
}
void handler_ch3()
{
  if(4 & GPIOA_BASE->IDR)
  {
    ch3_freq = ch3_start;
    ch3_start = TIMER2_BASE->CCR3;
    ch3_freq = ch3_start - ch3_freq;
    if(ch3_freq < 0)ch3_freq += 0xffff;

    TIMER2_BASE->CCER |= TIMER_CCER_CC3P;
  }
  else
  {
    ch3_value = TIMER2_BASE->CCR3 - ch3_start;
    if(ch3_value < 0)ch3_value += 0xffff;

    TIMER2_BASE->CCER &= ~TIMER_CCER_CC3P;
  }
}
void handler_ch4()
{
  if(8 & GPIOA_BASE->IDR)
  {
    ch4_freq = ch4_start;
    ch4_start = TIMER2_BASE->CCR4;
    ch4_freq = ch4_start - ch4_freq;
    if(ch4_freq < 0)ch4_freq += 0xffff;

    TIMER2_BASE->CCER |= TIMER_CCER_CC4P;
  }
  else
  {
    ch4_value = TIMER2_BASE->CCR4 - ch4_start;
    if(ch4_value < 0)ch4_value += 0xffff;

    TIMER2_BASE->CCER &= ~TIMER_CCER_CC4P;
  }
}
void handler_ch5()
{
  if(0x40 & GPIOA_BASE->IDR)
  {
    ch5_freq = ch5_start;
    ch5_start = TIMER3_BASE->CCR1;
    ch5_freq = ch5_start - ch5_freq;
    if(ch5_freq < 0)ch5_freq += 0xffff;

    TIMER3_BASE->CCER |= TIMER_CCER_CC1P;
  }
  else
  {
    ch5_value = TIMER3_BASE->CCR1 - ch5_start;
    if(ch5_value < 0)ch5_value += 0xffff;

    TIMER3_BASE->CCER &= ~TIMER_CCER_CC1P;
  }
}
void handler_ch6()
{
  if(0x80 & GPIOA_BASE->IDR)
  {
    ch6_freq = ch6_start;
    ch6_start = TIMER3_BASE->CCR2;
    ch6_freq = ch6_start - ch6_freq;
    if(ch6_freq < 0)ch6_freq += 0xffff;

    TIMER3_BASE->CCER |= TIMER_CCER_CC2P;
  }
  else
  {
    ch6_value = TIMER3_BASE->CCR1 - ch6_start;
    if(ch6_value < 0)ch6_value += 0xffff;

    TIMER3_BASE->CCER &= ~TIMER_CCER_CC2P;
  }
}
