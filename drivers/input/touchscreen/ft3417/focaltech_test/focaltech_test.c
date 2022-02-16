/*
 *
 * FocalTech TouchScreen driver.
 *
 * Copyright (c) 2012-2018, FocalTech Systems, Ltd., all rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/************************************************************************
*
* File Name: focaltech_test.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-08-01
*
* Modify:
*
* Abstract: create char device and proc node for  the comm between APK and TP
*
************************************************************************/

/*****************************************************************************
* Included header files
*****************************************************************************/
#include "focaltech_test.h"
/*[Arima_4101][allen_yu] fix self test fail for pcba 20190104 begin*/
#include "../focaltech_core.h"
/*[Arima_4101][allen_yu] 20190104 end*/

/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
struct ft3417_test_data ft3417_test_data;

struct test_funcs *ft3417_test_funcs_list[] = {
    &test_func_ft5x46,
};

struct test_ic_type ft3417_ic_types[] = {
    {"FT5X22", 6, IC_FT5X46},
    {"FT5X46", 6, IC_FT5X46},
    {"FT5X46i", 7, IC_FT5X46I},
    {"FT5526", 6, IC_FT5526},
    {"FT3X17", 6, IC_FT3X17},
    {"FT5436", 6, IC_FT5436},
    {"FT3X27", 6, IC_FT3X27},
    {"FT5526i", 7, IC_FT5526I},
    {"FT5416", 6, IC_FT5416},
    {"FT5426", 6, IC_FT5426},
    {"FT5435", 6, IC_FT5435},
    {"FT7681", 6, IC_FT7681},
    {"FT7661", 6, IC_FT7661},
    {"FT7511", 6, IC_FT7511},
    {"FT7421", 6, IC_FT7421},
    {"FT7311", 6, IC_FT7311},
    {"FT3327DQQ-001", 13, IC_FT3327DQQ_001},
    {"FT5446DQS-W01", 13, IC_FT5446DQS_W01},
    {"FT5446DQS-W02", 13, IC_FT5446DQS_W02},
    {"FT5446DQS-002", 13, IC_FT5446DQS_002},
    {"FT5446DQS-Q02", 13, IC_FT5446DQS_Q02},

    {"FT6X36", 6, IC_FT6X36},
    {"FT3X07", 6, IC_FT3X07},
    {"FT6416", 6, IC_FT6416},
    {"FT6336G/U", 9, IC_FT6426},
    {"FT6236U", 7, IC_FT6236U},
    {"FT6436U", 7, IC_FT6436U},
    {"FT3267", 6, IC_FT3267},
    {"FT3367", 6, IC_FT3367},
    {"FT7401", 6, IC_FT7401},
    {"FT3407U", 7, IC_FT3407U},

    {"FT5822", 6, IC_FT5822},
    {"FT5626", 6, IC_FT5626},
    {"FT5726", 6, IC_FT5726},
    {"FT5826B", 7, IC_FT5826B},
    {"FT3617", 6, IC_FT3617},
    {"FT3717", 6, IC_FT3717},
    {"FT7811", 6, IC_FT7811},
    {"FT5826S", 7, IC_FT5826S},
    {"FT3517U", 7, IC_FT3517U},

    {"FT3518", 6, IC_FT3518},
    {"FT3558", 6, IC_FT3558},

    {"FT8606", 6, IC_FT8606},

    {"FT8716U", 7, IC_FT8716U},
    {"FT8716F", 7, IC_FT8716F},
    {"FT8716", 6, IC_FT8716},
    {"FT8613", 6, IC_FT8613},

    {"FT3C47U", 7, IC_FT3C47U},

    {"FT8607U", 7, IC_FT8607U},
    {"FT8607", 6, IC_FT8607},

    {"FT8736", 6, IC_FT8736},

    {"FT3D47", 6, IC_FT3D47},

    {"FTE716", 6, IC_FTE716},

    {"FT8201", 6, IC_FT8201},

    {"FT8006M", 7, IC_FT8006M},
    {"FT7250", 6, IC_FT7250},

    {"FT8006U", 7, IC_FT8006U},

    {"FT8719", 6, IC_FT8719},
    {"FT8615", 6, IC_FT8615},

    {"FT8739", 6, IC_FT8739},
};

/*****************************************************************************
* Static function prototypes
*****************************************************************************/
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
extern int ft3417_test_result(int value);
/*[Arima_4101][allen_yu] 20190102 end*/
/*****************************************************************************
* functions body
*****************************************************************************/
void ft3417_sys_delay(int ms)
{
    msleep(ms);
}

int ft3417_focal_abs(int value)
{
    if (value < 0)
        value = 0 - value;

    return value;
}

void *ft3417_malloc(size_t size)
{
    return vmalloc(size);
}

void ft3417_free_proc(void *p)
{
    return vfree(p);
}

/********************************************************************
 * test i2c read/write interface
 *******************************************************************/
int ft3417_test_i2c_read(u8 *writebuf, int writelen, u8 *readbuf, int readlen)
{
    int ret = 0;
#if 1
    if (NULL == ft3417_data) {
        FTS_TEST_ERROR("ft3417_data is null, no test");
        return -EINVAL;
    }
    ret = ft3417_i2c_read(ft3417_data->client, writebuf, writelen, readbuf, readlen);
#else
    ret = ft3417_i2c_read(writebuf, writelen, readbuf, readlen);
#endif

    if (ret < 0)
        return ret;
    else
        return 0;
}

int ft3417_test_i2c_write(u8 *writebuf, int writelen)
{
    int ret = 0;
#if 1
    if (NULL == ft3417_data) {
        FTS_TEST_ERROR("ft3417_data is null, no test");
        return -EINVAL;
    }
    ret = ft3417_i2c_write(ft3417_data->client, writebuf, writelen);
#else
    ret = ft3417_i2c_write(writebuf, writelen);
#endif

    if (ret < 0)
        return ret;
    else
        return 0;
}

int ft3417_read_reg(u8 addr, u8 *val)
{
    return ft3417_test_i2c_read(&addr, 1, val, 1);
}

