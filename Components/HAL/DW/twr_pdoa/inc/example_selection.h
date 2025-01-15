/*! ----------------------------------------------------------------------------
 * @file    example_selection.h
 * @brief   Example selection is configured here
 *
 * @attention
 *
 * Copyright 2013 - 2020(c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */

#ifndef TEST_SELECTION_
#define TEST_SELECTION_

#ifdef __cplusplus
extern "C"
{
#endif

#include "examples_defines.h"

#define TEST_DS_TWR_STS_SDC_INITIATOR // �ܰ��ſ�Ĭ��demo���
#define TEST_DS_TWR_STS_SDC_RESPONDER // �ܰ��ſ�Ĭ��demo���

#if (EXAMPLE_DEMO)
    // Enable the needed example/test. Please enable only one example/test!

    // #define TEST_READING_DEV_ID							 //(��ȡDevID����)

    // #define TEST_SIMPLE_TX								   //(��Tx��������)
    // #define TEST_SIMPLE_RX								   //(��Rx��������)

    // #define TEST_SIMPLE_RX_PDOA							//(��PDoA Rx��������)
    // #define TEST_SIMPLE_TX_PDOA							//(��PDoA Tx��������)

    // #define TEST_TX_WAIT_RESP								//(Tx���͵ȴ�Response����)
    // #define TEST_TX_WAIT_RESP_INT							//(Tx���͵ȴ�Response����)
    // #define TEST_RX_SEND_RESP								//(Rx���ܷ���Response����)

    // #define TEST_DS_TWR_INI_STS						//(˫�߲��(DS TWR STS)����������)
    // #define TEST_DS_TWR_RESP_STS						//(˫�߲��(DS TWR STS)�ظ�������)

    // #define TEST_TX_SLEEP_TIMED							 //(���ͽ��������ڲ���������)

#endif
#ifdef __cplusplus
}
#endif

#endif
