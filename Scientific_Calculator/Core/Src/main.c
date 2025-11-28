/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "OLED.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
char buttonCheck(void);
void Calculator_Init(void);
void Calculator_Input(char key);
double Calculate_Result(char* formula);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define MAX_STACK 20         
double num_stack[MAX_STACK]; 
char op_stack[MAX_STACK];  
int num_top = -1;         
int op_top = -1;
void push_num(double val){ 
    if(num_top < MAX_STACK-1) num_stack[++num_top] = val; 
}
double pop_num(){ 
    return (num_top >= 0) ? num_stack[num_top--] : 0; 
}
double peek_num(){ 
    return (num_top >= 0) ? num_stack[num_top] : 0; 
}
void push_op(char op){ 
    if(op_top < MAX_STACK-1) op_stack[++op_top] = op; 
}
char pop_op(){ 
    return (op_top >= 0) ? op_stack[op_top--] : 0; 
}
char peek_op(){ 
    return (op_top >= 0) ? op_stack[op_top] : 0; 
}
int get_priority(char op){
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}
void do_calculation(){
    double b = pop_num();
    double a = pop_num();
    char op = pop_op();
    double res = 0;
    switch(op) {
        case '+': res = a + b; break;
        case '-': res = a - b; break;
        case '*': res = a * b; break;
        case '/': res = (b != 0) ? (a / b) : 0; break;
    }
    push_num(res);
}
double Calculate_Result(char* formula){
    num_top = -1;
    op_top = -1;
    int i = 0;
    while(formula[i] != '\0'){
        if((formula[i] >= '0' && formula[i] <= '9') || formula[i] == '.'){
            char* end_ptr;
            double val = strtod(&formula[i], &end_ptr);
            push_num(val);
            i = end_ptr - formula; 
            continue;
        }else if(formula[i] == '+' || formula[i] == '-' || formula[i] == '*' || formula[i] == '/'){
            while(op_top >= 0 && get_priority(peek_op()) >= get_priority(formula[i])){
                do_calculation();
            }
            push_op(formula[i]);
        }
        i++;
    }
    while(op_top >= 0){
        do_calculation();
    }
    return num_stack[0];
}
char KEY_MAP[4][4] = {
    {'7', '8', '9', '/'},
    {'4', '5', '6', '*'},
    {'1', '2', '3', '-'},
    {'C', '0', '=', '+'}
    };
char formula_buf[64];
uint8_t buf_index = 0;
uint32_t last_scan_time = 0;
char key = ' ';


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    /* USER CODE BEGIN 2 */
    
    OLED_Init();
    OLED_Clear();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1){
        if(HAL_GetTick() - last_scan_time > 20){
            last_scan_time = HAL_GetTick();
            key = buttonCheck(); 
            if(key != ' '){
                if(key == 'C'){
                    Calculator_Init();
                }else if(key == '='){
                    double result = Calculate_Result(formula_buf);
                    char result_str[20];
                    sprintf(result_str, "=%g", result); 
                    OLED_ShowString(2, 1, result_str); 
                }else{
                    Calculator_Input(key);
                }
            }
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
/*char buttonCheck(void){
    GPIOA -> ODR |= 0x000F;
    for(uint8_t r = 0;r < 4;r++){
        GPIOA -> ODR &= ~(1 << r);
        for(uint8_t l = 0;l<4;l++){
            if((GPIOA -> IDR & (1 <<(l+4))) == 0){
                if(key == ' '){
                    HAL_Delay(10);
                    if((GPIOA -> IDR & (1 <<(l+4))) == 0){
                        GPIOA -> ODR |= (1 << r);
                        return KEY_MAP[r][l];
                    }
                }else{
                    return key;
                }
            }

        }
        GPIOA -> ODR |= (1 << r);
    }
    if(key_lock == 1){
        HAL_Delay(10);
    }
    GPIOA -> ODR |= 0x000F;
    return ' ';
}*/
char buttonCheck(void){
    static char last_key = ' ';
    char current_key = ' ';
    GPIOA->ODR |= 0x000F;
    for(uint8_t r = 0; r < 4; r++){
        GPIOA->ODR &= ~(1 << r);
        for(uint8_t l = 0; l < 4; l++){
            if((GPIOA->IDR & (1 << (l+4))) == 0){
                HAL_Delay(5);
                if((GPIOA->IDR & (1 << (l+4))) == 0){
                    current_key = KEY_MAP[r][l];
                    if(current_key != last_key) {
                        last_key = current_key;
                        return current_key;
                    } else {
                        return ' ';
                    }
                }
            }
        }
        GPIOA->ODR |= (1 << r);
    }
    last_key = ' ';
    return ' ';
}
void Calculator_Init(void){
    buf_index = 0;
    for(uint8_t i = 0;i < 64;i++){
        formula_buf[i] = 0;
    }
    OLED_Clear();
    OLED_ShowString(1,1,"Ready");
}
void Calculator_Input(char key){
    if (buf_index < 60) {
        formula_buf[buf_index++] = key;
        formula_buf[buf_index] = '\0';
        OLED_Clear();
        OLED_ShowString(1, 1, formula_buf);
    }
}


/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
