/************************************************************************
* Copyright (C) 2012-2018, Focaltech Systems (R)，All Rights Reserved.
*
* File Name: focaltech_test_detail_threshold.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-08-01
*
* Abstract: Set Detail Threshold for all IC
*
************************************************************************/

#include "focaltech_test.h"

int ft3417_malloc_struct_DetailThreshold(void)
{
    FTS_TEST_FUNC_ENTER();

    /*malloc mcap detailthreshold*/
    ft3417_test_data.mcap_detail_thr.invalid_node = (unsigned char (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(unsigned char));
    if (NULL == ft3417_test_data.mcap_detail_thr.invalid_node) goto ERR;
    ft3417_test_data.mcap_detail_thr.invalid_node_sc = (unsigned char (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(unsigned char));
    if (NULL == ft3417_test_data.mcap_detail_thr.invalid_node_sc) goto ERR;
    ft3417_test_data.mcap_detail_thr.rawdata_test_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rawdata_test_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.rawdata_test_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rawdata_test_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.rawdata_test_low_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rawdata_test_low_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.rawdata_test_low_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rawdata_test_low_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.rawdata_test_high_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rawdata_test_high_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.rawdata_test_high_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rawdata_test_high_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.rx_linearity_test_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.rx_linearity_test_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.tx_linearity_test_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.tx_linearity_test_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_rawdata_on_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_rawdata_on_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_rawdata_on_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_rawdata_on_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_rawdata_off_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_rawdata_off_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_rawdata_off_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_rawdata_off_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_cb_test_on_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_cb_test_on_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_cb_test_on_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_cb_test_on_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_cb_test_off_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_cb_test_off_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.scap_cb_test_off_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.scap_cb_test_off_min) goto ERR;
    ft3417_test_data.mcap_detail_thr.noise_test_coefficient = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.noise_test_coefficient) goto ERR;
    ft3417_test_data.mcap_detail_thr.panel_differ_test_max = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.panel_differ_test_max) goto ERR;
    ft3417_test_data.mcap_detail_thr.panel_differ_test_min = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.panel_differ_test_min) goto ERR;
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
	ft3417_test_data.mcap_detail_thr.LCDNoistTest_Coefficient = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX*sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.LCDNoistTest_Coefficient) goto ERR;

    ft3417_test_data.mcap_detail_thr.NoistTest_Coefficient = (int (*)[RX_NUM_MAX])ft3417_malloc(NUM_MAX*sizeof(int));
    if (NULL == ft3417_test_data.mcap_detail_thr.NoistTest_Coefficient) goto ERR;
/*[Arima_4101][allen_yu] 20190102 end*/
    /*malloc incell detailthreshold*/
    ft3417_test_data.incell_detail_thr.invalid_node =  ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.invalid_node)  goto ERR;
    ft3417_test_data.incell_detail_thr.rawdata_test_min = ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.rawdata_test_min) goto ERR;
    ft3417_test_data.incell_detail_thr.rawdata_test_max = ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.rawdata_test_max)goto ERR;
    ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_min = ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.rawdata_test_min) goto ERR;
    ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_max = ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.rawdata_test_max)goto ERR;
    ft3417_test_data.incell_detail_thr.cb_test_min = ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.cb_test_min)goto ERR;
    ft3417_test_data.incell_detail_thr.cb_test_max = ft3417_malloc(NUM_MAX * sizeof(int));
    if (NULL ==  ft3417_test_data.incell_detail_thr.cb_test_max)goto ERR;

    FTS_TEST_FUNC_EXIT();

    return 0;

ERR:
    FTS_TEST_ERROR("ft3417_malloc memory failed in function.");
    return -ENOMEM;
}