int ft3417_write_reg(u8 addr, u8 val)
{
    int ret;
    u8 cmd[2] = {0};

    cmd[0] = addr;
    cmd[1] = val;
    ret = ft3417_test_i2c_write(cmd, 2);

    return ret;
}

/********************************************************************
 * test global function enter work/factory mode
 *******************************************************************/
int ft3417_enter_work_mode(void)
{
    int ret = 0;
    u8 mode = 0;
    int i = 0;
    int j = 0;

    FTS_TEST_FUNC_ENTER();

    ret = ft3417_read_reg(DEVIDE_MODE_ADDR, &mode);
    if ((0 == ret) && (((mode >> 4) & 0x07) == 0x00))
        return 0;

    for (i = 0; i < ENTER_WORK_FACTORY_RETRIES; i++) {
        ret = ft3417_write_reg(DEVIDE_MODE_ADDR, 0x00);
        if (0 == ret) {
            for (j = 0; j < 20; j++) {
                ret = ft3417_read_reg(DEVIDE_MODE_ADDR, &mode);
                if ((0 == ret) && (((mode >> 4) & 0x07) == 0x00))
                    return 0;
                else
                    ft3417_sys_delay(FACTORY_TEST_DELAY);
            }
        }

        ft3417_sys_delay(50);
    }

    if (i >= ENTER_WORK_FACTORY_RETRIES) {
        FTS_TEST_ERROR("Enter work mode fail");
        return -EIO;
    }

    FTS_TEST_FUNC_EXIT();
    return 0;
}

int ft3417_enter_factory_mode(void)
{
    int ret = 0;
    u8 mode = 0;
    int i = 0;
    int j = 0;

    ret = ft3417_read_reg(DEVIDE_MODE_ADDR, &mode);
    if ((0 == ret) && (((mode >> 4) & 0x07) == 0x04))
        return 0;

    for (i = 0; i < ENTER_WORK_FACTORY_RETRIES; i++) {
        ret = ft3417_write_reg(DEVIDE_MODE_ADDR, 0x40);
        if (0 == ret) {
            for (j = 0; j < 20; j++) {
                ret = ft3417_read_reg(DEVIDE_MODE_ADDR, &mode);
                if ((0 == ret) && (((mode >> 4) & 0x07) == 0x04)) {
                    ft3417_sys_delay(200);
                    return 0;
                } else
                    ft3417_sys_delay(FACTORY_TEST_DELAY);
            }
        }

        ft3417_sys_delay(50);
    }

    if (i >= ENTER_WORK_FACTORY_RETRIES) {
        FTS_TEST_ERROR("Enter factory mode fail");
        return -EIO;
    }

    return 0;
}

/************************************************************************
* Name: ft3417_i2c_read_write
* Brief:  Write/Read Data by IIC
* Input: writebuf, writelen, readlen
* Output: readbuf
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
u8 ft3417_i2c_read_write(unsigned char *writebuf, int  writelen, unsigned char *readbuf, int readlen)
{
    int ret;

    if (readlen > 0) {
        ret = ft3417_test_i2c_read(writebuf, writelen, readbuf, readlen);
    } else {
        ret = ft3417_test_i2c_write(writebuf, writelen);
    }

    if (ret >= 0)
        return (ERROR_CODE_OK);
    else
        return (ERROR_CODE_COMM_ERROR);
}

/*
 * ft3417_read_mass_data - read rawdata/short test data
 * addr - register addr which read data from
 * byte_num - read data length, unit:byte
 * buf - save data
 *
 * return 0 if read data succuss, otherwise return error code
 */
int ft3417_read_mass_data(u8 addr, int byte_num, int *buf)
{
    int ret = 0;
    int i = 0;
    u8 reg_addr = 0;
    u8 *data = NULL;
    int read_num = 0;
    int packet_num = 0;
    int packet_remainder = 0;
    int offset = 0;

    data = (u8 *)ft3417_malloc(byte_num * sizeof(u8));
    if (NULL == data) {
        FTS_TEST_SAVE_ERR("rawdata buffer malloc fail\n");
        return -ENOMEM;
    }

    /* read rawdata buffer */
    FTS_TEST_INFO("mass data len:%d", byte_num);
    packet_num = byte_num / BYTES_PER_TIME;
    packet_remainder = byte_num % BYTES_PER_TIME;
    if (packet_remainder)
        packet_num++;

    if (byte_num < BYTES_PER_TIME) {
        read_num = byte_num;
    } else {
        read_num = BYTES_PER_TIME;
    }
    FTS_TEST_INFO("packet num:%d, remainder:%d", packet_num, packet_remainder);

    reg_addr = addr;
    ret = ft3417_test_i2c_read(&reg_addr, 1, data, read_num);
    if (ret) {
        FTS_TEST_SAVE_ERR("read rawdata fail\n");
        goto READ_MASSDATA_ERR;
    }

    for (i = 1; i < packet_num; i++) {
        offset = read_num * i;
        if ((i == (packet_num - 1)) && packet_remainder) {
            read_num = packet_remainder;
        }

        ret = ft3417_test_i2c_read(NULL, 0, data + offset, read_num);
        if (ret) {
            FTS_TEST_SAVE_ERR("read much rawdata fail\n");
            goto READ_MASSDATA_ERR;
        }
    }

    for (i = 0; i < byte_num; i = i + 2) {
        buf[i >> 1] = (int)(((int)(data[i]) << 8) + data[i + 1]);
    }

READ_MASSDATA_ERR:
    if (data) {
        ft3417_free(data);
    }

    return ret;
}

/*
 * ft3417_chip_clb_incell - auto clb
 */
int ft3417_chip_clb_incell(void)
{
    int ret = 0;
    u8 val = 0;
    int times = 0;

    /* start clb */
    ret = ft3417_write_reg(FACTORY_REG_CLB, 0x04);
    if (ret) {
        FTS_TEST_SAVE_ERR("write start clb fail\n");
        return ret;
    }

    while (times++ < FACTORY_TEST_RETRY) {
        ft3417_sys_delay(FACTORY_TEST_RETRY_DELAY);
        ret = ft3417_read_reg(FACTORY_REG_CLB, &val);
        if ((0 == ret) && (0x02 == val)) {
            /* clb ok */
            break;
        } else
            FTS_TEST_DBG("reg%x=%x,retry:%d", FACTORY_REG_CLB, val, times);
    }

    if (times >= FACTORY_TEST_RETRY) {
        FTS_TEST_SAVE_ERR("chip clb timeout\n");
        return -EIO;
    }

    return 0;
}

