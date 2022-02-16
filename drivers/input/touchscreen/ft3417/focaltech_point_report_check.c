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

/*****************************************************************************
*
* File Name: focaltech_point_report_check.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-11-16
*
* Abstract: point report check function
*
* Version: v1.0
*
* Revision History:
*
*****************************************************************************/

/*****************************************************************************
* Included header files
*****************************************************************************/
#include "focaltech_core.h"

#if FTS_POINT_REPORT_CHECK_EN
/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/
#define POINT_REPORT_CHECK_WAIT_TIME              200    /* unit:ms */

/*****************************************************************************
* functions body
*****************************************************************************/
/*****************************************************************************
*  Name: ft3417_prc_func
*  Brief: ft3417 point report check work func, report whole up of points
*  Input:
*  Output:
*  Return:
*****************************************************************************/
static void ft3417_prc_func(struct work_struct *work)
{
    struct ft3417_ts_data *ts_data = container_of(work,
                                  struct ft3417_ts_data, prc_work.work);
    struct input_dev *input_dev = ts_data->input_dev;

#if FTS_MT_PROTOCOL_B_EN
    u32 finger_count = 0;
#endif

    FTS_FUNC_ENTER();
    mutex_lock(&ts_data->report_mutex);

#if FTS_MT_PROTOCOL_B_EN
    for (finger_count = 0; finger_count < ts_data->pdata->max_touch_number; finger_count++) {
        input_mt_slot(input_dev, finger_count);
        input_mt_report_slot_state(input_dev, MT_TOOL_FINGER, false);
    }
#else
    input_mt_sync(input_dev);
#endif
    input_report_key(input_dev, BTN_TOUCH, 0);
    input_sync(input_dev);

    mutex_unlock(&ts_data->report_mutex);

    FTS_FUNC_EXIT();
}

/*****************************************************************************
*  Name: ft3417_prc_queue_work
*  Brief: ft3417 point report check queue work, call it when interrupt comes
*  Input:
*  Output:
*  Return:
*****************************************************************************/
void ft3417_prc_queue_work(struct ft3417_ts_data *ts_data)
{
    cancel_delayed_work(&ts_data->prc_work);
    queue_delayed_work(ts_data->ts_workqueue, &ts_data->prc_work,
                       msecs_to_jiffies(POINT_REPORT_CHECK_WAIT_TIME));
}

/*****************************************************************************
*  Name: ft3417_point_report_check_init
*  Brief:
*  Input:
*  Output:
*  Return: < 0: Fail to create esd check queue
*****************************************************************************/
int ft3417_point_report_check_init(struct ft3417_ts_data *ts_data)
{
    FTS_FUNC_ENTER();

    if (ts_data->ts_workqueue) {
        INIT_DELAYED_WORK(&ts_data->prc_work, ft3417_prc_func);
    } else {
        FTS_ERROR("ft3417 workqueue is NULL, can't run point report check function");
        return -EINVAL;
    }

    FTS_FUNC_EXIT();
    return 0;
}

/*****************************************************************************
*  Name: ft3417_point_report_check_exit
*  Brief:
*  Input:
*  Output:
*  Return:
*****************************************************************************/
int ft3417_point_report_check_exit(struct ft3417_ts_data *ts_data)
{
    FTS_FUNC_ENTER();

    FTS_FUNC_EXIT();
    return 0;
}
#endif /* FTS_POINT_REPORT_CHECK_EN */