void ft3417_free_struct_DetailThreshold(void)
{

    /*free mcap detailthreshold*/
    if (NULL != ft3417_test_data.mcap_detail_thr.invalid_node) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.invalid_node);
        ft3417_test_data.mcap_detail_thr.invalid_node = NULL;
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.invalid_node_sc) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.invalid_node_sc);
        ft3417_test_data.mcap_detail_thr.invalid_node_sc = NULL;
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rawdata_test_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rawdata_test_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rawdata_test_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rawdata_test_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rawdata_test_low_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rawdata_test_low_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rawdata_test_low_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rawdata_test_low_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rawdata_test_high_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rawdata_test_high_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rawdata_test_high_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rawdata_test_high_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.rx_linearity_test_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.rx_linearity_test_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.tx_linearity_test_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.tx_linearity_test_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_rawdata_on_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_rawdata_on_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_rawdata_on_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_rawdata_on_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_rawdata_off_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_rawdata_off_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_rawdata_off_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_rawdata_off_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_cb_test_on_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_cb_test_on_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_cb_test_on_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_cb_test_on_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_cb_test_off_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_cb_test_off_max);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.scap_cb_test_off_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.scap_cb_test_off_min);
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.noise_test_coefficient) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.noise_test_coefficient);
        ft3417_test_data.mcap_detail_thr.noise_test_coefficient = NULL;
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.panel_differ_test_max) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.panel_differ_test_max);
        ft3417_test_data.mcap_detail_thr.panel_differ_test_max = NULL;
    }
    if (NULL != ft3417_test_data.mcap_detail_thr.panel_differ_test_min) {
        ft3417_free(ft3417_test_data.mcap_detail_thr.panel_differ_test_min);
        ft3417_test_data.mcap_detail_thr.panel_differ_test_min = NULL;
    }
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
	if (NULL !=	ft3417_test_data.mcap_detail_thr.LCDNoistTest_Coefficient) {
			ft3417_free(  ft3417_test_data.mcap_detail_thr.LCDNoistTest_Coefficient);
			ft3417_test_data.mcap_detail_thr.LCDNoistTest_Coefficient = NULL;
	}

	if (NULL != ft3417_test_data.mcap_detail_thr.NoistTest_Coefficient)
    {
        ft3417_free(ft3417_test_data.mcap_detail_thr.NoistTest_Coefficient);
        ft3417_test_data.mcap_detail_thr.NoistTest_Coefficient = NULL;
    }
