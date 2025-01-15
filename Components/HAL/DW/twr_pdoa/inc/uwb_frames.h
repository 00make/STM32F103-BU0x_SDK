/* @file    uwb_frames.h
 * @brief
 *          UWB message frames definitions and typedefs
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef __UWB_FRAMES__H__
#define __UWB_FRAMES__H__

#ifdef __cplusplus
extern "C"
{
#endif

#pragma anon_unions

#include <stdint.h>
#include <stdbool.h>
#include "OSAL_Comdef.h"

#define MAX_ANCHOR_LIST_SIZE (8) // NODE MAX
#define MAX_TAG_LIST_SIZE (32)	 // TAG MAX

#define TS_40B_SIZE (5)
#define TS_UWB_SIZE (5)

// UWB�������ݰ�-֡����&MAC��ַ����
#define FRAME_CTRL (0x4188)
#define EUI64_ADDR_SIZE (8)
#define STANDARD_FRAME_SIZE (127)

// UWB�������ݰ�-�������±�&������
#define MSG_FN_IDX (2 + 1 + 2 + 2 + 2)
#define TWR_POLL_MSG_FN (0x1A)
#define TWR_RESP_MSG_FN (0x1B)
#define TWR_FINAL_MSG_FN (0x1C)

#pragma pack(push, 1)
	typedef struct
	{
		uint16_t frameCtrl;	 // ����֡(2)
		uint8_t seqNum;		 // ͨ�����к�(1)
		uint16_t panID;		 // ��������(2)
		uint16_t destAddr;	 // Ŀ���ַ(2)
		uint16_t sourceAddr; // Դ��ַ(2)
	} mac_header_ss_t;

	typedef struct
	{
		uint8_t fCode; // ������
		uint8_t rNum;  // ���к�
	} poll_t;

	typedef struct
	{
		uint8_t fCode;		  // ������
		int32_t slotCorr_ms;  // ��ǩУ������
		uint8_t rNum;		  // ���к�
		uint16_t a2t_usercmd; // ��վ�Զ�������
		uint16_t pre_dist_cm; // ��վ���ñ�ǩ����ֵ(��һ�غ�)
	} resp_t;

	typedef struct
	{
		uint8_t fCode;								  // ������
		uint8_t rNum;								  // ���к�
		uint64_t pollTx_ts;							  // ��ǩPoll����ʱ���
		uint64_t responseRx_ts[MAX_ANCHOR_LIST_SIZE]; // ��ǩResp����ʱ���
		uint64_t finalTx_ts;						  // ��ǩFinal����ʱ���
		uint8_t rxResponseMask;						  // ��ǩ����Resp��Чλ

		uint16_t dist_cm[MAX_ANCHOR_LIST_SIZE]; // ��ǩ����վ�ľ���ֵ
	} final_t;

	typedef struct
	{
		mac_header_ss_t mac;
		poll_t poll;
		uint8_t fcs[2];
	} poll_msg_t; // poll���ݰ�

	typedef struct
	{
		mac_header_ss_t mac;
		resp_t resp;
		uint8_t fcs[2];
	} resp_msg_t; // resp���ݰ�

	typedef struct
	{
		mac_header_ss_t mac;
		final_t final;
		uint8_t fcs[2];
	} final_msg_t; // final���ݰ�
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* __UWB_FRAMES__H__ */
