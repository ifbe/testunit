void haha(void* frame)
{
}
__attribute__((interrupt)) void isr(void* frame)
{
	haha(frame);
}