/*
 * ft3417_start_scan_incell - start to scan a frame for incell ic
 */
int ft3417_start_scan_incell(void)
{
    int ret = 0;
    u8 val = 0x00;
    int times = 0;

    ret = ft3417_read_reg(DEVIDE_MODE_ADDR, &val);
    if (ret) {
        FTS_TEST_SAVE_ERR("read device mode fail\n");
        return ret;
    }

    /* Top bit position 1, start scan */
    val |= 0x80;
    ret = ft3417_write_reg(DEVIDE_MODE_ADDR, val);
    if (ret) {
        FTS_TEST_SAVE_ERR("write device mode fail\n");
        return ret;
    }

    while (times++ < FACTORY_TEST_RETRY) {
        /* Wait for the scan to complete */
        ft3417_sys_delay(FACTORY_TEST_DELAY);

        ret = ft3417_read_reg(DEVIDE_MODE_ADDR, &val);
        if ((0 == ret) && (0 == (val >> 7))) {
            break;
        } else
            FTS_TEST_DBG("reg%x=%x,retry:%d", DEVIDE_MODE_ADDR, val, times);
    }

    if (times >= FACTORY_TEST_RETRY) {
        FTS_TEST_SAVE_ERR("start scan timeout\n");
        return -EIO;
    }

    return 0;
}

/*
 * ft3417_wait_state_update - wait fw status update
 */
int ft3417_wait_state_update(void)
{
    int ret = 0;
    int times = 0;
    u8 state = 0xFF;

    while (times++ < FACTORY_TEST_RETRY) {
        ft3417_sys_delay(FACTORY_TEST_RETRY_DELAY);
        /* Wait register status update */
        state = 0xFF;
        ret = ft3417_read_reg(FACTORY_REG_PARAM_UPDATE_STATE, &state);
        if ((0 == ret) && (0x00 == state))
            break;
        else
            FTS_TEST_DBG("reg%x=%x,retry:%d", \
                         FACTORY_REG_PARAM_UPDATE_STATE, state, times);
    }

    if (times >= FACTORY_TEST_RETRY) {
        FTS_TEST_SAVE_ERR("Wait State Update fail\n");
        return -EIO;
    }

    return 0;
}

/*
 * ft3417_get_rawdata_incell - read rawdata/diff for incell ic
 */
int ft3417_get_rawdata_incell(int *data)
{
    int ret = 0;
    u8 tx_num = 0;
    u8 rx_num = 0;
    u8 key_num = 0;
    int va_num = 0;

    FTS_TEST_FUNC_ENTER();
    /* enter into factory mode */
    ret = ft3417_enter_factory_mode();
    if (ret) {
        FTS_TEST_SAVE_ERR("Failed to Enter Factory Mode\n");
        return ret;
    }

    /* get tx/rx num */
    tx_num = ft3417_test_data.screen_param.tx_num;
    rx_num = ft3417_test_data.screen_param.rx_num;
    va_num = tx_num * rx_num;
    key_num = ft3417_test_data.screen_param.key_num_total;

    /* start to scan a frame */
    ret = ft3417_start_scan_incell();
    if (ret) {
        FTS_TEST_SAVE_ERR("Failed to Scan ...\n");
        return ret;
    }

    /* Read RawData for va Area */
    ret = ft3417_write_reg(FACTORY_REG_LINE_ADDR, 0xAD);
    if (ret) {
        FTS_TEST_SAVE_ERR("wirte AD to reg01 fail\n");
        return ret;
    }
    ret = ft3417_read_mass_data(FACTORY_REG_RAWDATA_ADDR, va_num * 2, data);
    if (ret) {
        FTS_TEST_SAVE_ERR("Failed to Get RawData of channel.\n");
        return ret;
    }

    /* Read RawData for key Area */
    ret = ft3417_write_reg(FACTORY_REG_LINE_ADDR, 0xAE);
    if (ret) {
        FTS_TEST_SAVE_ERR("wirte AE to reg01 fail\n");
        return ret;
    }
    ret = ft3417_read_mass_data(FACTORY_REG_RAWDATA_ADDR, key_num * 2, data + va_num);
    if (ret) {
        FTS_TEST_SAVE_ERR("Failed to Get RawData of keys.\n");
        return ret;
    }

    FTS_TEST_FUNC_EXIT();
    return 0;
}

/*
 * ft3417_get_cb_incell - get cb data for incell IC
 */
int ft3417_get_cb_incell(u16 saddr, int byte_num, int *cb_buf)
{
    int ret = 0;
    int i = 0;
    u8 cb_addr = 0;
    u8 addr_h = 0;
    u8 addr_l = 0;
    int read_num = 0;
    int packet_num = 0;
    int packet_remainder = 0;
    int offset = 0;
    int addr = 0;
    u8 *data = NULL;

    data = (u8 *)ft3417_malloc(byte_num * sizeof(u8));
    if (NULL == data) {
        FTS_TEST_SAVE_ERR("cb buffer malloc fail\n");
        return -ENOMEM;
    }

    packet_num = byte_num / BYTES_PER_TIME;
    packet_remainder = byte_num % BYTES_PER_TIME;
    if (packet_remainder)
        packet_num++;
    read_num = BYTES_PER_TIME;

    FTS_TEST_INFO("cb packet:%d,remainder:%d", packet_num, packet_remainder);
    cb_addr = FACTORY_REG_CB_ADDR;
    for (i = 0; i < packet_num; i++) {
        offset = read_num * i;
        addr = saddr + offset;
        addr_h = (addr >> 8) & 0xFF;
        addr_l = addr & 0xFF;
        if ((i == (packet_num - 1)) && packet_remainder) {
            read_num = packet_remainder;
        }

        ret = ft3417_write_reg(FACTORY_REG_CB_ADDR_H, addr_h);
        if (ret) {
            FTS_TEST_SAVE_ERR("write cb addr high fail\n");
            goto TEST_CB_ERR;
        }
        ret = ft3417_write_reg(FACTORY_REG_CB_ADDR_L, addr_l);
        if (ret) {
            FTS_TEST_SAVE_ERR("write cb addr low fail\n");
            goto TEST_CB_ERR;
        }

        ret = ft3417_test_i2c_read(&cb_addr, 1, data + offset, read_num);
        if (ret) {
            FTS_TEST_SAVE_ERR("read cb fail\n");
            goto TEST_CB_ERR;
        }
    }

    for (i = 0; i < byte_num; i++) {
        cb_buf[i] = data[i];
    }

TEST_CB_ERR:
    ft3417_free(data);
    return ret;
}

