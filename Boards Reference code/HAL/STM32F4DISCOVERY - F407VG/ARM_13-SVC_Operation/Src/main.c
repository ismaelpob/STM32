/**
  ******************************************************************************
  * @Project        : ARM_13-SVC_Operation
  * @Autor          : Ismael Poblete V.
  * @Company		: -
  * @Date         	: 03-01-2021
  * @Target			: DISCOVERY-DISC1 STM32F407VG
  * @brief          : Example of acquisitions SVC number.
  * @Lib			: -
  * @System Clock
  * 	SYSSource:		HSI
  * 	SYSCLK: 		16MHz
  * 	RTCSource:		None
  * 	RTCCLK: 		None
  * @Perf: none
  * @note:
  *		-Generate a SVC number witch trigger the interrupt of SVC_Handler for
  *		each operation add, sub, mul, div numbers. In SVC_Handlers identifies
  *		the SVC number and in the SVC_Handler it execute the operation.
  *
  ******************************************************************************
**/
#include<stdint.h>
#include<stdio.h>

void SVC_Handle_c(uint32_t *pBaseStackFrame);

int32_t add_numbers(int32_t x , int32_t y )
{
	int32_t res;
	__asm volatile("SVC #36");
	__asm volatile ("MOV %0,R0": "=r"(res) ::);
	return res;

}


int32_t sub_numbers(int32_t x , int32_t y )
{
	int32_t res;
	__asm volatile("SVC #37");
	__asm volatile ("MOV %0,R0": "=r"(res) ::);
	return res;


}

int32_t mul_numbers(int32_t x , int32_t y )
{
	int32_t res;
	__asm volatile("SVC #38");
	__asm volatile ("MOV %0,R0": "=r"(res) ::);
	return res;


}

int32_t div_numbers(int32_t x , int32_t y )
{
	int32_t res;
	__asm volatile("SVC #39");
	__asm volatile ("MOV %0,R0": "=r"(res) ::);
	return res;

}


int main(void)
{
	int32_t res;

	res = add_numbers(40, -90);
	printf("Add result = %ld\n",res);

	res = sub_numbers(25,150);
	printf("Sub result = %ld\n",res);

    res = mul_numbers(374,890);
	printf("mul result = %ld\n", res);

	res = div_numbers(67,-3);
	printf("div result = %ld\n",res);



	for(;;);
}


__attribute__( ( naked ) ) void SVC_Handler( void )
{
	__asm ("MRS r0,MSP");
	__asm( "B SVC_Handler_c");
}


void SVC_Handler_c(uint32_t *pBaseOfStackFrame)
{
	printf("in SVC handler\n");

	int32_t arg0, arg1,res;

	uint8_t *pReturn_addr = (uint8_t*)pBaseOfStackFrame[6];

	/*2. decrement the return address by 2 to point to
	  opcode of the SVC instruction in the program memory*/
	pReturn_addr-=2;


	/*3. extract the SVC number (LSByte of the opcode)*/
	uint8_t svc_number = *pReturn_addr;
	printf("Svc number is : %d\n",svc_number);

    /* get the operands for the operation */
	arg0 = pBaseOfStackFrame[0];
	arg1 = pBaseOfStackFrame[1];

	/*SVC number signifies the type of operation , decode it */
	switch(svc_number)
	{
	/*SVC number to sum*/
	case 36:
		res = arg0 + arg1;
		break;
	/*SVC number to sub*/
	case 37:
		res = arg0 - arg1;
		break;
	/*SVC number to mul*/
	case 38:
		res = arg0 * arg1;
		break;
	/*SVC number to div*/
	case 39:
		res = arg0/arg1;
		break;
	default:
		printf("invalid svc code\n");
	}

	/*store the result back to stack frame at R0's position */
	pBaseOfStackFrame[0] = res;

}
