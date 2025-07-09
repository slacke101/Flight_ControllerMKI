#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

// I2C and UART Handles
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

// Sensor I2C addresses (7-bit shifted left 1 for HAL)
#define BMI270_ADDR       (0x68 << 1)
#define QMC5883L_ADDR     (0x1A << 1)
#define MPL3115A2_ADDR    (0x60 << 1)

// Forward declarations
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void MX_USART2_UART_Init(void);
int _write(int file, char *ptr, int len); // retarget printf

// Read 1 byte from I2C device register
uint8_t I2C_ReadRegister(uint16_t DevAddress, uint8_t Reg)
{
    uint8_t val = 0;
    if (HAL_I2C_Mem_Read(&hi2c1, DevAddress, Reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100) != HAL_OK)
    {
        printf("Error reading register 0x%02X from device 0x%02X\r\n", Reg, DevAddress >> 1);
    }
    return val;
}

// Read multiple bytes from I2C device registers
HAL_StatusTypeDef I2C_ReadRegisters(uint16_t DevAddress, uint8_t Reg, uint8_t *buffer, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, DevAddress, Reg, I2C_MEMADD_SIZE_8BIT, buffer, len, 100);
}

void ReadAndPrintSensorIDs(void)
{
    uint8_t bmi270_id = I2C_ReadRegister(BMI270_ADDR, 0x00); // BMI270 Chip ID, expect 0x24
    uint8_t qmc5883l_id[3] = {0};
    uint8_t mpl3115a2_id = 0;

    // QMC5883L WHO_AM_I registers are 0x0D, 0x0E, 0x0F with 'Q','M','C'
    if (I2C_ReadRegisters(QMC5883L_ADDR, 0x0D, qmc5883l_id, 3) != HAL_OK)
    {
        printf("Error reading QMC5883L ID\r\n");
    }

    mpl3115a2_id = I2C_ReadRegister(MPL3115A2_ADDR, 0x0C); // MPL3115A2 WHO_AM_I, expect 0xC4

    // Print results
    printf("BMI270 ID: 0x%02X (expect 0x24)\r\n", bmi270_id);
    printf("QMC5883L ID: %c%c%c (expect QMC)\r\n", qmc5883l_id[0], qmc5883l_id[1], qmc5883l_id[2]);
    printf("MPL3115A2 ID: 0x%02X (expect 0xC4)\r\n", mpl3115a2_id);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    printf("STM32F405 Sensor ID Reader Started\r\n");

    while (1)
    {
        ReadAndPrintSensorIDs();
        HAL_Delay(2000);
    }
}

/* ----------------- Initialization code ----------------- */

void SystemClock_Config(void)
{
    // Use CubeMX generated code or your clock config here
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Enable HSE Oscillator and activate PLL with HSE as source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 336;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ       = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { while(1); }

    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | 
                                       RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // 42 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // 84 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { while(1); }
}

void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PB6, PB7 as I2C1 SCL and SDA (Alternate Function Open-Drain)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure PA2 (USART2 TX), PA3 (USART2 RX)
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MX_I2C1_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000; // 100kHz standard mode
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        // Initialization Error
        while(1);
    }
}

void MX_USART2_UART_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        // Initialization Error
        while(1);
    }
}

// Retarget printf to UART2
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}