/*
 * ft3417_weakshort_get_adc_data_incell - get short(adc) data for incell IC
 */
int ft3417_weakshort_get_adc_data_incell(u8 retval, u8 ch_num, int byte_num, int *adc_buf)
{
    int ret = 0;
    int times = 0;
    u8 short_state = 0;

    FTS_TEST_FUNC_ENTER();

    /* Start ADC sample */
    ret = ft3417_write_reg(FACTORY_REG_SHORT_TEST_EN, 0x01);
    if (ret) {
        FTS_TEST_SAVE_ERR("start short test fail\n");
        goto ADC_ERROR;
    }
    ft3417_sys_delay(ch_num * FACTORY_TEST_DELAY);

    for (times = 0; times < FACTORY_TEST_RETRY; times++) {
        ret = ft3417_read_reg(FACTORY_REG_SHORT_TEST_STATE, &short_state);
        if ((0 == ret) && (retval == short_state))
            break;
        else
            FTS_TEST_DBG("reg%x=%x,retry:%d",
                         FACTORY_REG_SHORT_TEST_STATE, short_state, times);

        ft3417_sys_delay(FACTORY_TEST_RETRY_DELAY);
    }
    if (times >= FACTORY_TEST_RETRY) {
        FTS_TEST_SAVE_ERR("short test timeout, ADC data not OK\n");
        ret = -EIO;
        goto ADC_ERROR;
    }

    ret = ft3417_read_mass_data(FACTORY_REG_SHORT_ADDR, byte_num, adc_buf);
    if (ret) {
        FTS_TEST_SAVE_ERR("get short(adc) data fail\n");
    }

ADC_ERROR:
    FTS_TEST_FUNC_EXIT();
    return ret;
}

/*
 * ft3417_show_data_incell - show and save test data to testresult.txt
 */
void ft3417_show_data_incell(int *data, bool include_key)
{
    int row = 0;
    int col = 0;
    int tx_num = ft3417_test_data.screen_param.tx_num;
    int rx_num = ft3417_test_data.screen_param.rx_num;
    int key_num = ft3417_test_data.screen_param.key_num_total;

    FTS_TEST_SAVE_INFO("\nVA Channels: ");
    for (row = 0; row < tx_num; row++) {
        FTS_TEST_SAVE_INFO("\nCh_%02d:  ", row + 1);
        for (col = 0; col < rx_num; col++) {
            FTS_TEST_SAVE_INFO("%5d, ", data[row * rx_num + col]);
        }
    }

    if (include_key) {
        FTS_TEST_SAVE_INFO("\nKeys:   ");
        for (col = 0; col < key_num; col++) {
            FTS_TEST_SAVE_INFO("%5d, ",  data[rx_num * tx_num + col]);
        }
    }
    FTS_TEST_SAVE_INFO("\n");
}

/*
 * ft3417_compare_data_incell - check data in range or not
 *
 * return true if check pass, or return false
 */
bool ft3417_compare_data_incell(int *data, int min, int max, int vk_min, int vk_max, bool include_key)
{
    int row = 0;
    int col = 0;
    int value = 0;
    bool tmp_result = true;
    int tx_num = ft3417_test_data.screen_param.tx_num;
    int rx_num = ft3417_test_data.screen_param.rx_num;
    int key_num = ft3417_test_data.screen_param.key_num_total;

    /* VA area */
    for (row = 0; row < tx_num; row++) {
        for (col = 0; col < rx_num; col++) {
            if (0 == ft3417_test_data.incell_detail_thr.invalid_node[row * rx_num + col])
                continue; /* Invalid Node */
            value = data[row * rx_num + col];
            if (value < min || value > max) {
                tmp_result = false;
                FTS_TEST_SAVE_INFO("test failure. Node=(%d, %d), \
                    Get_value=%d,  Set_Range=(%d, %d)\n", \
                                   row + 1, col + 1, value, min, max);
            }
        }
    }
    /* Key area */
    if (include_key) {
        if (ft3417_test_data.screen_param.key_flag) {
            key_num = ft3417_test_data.screen_param.key_num;
        }
        row = ft3417_test_data.screen_param.tx_num;
        for (col = 0; col < key_num; col++) {
            if (0 == ft3417_test_data.incell_detail_thr.invalid_node[tx_num * rx_num + col])
                continue; /* Invalid Node */
            value = data[rx_num * tx_num + col];
            if (value < vk_min || value > vk_max) {
                tmp_result = false;
                FTS_TEST_SAVE_INFO("test failure. Node=(%d, %d), \
                    Get_value=%d,  Set_Range=(%d, %d)\n", \
                                   row + 1, col + 1, value, vk_min, vk_max);
            }
        }
    }

    return tmp_result;
}


