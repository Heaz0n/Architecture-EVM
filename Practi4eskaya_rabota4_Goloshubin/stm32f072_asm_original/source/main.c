//
// Пример проекта для платы 
// Judchin
// 

#include "main.h"
#include <stm32f0xx.h>


/*******************************************************************/


int main(void)
{	
	GPB();
	led4();
	while (1);
}
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %drn", file, line) */
 
  /* Infinite loop */
  while (1);
}
#endif
