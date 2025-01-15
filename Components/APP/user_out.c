/**********************************************************************************
 * @file    user_out.c
 * @author Ai-Thinker
 * @version V1.0.0
 * @brief  distance and range output, Users can handle it themselves
 * @date    2024.9.18
 **********************************************************************************/
#include "user_out.h"
#include "hal_usart.h"

bool aiio_output = true;
uint16_t user_tag_dist_cm[MAX_ANCHOR_LIST_SIZE] = {0};

/**
 * ���ð��ſ�Ĭ������Ŀ���״̬��
 *
 * @param output ���Ϊtrue��������������Ϊfalse����ر������
 */
void set_aiio_output_state(bool output)
{
    aiio_output = output;
}

/**
 * ��ȡ���ſ�Ĭ������Ŀ���״̬��
 *
 * @return �������ѿ������򷵻�true�����򷵻�false
 */
bool get_aiio_output_state(void)
{
    return aiio_output;
}

/**
 * ��� twr�㷨(��վ)����������û�����������
 *
 */
void node_twr_output_user(result_t *pRes)
{
    if (pRes == NULL)
    {
        _dbg_printf("Invalid result pointer\n");
        return;
    }

    for (int i = 0; i < MAX_ANCHOR_LIST_SIZE; i++)
    {
        _dbg_printf("dist_cm[%zu] = %3.3f m\n", i, pRes->dist_cm[i] / 100.0);
    }
}

/**
 * ��� twr�㷨(��ǩ)����������û�����������
 *
 */
void tag_twr_output_user(uint16_t *dis)
{
    if (dis == NULL)
    {
        _dbg_printf("Invalid result pointer\n");
        return;
    }

    for (int i = 0; i < MAX_ANCHOR_LIST_SIZE; i++)
    {
        _dbg_printf("dist_cm[%zu] = %3.3f m\n", i, dis[i] / 100.0);
    }
}

/**
 *��� pdoa�㷨����վ������������û�����������
 *
 */
void node_pdoa_output_user(result_pdoa_t *Res)
{
    if (Res == NULL)
    {
        _dbg_printf("Invalid result pointer\n");
        return;
    }

    _dbg_printf("dist_cm = %3.3f m\n", Res->dist_cm / 100.0);
    _dbg_printf("angle = %3.1f \n", Res->angle);
}

/**
 *��� pdoa�㷨����ǩ������������û�����������
 *
 */
void tag_pdoa_output_user(result_pdoa_t *Res)
{
    if (Res == NULL)
    {
        _dbg_printf("Invalid result pointer\n");
        return;
    }

    _dbg_printf("dist_cm = %3.3f m\n", Res->dist_cm / 100.0);
    _dbg_printf("angle = %3.1f \n", Res->angle);
}