/************************************************************************
* Name: ft3417_compare_detailthreshold_data_incell
* Brief:  ft3417_compare_detailthreshold_data_incell
* Input: none
* Output: none
* Return: none.
***********************************************************************/
bool ft3417_compare_detailthreshold_data_incell(int *data, int *data_min, int *data_max, bool include_key)
{
    int row, col;
    int value;
    bool tmp_result = true;
    int tmp_min, tmp_max;
    int rx_num = ft3417_test_data.screen_param.rx_num;
    int tx_num = ft3417_test_data.screen_param.tx_num;
    int key_num = ft3417_test_data.screen_param.key_num_total;
    // VA
    for (row = 0; row < tx_num; row++) {
        for (col = 0; col < rx_num; col++) {
            if (ft3417_test_data.incell_detail_thr.invalid_node[row * rx_num + col] == 0)continue; //Invalid Node
            tmp_min = data_min[row * rx_num + col];
            tmp_max = data_max[row * rx_num + col];
            value = data[row * rx_num + col];
            if (value < tmp_min || value > tmp_max) {
                tmp_result = false;
                FTS_TEST_SAVE_INFO(" \n test failure. Node=(%d,  %d), Get_value=%d,  Set_Range=(%d, %d)", \
                                   row + 1, col + 1, value, tmp_min, tmp_max);
            }
        }
    }
    // Key
    if (include_key) {
        if (ft3417_test_data.screen_param.key_flag) {
            key_num = ft3417_test_data.screen_param.key_num;
        }
        row = ft3417_test_data.screen_param.tx_num;
        for ( col = 0; col < key_num; col++ ) {
            if (ft3417_test_data.incell_detail_thr.invalid_node[rx_num * tx_num + col] == 0)continue; //Invalid Node
            tmp_min = data_min[rx_num * tx_num + col];
            tmp_max = data_max[rx_num * tx_num + col];
            value = data[rx_num * tx_num + col];
            if (value < tmp_min || value > tmp_max) {
                tmp_result = false;
                FTS_TEST_SAVE_INFO(" \n test failure. Node=(%d,  %d), Get_value=%d,  Set_Range=(%d, %d)", \
                                   row + 1, col + 1, value, tmp_min, tmp_max);
            }
        }
    }

    return tmp_result;
}

/*
 * ft3417_save_testdata_incell - save data to testdata.csv
 */
void ft3417_save_testdata_incell(int *data, char *test_num, int index, u8 row, u8 col, u8 item_count)
{
    int len = 0;
    int i = 0, j = 0;

    FTS_TEST_FUNC_ENTER();

    /* Save Msg */
    len = snprintf(ft3417_test_data.tmp_buffer, BUFF_LEN_TMP_BUFFER, \
                   "%s, %d, %d, %d, %d, %d, ", \
                   test_num, ft3417_test_data.test_item_code, row, col, \
                   ft3417_test_data.start_line, item_count);

    memcpy(ft3417_test_data.msg_area_line2 + ft3417_test_data.len_msg_area_line2, ft3417_test_data.tmp_buffer, len);
    ft3417_test_data.len_msg_area_line2 += len;

    ft3417_test_data.start_line += row;
    ft3417_test_data.ft3417_test_data_count++;

    /* Save Data */
    for (i = 0 + index; (i < row + index) && (i < TX_NUM_MAX); i++) {
        for (j = 0; (j < col) && (j < RX_NUM_MAX); j++) {
            if (j == (col - 1)) {
                /* The Last Data of the row, add "\n" */
                len = snprintf(ft3417_test_data.tmp_buffer, BUFF_LEN_TMP_BUFFER, \
                               "%d, \n", data[col * (i + index) + j]);
            } else {
                len = snprintf(ft3417_test_data.tmp_buffer, BUFF_LEN_TMP_BUFFER, \
                               "%d, ", data[col * (i + index) + j]);
            }

            memcpy(ft3417_test_data.store_data_area + ft3417_test_data.len_store_data_area, ft3417_test_data.tmp_buffer, len);
            ft3417_test_data.len_store_data_area += len;
        }
    }

    FTS_TEST_FUNC_EXIT();
}

/*
 * ft3417_ic_table_get_ic_code_from_ic_name - get ic code from ic name
 */
u32 ft3417_ic_table_get_ic_code_from_ic_name(char *ic_name)
{
    int i = 0;
    int type_size = 0;

    type_size = sizeof(ft3417_ic_types) / sizeof(ft3417_ic_types[0]);
    for (i = 0; i < type_size; i++) {
        if (0 == strncmp(ic_name, ft3417_ic_types[i].ic_name, ft3417_ic_types[i].len))
            return ft3417_ic_types[i].ic_type;
    }

    FTS_TEST_ERROR("no IC type match");
    return 0;
}

/*
 * ft3417_init_test_funcs - get test function based on ic_type
 */
int ft3417_init_test_funcs(u32 ic_type)
{
    int i = 0;
    struct test_funcs *funcs = ft3417_test_funcs_list[0];
    int funcs_len = sizeof(ft3417_test_funcs_list) / sizeof(ft3417_test_funcs_list[0]);
    u32 ic_series = 0;

    ic_series = TEST_ICSERIES(ic_type);
    FTS_TEST_INFO("ic_type:%x, test functions len:%x", ic_type, funcs_len);
    for (i = 0; i < funcs_len; i++) {
        funcs = ft3417_test_funcs_list[i];
        if (ic_series == funcs->ic_series) {
            ft3417_test_data.func = funcs;
            break;
        }
    }

    if (i >= funcs_len) {
        FTS_TEST_ERROR("no ic serial function match");
        return -ENODATA;
    }

    return 0;
}

/*
 * ft3417_test_save_test_data - Save test data to SD card etc.
 */
