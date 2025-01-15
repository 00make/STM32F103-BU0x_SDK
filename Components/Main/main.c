/**********************************************************************************
 * @file    main.c
 * @author Ai-Thinker
 * @version V1.0.0
 * @date    2024.5.16
 **********************************************************************************/

#include "stm32f10x.h"
#include "Generic.h"
#include "node.h"
#include "tag.h"
#include "OSAL.h"
#include "hal_drivers.h"

#include "config.h"
#include "hal_iic.h"
#include "cmd_fn.h"

#include "shared_functions.h"
#include "user_out.h"
#include "examples_defines.h"

/*******************************************************************************
*******************************************************************************/

void RCC_Configuration_part(void)
{
    ErrorStatus HSEStartUpStatus;
    RCC_ClocksTypeDef RCC_ClockFreq;

    /* ��RCC�Ĵ�����������ΪĬ��ֵ */
    RCC_DeInit();

    /* ���ⲿ����ʱ�Ӿ���HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* �ȴ��ⲿ����ʱ�Ӿ����� */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus != ERROR)
    {
        /* ����FlashԤ�����幦��,ʱ�������ʹ�� */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* 48~72Mhz�Ƽ�LatencyΪ2 */
        FLASH_SetLatency(FLASH_Latency_2);

        /* ����AHBʱ�ӣ�72MHz HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        /* ���ø���APB2ʱ�ӣ�1��Ƶ72MHz PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);
        /* ���õ���APB1ʱ�ӣ�2��Ƶ36MHz PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);
        /*  ����ADCʱ�� ADCCLK = PCLK2/4 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);

        // ����PLLʱ��Դ����Ƶϵ�� ����Ƶ��RCC_PLLSource_HSE_Div1 9��Ƶ��RCC_PLLMul_9
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        /* ��PLL */
        RCC_PLLCmd(ENABLE);
        /* �ȴ�PLL�ȶ����� */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* ѡ��PLLʱ����Ϊʱ��Դ */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* �ȴ�ʱ��Դ�л��������ȶ�״̬ */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }

    RCC_GetClocksFreq(&RCC_ClockFreq);
}

/*******************************************************************************
 * ������  : init
 * ����    : ��ʼ������
 * ����    : ��
 * ���    : ��
 * ����ֵ  : ��
 *******************************************************************************/
void init(void)
{
    SystemInit();

    RCC_Configuration_part();

    Hal_Driver_Init();
    
    // ��ʼ����ʱ��
    HalTimerInit();
    
    App_Module_Init();
    
    // ������ʱ��
    HalTimerStart();
}

// ��Ӷ�ʱ���жϴ�����
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        
        // ÿ�����һ���������
        result_pdoa_t result;
        // TODO: ��ȡʵ�ʵĲ������
        node_pdoa_output_user(&result);
    }
}

/*******************************************************************************
 * ������  : nt_task
 * ����    : main task
 * ����    : ��
 * ���    : ��
 * ����ֵ  : ��
 *******************************************************************************/
void nt_task(void)
{
    // ��ȡflash
    load_bssConfig();

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        OLED_ShowStr(19, 2, "Please Send", 2);
        OLED_ShowStr(19, 4, "AT Command", 2);

        while (app.pConfig->s.userConfig.nodeAddr == 0xFFFF) // AOA�Ƿ�������
        {
            App_Module_Sys_Work_Mode_Event();
        }
        if (app.pConfig->s.userConfig.twr_pdoa_mode == 0)
        {
            if (sys_para.flag == 0xAAAA)
            {
                if (sys_para.param_Config.s.userConfig.role == 1) // node
                {
                    node_start();
                }
                else if (sys_para.param_Config.s.userConfig.role == 0) // tag
                {
                    tag_start();
                }
            }
        }
        else
        {
            if (sys_para.param_Config.s.userConfig.role == 1)
            {
                ds_twr_sts_sdc_responder();
            }
            else if (sys_para.param_Config.s.userConfig.role == 0)
            {
                ds_twr_sts_sdc_initiator();
            }
        }
    }
    else
    {
        while (1)
        {
            App_Module_Sys_Work_Mode_Event();
        }
    }

    for (;;)
        ;
}

/*******************************************************************************
 * ������  : main
 * ����    : ������
 * ����    : ��
 * ���    : ��
 * ����ֵ  : ��
 ********************************************************************************/
int main(void)
{
    init();
#if (EXAMPLE_DEMO)
    build_examples();
#else
    nt_task();
#endif
    for (;;)
    {
    }
}
