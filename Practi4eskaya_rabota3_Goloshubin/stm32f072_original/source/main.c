#include "main.h"

static _Bool statePB0 = 0; // Текущее состояние на выводе PB.0: 0 – светодиод погашен; 1 - светодиод горит
static _Bool statePB1 = 0;
static uint32_t half_period_1 = 167; // Полупериод мигания светодиода 1 (3 Гц)
static uint32_t half_period_2 = 63; // Полупериод мигания светодиода 2 (7.9 Гц)

void delay(void) {
    volatile uint16_t i; // volatile - указывает компилятору не оптимизировать переменную
    uint16_t d;
    
    d = (GPIOB->IDR & 0xf000) + 1; // задержка определяется микропереключателями, минимальная 1
    for (i = d; i > 0; i--);
}

void init_gpio(void) {
    // Включить тактирование портов А, B
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
    
    // инициализация светодиодов (PB.0-PB.7), PB.8 - линия управления светодиодами (все на выход)
    GPIOB->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | 
                    GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | 
                    GPIO_MODER_MODER8_0;
    
    // Гарантированно очищаем PB.12-PB.15 (настройка на вход)
    GPIOB->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
}

void InitTimer6(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; // Включение тактирования модуля TIM6
    TIM6->PSC = 7999; // Установка коэффициента предделителя равного 8000
    TIM6->ARR = half_period_1;
    TIM6->DIER |= 0x1;
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC->ISER[0] |= 0x20000;
}

void InitTimer3(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 7999; // Установка коэффициента предделителя равного 8000
    TIM3->ARR = half_period_2;
    TIM3->DIER |= 0x1;
    NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM6_DAC_IRQHandler(void) {
    statePB0 = !statePB0;
    if (statePB0) {
        GPIOB->BSRR = 1 << 5;
    } else {
        GPIOB->BSRR = 1 << (5 + 16);
    }
    TIM6->SR = 0;
}

void TIM3_IRQHandler(void) {
    statePB1 = !statePB1;
    if (statePB1) {
        GPIOB->BSRR = 1 << 7;
    } else {
        GPIOB->BSRR = 1 << (7 + 16);
    }
    TIM3->SR = 0;
}

int main(void) {
    __disable_irq(); // Глобальное запрещение прерываний
    init_gpio();
    InitTimer6(); 
    InitTimer3(); // Разрешение в NVIC прерывания от модуля TIM6 (17 линия)
    __enable_irq(); // Глобальное разрешение прерываний

    TIM6->CR1 |= 0x1; // Разрешение работы таймера
    TIM3->CR1 |= 0x1;

    while (1) {};
}