
#include "bsp.h"

static rt_thread_t led1_thread = RT_NULL;
static void led1_thread_entry(void* parameter);

int main(void)
{
	/* 创建线程 
	
	*/
	led1_thread = rt_thread_create("led",
									led1_thread_entry,
									RT_NULL,
									512,
									3,
									20);
	if(led1_thread != RT_NULL)
		rt_thread_startup(led1_thread);
	else
		return -1;
}

static void led1_thread_entry(void *parameter)
{
	while(1)
	{
		toggle_pin(LED1_GPIO,LED1_PIN);
		rt_thread_delay(1000);
	}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
