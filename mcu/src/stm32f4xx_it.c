#include "stm32f4xx_it.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "chip_init.h"
#include "task_adc.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress) {
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  /* Link register. */
    volatile uint32_t pc;  /* Program counter. */
    volatile uint32_t psr; /* Program status register. */

    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];

    (void)r0;
    (void)r1;
    (void)r2;
    (void)r3;

    (void)r12;
    (void)lr;
    (void)pc;
    (void)psr;
    /* When the following line is hit, the variables contain the register values. */
    for (;;)
        ;
}

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void) {
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
    __asm volatile(" tst lr, #4                                                \n"
                   " ite eq                                                    \n"
                   " mrseq r0, msp                                             \n"
                   " mrsne r0, psp                                             \n"
                   " ldr r1, [r0, #24]                                         \n"
                   " ldr r2, handler2_address_const                            \n"
                   " bx r2                                                     \n"
                   " handler2_address_const: .word prvGetRegistersFromStack    \n");
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles main I2S interrupt.
  * @param  None
  * @retval 0 if correct communication, else wrong communication
  */
void DMA1_Stream7_IRQHandler(void) {
}

/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Stream3_IRQHandler(void) {
}

void DMA1_Stream5_IRQHandler(void) {
    CLEAR_DISCO_LED_GREEN();
    HAL_DMA_IRQHandler(hi2s3.hdmatx);
    SET_DISCO_LED_GREEN();
} /* DMA1 Stream 5                */
  /**
    * @brief  This function handles DMA Stream interrupt request.
    * @param  None
    * @retval None
    */
void DMA1_Stream2_IRQHandler(void) {
    CLEAR_DISCO_LED_RED();
    HAL_DMA_IRQHandler(hi2s3.hdmarx);
    SET_DISCO_LED_RED();
}
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//__weak
// void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void) {
}

void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
}

void WWDG_IRQHandler(void) {
} /* Window WatchDog              */
void PVD_IRQHandler(void) {
} /* PVD through EXTI Line detection */
void TAMP_STAMP_IRQHandler(void) {
} /* Tamper and TimeStamps through the EXTI line */
void RTC_WKUP_IRQHandler(void) {
} /* RTC Wakeup through the EXTI line */
void FLASH_IRQHandler(void) {
} /* FLASH                        */
void RCC_IRQHandler(void) {
} /* RCC                          */
void EXTI0_IRQHandler(void) {
} /* EXTI Line0                   */
void EXTI1_IRQHandler(void) {
} /* EXTI Line1                   */
void EXTI2_IRQHandler(void) {
} /* EXTI Line2                   */
void EXTI3_IRQHandler(void) {
} /* EXTI Line3                   */
void EXTI4_IRQHandler(void) {
} /* EXTI Line4                   */
void DMA1_Stream0_IRQHandler(void) {
} /* DMA1 Stream 0                */
void DMA1_Stream1_IRQHandler(void) {
} /* DMA1 Stream 1                */
// void DMA1_Stream2_IRQHandler(void) {
//} /* DMA1 Stream 2                */
// void DMA1_Stream3_IRQHandler(void) {
//} /* DMA1 Stream 3                */
void DMA1_Stream4_IRQHandler(void) {
} /* DMA1 Stream 4                */

