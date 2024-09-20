/**********************************************************************************
 * @file    user_out.h
 * @author Ai-Thinker
 * @brief  header file for user_out.c
 * @date    2024.9.18
**********************************************************************************/
#ifndef __USER_OUT_H_
#define __USER_OUT_H_

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <node.h>
#include "shared_functions.h"

extern uint16_t  user_tag_dist_cm[MAX_ANCHOR_LIST_SIZE];


/**
 * ���ð��ſ�Ĭ������Ŀ���״̬��
 *
 * @param output ���Ϊtrue��������������Ϊfalse����ر������
 */
void set_aiio_output_state (bool output);

/**
 * ��ȡ���ſ�Ĭ������Ŀ���״̬��
 *
 * @return �������ѿ������򷵻�true�����򷵻�false
 */
bool get_aiio_output_state (void);

/**
 * ��� twr�㷨����������û�����������
 *
 */
void node_twr_output_user (result_t* pRes);

/**
 * ��� twr�㷨(��ǩ)����������û�����������
 *
 */
void tag_twr_output_user (uint16_t* dis);

/**
 *��� pdoa�㷨����վ������������û�����������
 *
 */
void node_pdoa_output_user (result_pdoa_t* Res);

/**
 *��� pdoa�㷨����ǩ������������û�����������
 *
 */
void tag_pdoa_output_user (result_pdoa_t* Res);

#endif
