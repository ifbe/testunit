int ch1_value, ch1_freq;
int ch1_start, ch1_stop;
int val=0;
HardwareTimer timer(1);

void setup()
{
  Serial.begin(115200);

  pinMode(PA8, PWM);
  pinMode(PA9, PWM);
  timer.setPrescaleFactor(72);
  timer.setOverflow(1000);

  pinMode(PB0, INPUT_ANALOG);

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
  pwmWrite(PA9, 1000-val);
  val++;
  if(val > 990)val = 10;
  delay(10);

  Serial.print(ch1_value);
  Serial.print(',');
  Serial.print(ch1_freq);
  Serial.print(',');
  Serial.print(map(analogRead(PB0), 0, 4096, 0, 3300));
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
