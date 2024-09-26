    AREA    |.text|, CODE, READONLY  ; ??????????? ??????? ????

    EXPORT GPB
    EXPORT led4
    GET stm32f0.inc

GPB PROC
    LDR R0, =RCC_AHB1ENR 
    LDR R1, [R0]
    LDR R2, =RCC_AHB1ENR_GPIOBEN
    ORR R1, R1, R2
    LDR R2, =RCC_AHB1ENR_GPIOAEN 
    ORR R1, R1, R2
    STR R1, [R0] 
    
    LDR R0, =GPIOB_BASE 
    LDR R1, [R0, #GPIO_MODER_OFFSET]
    LDR R2, =GPIO_MODER_MODER0_0
    ORR R2, R2, GPIO_MODER_MODER1_0
    ORR R2, R2, GPIO_MODER_MODER2_0
    ORR R2, R2, GPIO_MODER_MODER3_0
    ORR R2, R2, GPIO_MODER_MODER4_0
    ORR R2, R2, GPIO_MODER_MODER5_0
    ORR R2, R2, GPIO_MODER_MODER6_0
    ORR R2, R2, GPIO_MODER_MODER7_0
    ORR R2, R2, GPIO_MODER_MODER8_0 
    ORR R1, R1, R2
    STR R1, [R0, #GPIO_MODER_OFFSET]
    
    LDR R3, =0x155
    STR R3, [R0, #GPIO_ODR_OFFSET]
    
    BX LR
    ENDP  ; ?????????? ????????? GPB
  
led_off PROC
    LDR R0, =GPIOB_BASE 
    LDR R3, =0x0
    STR R3, [R0, #GPIO_ODR_OFFSET]
    
    B loop
    ENDP  ; ?????????? ????????? led_off
    
led_on PROC
    LDR R0, =GPIOB_BASE 
    LDR R3, =0x105
    STR R3, [R0, #GPIO_ODR_OFFSET]
    
    B loop
    ENDP  ; ?????????? ????????? led_on
    
check PROC
    LDR R0, =GPIOB_BASE 
    LDR R4, [R0, #GPIO_IDR_OFFSET]
    LDR R2, [R0, #GPIO_IDR_OFFSET]
    
    LDR R7, =0x1
    LSLS R7, R7, #12
    ANDS R4, R4, R7
    
    CMP R4, #0
    BEQ led_on
    
    ENDP  ; ?????????? ????????? check

led4 PROC
    LDR R0, =GPIOB_BASE 
    LDR R1, [R0, #GPIO_IDR_OFFSET]
    LDR R2, [R0, #GPIO_IDR_OFFSET]
    
    LDR R7, =0x1
    LSLS R7, R7, #12
    ANDS R1, R7, R1
    
    LDR R6, =0x1
    LSLS R6, R6, #13
    ANDS R2, R6, R2
    
    CMP R1, R7 
    BEQ led_off
    
    CMP R2, R6
    BEQ check
    
loop
    MOV R4, R2
    B led4
    ENDP  ; ?????????? ????????? led4

END  ; ????? ?????????