void DMA1_Stream6_IRQHandler(void) {
} /* DMA1 Stream 6                */
void ADC_IRQHandler(void) {
    ADC1_IRQHandler();
} /* ADC1, ADC2 and ADC3s         */
void CAN1_TX_IRQHandler(void) {
} /* CAN1 TX                      */
void CAN1_RX0_IRQHandler(void) {
} /* CAN1 RX0                     */
void CAN1_RX1_IRQHandler(void) {
} /* CAN1 RX1                     */
void CAN1_SCE_IRQHandler(void) {
} /* CAN1 SCE                     */
void EXTI9_5_IRQHandler(void) {
} /* External Line[9:5]s          */
void TIM1_BRK_TIM9_IRQHandler(void) {
} /* TIM1 Break and TIM9          */
void TIM1_UP_TIM10_IRQHandler(void) {
} /* TIM1 Update and TIM10        */
void TIM1_TRG_COM_TIM11_IRQHandler(void) {
} /* TIM1 Trigger and Commutation and TIM11 */
void TIM1_CC_IRQHandler(void) {
} /* TIM1 Capture Compare         */
void TIM3_IRQHandler(void) {
} /* TIM3                         */
void TIM4_IRQHandler(void) {
} /* TIM4                         */
void I2C1_EV_IRQHandler(void) {
} /* I2C1 Event                   */
void I2C1_ER_IRQHandler(void) {
} /* I2C1 Error                   */
void I2C2_EV_IRQHandler(void) {
} /* I2C2 Event                   */
void I2C2_ER_IRQHandler(void) {
} /* I2C2 Error                   */
void SPI1_IRQHandler(void) {
} /* SPI1                         */
void SPI2_IRQHandler(void) {
} /* SPI2                         */
void USART1_IRQHandler(void) {
} /* USART1                       */
// void USART2_IRQHandler            (void){}     /* USART2                       */
// void USART3_IRQHandler            (void){}     /* USART3                       */
void EXTI15_10_IRQHandler(void) {
} /* External Line[15:10]s        */
void RTC_Alarm_IRQHandler(void) {
} /* RTC Alarm (A and B) through EXTI Line */
void OTG_FS_WKUP_IRQHandler(void) {
} /* USB OTG FS Wakeup through EXTI line */
void TIM8_BRK_TIM12_IRQHandler(void) {
} /* TIM8 Break and TIM12         */
void TIM8_UP_TIM13_IRQHandler(void) {
} /* TIM8 Update and TIM13        */
void TIM8_TRG_COM_TIM14_IRQHandler(void) {
} /* TIM8 Trigger and Commutation and TIM14 */
void TIM8_CC_IRQHandler(void) {
} /* TIM8 Capture Compare         */
// void DMA1_Stream7_IRQHandler(void) {
//} /* DMA1 Stream7                 */
void FSMC_IRQHandler(void) {
} /* FSMC                         */
void SDIO_IRQHandler(void) {
} /* SDIO                         */
void TIM5_IRQHandler(void) {
} /* TIM5                         */
void SPI3_IRQHandler(void) {
    /* USER CODE BEGIN SPI3_IRQn 0 */

    /* USER CODE END SPI3_IRQn 0 */
    HAL_I2S_IRQHandler(&hi2s3);
    /* USER CODE BEGIN SPI3_IRQn 1 */

} /* SPI3                         */
void UART4_IRQHandler(void) {
} /* UART4                        */
void UART5_IRQHandler(void) {
} /* UART5                        */
void TIM6_DAC_IRQHandler(void) {
} /* TIM6 and DAC1&2 underrun errors */
void TIM7_IRQHandler(void) {
} /* TIM7                         */
void DMA2_Stream0_IRQHandler(void) {
} /* DMA2 Stream 0                */
void DMA2_Stream1_IRQHandler(void) {
} /* DMA2 Stream 1                */
void DMA2_Stream2_IRQHandler(void) {
} /* DMA2 Stream 2                */
void DMA2_Stream3_IRQHandler(void) {
} /* DMA2 Stream 3                */
void DMA2_Stream4_IRQHandler(void) {
} /* DMA2 Stream 4                */
void ETH_IRQHandler(void) {
} /* Ethernet                     */
void ETH_WKUP_IRQHandler(void) {
} /* Ethernet Wakeup through EXTI line */
void CAN2_TX_IRQHandler(void) {
} /* CAN2 TX                      */
void CAN2_RX0_IRQHandler(void) {
} /* CAN2 RX0                     */
void CAN2_RX1_IRQHandler(void) {
} /* CAN2 RX1                     */
void CAN2_SCE_IRQHandler(void) {
} /* CAN2 SCE                     */
void OTG_FS_IRQHandler(void) {
} /* USB OTG FS                   */
void DMA2_Stream5_IRQHandler(void) {
} /* DMA2 Stream 5                */
void DMA2_Stream6_IRQHandler(void) {
} /* DMA2 Stream 6                */
void DMA2_Stream7_IRQHandler(void) {
} /* DMA2 Stream 7                */
void USART6_IRQHandler(void) {
} /* USART6                       */
void I2C3_EV_IRQHandler(void) {
} /* I2C3 event                   */
void I2C3_ER_IRQHandler(void) {
} /* I2C3 error                   */
void OTG_HS_EP1_OUT_IRQHandler(void) {
} /* USB OTG HS End Point 1 Out   */
void OTG_HS_EP1_IN_IRQHandler(void) {
} /* USB OTG HS End Point 1 In    */
void OTG_HS_WKUP_IRQHandler(void) {
} /* USB OTG HS Wakeup through EXTI */
void OTG_HS_IRQHandler(void) {
} /* USB OTG HS                   */
void DCMI_IRQHandler(void) {
} /* DCMI                         */

void HASH_RNG_IRQHandler(void) {
} /* Hash and Rng                 */
void FPU_IRQHandler(void) {
} /* FPU                          */
