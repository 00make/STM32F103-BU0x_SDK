/*
 * @file    cmd.c
 * @brief     command string as specified in document SWxxxx version X.x.x
 * @param    *text - string
 *             source - is an input stream source: this will be used to reply to the specified direction
 * @author Decawave Software
 *
 *
 */
#include <cmd.h>
#include <cmd_fn.h>

#include "Generic_cmd.h"
/*
 *    Command interface
 */

/* IMPLEMENTATION */

/*
 * @brief "error" will be sent if error during parser or command execution returned error
 * */
static void cmd_onERROR (const char* err, control_t* pcmd)
{
    char* str = (char*) CMD_MALLOC (MAX_STR_SIZE);

    if (str)
    {
        strcpy (str, "error \r\n");

        if (strlen (err) < (sizeof (str) - 6 - 3 - 1))
        {
            strcpy (&str[6], err);
            strcpy (&str[6 + strlen (err)], "\r\n");
        }

        port_tx_msg ( (uint8_t*) str, strlen (str));

        CMD_FREE (str);
    }
}

/*
 * @brief �ַ�����ֽ�������
 * @return ������Ĳ�������
 **/
int string_split (char* strp, uint32_t strsize, char ch, char* argv[], uint32_t argcM)
{
    int argc_index = 0;
    int ch_index = 0;
    int start_index = 0;

    if (!strsize || !argcM) return 0;

    // �����ַ���
    while (ch_index < strsize && argc_index < argcM - 1)   // ��1��Ϊ����ĩβ���һ����ָ��
    {
        if (strp[ch_index] == ch)   // ����Ƿ��ҵ��ָ���
        {
            if (start_index != ch_index)   // ������ǿ�ʼλ�ã�����Ӳ���
            {
                argv[argc_index++] = &strp[start_index];
                strp[ch_index] = '\0'; // ���ָ����滻Ϊ�ַ���������
            }

            start_index = ch_index + 1; // ������һ����������ʼλ��
        }

        ch_index++;
    }

    // ������һ��������������ڣ�
    if (start_index < strsize)
    {
        argv[argc_index++] = &strp[start_index];
    }

    // ��ӿ�ָ�뵽argv�����ĩβ
    argv[argc_index] = NULL;

    return argc_index;
}

#define respond_error() _dbg_printf("\r\nERR\r\n")
#define respond_ok()        _dbg_printf("\r\nOK\r\n");

/* @fn         command_parser
 * @brief    checks if input "text" string in known "COMMAND" or "PARAMETER VALUE" format,
 *             checks their execution permissions, a VALUE range if restrictions and
 *             executes COMMAND or sets the PARAMETER to the VALUE
 * */
int command_parser (uint8_t* pdata, uint16_t size)
{
    int ret  = -1;
    char* ptr = NULL;
    int argc = ARGC_LIMIT;
    uint16_t offset = 0;
    int index = 0;
    char* argv[ARGC_LIMIT] = { (char*) 0 };

    if (strstr ( (const char*) pdata, "AT") == NULL) goto at_end;

    for (index = 0; known_commands[index].name != NULL; index++)
    {
        ptr = strstr ( (const char*) pdata, known_commands[index].name);

        if (ptr != NULL)
        {
            ptr += strlen (known_commands[index].name);
            offset = ptr - (char*) pdata;
            break;
        }
    }

    /* ������ѯ���� */
    if ( (ptr[0] == '?') && (ptr[1] == '\r') && (ptr[2] == '\n'))
    {
        if (NULL != known_commands[index].deal_func)
        {
            ret = known_commands[index].deal_func (QUERY_CMD, argc, argv);
        }
    }
    else if ( (ptr[0] == '\r') && (ptr[1] == '\n'))    /* ����ִ������ */
    {
        if (NULL != known_commands[index].deal_func)
        {
            ret = known_commands[index].deal_func (EXECUTE_CMD, argc, argv);
        }
    }
    else if (ptr[0] == '=')     /* ������������ */
    {
        ptr += 1;
        argc = string_split ( (char*) ptr, size - offset, ',', argv, argc);

        if (NULL != known_commands[index].deal_func)
        {
            ret = known_commands[index].deal_func (SET_CMD, argc, argv);
        }
    }
    else
    {
        ret = -1;
    }

at_end:

    if (-1 == ret) respond_error();
    else respond_ok();

    return ret;
}

/* end of cmd.c */