/*[Arima_4101][allen_yu] 20190102 end*/
    /*free incell detailthreshold*/
    if (NULL !=  ft3417_test_data.incell_detail_thr.invalid_node) {
        ft3417_free( ft3417_test_data.incell_detail_thr.invalid_node);
        ft3417_test_data.incell_detail_thr.invalid_node = NULL;
    }
    if (NULL !=   ft3417_test_data.incell_detail_thr.rawdata_test_min) {
        ft3417_free(  ft3417_test_data.incell_detail_thr.rawdata_test_min);
        ft3417_test_data.incell_detail_thr.rawdata_test_min = NULL;
    }
    if (NULL !=   ft3417_test_data.incell_detail_thr.rawdata_test_max) {
        ft3417_free(  ft3417_test_data.incell_detail_thr.rawdata_test_max);
        ft3417_test_data.incell_detail_thr.rawdata_test_max = NULL;
    }
    if (NULL !=   ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_min) {
        ft3417_free(  ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_min);
        ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_min = NULL;
    }
    if (NULL !=   ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_max) {
        ft3417_free(  ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_max);
        ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_max = NULL;
    }
    if (NULL !=   ft3417_test_data.incell_detail_thr.cb_test_min) {
        ft3417_free(  ft3417_test_data.incell_detail_thr.cb_test_min);
        ft3417_test_data.incell_detail_thr.cb_test_min = NULL;
    }
    if (NULL !=   ft3417_test_data.incell_detail_thr.cb_test_max) {
        ft3417_free(  ft3417_test_data.incell_detail_thr.cb_test_max);
        ft3417_test_data.incell_detail_thr.cb_test_max = NULL;
    }
}
void ft3417_OnInit_InvalidNode(char *strIniFile)
{

    char str[MAX_PATH] = {0}, strTemp[MAX_PATH] = {0};
    int i = 0, j = 0;
    FTS_TEST_FUNC_ENTER();

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            sprintf(strTemp, "InvalidNode[%d][%d]", (i + 1), (j + 1));

            ft3417_GetPrivateProfileString("INVALID_NODE", strTemp, "1", str, strIniFile);
            if (ft3417_atoi(str) == 0) {
                ft3417_test_data.mcap_detail_thr.invalid_node[i][j] = 0;
                ft3417_test_data.incell_detail_thr.invalid_node[i * ft3417_test_data.screen_param.rx_num + j] = 0;
                FTS_TEST_DBG("node (%d, %d)", (i + 1),  (j + 1));

            } else if ( ft3417_atoi( str ) == 2 ) {
                ft3417_test_data.mcap_detail_thr.invalid_node[i][j] = 2;
                ft3417_test_data.incell_detail_thr.invalid_node[i * ft3417_test_data.screen_param.rx_num + j] = 2;
            } else {
                ft3417_test_data.mcap_detail_thr.invalid_node[i][j] = 1;
                ft3417_test_data.incell_detail_thr.invalid_node[i * ft3417_test_data.screen_param.rx_num + j] = 1;
            }
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            sprintf(strTemp, "InvalidNodeS[%d][%d]", (i + 1), (j + 1));
            ft3417_GetPrivateProfileString("INVALID_NODES", strTemp, "1", str, strIniFile);
            if (ft3417_atoi(str) == 0) {
                ft3417_test_data.mcap_detail_thr.invalid_node_sc[i][j] = 0;
            } else if ( ft3417_atoi( str ) == 2 ) {
                ft3417_test_data.mcap_detail_thr.invalid_node_sc[i][j] = 2;
            } else
                ft3417_test_data.mcap_detail_thr.invalid_node_sc[i][j] = 1;
        }

    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_RawDataTest(char *strIniFile)
{
    char str[128];
    char strTemp[MAX_PATH];
    char strValue[MAX_PATH];
    int MaxValue, MinValue, B_MaxValue, B_MinValue;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    /*RawData Test*/
    ft3417_GetPrivateProfileString( "Basic_Threshold", "RawDataTest_Max", "10000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_BFrame_Max", "5000", str, strIniFile);
    B_MaxValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rawdata_test_max[i][j] = MaxValue;
            ft3417_test_data.incell_detail_thr.rawdata_test_max[i * ft3417_test_data.screen_param.rx_num + j] = MaxValue;
            ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_max[i * ft3417_test_data.screen_param.rx_num + j] = B_MaxValue;
        }
    }

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Max_Tx%d", (i + 1));
        //FTS_TEST_DBG("%s ",  str);
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "111", strTemp, strIniFile);
        //FTS_TEST_DBG("ft3417_GetPrivateProfileString = %d ",  dividerPos);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rawdata_test_max[i][k] = (short)(ft3417_atoi(str_tmp));
                ft3417_test_data.incell_detail_thr.rawdata_test_max[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) Rawdata_Max_Tx%d = %d \n", (i+1), (k+1), (i+1),ft3417_test_data.incell_detail_thr.rawdata_test_max[i*ft3417_test_data.screen_param.rx_num+k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }

    }

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_Min", "7000", str, strIniFile);
    MinValue = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_BFrame_Min", "11000", str, strIniFile);
    B_MinValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rawdata_test_min[i][j] = MinValue;
            ft3417_test_data.incell_detail_thr.rawdata_test_min[i * ft3417_test_data.screen_param.rx_num + j] = MinValue;
            ft3417_test_data.incell_detail_thr.rawdata_test_b_frame_min[i * ft3417_test_data.screen_param.rx_num + j] = B_MinValue;
        }
    }
    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Min_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rawdata_test_min[i][k] = (short)(ft3417_atoi(str_tmp));
                ft3417_test_data.incell_detail_thr.rawdata_test_min[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) Rawdata_Min_Tx%d = %d \n", (i+1), (k+1), (i+1),ft3417_test_data.incell_detail_thr.rawdata_test_min[i*ft3417_test_data.screen_param.rx_num+k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    /*RawData Test Low*/
    ft3417_GetPrivateProfileString( "Basic_Threshold", "RawDataTest_Low_Max", "15000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rawdata_test_low_max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Max_Low_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rawdata_test_low_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_Low_Min", "3000", str, strIniFile);
    MinValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rawdata_test_low_min[i][j] = MinValue;
        }
    }
    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Min_Low_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rawdata_test_low_min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    /*RawData Test High*/
    ft3417_GetPrivateProfileString( "Basic_Threshold", "RawDataTest_High_Max", "15000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rawdata_test_high_max[i][j] = MaxValue;
        }
    }
    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_High_Min", "3000", str, strIniFile);
    MinValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rawdata_test_high_min[i][j] = MinValue;
        }
    }
    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Max_High_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rawdata_test_high_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }


    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Min_High_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rawdata_test_high_min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_SCapRawDataTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    /*SCapRawDataTest_OFF*/
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapRawDataTest_OFF_Min", "150", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapRawDataTest_OFF_Max", "1000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    /*Max*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_rawdata_off_max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapRawData_OFF_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_rawdata_off_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    /*Min*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_rawdata_off_min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapRawData_OFF_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_rawdata_off_min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    /*ON*/
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapRawDataTest_ON_Min", "150", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapRawDataTest_ON_Max", "1000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    /*Max*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_rawdata_on_max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapRawData_ON_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_rawdata_on_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    /*Min*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_rawdata_on_min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapRawData_ON_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_rawdata_on_min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_SCapCbTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    /*SCapCbTest_ON*/
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapCbTest_ON_Min", "0", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapCbTest_ON_Max", "240", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    /*Max*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_cb_test_on_max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapCB_ON_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_cb_test_on_max[i][k] = (ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    /*Min*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_cb_test_on_min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapCB_ON_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_cb_test_on_min[i][k] = (ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    /*SCapCbTest_OFF*/
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapCbTest_OFF_Min", "0", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "SCapCbTest_OFF_Max", "240", str, strIniFile);
    MaxValue = ft3417_atoi(str);
    /*Max*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_cb_test_off_max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapCB_OFF_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_cb_test_off_max[i][k] = (ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    /*Min*/
    for (i = 0; i < 2; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.scap_cb_test_off_min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 2; i++) {
        sprintf(str, "ScapCB_OFF_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.scap_cb_test_off_min[i][k] = (ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_PanelDifferTest(char *strIniFile)
{

    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue;
    int dividerPos = 0;
    int index = 0;
    int  k = 0, i = 0, j = 0;
    char str_tmp[128];

    FTS_TEST_FUNC_ENTER();
    ////////////////////////////Panel_Differ Test
    ft3417_GetPrivateProfileString("Basic_Threshold", "PanelDifferTest_Max", "1000", str, strIniFile);
    MaxValue = ft3417_atoi(str);
    for ( i = 0; i < TX_NUM_MAX; i++) {
        for ( j = 0; j < RX_NUM_MAX; j++) {
            ft3417_test_data.mcap_detail_thr.panel_differ_test_max[i][j] = MaxValue;
        }
    }

    for ( i = 0; i < TX_NUM_MAX; i++) {
        sprintf(str, "Panel_Differ_Max_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.panel_differ_test_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }


    ft3417_GetPrivateProfileString("Basic_Threshold", "PanelDifferTest_Min", "150", str, strIniFile);
    MinValue = ft3417_atoi(str);
    for ( i = 0; i < TX_NUM_MAX; i++) {
        for ( j = 0; j < RX_NUM_MAX; j++) {
            ft3417_test_data.mcap_detail_thr.panel_differ_test_min[i][j] = MinValue;
        }
    }

    for ( i = 0; i < TX_NUM_MAX; i++) {
        sprintf(str, "Panel_Differ_Min_Tx%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.panel_differ_test_min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();

}



void ft3417_OnInit_DThreshold_RxLinearityTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue = 0;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    ////////////////////////////Rx_Linearity Test
    ft3417_GetPrivateProfileString( "Basic_Threshold", "RxLinearityTest_Max", "50", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    //FTS_TEST_DBG("MaxValue = %d  ",  MaxValue);

    for (i = 0; i < ft3417_test_data.screen_param.used_max_tx_num; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.rx_linearity_test_max[i][j] = MaxValue;
        }
    }

    for (i = 0; i < ft3417_test_data.screen_param.used_max_tx_num; i++) {
        sprintf(str, "Rx_Linearity_Max_Tx%d", (i + 1));
        //FTS_TEST_DBG("%s ",  str);
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "111", strTemp, strIniFile);
        //FTS_TEST_DBG("ft3417_GetPrivateProfileString = %d ",  dividerPos);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.rx_linearity_test_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }

    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_TxLinearityTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue = 0;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    ////////////////////////////Tx_Linearity Test
    ft3417_GetPrivateProfileString( "Basic_Threshold", "TxLinearityTest_Max", "50", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    //FTS_TEST_DBG("MaxValue = %d  ",  MaxValue);

    for (i = 0; i < ft3417_test_data.screen_param.used_max_tx_num; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.tx_linearity_test_max[i][j] = MaxValue;
        }
    }

    for (i = 0; i < ft3417_test_data.screen_param.used_max_tx_num; i++) {
        sprintf(str, "Tx_Linearity_Max_Tx%d", (i + 1));
        //FTS_TEST_DBG("%s ",  str);
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "111", strTemp, strIniFile);
        //FTS_TEST_DBG("ft3417_GetPrivateProfileString = %d ",  dividerPos);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.tx_linearity_test_max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }

    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_ForceTouch_SCapRawDataTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    //////////////////OFF
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapRawDataTest_OFF_Min", "150", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapRawDataTest_OFF_Max", "1000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    ///Max
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_OFF_Max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapRawData_OFF_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_OFF_Max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    ////Min
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_OFF_Min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapRawData_OFF_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_OFF_Min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    //////////////////ON
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapRawDataTest_ON_Min", "150", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapRawDataTest_ON_Max", "1000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    //FTS_TEST_DBG("%d:%d\r", MinValue, MaxValue);
    //////读取阈值，若无特殊设置，则以Basic_Threshold替代

    ///Max
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_ON_Max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapRawData_ON_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp); //FTS_TEST_DBG("%s:%s\r", str, strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_ON_Max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    ////Min
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_ON_Min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapRawData_ON_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp); //FTS_TEST_DBG("%s:%s\r", str, strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapRawDataTest_ON_Min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}

void ft3417_OnInit_DThreshold_ForceTouch_SCapCbTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    //////////////////ON
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapCbTest_ON_Min", "0", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapCbTest_ON_Max", "240", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    //FTS_TEST_DBG("%d:%d\r", MinValue, MaxValue);
    //////读取阈值，若无特殊设置，则以Basic_Threshold替代

    ///Max
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_ON_Max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapCB_ON_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp); //FTS_TEST_DBG("%s:%s\r", str, strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_ON_Max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    ////Min
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_ON_Min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapCB_ON_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp); //FTS_TEST_DBG("%s:%s\r", str, strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        FTS_TEST_DBG("%s\r", strTemp);
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_ON_Min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    //////////////////OFF
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapCbTest_OFF_Min", "0", str, strIniFile);
    MinValue = ft3417_atoi(str);
    ft3417_GetPrivateProfileString("Basic_Threshold", "ForceTouch_SCapCbTest_OFF_Max", "240", str, strIniFile);
    MaxValue = ft3417_atoi(str);
    ///Max
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_OFF_Max[i][j] = MaxValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapCB_OFF_Max_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_OFF_Max[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    ////Min
    for (i = 0; i < 1; i++) {
        for (j = 0; j < ft3417_test_data.screen_param.used_max_rx_num; j++) {
            ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_OFF_Min[i][j] = MinValue;
        }
    }
    for (i = 0; i < 1; i++) {
        sprintf(str, "ForceTouch_ScapCB_OFF_Min_%d", (i + 1));
        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0x00, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.mcap_detail_thr.ForceTouch_SCapCbTest_OFF_Min[i][k] = (short)(ft3417_atoi(str_tmp));
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}
void ft3417_OnInit_DThreshold_CBTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue, MaxValue_Vkey, MinValue_Vkey;
    int ChannelNumTest_ChannelXNum, ChannelNumTest_ChannelYNum;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    ft3417_GetPrivateProfileString("Basic_Threshold", "CBTest_Max", "100", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "CBTest_Max_Vkey", "100", str, strIniFile);
    MaxValue_Vkey = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "ChannelNumTest_ChannelX", "15", str, strIniFile);
    ChannelNumTest_ChannelXNum = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "ChannelNumTest_ChannelY", "24", str, strIniFile);
    ChannelNumTest_ChannelYNum = ft3417_atoi(str);

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.incell_detail_thr.cb_test_max[i * ft3417_test_data.screen_param.rx_num + j] = MaxValue;
        }

        if (i == ChannelNumTest_ChannelXNum) {
            for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
                ft3417_test_data.incell_detail_thr.cb_test_max[i * ft3417_test_data.screen_param.rx_num + j] = MaxValue_Vkey;
            }
        }
    }

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "CB_Max_Tx%d", (i + 1));
        //FTS_TEST_DBG("%s ",  str);
        dividerPos  = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        // FTS_TEST_DBG("i = %d, dividerPos = %d \n", i+1, dividerPos);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.incell_detail_thr.cb_test_max[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) CB_Max_Tx%d = %d cb_test_max[%d] = %d\n", (i+1), (k+1), (i+1),ft3417_test_data.incell_detail_thr.cb_test_max[i*ft3417_test_data.screen_param.rx_num+k], i*ft3417_test_data.screen_param.rx_num+k, ft3417_test_data.incell_detail_thr.cb_test_max[i*ft3417_test_data.screen_param.rx_num+k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }



    ft3417_GetPrivateProfileString("Basic_Threshold", "CBTest_Min", "3", str, strIniFile);
    MinValue = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "CBTest_Min_Vkey", "3", str, strIniFile);
    MinValue_Vkey = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.incell_detail_thr.cb_test_min[i * ft3417_test_data.screen_param.rx_num + j] = MinValue;
        }

        if (i == ChannelNumTest_ChannelXNum) {
            for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
                ft3417_test_data.incell_detail_thr.cb_test_min[i * ft3417_test_data.screen_param.rx_num + j] = MinValue_Vkey;
            }
        }
    }

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "CB_Min_Tx%d", (i + 1));
        dividerPos  =  ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        //FTS_TEST_DBG("i = %d, dividerPos = %d \n", i+1, dividerPos);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.incell_detail_thr.cb_test_min[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) CB_Min_Tx%d = %d cb_test_min[%d] = %d \n", (i+1), (k+1), (i+1),ft3417_test_data.incell_detail_thr.cb_test_min[i*ft3417_test_data.screen_param.rx_num+k], i*ft3417_test_data.screen_param.rx_num+k, ft3417_test_data.incell_detail_thr.cb_test_min[i*ft3417_test_data.screen_param.rx_num+k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}


