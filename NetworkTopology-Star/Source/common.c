void delay_10us(unsigned int i)
{
	int j;
	while (i--)
	{
		for (j = 0; j < 4; j++)
		{
			asm("nop");	//1´ÎÖ¸ÁîÔ¼0.315us
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
		}
	}
}

void delay_1ms(unsigned int i)
{
	while (i--)
	{
		delay_10us(100);
	}
}