#include "main.h"
#include "stm32f0xx.h"
#include <stdio.h>

void InitUSART1(uint32_t baudrate, uint16_t parity, uint16_t stopbits);
void USART_SendString(const char *str);
void UpdateLEDState(uint8_t led_index, uint8_t state);
void SendAllLEDStatus(void);

int main(void) {
    char rx_data;
    char message[50];
	  
    // Настройка USART на 57600 бит/с, четный паритет, 2 стоп-бита
    InitUSART1(57600, USART_CR1_PCE, USART_CR2_STOP_1 | USART_CR2_STOP_0);
	  
	  // Настройка USART на 19200 бит/с, нечетный паритет, 1 стоп-бит
	  //InitUSART1(SystemCoreClock / 19200, USART_CR1_PCE | USART_CR1_PS, 0);

    // Включаем тактирование порта A
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Настраиваем PA0-PA7 на вывод
    GPIOA->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 |
                    GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 |
                    GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0;

    while (1) {
        // Ожидание приема символа
        while (!(USART1->ISR & USART_ISR_RXNE)) { }
        // Чтение символа из регистра данных USART
        rx_data = USART1->RDR;

        if (rx_data >= '1' && rx_data <= '8') {
            uint8_t led_index = rx_data - '1'; // Индекс светодиода
            uint8_t current_state = (GPIOA->ODR & (1 << led_index)) ? 1 : 0; // Текущее состояние светодиода
            UpdateLEDState(led_index, !current_state); // Инвертируем состояние светодиода

            // Формирование сообщения о состоянии светодиода
            sprintf(message, "LED %c: %s\r\n", rx_data, (current_state ? "OFF" : "ON"));
            USART_SendString(message); // Отправка сообщения в PuTTY
        } else if (rx_data == '0') {
            GPIOA->ODR = 0x00; // Выключение всех светодиодов
            USART_SendString("All LEDs turned OFF\r\n"); // Отправка сообщения в PuTTY
        } else if (rx_data == '9') {
            SendAllLEDStatus(); // Отправка состояния всех светодиодов
        }
    }
}

void InitUSART1(uint32_t baudrate, uint16_t parity, uint16_t stopbits) {
    // Включаем тактирование USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // Включаем тактирование порта A
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // AF режим для PA9 (TX), PA10 (RX)
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
    // AF1 для PA9 и PA10
    GPIOA->AFR[1] |= 0x00000110;

    // Настройка скорости передачи
    USART1->BRR = SystemCoreClock / baudrate;

    // Включаем USART, передачу и прием
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    // Настройка паритета
    USART1->CR1 |= parity;
    // Очищаем биты количества стоп-бит
    USART1->CR2 &= ~USART_CR2_STOP;
    // Настройка количества стоп-бит
    USART1->CR2 |= stopbits;
}

void USART_SendString(const char *str) {
    while (*str) {
        // Ожидание готовности передатчика
        while (!(USART1->ISR & USART_ISR_TXE)) { }
        // Передача символа
        USART1->TDR = *str++;
    }
}

void UpdateLEDState(uint8_t led_index, uint8_t state) {
    if (state) {
        // Включение светодиода
        GPIOA->BSRR = (1 << led_index);
    } else {
        // Выключение светодиода
        GPIOA->BRR = (1 << led_index);
    }
}

void SendAllLEDStatus(void) {
    char message[100];
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        sprintf(message + i * 10, "LED %d: %s\r\n", i + 1, (GPIOA->ODR & (1 << i)) ? "ON" : "OFF");
    }
    // Отправка сообщения о состоянии всех светодиодов в PuTTY
    USART_SendString(message);
}