void ft3417_OnInit_DThreshold_AllButtonCBTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    ft3417_GetPrivateProfileString("Basic_Threshold", "CBTest_Max", "100", str, strIniFile);
    MaxValue = ft3417_atoi(str);


    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.incell_detail_thr.cb_test_max[i * ft3417_test_data.screen_param.rx_num + j] = MaxValue;
        }
    }

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "CB_Max_Tx%d", (i + 1));
        //FTS_TEST_DBG("%s ",  str);
        dividerPos  = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        // FTS_TEST_DBG("i = %d, dividerPos = %d \n", i+1, dividerPos);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.incell_detail_thr.cb_test_max[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) value = %d \n", (i+1), (k+1), ft3417_test_data.incell_detail_thr.cb_test_max[i][k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }



    ft3417_GetPrivateProfileString("Basic_Threshold", "CBTest_Min", "3", str, strIniFile);
    MinValue = ft3417_atoi(str);

    for (i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.incell_detail_thr.cb_test_min[i * ft3417_test_data.screen_param.rx_num + j] = MinValue;
        }
    }

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "CB_Min_Tx%d", (i + 1));
        dividerPos  =  ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        //FTS_TEST_DBG("i = %d, dividerPos = %d \n", i+1, dividerPos);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.incell_detail_thr.cb_test_min[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) value = %d \n", (i+1), (k+1), ft3417_test_data.incell_detail_thr.cb_test_min[i*dividerPos+k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    FTS_TEST_FUNC_EXIT();
}