static int ft3417_test_save_test_data(char *file_name, char *data_buf, int len)
{
    struct file *pfile = NULL;
    char filepath[128];
    loff_t pos;
    mm_segment_t old_fs;

    FTS_TEST_FUNC_ENTER();
    memset(filepath, 0, sizeof(filepath));
    sprintf(filepath, "%s%s", FTS_INI_FILE_PATH, file_name);
    if (NULL == pfile) {
        pfile = filp_open(filepath, O_TRUNC | O_CREAT | O_RDWR, 0);
    }
    if (IS_ERR(pfile)) {
        FTS_TEST_ERROR("error occured while opening file %s.",  filepath);
        return -EIO;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    pos = 0;
    vfs_write(pfile, data_buf, len, &pos);
    filp_close(pfile, NULL);
    set_fs(old_fs);

    FTS_TEST_FUNC_EXIT();
    return 0;
}

/************************************************************************
* Name: ft3417_set_testitem
* Brief:  set test item code and name
* Input: null
* Output: null
* Return:
**********************************************************************/
void ft3417_set_testitem(u8 itemcode)
{
    ft3417_test_data.test_item[ft3417_test_data.test_num].itemcode = itemcode;
    ft3417_test_data.test_item[ft3417_test_data.test_num].testnum = ft3417_test_data.test_num;
    ft3417_test_data.test_item[ft3417_test_data.test_num].testresult = RESULT_NULL;
    ft3417_test_data.test_num++;
}

/************************************************************************
* Name: ft3417_merge_all_testdata
* Brief:  Merge All Data of test result
* Input: none
* Output: none
* Return: none
***********************************************************************/
void ft3417_merge_all_testdata(void)
{
    int len = 0;

    //Msg Area, Add Line1
    ft3417_test_data.len_store_msg_area += sprintf(ft3417_test_data.store_msg_area, "ECC, 85, 170, IC Name, %s, IC Code, %x\n",  ft3417_test_data.ini_ic_name,  ft3417_test_data.screen_param.selected_ic);

    /* Add the head part of Line2 */
    len = sprintf(ft3417_test_data.tmp_buffer, "TestItem Num, %d, ", ft3417_test_data.ft3417_test_data_count);
    memcpy(ft3417_test_data.store_msg_area + ft3417_test_data.len_store_msg_area, ft3417_test_data.tmp_buffer, len);
    ft3417_test_data.len_store_msg_area += len;

    /* Add other part of Line2, except for "\n" */
    memcpy(ft3417_test_data.store_msg_area + ft3417_test_data.len_store_msg_area, ft3417_test_data.msg_area_line2, ft3417_test_data.len_msg_area_line2);
    ft3417_test_data.len_store_msg_area += ft3417_test_data.len_msg_area_line2;

    /* Add Line3 ~ Line10 */
    len = sprintf(ft3417_test_data.tmp_buffer, "\n\n\n\n\n\n\n\n\n");
    memcpy(ft3417_test_data.store_msg_area + ft3417_test_data.len_store_msg_area, ft3417_test_data.tmp_buffer, len);
    ft3417_test_data.len_store_msg_area += len;

    /* 1.Add Msg Area */
    memcpy(ft3417_test_data.store_all_data, ft3417_test_data.store_msg_area,  ft3417_test_data.len_store_msg_area);

    /* 2.Add Data Area */
    if (0 != ft3417_test_data.len_store_data_area) {
        memcpy(ft3417_test_data.store_all_data + ft3417_test_data.len_store_msg_area, ft3417_test_data.store_data_area, ft3417_test_data.len_store_data_area);
    }

    FTS_TEST_DBG("lenStoreMsgArea=%d,  lenStoreDataArea = %d",   ft3417_test_data.len_store_msg_area, ft3417_test_data.len_store_data_area);
}

void ft3417_init_storeparam_testdata(void)
{
    ft3417_test_data.testresult_len = 0;

    ft3417_test_data.len_store_msg_area = 0;
    ft3417_test_data.len_msg_area_line2 = 0;
    ft3417_test_data.len_store_data_area = 0;
    /* The Start Line of Data Area is 11 */
    ft3417_test_data.start_line = 11;
    ft3417_test_data.ft3417_test_data_count = 0; 
}

int ft3417_allocate_init_testdata_memory(void)
{
    ft3417_test_data.testresult = ft3417_malloc(BUFF_LEN_TESTRESULT_BUFFER);
    if (NULL == ft3417_test_data.testresult)
        goto ERR;

    ft3417_test_data.store_all_data = ft3417_malloc(FTS_TEST_STORE_DATA_SIZE);
    if (NULL == ft3417_test_data.store_all_data)
        goto ERR;

    ft3417_test_data.store_msg_area = ft3417_malloc(BUFF_LEN_STORE_MSG_AREA);
    if (NULL == ft3417_test_data.store_msg_area)
        goto ERR;

    ft3417_test_data.msg_area_line2 = ft3417_malloc(BUFF_LEN_MSG_AREA_LINE2);
    if (NULL == ft3417_test_data.msg_area_line2)
        goto ERR;

    ft3417_test_data.store_data_area = ft3417_malloc(BUFF_LEN_STORE_DATA_AREA);
    if (NULL == ft3417_test_data.store_data_area)
        goto ERR;

    ft3417_test_data.tmp_buffer = ft3417_malloc(BUFF_LEN_TMP_BUFFER);
    if (NULL == ft3417_test_data.tmp_buffer)
        goto ERR;

    ft3417_init_storeparam_testdata();
    return 0;
ERR:
    FTS_TEST_ERROR("ft3417_malloc memory failed in function.");
    return -ENOMEM;
}

/************************************************************************
* Name: ft3417_free_testdata_memory
* Brief:  Release pointer memory
* Input: none
* Output: none
* Return: none
***********************************************************************/
void ft3417_free_testdata_memory(void)
{
    /* free buff */
    if (NULL != ft3417_test_data.testresult)
        ft3417_free(ft3417_test_data.testresult);

    if (NULL !=  ft3417_test_data.store_all_data)
        ft3417_free(ft3417_test_data.store_all_data);

    if (NULL != ft3417_test_data.store_msg_area)
        ft3417_free(ft3417_test_data.store_msg_area);

    if (NULL != ft3417_test_data.msg_area_line2)
        ft3417_free(ft3417_test_data.msg_area_line2);

    if (NULL != ft3417_test_data.store_data_area)
        ft3417_free(ft3417_test_data.store_data_area);

    if (NULL != ft3417_test_data.tmp_buffer)
        ft3417_free(ft3417_test_data.tmp_buffer);

}

int ft3417_get_tx_rx_num(u8 tx_rx_reg, u8 *ch_num, u8 ch_num_max)
{
    int ret = 0;
    int i = 0;

    for (i = 0; i < 3; i++) {
        ret = ft3417_read_reg(tx_rx_reg, ch_num);
        if ((ret < 0) || (*ch_num > ch_num_max)) {
            ft3417_sys_delay(50);
        } else
            break;
    }

    if (i >= 3) {
        FTS_TEST_ERROR("get channel num fail");
        return -EIO;
    }

    return 0;
}

int ft3417_get_channel_num(void)
{
    int ret = 0;
    u8 tx_num = 0;
    u8 rx_num = 0;

    FTS_TEST_FUNC_ENTER();

    ret = ft3417_enter_factory_mode();
    if (ret < 0) {
        FTS_TEST_ERROR("enter factory mode fail, can't get tx/rx num");
        return ret;
    }

    ft3417_test_data.screen_param.used_max_tx_num = TX_NUM_MAX;
    ft3417_test_data.screen_param.used_max_rx_num = RX_NUM_MAX;
    ft3417_test_data.screen_param.key_num_total = KEY_NUM_MAX;
    ret = ft3417_get_tx_rx_num(FACTORY_REG_CHX_NUM, &tx_num, TX_NUM_MAX);
    if (ret < 0) {
        FTS_TEST_ERROR("get tx_num fail");
        return ret;
    }

    ret = ft3417_get_tx_rx_num(FACTORY_REG_CHY_NUM, &rx_num, RX_NUM_MAX);
    if (ret < 0) {
        FTS_TEST_ERROR("get rx_num fail");
        return ret;
    }

    ft3417_test_data.screen_param.tx_num = (int)tx_num;
    ft3417_test_data.screen_param.rx_num = (int)rx_num;

    FTS_TEST_INFO("TxNum=%d, RxNum=%d, MaxTxNum=%d, MaxRxNum=%d",
                  ft3417_test_data.screen_param.tx_num,
                  ft3417_test_data.screen_param.rx_num,
                  ft3417_test_data.screen_param.used_max_tx_num,
                  ft3417_test_data.screen_param.used_max_rx_num);

    FTS_TEST_FUNC_EXIT();
    return 0;
}

static int ft3417_test_init_basicinfo(void)
{
    int ret = 0;
    u8 val = 0;

    ret = ft3417_read_reg(REG_FW_VERSION, &val);
    FTS_TEST_SAVE_INFO("FW version:0x%02x\n", val);

    ret = ft3417_read_reg(REG_VA_TOUCH_THR, &val);
    ft3417_test_data.va_touch_thr = val;
    ret = ft3417_read_reg(REG_VKEY_TOUCH_THR, &val);
    ft3417_test_data.key_touch_thr = val;

    /* enter into factory mode and read tx/rx num */
    ret = ft3417_get_channel_num();
    FTS_TEST_SAVE_INFO("tx_num:%d, rx_num:%d\n",
                       ft3417_test_data.screen_param.tx_num,
                       ft3417_test_data.screen_param.rx_num);

    return ret;
}

static int ft3417_test_main_init(void)
{
    int ret = 0;
    int len = 0;

    FTS_TEST_FUNC_ENTER();
    /* allocate memory for test data:csv&txt */
    ret = ft3417_allocate_init_testdata_memory();
    if (ret < 0) {
        FTS_TEST_ERROR("allocate memory for test data fail");
        return ret;
    }

    /* get basic information: tx/rx num */
    ret = ft3417_test_init_basicinfo();
    if (ret < 0) {
        FTS_TEST_ERROR("test init basicinfo fail");
        return ret;
    }

    /* Allocate memory for detail threshold structure */
    ret = ft3417_malloc_struct_DetailThreshold();
    if (ret < 0) {
        FTS_TEST_ERROR("Failed to malloc memory for detaithreshold");
        return ret;
    }

    /*Allocate test data buffer*/
    len = (ft3417_test_data.screen_param.tx_num + 1) * ft3417_test_data.screen_param.rx_num;
    ft3417_test_data.buffer = (int *)ft3417_malloc(len * sizeof(int));
    if (NULL == ft3417_test_data.buffer) {
        FTS_TEST_ERROR("ft3417_test_data.buffer malloc fail");
        return -ENOMEM;
    }
    memset(ft3417_test_data.buffer, 0, len * sizeof(int));

    FTS_TEST_FUNC_EXIT();
    return ret;
}

/*
 * ft3417_test_get_testparams - get test parameter from ini
 */
/*[Arima_4101][allen_yu] fix self test fail for pcba 20190104 begin*/
static int ft3417_test_get_testparams_forapk(int test)
{
    int ret = 0;

    ret = ft3417_test_get_testparam_from_ini_forapk(test);

    return ret;
}


/*
 * ft3417_test_get_testparams - get test parameter from ini
 */
/*[Arima_4101][allen_yu] 20190104 end*/
static int ft3417_test_get_testparams(char *config_name)
{
    int ret = 0;

    ret = ft3417_test_get_testparam_from_ini(config_name);

    return ret;
}

static bool ft3417_test_start(void)
{
    bool testresult = false;

    FTS_TEST_FUNC_ENTER();
    FTS_TEST_DBG("IC_%s Test",  ft3417_test_data.ini_ic_name);

    if (ft3417_test_data.func && ft3417_test_data.func->start_test) {
        testresult = ft3417_test_data.func->start_test();
    } else {
        FTS_TEST_ERROR("test func/start_test func is null");
        testresult = false;
    }

    FTS_TEST_FUNC_EXIT();
    return testresult;
}

static int ft3417_test_main_exit(void)
{
    int data_len = 0;
    FTS_TEST_FUNC_ENTER();

    /* Merge test data */
    ft3417_merge_all_testdata();

    /*Gets the number of tests in the test library and saves it*/
    data_len = ft3417_test_data.len_store_msg_area + ft3417_test_data.len_store_data_area;
    ft3417_test_save_test_data(FTS_TESTDATA_FILE_NAME, ft3417_test_data.store_all_data, data_len);
    ft3417_test_save_test_data(FTS_TESTRESULT_FILE_NAME, ft3417_test_data.testresult, ft3417_test_data.testresult_len);

    /* free memory */
    ft3417_free_struct_DetailThreshold();
    ft3417_free_testdata_memory();

    /*free test data buffer*/
    if (ft3417_test_data.buffer)
        ft3417_free(ft3417_test_data.buffer);

    FTS_TEST_FUNC_EXIT();
    return 0;
}

/*
 * ft3417_test_entry - test main entry
 *
 * warning - need disable irq & esdcheck before call this function
 *
 */
static int ft3417_test_entry(char *ini_file_name)
{
    int ret = 0;

/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
    int test_result=0;
/*[Arima_4101][allen_yu] 20190102 end*/

    FTS_TEST_FUNC_ENTER();

    /* test initialize */
    ret = ft3417_test_main_init();
    if (ret < 0) {
        FTS_TEST_ERROR("ft3417_test_main_init() error.");
        goto TEST_ERR;
    }

    /*Read parse configuration file*/
    FTS_TEST_SAVE_INFO("ini_file_name = %s\n", ini_file_name);
    ret = ft3417_test_get_testparams(ini_file_name);
    if (ret < 0) {
        FTS_TEST_ERROR("get testparam failed");
        goto TEST_ERR;
    }

    /* Start testing according to the test configuration */
    if (true == ft3417_test_start()) {
        FTS_TEST_SAVE_INFO("\n=======Tp test pass. \n\n");
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
        test_result=1;
/*[Arima_4101][allen_yu] 20190102 end*/
    } else {
        FTS_TEST_SAVE_INFO("\n=======Tp test failure. \n\n");
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
		test_result=0;
/*[Arima_4101][allen_yu] 20190102 end*/
    }

/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
    ft3417_test_result(test_result);
/*[Arima_4101][allen_yu] 20190102 end*/

    ret = 0;
TEST_ERR:
    ft3417_enter_work_mode();
    ft3417_test_main_exit();

    FTS_TEST_FUNC_EXIT();
    return ret;
}

/*[Arima_4101][allen_yu] fix self test fail for pcba 20190104 begin*/
int ft3417_test_entry_forapk()
{
    int ret = 0;

/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
    int test_result=0;
/*[Arima_4101][allen_yu] 20190102 end*/

    FTS_TEST_FUNC_ENTER();

    /* test initialize */
    ret = ft3417_test_main_init();
    if (ret < 0) {
        FTS_TEST_ERROR("ft3417_test_main_init() error.");
        goto TEST_ERR;
    }

    /*Read parse configuration file*/

    ret = ft3417_test_get_testparams_forapk(0);
    if (ret < 0) {
        FTS_TEST_ERROR("get testparam failed");
        goto TEST_ERR;
    }

    /* Start testing according to the test configuration */
    if (true == ft3417_test_start()) {
        FTS_TEST_SAVE_INFO("\n=======Tp test pass. \n\n");
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
        test_result=1;
/*[Arima_4101][allen_yu] 20190102 end*/
    } else {
        FTS_TEST_SAVE_INFO("\n=======Tp test failure. \n\n");
/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
		test_result=0;
/*[Arima_4101][allen_yu] 20190102 end*/
    }

/*[Arima_4101][allen_yu] add lcm noise test and uniformity test for pcba self test 20190102 begin*/
    ft3417_test_result(test_result);
/*[Arima_4101][allen_yu] 20190102 end*/

    ret = 0;
TEST_ERR:
    ft3417_enter_work_mode();
    ft3417_test_main_exit();

    FTS_TEST_FUNC_EXIT();
    return ret;
}

void ft3417_selftest_forapk(){
	struct ft3417_ts_data *ts_data = ft3417_data;
    struct input_dev *input_dev = ts_data->input_dev;

	FTS_TEST_FUNC_ENTER();

    mutex_lock(&input_dev->mutex);
    disable_irq(ts_data->irq);
    ft3417_test_entry_forapk();

    enable_irq(ts_data->irq);
    mutex_unlock(&input_dev->mutex);

	FTS_TEST_FUNC_EXIT();
}
/*[Arima_4101][allen_yu] 20190104 end*/
/************************************************************************
* Name: ft3417_test_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return: EPERM
***********************************************************************/
static ssize_t ft3417_test_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return -EPERM;
}

