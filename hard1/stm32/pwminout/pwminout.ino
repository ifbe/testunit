int ch1, ch1_start, ch1_stop;
int val=0;
HardwareTimer timer(1);

void setup()
{
  Serial.begin(115200);

  pinMode(PA8, PWM);
  timer.setPrescaleFactor(72);
  timer.setOverflow(1000);

  pinMode(PA0, INPUT);
  delay(250);
  Timer2.attachCompare1Interrupt(handler_ch1);
  TIMER2_BASE->CR1 = TIMER_CR1_CEN;
  TIMER2_BASE->CR2 = 0;
  TIMER2_BASE->SMCR = 0;
  TIMER2_BASE->DIER = TIMER_DIER_CC1IE;
  TIMER2_BASE->EGR = 0;
  TIMER2_BASE->CCMR1 = TIMER_CCMR1_CC1S_INPUT_TI1;
  TIMER2_BASE->CCMR2 = 0;
  TIMER2_BASE->CCER = TIMER_CCER_CC1E;
  TIMER2_BASE->PSC = 71;
  TIMER2_BASE->ARR = 0xffff;
  TIMER2_BASE->DCR = 0;
}

void loop()
{
  pwmWrite(PA8, val);
  val++;
  if(val >= 1000)val = 0;
  delay(9);

  Serial.println(ch1);
}

void handler_ch1()
{
  if(1 & GPIOA_BASE->IDR)
  {
    ch1_start = TIMER2_BASE->CCR1;
    TIMER2_BASE->CCER |= TIMER_CCER_CC1P;
  }
  else
  {
    ch1 = TIMER2_BASE->CCR1 - ch1_start;
    if(ch1 < 0)ch1 += 0xffff;
    TIMER2_BASE->CCER &= ~TIMER_CCER_CC1P;
  }
}