void ft3417_OnThreshold_VkAndVaRawDataSeparateTest(char *strIniFile)
{
    char str[128], strTemp[MAX_PATH], strValue[MAX_PATH];
    int MaxValue, MinValue, MaxValue_Vkey, MinValue_Vkey;
    int ChannelNumTest_ChannelXNum, ChannelNumTest_ChannelYNum;
    int   dividerPos = 0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_Max", "11000", str, strIniFile);
    MaxValue = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_Max_VKey", "11000", str, strIniFile);
    MaxValue_Vkey = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "ChannelNumTest_ChannelX", "15", str, strIniFile);
    ChannelNumTest_ChannelXNum = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "ChannelNumTest_ChannelY", "24", str, strIniFile);
    ChannelNumTest_ChannelYNum = ft3417_atoi(str);

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.incell_detail_thr.rawdata_test_max[i * ft3417_test_data.screen_param.rx_num + j] = MaxValue;
        }

        if (i == ChannelNumTest_ChannelXNum) {
            for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
                ft3417_test_data.incell_detail_thr.rawdata_test_max[i * ft3417_test_data.screen_param.rx_num + j] = MaxValue_Vkey;
            }
        }
    }


    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Max_Tx%d", (i + 1));
        dividerPos  = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        //FTS_TEST_DBG("i = %d, dividerPos = %d \n", i+1, dividerPos);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.incell_detail_thr.rawdata_test_max[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //  FTS_TEST_DBG("node (%d, %d) value = %d \n", (i+1), (k+1), ft3417_test_data.incell_detail_thr.rawdata_test_max[i][k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_Min", "5000", str, strIniFile);
    MinValue = ft3417_atoi(str);

    ft3417_GetPrivateProfileString("Basic_Threshold", "RawDataTest_Min_VKey", "5000", str, strIniFile);
    MinValue_Vkey = ft3417_atoi(str);

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
            ft3417_test_data.incell_detail_thr.rawdata_test_min[i * ft3417_test_data.screen_param.rx_num + j] = MinValue;
        }

        if (i == ChannelNumTest_ChannelXNum) {
            for ( j = 0; j < ft3417_test_data.screen_param.rx_num; j++) {
                ft3417_test_data.incell_detail_thr.rawdata_test_min[i * ft3417_test_data.screen_param.rx_num + j] = MinValue_Vkey;
            }
        }
    }

    for ( i = 0; i < ft3417_test_data.screen_param.tx_num + 1; i++) {
        sprintf(str, "RawData_Min_Tx%d", (i + 1));
        dividerPos  = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL", strTemp, strIniFile);
        sprintf(strValue, "%s", strTemp);
        //FTS_TEST_DBG("i = %d, dividerPos = %d \n", i+1, dividerPos);
        if (0 == dividerPos) continue;
        index = 0;
        k = 0;
        memset(str_tmp, 0, sizeof(str_tmp));
        for (j = 0; j < dividerPos; j++) {
            if (',' == strValue[j]) {
                ft3417_test_data.incell_detail_thr.rawdata_test_min[i * ft3417_test_data.screen_param.rx_num + k] = (short)(ft3417_atoi(str_tmp));
                //FTS_TEST_DBG("node (%d, %d) value = %d \n", (i+1), (k+1), ft3417_test_data.incell_detail_thr.rawdata_test_min[i][k]);
                index = 0;
                memset(str_tmp, 0x00, sizeof(str_tmp));
                k++;
            } else {
                if (' ' == strValue[j])
                    continue;
                str_tmp[index] = strValue[j];
                index++;
            }
        }
    }
    FTS_TEST_FUNC_EXIT();
}