/************************************************************************
* Name: ft3417_test_store
* Brief:  upgrade from app.bin
* Input: device, device attribute, char buf, char count
* Output: no
* Return: char count
***********************************************************************/
static ssize_t ft3417_test_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    char fwname[128] = {0};
    struct ft3417_ts_data *ts_data = ft3417_data;
    struct input_dev *input_dev;

    FTS_TEST_FUNC_ENTER();

    if (ts_data->suspended) {
        FTS_INFO("In suspend, no test, return now");
        return -EINVAL;
    }

    input_dev = ts_data->input_dev;
    memset(fwname, 0, sizeof(fwname));
    sprintf(fwname, "%s", buf);
    fwname[count - 1] = '\0';
    FTS_TEST_DBG("fwname:%s.", fwname);

    mutex_lock(&input_dev->mutex);
    disable_irq(ts_data->irq);

#if defined(FTS_ESDCHECK_EN) && (FTS_ESDCHECK_EN)
    ft3417_esdcheck_switch(DISABLE);
#endif

    ft3417_test_entry(fwname);

#if defined(FTS_ESDCHECK_EN) && (FTS_ESDCHECK_EN)
    ft3417_esdcheck_switch(ENABLE);
#endif

    enable_irq(ts_data->irq);
    mutex_unlock(&input_dev->mutex);

    FTS_TEST_FUNC_EXIT();
    return count;
}

