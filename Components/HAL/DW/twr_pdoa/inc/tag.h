/*! ---------------------------------------------------------------------------
 * @file    node.h
 * @brief   DecaWave
 *             bare implementation layer
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef __NODE__H__
#define __NODE__H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "uwb_frames.h"
#include "msg_time.h"

#include "default_config.h"
#include "error.h"

#define DEEP_SLEEP (0) // ���߱�־λ

#define EVENT_BUF_SIZE (0x02) // �¼�buf��С
#define FILTER_SIZE (10)	  // �˶��˲�ֵ

#define MASK_40BIT (0x00FFFFFFFFFFULL) // DW1000 counter is 40 bits
#define MASK_TXDTS (0x00FFFFFFFE00ULL) // The TX timestamp will snap to 8 ns resolution - mask lower 9 bits.

#define GATEWAY_ADDR (0x0000) // ���ص�ַ(ʱ�ֶ�ַ����)

#define TAG_BLINK_TX_RC_RX_US (500) // ��ǩ����Blink->����RngCfg�ӳ�ʱ��
#define RC_RX_TIMEOUT_US (300)		// ��ǩ����rngcfg���ܳ�ʱ
#define RX_RELAX_TIMEOUT_SY (20)	// ���ܳ�ʱʱ������
#define RX_FIXED_TIMEOUT_US (300)	// �̶����ܳ�ʱʱ��

#define SPEED_OF_LIGHT (299702547.0) // in m/s in the air

	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------

	/* �����ϱ��ṹ�� */
	typedef struct
	{
		uint16_t addr16;	 // ��ǩ��ַ
		uint8_t rangeNum;	 // ��ǩ������к� number from Tag Poll and Final messages, which indicates the current range number
		uint32_t resTime_us; // ��ǩ���ʱ�� reception time of the end of the Final from the Tag wrt node's SuperFrame start, microseconds
		uint16_t mask;
		uint16_t dist_cm[MAX_ANCHOR_LIST_SIZE]; // ��ǩ�ϱ����

		float pre_dist_m;  // �����
		float pre_fdist_m; // �����(�˲�)
	} result_t;

	/* ���ͽṹ�� */
	typedef struct
	{
		struct
		{
			int16_t txDataLen;				  // ���ͳ���
			uint8_t arr[STANDARD_FRAME_SIZE]; // �������ݰ�
		};

		enum
		{
			Twr_Tx_Poll_Sent,  // ��ǩ����Poll״̬
			Twr_Tx_Resp_Sent,  // �ڵ㷢��Resp״̬
			Twr_Tx_Final_Sent, // ��ǩ����Final״̬
		} txState;
		uint8_t is_send; // ���ͳɹ��ж�λ
		uint8_t seqNum;	 // ��������seq
		uint8_t txFlag;	 // ���ͱ�־λ DWT_START_TX_IMMEDIATE DWT_START_TX_DELAYED & DWT_RESPONSE_EXPECTED

		uint8_t t_timeStamp[TS_40B_SIZE]; // ����ʱ���

		uint32_t delayedTxTimeH_sy;	  // �����ӳ�(��32λ) Delayed transmit time (high32)
		uint32_t delayedRxTime_sy;	  // �����ӳ�
		uint16_t delayedRxTimeout_sy; // ���ܳ�ʱ
	} tx_pckt_t;

	/* ���ܽṹ�� */
	typedef struct
	{
		struct
		{
			int16_t rxDataLen;				  // ���ܳ���
			uint8_t arr[STANDARD_FRAME_SIZE]; // �������ݰ�
		};

		uint8_t r_timeStamp[TS_40B_SIZE]; // ����ʱ���

		uint32_t TickTimeStamp; // �δ�ʱ��
	} rx_pckt_t;

	/* TWR�ṹ�� */
	typedef struct
	{
		int testAppState; // ״̬
		int done;		  // �¼�

		// ����UWB����
		struct
		{
			rx_pckt_t buf[EVENT_BUF_SIZE];
			int head;
			int tail;
		} rxPcktBuf;

		// ����UWB����
		struct
		{
			tx_pckt_t buf;
		} txPcktBuf;

		// twr��ǩ����
		struct
		{
			struct
			{
				uint16_t tagAddr;	 // ��ǩ�̵�ַ
				uint16_t panID;		 // ��ǩpanID
				sfConfig_t sfConfig; // ����֡
			} general;

			struct
			{
				uint8_t twr_sn; // ������к�
				int8_t remainingRespToRx;
				int8_t rxResponseMask;
				int slotCorr_ms;	  // ʱ�����
				uint16_t a2t_usercmd; // ��վ���͵��Զ���ָ��

				struct
				{
					uint64 tagPollTxTime;						// ��ǩPoll���ݰ�����ʱ��
					uint64 tagRespRxTime[MAX_ANCHOR_LIST_SIZE]; // ��ǩResp���ݰ�����ʱ��
					uint64 tagFinalTxTime;						// ��ǩFinal���ݰ�����ʱ��
				} twr_timestamp;
			} measure;

			struct
			{
				bool DwCanSleep; // �豸����״̬
				uint8_t event;	 // ���߳�ʱ�¼�

				bool timeron;				// ������ʱ����־λ
				uint32_t interval_in_ms;	// �豸����ʱ��,����blinkfast blinkslow����
				uint32_t nextWakeUpTime_ms; // ������ʱ��������ʱ�����
				uint32_t preWakeUpTime_ms;	// ��һ�λ��ѷ���ʱ��
			} machine_period;

			struct
			{
				msg_time_t poll;
				msg_time_t response;
				msg_time_t final;
			} msg_time;
		} twr_tag;
	} twr_tag_info_t;

	enum inst_done
	{
		INST_NOT_DONE_YET = 0,		   // ��ǰ�¼���Ȼ��Ҫ�������ڴ�����
		INST_DONE_WAIT_FOR_NEXT_EVENT, // ��ǰ�¼��Ѿ��������������

		INST_DONE_WAIT_FOR_NEXT_EVENT_TO // ��ǰʱ���Ѿ��������ȴ��¼���ʱ(��ǩ����ʹ��)
	};

	enum inst_states
	{
		TA_INIT,

		/*TOF*/
		TA_TXPOLL_WAIT_SEND,
		TA_TXRESP_WAIT_SEND,
		TA_TXFINAL_WAIT_SEND,
		TA_TXSYSY_WAIT_SEND,
		TA_TXE_WAIT,

		TA_TX_WAIT_CONF, // �ȴ����ݷ������ȷ��
		TA_RXE_WAIT,	 // ������������
		TA_RX_WAIT_DATA, // �������ݽ׶�
		TA_SLEEP_DONE,	 // �豸˯��

		DWT_SIG_SLEEP_TIMEOUT // ���߳�ʱ����
	};

	//-----------------------------------------------------------------------------
	// exported functions prototypes
	//
	extern twr_tag_info_t *getTwrInfoPtr_tag(void);
	static void tag_received_timeout_set(twr_tag_info_t *pTwrInfo, bool error);

	// ��������
	static void prepare_twr_poll_msg(tx_pckt_t *pTxPckt, twr_tag_info_t *pTwrInfo);	 // �ڵ����poll
	static void prepare_twr_resp_msg(rx_pckt_t *pRxPckt, twr_tag_info_t *pTwrInfo);	 // �ڵ㷢��resp
	static void prepare_twr_final_msg(tx_pckt_t *pTxPckt, twr_tag_info_t *pTwrInfo); // �ڵ����final
	static void twr_uwb_process(rx_pckt_t *pRxPckt, twr_tag_info_t *pTwrInfo);

	int tagtestapprun(twr_tag_info_t *pTwrInfo);
	void tag_instance_run(void);
	void tag_pdoa_task(void);

	extern void tag_start(void);
	//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* __NODE__H__ */
