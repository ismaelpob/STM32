/**
  ******************************************************************************
  * @file    main.c
  * @author  Auto-generated by STM32CubeIDE
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/

int main(void)
{


//Parte 1. Uso de ensamblador y inline
//Carga dos valores de la memoria y sumalos y alamacenalos en la memoria usando inline assembly

#if 0

	//Inicializo direccion en R1
	__asm volatile("LDR R1,=#0x20001000");
	//Inicializo direccion en R2
	__asm volatile("LDR R2,=#0x20001004");

	//Colocar breakpoint y de manera manual diriguirse a
	//la direccion R1 0x20001000 y R2, dar un valor para realizar la suma.

	//Almaceno valor de R1 en R0
	__asm volatile("LDR R0,[R1]");
	//Almaceno valor de R2 en R1
	__asm volatile("LDR R1,[R2]");
	//Sumo R0 y R1 y lo guardo en R0
	__asm volatile("ADD R0,R0,R1"); //ADD 1,2,3
	//Almaceno valor de la memoria utilizada R0 a R2
	__asm volatile("STR R0,[R2]");
#endif

//Parte 2. Uso de ensamblador con expresiones de C, Uso de constraint.
//Lectura de registro especial CONTROL, escritura en de variable c en R0, lectura de puntero.

	// Almaceno variable val en registro R0
	int val=50;
	__asm volatile("MOV R0,%0": :"r"(val));

	//Leo el valor del registro "CONTROL" del procesador y lo almaceno en control_reg.
	int control_reg;
	__asm volatile("MRS %0,CONTROL": "=r"(control_reg) );


    //Leo el valor presente en el puntero p2
	int p1, *p2;
	p2 = (int*)0x20000008;
	__asm volatile("LDR %0,[%1]": "=r"(p1): "r"(p2));

	for(;;);
}