/*  test from test.ini
*    example:echo "***.ini" > ft3417_test
*/
static DEVICE_ATTR(ft3417_test, S_IRUGO | S_IWUSR, ft3417_test_show, ft3417_test_store);

/* add your attr in here*/
static struct attribute *ft3417_test_attributes[] = {
    &dev_attr_ft3417_test.attr,
    NULL
};

static struct attribute_group ft3417_test_attribute_group = {
    .attrs = ft3417_test_attributes
};

int ft3417_test_init(struct i2c_client *client)
{
    int ret = 0;

    FTS_TEST_FUNC_ENTER();

    ret = sysfs_create_group(&client->dev.kobj, &ft3417_test_attribute_group);
    if (0 != ret) {
        FTS_TEST_ERROR( "[focal] %s() - ERROR: sysfs_create_group() failed.",  __func__);
        sysfs_remove_group(&client->dev.kobj, &ft3417_test_attribute_group);
    } else {
        FTS_TEST_DBG("[focal] %s() - sysfs_create_group() succeeded.", __func__);
    }

    FTS_TEST_FUNC_EXIT();

    return ret;
}

int ft3417_test_exit(struct i2c_client *client)
{
    FTS_TEST_FUNC_ENTER();

    sysfs_remove_group(&client->dev.kobj, &ft3417_test_attribute_group);

    FTS_TEST_FUNC_EXIT();
    return 0;
}