/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
void ft3417_OnInit_DThreshold_LCDNoiseTest(char * strIniFile)
{
    char str[128], strTemp[MAX_PATH],strValue[MAX_PATH];
    int MaxValue = 0;
    int   dividerPos=0;
    char str_tmp[128];
    int index = 0;
    int  k = 0, i = 0, j = 0;

	////////////////////////////LCD NoiseTest
	ft3417_GetPrivateProfileString("TestItem","LCD_NOISE_TEST","0",str,strIniFile);
	if( 1 != ft3417_atoi( str ) )
		return;

	ft3417_GetPrivateProfileString("Basic_Threshold","Lcd_Noise_NoiseCoefficient","50",str,strIniFile);
	MaxValue = ft3417_atoi(str);

	for( i = 0; i < TX_NUM_MAX; i++)
	{
		for( j = 0; j < RX_NUM_MAX; j++)
		{
			ft3417_test_data.mcap_detail_thr.LCDNoistTest_Coefficient[i][j] = MaxValue;
		}
	}

	for ( i = 0; i < TX_NUM_MAX; i++)
   	{
        	sprintf(str, "LCDNoistTestCoefficient_Tx%d", (i + 1));
	        dividerPos = ft3417_GetPrivateProfileString( "SpecialSet", str, "NULL",strTemp, strIniFile);
	        sprintf(strValue, "%s",strTemp);
	        if (0 == dividerPos) continue;
	        index = 0;
	        k = 0;
	        memset(str_tmp, 0, sizeof(str_tmp));
	        for (j=0; j<dividerPos; j++)
	        {
	            if (',' == strValue[j])
	            {
	                ft3417_test_data.mcap_detail_thr.NoistTest_Coefficient[i][k] = (short)(ft3417_atoi(str_tmp));
	                index = 0;
	                memset(str_tmp, 0x00, sizeof(str_tmp));
	                k++;
	            }
	            else
	            {
	                if (' ' == strValue[j])
	                    continue;
	                str_tmp[index] = strValue[j];
	                index++;
	            }
	        }
    	}

}
/*[Arima_4101][allen_yu] 20190102 end*/