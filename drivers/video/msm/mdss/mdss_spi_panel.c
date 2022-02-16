/* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/qpnp/pin.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/leds.h>
#include <linux/qpnp/pwm.h>
#include <linux/of_device.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/fcntl.h>
#include <linux/firmware.h>
//#include <stdio.h>
//#include <linux/stdio.h>
//#include "stdio.h"

#include "mdss.h"
#include "mdss_panel.h"
#include "mdss_spi_panel.h"
#include "mdss_spi_client.h"
#include "mdp3.h"
#include <linux/uaccess.h>
#include <linux/workqueue.h>
//[4101][Raymond] Implement call back feature - begin
struct device *g_dev = NULL;
static int gdev_done=0;
//[4101][Raymond] Implement call back feature - end
#define debug_panel 0
//<2018/10/08,Yuchen-[4101] define value
#define TOTAL_PIXEL_ARY_CNT 144000
#define EXT_UPDATE_PIXEL 10
//[4101][Raymond]revert defalut os_mode is Light os mode - begin
static int DYN_PARTIAL_UPDATE =1;
//[4101][Raymond]revert defalut os_mode is Light os mode- end
//#define DYN_PARTIAL_UPDATE 1
//>2018/10/08,Yuchen
#define HOT_FIX_LCM_INI_FAIL 1/* 2018/10/23,Yuchen */
int frame_drop = 1;
//[4101][LCM][RaymondLin] Add LCM_vendor file node for PCBA function test begin
#include <linux/proc_fs.h>
//[4101][LCM][RaymondLin] Add LCM_vendor file node for PCBA function test end
//[4101][Raymond]off charging icon - begin
//2019/02/23,Yuchen #include "black_offcharging.h"
#include "white_offcharging.h"
//[4101][Raymond]picture of the battery full for off-charging -begin
#include "white_full_chg.h"
//[4101][Raymond]picture of the battery full for off-charging -end
//[4101][Raymond]off charging icon - end

static bool rcy_mode=false; //2019/03/20,Yuchen-[4101] add recovery mode

static int wf_mode=3;
static int pu_en=0;
static int pu_x=0;
static int pu_y=0;
static int pu_rx=0;
static int pu_ry=0;
static int pu_w=600;
static int pu_l=480;
//[4101][Raymond]revert defalut os_mode is Light os mode - begin
int os_mode=0; // 0:android , 1:light os
static int bflash=1; // 0:not flash , 1:flash
//[4101][Raymond]revert defalut os_mode is Light os mode - end
static int done_flash=1; // 0:Not done flash, 1:Done flash
//[4101][Raymond]picture of the battery full for off-charging -begin
static int battery_full=0; //1:full chared , 0:charging
//[4101][Raymond]picture of the battery full for off-charging -end
u16 drx_x3=0x00;
u16 drx_x4=0x00;
u16 drx_y5=0x00;
u16 drx_y6=0x00;
u16 drx_w7=0x02;
u16 drx_w8=0x58;
u16 drx_l9=0x01;
u16 drx_l10=0xE0;
u16 vcom_value=0x31; //default 2.45
//<2019/02/23,Yuchen- [4101] reduce icon raw data
char EPD_Macro_Insert_4bit[TOTAL_PIXEL_ARY_CNT+2]={ 0 };//Macro Insert use
#define Insert_pixel(xor_c,pixel_buf,icon_w,icon_h,h_gap,w_gap)  \
do{                                                              \
  int i,j,k;                                                     \
  j=0;                                                           \
  for(i=0; i<icon_h; i++){                                       \
    for(k=0; k<icon_w; k++){                                     \
      EPD_Macro_Insert_4bit[(300*(i+w_gap))+h_gap+k]=pixel_buf[j] ^ xor_c; \
      j++;                                                       \
    }                                                            \
  }                                                              \
}while(0)
//>2019/02/23,Yuchen

#define TSC         0x40
#define INIT        	0
#define A2        	1
#define DU        	2
#define GL16        3
#define GC16        4
//#define WF_FILE_NAME "V270_W006_68_TC9999_ED028TC1U2_CTC.bin"
//#define WF_FILE_NAME "V270_W006_TC9999_ED028TC1U2_CTC_181105.bin"
#define WF_FILE_NAME "V270_W006_68_TC9905_ED028TC1U2_CTC_20190417.bin"
const struct firmware *wf_file_cfg = NULL;
static int first_request=1;
//[4101][Raymond]off charging icon - begin
static int chg_mode=0; //0:normal , 1:off charging mode
static int color_id=1; // 1:black , 2:white
static int chg_mode_show=1;
static int show_count=0;
static int EPD_POF=0;
//[4101][Raymond]off charging icon - end
//[4101][Raymond]three seconds Power Off command Implementation - begin
static struct delayed_work epd_pof_work;
//[4101][Raymond]three seconds Power Off command Implementation - end

//[4101][Raymond] Implement call back feature - begin	
	char *envp[2] = {"EPD_BUSY=0", NULL};
//[4101][Raymond] Implement call back feature - end
//[4101][Raymond] impelment uevent feature - begin
	char *envp2[2] = {"EPD_BUSY=1", NULL};
	char *power0[2] = {"POWER=0", NULL};
	char *power1[2] = {"POWER=1", NULL};
//[4101][Raymond] impelment uevent feature - end
static int panel_suspend=0;

DEFINE_LED_TRIGGER(bl_led_trigger);

static int check_busy_n_status(unsigned int busy_gpio)
{
	int rc = 0;
	#if 0
	rc = gpio_request(busy_gpio, "disp_busy");
	if (rc) {
		pr_err("disp_busy: gpio request failed\n");
		gpio_free(busy_gpio);
		return 1;
	}

	rc = gpio_direction_input(busy_gpio);
		if (rc < 0) {
		pr_err("disp_busy: gpio direction_input failed\n");
		gpio_free(busy_gpio);
		return 1;
	}
	mdelay(10);
	#endif
	 rc=gpio_get_value(busy_gpio);
	//gpio_free(busy_gpio);
	return rc;
}

static int mdss_spi_panel_reset(struct mdss_panel_data *pdata, int enable)
{
	struct spi_panel_data *ctrl_pdata = NULL;
	struct mdss_panel_info *pinfo = NULL;
	int i, rc = 0;
#if debug_panel
pr_err("%s ++, enable=%d \n",__func__,enable);
#endif
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	if (!gpio_is_valid(ctrl_pdata->rst_gpio)) {
		pr_debug("%s:%d, reset line not configured\n",
			   __func__, __LINE__);
		return rc;
	}
//raymond test remove disp_dc_gpio pin - begin
#if 0
	if (!gpio_is_valid(ctrl_pdata->disp_dc_gpio)) {
		pr_debug("%s:%d, dc line not configured\n",
			   __func__, __LINE__);
		return rc;
	}
#endif
//raymond test remove disp_dc_gpio pin - end
	pr_debug("%s: enable = %d\n", __func__, enable);
	pinfo = &(ctrl_pdata->panel_data.panel_info);

	if (enable) {
		rc = gpio_request(ctrl_pdata->rst_gpio, "disp_rst_n");
		if (rc) {
			pr_err("display reset gpio request failed\n");
			gpio_free(ctrl_pdata->rst_gpio);
			return rc;
		}
//raymond test remove disp_dc_gpio pin - begin
#if 0
		rc = gpio_request(ctrl_pdata->disp_dc_gpio, "disp_dc");
		if (rc) {
			pr_err("display dc gpio request failed\n");
			return rc;
		}
#endif
//raymond test remove disp_dc_gpio pin - end
		if (!pinfo->cont_splash_enabled) {
			for (i = 0; i < pdata->panel_info.rst_seq_len; ++i) {
				gpio_direction_output((ctrl_pdata->rst_gpio),
					pdata->panel_info.rst_seq[i]);
				if (pdata->panel_info.rst_seq[++i])
					usleep_range(pinfo->rst_seq[i] * 1000,
					pinfo->rst_seq[i] * 1000);
			}
		}

		if (ctrl_pdata->ctrl_state & CTRL_STATE_PANEL_INIT) {
			pr_debug("%s: Panel Not properly turned OFF\n",
						__func__);
			ctrl_pdata->ctrl_state &= ~CTRL_STATE_PANEL_INIT;
			pr_err("%s: Reset panel done\n", __func__);
		}
	} else {
	
		//[4101][Jialong]Modify Reset pin pull low when suspend 20190812 Start
		gpio_direction_output((ctrl_pdata->rst_gpio), 0);
		gpio_free(ctrl_pdata->rst_gpio);
		//[4101][Jialong]Modify Reset pin pull low when suspend 20190812 End

//raymond test remove disp_dc_gpio pin - begin
#if 0
		gpio_direction_output(ctrl_pdata->disp_dc_gpio, 0);
		gpio_free(ctrl_pdata->disp_dc_gpio);
#endif
//raymond test remove disp_dc_gpio pin - end
	}
#if debug_panel
	pr_err("%s --\n",__func__);
#endif
	return rc;
}


static int mdss_spi_panel_pinctrl_set_state(
	struct spi_panel_data *ctrl_pdata,
	bool active)
{
	struct pinctrl_state *pin_state;
	int rc = -EFAULT;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (IS_ERR_OR_NULL(ctrl_pdata->pin_res.pinctrl))
		return PTR_ERR(ctrl_pdata->pin_res.pinctrl);

	pin_state = active ? ctrl_pdata->pin_res.gpio_state_active
				: ctrl_pdata->pin_res.gpio_state_suspend;
	if (!IS_ERR_OR_NULL(pin_state)) {
		rc = pinctrl_select_state(ctrl_pdata->pin_res.pinctrl,
				pin_state);
		if (rc)
			pr_err("%s: can not set %s pins\n", __func__,
			       active ? MDSS_PINCTRL_STATE_DEFAULT
			       : MDSS_PINCTRL_STATE_SLEEP);
	} else {
		pr_err("%s: invalid '%s' pinstate\n", __func__,
		       active ? MDSS_PINCTRL_STATE_DEFAULT
		       : MDSS_PINCTRL_STATE_SLEEP);
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return rc;
}


static int mdss_spi_panel_pinctrl_init(struct platform_device *pdev)
{
	struct spi_panel_data *ctrl_pdata;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	ctrl_pdata = platform_get_drvdata(pdev);
	ctrl_pdata->pin_res.pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR_OR_NULL(ctrl_pdata->pin_res.pinctrl)) {
		pr_err("%s: failed to get pinctrl\n", __func__);
		return PTR_ERR(ctrl_pdata->pin_res.pinctrl);
	}

	ctrl_pdata->pin_res.gpio_state_active
		= pinctrl_lookup_state(ctrl_pdata->pin_res.pinctrl,
				MDSS_PINCTRL_STATE_DEFAULT);
	if (IS_ERR_OR_NULL(ctrl_pdata->pin_res.gpio_state_active))
		pr_warn("%s: can not get default pinstate\n", __func__);

	ctrl_pdata->pin_res.gpio_state_suspend
		= pinctrl_lookup_state(ctrl_pdata->pin_res.pinctrl,
				MDSS_PINCTRL_STATE_SLEEP);
	if (IS_ERR_OR_NULL(ctrl_pdata->pin_res.gpio_state_suspend))
		pr_warn("%s: can not get sleep pinstate\n", __func__);
pr_err("%s --\n",__func__);
	return 0;
}


static int mdss_spi_panel_power_on(struct mdss_panel_data *pdata)
{
	int ret = 0;
	int rc=0;
	struct spi_panel_data *ctrl_pdata = NULL;

pr_err("%s ++\n",__func__);
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);
	ret = msm_dss_enable_vreg(
		ctrl_pdata->panel_power_data.vreg_config,
		ctrl_pdata->panel_power_data.num_vreg, 1);
	if (ret) {
		pr_err("%s: failed to enable vregs for %s\n",
			__func__, "PANEL_PM");
	}
//raymond test remove disp_dc_gpio pin - begin
//[4101][Raymond] decrease power consumption in suspend - begin
#if 1
	if (!gpio_is_valid(ctrl_pdata->vcc_en_gpio)) {
		pr_err("%s:%d, dc line not configured\n",
			   __func__, __LINE__);
		return rc;
	}

	gpio_direction_output((ctrl_pdata->vcc_en_gpio),1);
pr_err("%s:%d, pull high vcc en gpio_65 \n",__func__, __LINE__);

	if (!gpio_is_valid(ctrl_pdata->boost_en_gpio)) {
		pr_err("%s:%d, SHE...!\n",__func__, __LINE__);
		return rc;
	}
	gpio_direction_output((ctrl_pdata->boost_en_gpio),1);
pr_err("%s:%d, pull high boost_pwr gpio_76 \n",__func__, __LINE__);
	if (!gpio_is_valid(ctrl_pdata->ls_en_gpio)) {
		pr_err("%s:%d, SHE...!\n",__func__, __LINE__);
		return rc;
	}
//mdelay(100);
	gpio_direction_output((ctrl_pdata->ls_en_gpio),1);
pr_err("%s:%d, pull high level_shift_en gpio_99 \n",__func__, __LINE__);
       //mdelay(2000);

#endif
//[4101][Raymond] decrease power consumption in suspend - end
//raymond test remove disp_dc_gpio pin - end
	/*
	 * If continuous splash screen feature is enabled, then we need to
	 * request all the GPIOs that have already been configured in the
	 * bootloader. This needs to be done irresepective of whether
	 * the lp11_init flag is set or not.
	 */
	if (pdata->panel_info.cont_splash_enabled) {
		if (mdss_spi_panel_pinctrl_set_state(ctrl_pdata, true))
			pr_debug("reset enable: pinctrl not enabled\n");

		//[Jialong] no set mdelay(10), because MDSS_EVENT_LINK_READY had set reset pin high 
		ret = mdss_spi_panel_reset(pdata, 1);
		if (ret)
			pr_err("%s: Panel reset failed. rc=%d\n",
					__func__, ret);
	}
pr_err("%s --\n",__func__);
	return ret;
}


static int mdss_spi_panel_power_off(struct mdss_panel_data *pdata)
{
	int ret = 0;
	int rc=0;
	struct spi_panel_data *ctrl_pdata = NULL;
pr_err("%s ++\n",__func__);
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		ret = -EINVAL;
		goto end;
	}
	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	//[4101][Jialong]Add delay 10ms after reset pin set low 20190812 Start
	ret = mdss_spi_panel_reset(pdata, 0);
	mdelay(10);
	//[4101][Jialong]Add delay 10ms after reset pin set low 20190812 End
	if (ret) {
		pr_warn("%s: Panel reset failed. rc=%d\n", __func__, ret);
		ret = 0;
	}

	if (mdss_spi_panel_pinctrl_set_state(ctrl_pdata, false))
		pr_warn("reset disable: pinctrl not enabled\n");

	ret = msm_dss_enable_vreg(
		ctrl_pdata->panel_power_data.vreg_config,
		ctrl_pdata->panel_power_data.num_vreg, 0);
	if (ret)
		pr_err("%s: failed to disable vregs for %s\n",
			__func__, "PANEL_PM");

	//raymond test remove disp_dc_gpio pin - begin
//[4101][Raymond] decrease power consumption in suspend - begin
#if 1
//[4101][Jialong]modify power off sequence, VDD turn off at last 20190812 Start
	if (!gpio_is_valid(ctrl_pdata->boost_en_gpio)) {
		pr_err("%s:%d, SHE...!\n",__func__, __LINE__);
		return rc;
	}
	gpio_direction_output((ctrl_pdata->boost_en_gpio),0);

	if (!gpio_is_valid(ctrl_pdata->ls_en_gpio)) {
		pr_err("%s:%d, SHE...!\n",__func__, __LINE__);
		return rc;
	}
	gpio_direction_output((ctrl_pdata->ls_en_gpio),0);

	if (!gpio_is_valid(ctrl_pdata->vcc_en_gpio)) {
		pr_err("%s:%d, dc line not configured\n",
			   __func__, __LINE__);
		return rc;
	}

	gpio_direction_output((ctrl_pdata->vcc_en_gpio),0);

    //[4101][Jialong]modify power off sequence, VDD turn off at last 20190812 End

#endif
//[4101][Raymond] decrease power consumption in suspend - end
//raymond test remove disp_dc_gpio pin - end

end:
	pr_err("%s --\n",__func__);
	return ret;
}


static int mdss_spi_panel_power_ctrl(struct mdss_panel_data *pdata,
	int power_state)
{
	int ret;
	struct mdss_panel_info *pinfo;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	pinfo = &pdata->panel_info;
	pr_debug("%s: cur_power_state=%d req_power_state=%d\n", __func__,
		pinfo->panel_power_state, power_state);

	if (pinfo->panel_power_state == power_state) {
		pr_debug("%s: no change needed\n", __func__);
		return 0;
	}

	switch (power_state) {
	case MDSS_PANEL_POWER_OFF:
		ret = mdss_spi_panel_power_off(pdata);
		break;
	case MDSS_PANEL_POWER_ON:
		ret = mdss_spi_panel_power_on(pdata);
		break;
	default:
		pr_err("%s: unknown panel power state requested (%d)\n",
			__func__, power_state);
		ret = -EINVAL;
	}

	if (!ret)
		pinfo->panel_power_state = power_state;
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return ret;
}

static int mdss_spi_panel_unblank(struct mdss_panel_data *pdata)
{
	int ret = 0;
	struct spi_panel_data *ctrl_pdata = NULL;
//pr_err("%s ++\n",__func__);
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	if (!(ctrl_pdata->ctrl_state & CTRL_STATE_PANEL_INIT)) {
			ret = ctrl_pdata->on(pdata);
			if (ret) {
				pr_err("%s: unable to initialize the panel\n",
							__func__);
				return ret;
		}
		ctrl_pdata->ctrl_state |= CTRL_STATE_PANEL_INIT;
	}
//pr_err("%s --\n",__func__);
	return ret;
}

static int mdss_spi_panel_blank(struct mdss_panel_data *pdata, int power_state)
{
	int ret = 0;
	struct spi_panel_data *ctrl_pdata = NULL;
//pr_err("%s ++\n",__func__);
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	if (ctrl_pdata->ctrl_state & CTRL_STATE_PANEL_INIT) {
			ret = ctrl_pdata->off(pdata);
			if (ret) {
				pr_err("%s: Panel OFF failed\n", __func__);
				return ret;
			}
		ctrl_pdata->ctrl_state &= ~CTRL_STATE_PANEL_INIT;
	}
//pr_err("%s --\n",__func__);
	return ret;
}


static int mdss_spi_panel_event_handler(struct mdss_panel_data *pdata,
				  int event, void *arg)
{
	int rc = 0;
	struct spi_panel_data *ctrl_pdata = NULL;
	int power_state;
//pr_err("%s ++\n",__func__);
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}
	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	switch (event) {
	case MDSS_EVENT_LINK_READY:
		rc = mdss_spi_panel_power_ctrl(pdata, MDSS_PANEL_POWER_ON);
		if (rc) {
			pr_err("%s:Panel power on failed. rc=%d\n",
							__func__, rc);
			return rc;
		}
		mdss_spi_panel_pinctrl_set_state(ctrl_pdata, true);
		//[4101][Jialong]Add delay 10ms after reset pin set high 20190812 Start
		mdelay(10);
		mdss_spi_panel_reset(pdata, 1);
		//[4101][Jialong]Add delay 10ms after reset pin set high 20190812 End
		break;
	case MDSS_EVENT_UNBLANK:
		rc = mdss_spi_panel_unblank(pdata);
		break;
	case MDSS_EVENT_PANEL_ON:
		ctrl_pdata->ctrl_state |= CTRL_STATE_MDP_ACTIVE;
		break;
	case MDSS_EVENT_BLANK:
		power_state = (int) (unsigned long) arg;
		break;
	case MDSS_EVENT_PANEL_OFF:
		power_state = (int) (unsigned long) arg;
		ctrl_pdata->ctrl_state &= ~CTRL_STATE_MDP_ACTIVE;
		rc = mdss_spi_panel_blank(pdata, power_state);
		rc = mdss_spi_panel_power_ctrl(pdata, power_state);
		break;
	default:
		pr_debug("%s: unhandled event=%d\n", __func__, event);
		break;
	}
	pr_debug("%s-:event=%d, rc=%d\n", __func__, event, rc);
	return rc;
}

int is_spi_panel_continuous_splash_on(struct mdss_panel_data *pdata)
{
	int i = 0, voltage = 0;
	struct dss_vreg *vreg;
	int num_vreg;
	struct spi_panel_data *ctrl_pdata = NULL;
pr_err("%s ++\n",__func__);
	ctrl_pdata = container_of(pdata, struct spi_panel_data,
			panel_data);
	vreg = ctrl_pdata->panel_power_data.vreg_config;
	num_vreg = ctrl_pdata->panel_power_data.num_vreg;

	for (i = 0; i < num_vreg; i++) {
		if (regulator_is_enabled(vreg[i].vreg) <= 0)
			return false;
		voltage = regulator_get_voltage(vreg[i].vreg);
		if (!(voltage >= vreg[i].min_voltage &&
			 voltage <= vreg[i].max_voltage))
			return false;
	}
pr_err("%s --\n",__func__);
	return true;
}

static void enable_spi_panel_te_irq(struct spi_panel_data *ctrl_pdata,
							bool enable)
{
	static bool is_enabled = true;
//pr_err("%s ++\n",__func__);
	if (is_enabled == enable)
		return;

	if (!gpio_is_valid(ctrl_pdata->disp_te_gpio)) {
		pr_err("%s:%d,SPI panel TE GPIO not configured\n",
			   __func__, __LINE__);
		return;
	}
#if 0
	if (enable)
		enable_irq(gpio_to_irq(ctrl_pdata->disp_te_gpio));
	else
		disable_irq(gpio_to_irq(ctrl_pdata->disp_te_gpio));
#endif
	is_enabled = enable;
	//pr_err("%s --\n",__func__);
}

//char EPD_fb[36002]={ 0 };
//char EPD_fb_2bit[72002]={ 0 };
char EPD_fb_4bit[TOTAL_PIXEL_ARY_CNT+2]={ 0 };//2018/10/08,Yuchen- [4101] 144002 -> TOTAL_PIXEL_ARY_CNT+2
//<2018/10/08,Yuchen-[4101]partial update
char EPD_fb_4bit_partial[TOTAL_PIXEL_ARY_CNT+2]={ 0 }; 
//#if DYN_PARTIAL_UPDATE
char EPD_fb_4bit_pre[TOTAL_PIXEL_ARY_CNT + 3]={ 0x00 };
bool has_pre_frame=false;
//#endif

static bool bRemoteContentInconsistant = true;
//>2018/10/08,Yuchen
bool already_probe=false;//2018/10/23,Yuchen
     //char parm_R11h[]={0x11, 0x00};
char parm_b5[]={0x12, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
char parm_dtmw_full[]={0x83, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
char parm_dtmw_partial[]={0x83, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
char pixel1[1]={ 0 };
char pixel2[1]={ 0 };
char PON_REG[]={0x04}; //PON
char POF_REG[]={02}; //POF

#if 0
static void epd_disable_irq(struct mdss_panel_data *pdata)
{
	struct spi_panel_data *ctrl = NULL;
	struct spi_panel_data *ctrl_pdata = (struct spi_panel_data *)pdata;
	unsigned long flags;
pr_err("%s [Raymond] ++  \n",__func__);
	ctrl = container_of(pdata, struct spi_panel_data,
				panel_data);
	
	spin_lock_irqsave(&ctrl_pdata->irq_enabled_lock, flags);
	if (&ctrl_pdata->irq_enabled) {
		disable_irq_nosync(ctrl_pdata->irq);
		ctrl->irq_enabled = false;
	}
	spin_unlock_irqrestore(&ctrl_pdata->irq_enabled_lock, flags);
pr_err("%s [Raymond] --  \n",__func__);	
}
#endif
#if 1
irqreturn_t epd_irq_handler(int irq, void *data)
{
	struct spi_panel_data *ctrl_pdata = (struct spi_panel_data *)data;
	//int rc =0;
	
	//pr_err("%s ++  \n",__func__);
	if (!ctrl_pdata) {
		pr_err("%s: SPI display not available\n", __func__);
		return IRQ_HANDLED;
	}
	
	if(ctrl_pdata->irq_enable){
		//pr_err("%s disable_irq_nosync  \n",__func__);
		disable_irq_nosync(ctrl_pdata->irq);
		ctrl_pdata->irq_enable = false;
		}
	//rc= gpio_get_value(ctrl_pdata->disp_busy_gpio);
	wake_up(&ctrl_pdata->busy_wq);
	//pr_err("%s -- \n",__func__);
	return IRQ_HANDLED;
}
#endif

int mdss_spi_panel_kickoff(struct mdss_panel_data *pdata,
			char *buf, int len, int dma_stride, bool bDelay)
{
	struct spi_panel_data *ctrl_pdata = NULL;
	char *tx_buf;
	int rc = 0;
	int panel_yres;
	int panel_xres;
	int padding_length = 0;
	int actual_stride = 0;
	int byte_per_pixel = 0;
	int scan_count = 0;
	int i,j;
	//int busy_n;
	//bool bNeedUpdate = os_mode?false:true;
	bool bNeedUpdate = true;
	int bflash_times=1;
	//<2018/10/08,Yuchen-[4101]partial update

//#if DYN_PARTIAL_UPDATE
	char parm_b5_partial[]={0x12, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
	char temp_coordinate;
	int x1,x2,y1,y2;
	int irq_gpio_value=0;
	int ret;

//#endif

	//>2018/10/08,Yuchen


//	static int count = 0;
//pr_err("[B]%s(%d): len=%d\n", __func__, __LINE__, len);
//if(frame_drop==5){
if(bDelay && bflash>=0 && panel_suspend==0){
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

//[4101][Raymond]three seconds Power Off command Implementation - begin
//pr_err("%s [Raymond] bflash:%d  \n",__func__,bflash);
cancel_delayed_work_sync(&epd_pof_work);
//[4101][Raymond]three seconds Power Off command Implementation - end
if(os_mode==1){
bflash_times=bflash;
//bflash=0;
}
	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	tx_buf = ctrl_pdata->tx_buf;
	panel_xres = ctrl_pdata->panel_data.panel_info.xres;
	panel_yres = ctrl_pdata->panel_data.panel_info.yres;
//pr_err("[B]%s(%d): panel_xres=%d, panel_yres=%d, ctrl_pdata->panel_data.panel_info.bpp=%d\n", __func__, __LINE__, panel_xres, panel_yres, ctrl_pdata->panel_data.panel_info.bpp);

#if 1
	byte_per_pixel = 4;
#else
	byte_per_pixel = ctrl_pdata->panel_data.panel_info.bpp / 8;
#endif
	actual_stride = panel_xres * byte_per_pixel;
	padding_length = dma_stride - actual_stride;

//<2018/10/23,Yuchen-[4101] workaround. lcm initial fail issue.
#if HOT_FIX_LCM_INI_FAIL
	if(already_probe){
		already_probe=false;
		mutex_lock(&ctrl_pdata->spi_tx_mutex);

		pr_err("========= do power off  =========\n");
		rc = mdss_spi_panel_power_off(pdata);
		if (rc) {
			pr_err("unable to off power. rc:[%d]\n", rc );
		}

		pr_err("========= power on again =========\n");
		rc = mdss_spi_panel_power_on(pdata);
		if (rc) {
			pr_err(" panel power on fail. rc:[%d]\n", rc );
		}
		rc = mdss_spi_panel_pinctrl_set_state(ctrl_pdata, true);
		if (rc){
			pr_err(" panel pinctrl fail. rc:[%d]\n", rc );
		}
		rc = mdss_spi_panel_reset(pdata, 1);
		if (rc)
			pr_err(" Panel reset failed. rc:[%d]\n", rc);

		rc = ctrl_pdata->on(pdata);
		if (rc) {
			pr_err(" panel on fail. rc:[%d]\n", rc );
		}else{
			ctrl_pdata->ctrl_state |= CTRL_STATE_PANEL_INIT;
		}

		mutex_unlock(&ctrl_pdata->spi_tx_mutex);
	}
#endif
//>2018/10/23,Yuchen

//pr_err("[B]%s(%d): byte_per_pixel=%d, actual_stride=%d, padding_length=%d\n", __func__, __LINE__, byte_per_pixel, actual_stride, padding_length);
	/* remove the padding and copy to continuous buffer */
if(os_mode==0){
	while (scan_count < panel_yres) {
		if(!bNeedUpdate){
			char * pX;
			char * pB;
			const char * pBound = tx_buf + scan_count * actual_stride + actual_stride;
			for(pX=tx_buf + scan_count * actual_stride,
				pB=buf + scan_count * (actual_stride + padding_length)
				;
				pX<pBound
				;
				pX++,pB++){
					if((*pX)^(*pB)){
						bNeedUpdate = true;
						break;
					}
			}
		}
		if(bNeedUpdate)break;
		scan_count++;
	}

if(bRemoteContentInconsistant){
	bRemoteContentInconsistant = false;
	bNeedUpdate = true;
}
}
//pr_err("[Raymond] bNeedUpdate:%d \n",bNeedUpdate);
if(bNeedUpdate){
	if(os_mode){
		scan_count = 0;
		while (scan_count < panel_yres) {
			memcpy((tx_buf + scan_count * actual_stride),
				(buf + scan_count * (actual_stride + padding_length)),
					actual_stride);

			scan_count++;
		}
	}


	//enable_spi_panel_te_irq(ctrl_pdata, true);

	mutex_lock(&ctrl_pdata->spi_tx_mutex);
	//reinit_completion(&ctrl_pdata->spi_panel_te);

	//rc = wait_for_completion_timeout(&ctrl_pdata->spi_panel_te,
				   //msecs_to_jiffies(SPI_PANEL_TE_TIMEOUT));

	//if (rc == 0)
		//pr_err("wait panel TE time out\n");


if(EPD_POF==1){
	mdss_spi_tx_parameter(PON_REG,sizeof(PON_REG)); //PON	
	//raymond implement begin
		irq_gpio_value =gpio_get_value(ctrl_pdata->disp_busy_gpio);
		if(irq_gpio_value==0){
			while(1){
				ret = 0;
				
				if(!ctrl_pdata->irq_enable){
				ctrl_pdata->irq_enable=true;
				enable_irq(ctrl_pdata->irq);
				}
				
				if(!gpio_get_value(ctrl_pdata->disp_busy_gpio)){
					ret = wait_event_interruptible(ctrl_pdata->busy_wq, !ctrl_pdata->irq_enable);
				}

				if(ctrl_pdata->irq_enable){
					disable_irq_nosync(ctrl_pdata->irq);
					ctrl_pdata->irq_enable=false;
				}

				if(gpio_get_value(ctrl_pdata->disp_busy_gpio))
				break;
			}
		}
		//raymond implement end
	EPD_POF=0;
}
#if 1
#if 1 //capture RGB value to 4 bit and swap
//[4101]Fine tune performance begin
#if 1
{
    char *temp_tx_buf_DataPointW;
    char *temp_tx_buf_DataPointH1;
    char *temp_tx_buf_DataPointH2;
    char *temp_EPD_fb_4bit_DataPointW;
    char *temp_EPD_fb_4bit_DataPointH;
    int width = 0;
    int height = 0;
    char temp_2Pixels;
    char temp_Pixel1;
    char temp_Pixel2;
    bool temp_width_changed = false;

    j = j + 1;
    i = i + 1;

    //<2018/10/08,Yuchen-[4101]find region
    x1 = 600;//start width
    x2 = 0;//end width
    y1 = 480;//start height
    y2 = 0;//end height
    //>2018/10/08,Yuchen

    EPD_fb_4bit[0] = 0x10;
    EPD_fb_4bit[1] = 0x03;
    temp_tx_buf_DataPointW = tx_buf;
    temp_EPD_fb_4bit_DataPointW = &EPD_fb_4bit[143999 + 2];
    for(width = 598; width >= 0; width -= 2) // process 2 pixles in 1 loop
    {
      temp_tx_buf_DataPointH1 = temp_tx_buf_DataPointW;
      temp_tx_buf_DataPointH2 = temp_tx_buf_DataPointW + 1920;
      temp_EPD_fb_4bit_DataPointH = temp_EPD_fb_4bit_DataPointW;
      for(height = 479; height >= 0; height--)
      {
        // 2 pixels of each line
        temp_Pixel1 = ((temp_tx_buf_DataPointH1[0] * 300) + (temp_tx_buf_DataPointH1[1] * 600) + (temp_tx_buf_DataPointH1[2] * 124)) >> 10;
        temp_Pixel2 = ((temp_tx_buf_DataPointH2[0] * 300) + (temp_tx_buf_DataPointH2[1] * 600) + (temp_tx_buf_DataPointH2[2] * 124)) >> 10;
        //temp_Pixel1 = ((*(temp_tx_buf_DataPointH1) << 8) + (*(temp_tx_buf_DataPointH1 + 1) << 9) + (*(temp_tx_buf_DataPointH1 + 2) << 7)) >> 10;
        //temp_Pixel2 = ((*(temp_tx_buf_DataPointH2) << 8) + (*(temp_tx_buf_DataPointH2 + 1) << 9) + (*(temp_tx_buf_DataPointH2 + 2) << 7)) >> 10;

        // combine 2 pixels into 1 byte
        temp_2Pixels = (temp_Pixel2 & 0xF0) | ((temp_Pixel1 & 0xF0) >> 4);

        // check dirty update
        if(DYN_PARTIAL_UPDATE == 1)
        {
          if(temp_EPD_fb_4bit_DataPointH[0] != temp_2Pixels)
          {
            temp_EPD_fb_4bit_DataPointH[0] = temp_2Pixels;
            temp_width_changed = true;
            //--update region---
            if( height < y1) y1 = height;
            if( height > y2) y2 = height;
          }
        }
	else{
		if(temp_EPD_fb_4bit_DataPointH[0] != temp_2Pixels)
           	 temp_EPD_fb_4bit_DataPointH[0] = temp_2Pixels;
		}
        temp_tx_buf_DataPointH1 += 4;
        temp_tx_buf_DataPointH2 += 4;
        temp_EPD_fb_4bit_DataPointH -= 300;
      }
      if(temp_width_changed)
      {
        if( width < x1) x1 = width; //find smallest point
        if( width > x2) x2 = width; //find largest point
        temp_width_changed = false;
      }
      temp_tx_buf_DataPointW += 3840;//1920;
      temp_EPD_fb_4bit_DataPointW--;
    }
	if(os_mode==0){	
		mdss_spi_tx_parameter(EPD_fb_4bit, TOTAL_PIXEL_ARY_CNT + 2); //2018/10/08,Yuchen- [4101] 144002 -> TOTAL_PIXEL_ARY_CNT+2
	}
  }
#else
//[4101]Fine tune performance end

	{
		int k=0;
		int p=0;
		j=1;
		//<2018/10/08,Yuchen-[4101]find region

          //#if DYN_PARTIAL_UPDATE
		x1 = 600;//start width
		x2 = 0;//end width
		y1 = 480;//start height
		y2 = 0;//end height
          //#endif

		//>2018/10/08,Yuchen
		EPD_fb_4bit[0] = 0x10;
		EPD_fb_4bit[1] = 0x03;
		for(i=143999;i>=143700;i--){
			for(k=0;k<480;k++){
				pixel1[0]= ( (tx_buf[j+p-1]*300)+(tx_buf[j+p]*600)+(tx_buf[j+p+1]*124) )>>10 ;
				pixel2[0]= ( (tx_buf[j+p+1919]*300)+(tx_buf[j+p+1920]*600)+(tx_buf[j+p+1921]*124) )>>10 ;
				EPD_fb_4bit[i-(k*300)+2] =((pixel2[0]&0xF0)>>0) |  ((pixel1[0]&0xF0)>>4);
				//EPD_fb_4bit[i*300+k+2] =((pixel1[0]&0xF0)>>4) |  ((pixel2[0]&0xF0)>>0);
				//EPD_fb_4bit[i*300+k+2] = ((( (tx_buf[j-1]*300)+(tx_buf[j]*600)+(tx_buf[j+1]*124)>>10)>>4) >>4)  | ((( (tx_buf[j+3]*300)+(tx_buf[j+4]*600)+(tx_buf[j+5]*124)>>10)>>4) >>0) ;
				//EPD_fb_4bit[i*300+k+2] =((tx_buf[j]&0xF0)>>4) |  ((tx_buf[j+4]&0xF0)>>0);
                           //pr_err("[%s][%d]\n",__func__,(i-(k*300)+2));
				//<2018/10/08,Yuchen-[4101]count pixel and find region
			if(DYN_PARTIAL_UPDATE==1){
                          //#if DYN_PARTIAL_UPDATE
				if(has_pre_frame){
					if(EPD_fb_4bit[i-(k*300)+2]!=EPD_fb_4bit_pre[i-(k*300)+2]){
			//--update region---
			if( (i-143700)*2 < x1)x1=(i-143700)*2;//find smallest point
			if( (i-143700)*2 > x2)x2=(i-143700)*2;//find largest point
			if( (479-k) < y1)y1=(479-k);
			if( (479-k) > y2)y2=(479-k);
					}
				}
                          //#endif
				}
				//>2018/10/08,Yuchen
				j=j+4;
			}
			p=p+1920;
		}
		//<2018/10/08,Yuchen-[4101]drop frame. all white / all black / the same picture
	if(DYN_PARTIAL_UPDATE==1){
          //#if DYN_PARTIAL_UPDATE
		if(!has_pre_frame){
			has_pre_frame=true;
		}
		memcpy(EPD_fb_4bit_pre,EPD_fb_4bit,sizeof(EPD_fb_4bit));
          //#endif
		}
		//>2018/10/08,Yuchen
		mdss_spi_tx_parameter(EPD_fb_4bit,TOTAL_PIXEL_ARY_CNT+2);//2018/10/08,Yuchen- [4101] 144002 -> TOTAL_PIXEL_ARY_CNT+2
	}
#endif
#else //capture RGB value to 4 bit
	{
		j=1;
		EPD_fb_4bit[0] = 0x10;
		EPD_fb_4bit[1] = 0x03;
		for(i=0;i<TOTAL_PIXEL_ARY_CNT;i++){//2018/10/08,Yuchen- [4101] 144000 -> TOTAL_PIXEL_ARY_CNT
			pixel1[0]= ( (tx_buf[j-1]*300)+(tx_buf[j]*600)+(tx_buf[j+1]*124) )>>10 ;
			pixel2[0]= ( (tx_buf[j+3]*300)+(tx_buf[j+4]*600)+(tx_buf[j+5]*124) )>>10 ;
			EPD_fb_4bit[i+2] =((pixel1[0]&0xF0)>>0) |  ((pixel2[0]&0xF0)>>4);
		//	EPD_fb[i+2] =(!!(tx_buf[j]&0xF0)<<0) |  (!!(tx_buf[j+4]&0xF0)<<1)  |  (!!(tx_buf[j+8]&0xF0)<<2) |  (!!(tx_buf[j+12]&0xF0)<<3) | (!!(tx_buf[j+16]&0xF0)<<4)  |  (!!(tx_buf[j+20]&0xF0)<<5) |  (!!(tx_buf[j+24]&0xF0)<<6) |  (!!(tx_buf[j+28]&0xF0)<<7);
			j=j+8;
		}
		mdss_spi_tx_parameter(EPD_fb_4bit,TOTAL_PIXEL_ARY_CNT+2);//2018/10/08,Yuchen- [4101] 144002 -> TOTAL_PIXEL_ARY_CNT+2
	}

#endif
#endif

  //<2018/10/08,Yuchen-[4101]partial update
  if(DYN_PARTIAL_UPDATE==1){
  //#if DYN_PARTIAL_UPDATE
	x1 = (x1-EXT_UPDATE_PIXEL)<0   ? 0   : (x1-EXT_UPDATE_PIXEL);
	x2 = (x2+EXT_UPDATE_PIXEL)>600 ? 600 : (x2+EXT_UPDATE_PIXEL);
	y1 = (y1-EXT_UPDATE_PIXEL)<0   ? 0   : (y1-EXT_UPDATE_PIXEL);
	y2 = (y2+EXT_UPDATE_PIXEL)>480 ? 480 : (y2+EXT_UPDATE_PIXEL);

	if( (x1 < x2) && (y1 < y2) ){
		#if debug_panel
		  pr_err("[YC] partial update x1[%d]- x2[%d], y1[%d]-y2[%d]\n", x1, x2, y1, y2);
		#endif
		temp_coordinate = (x1 & 0xffff)>>8;
		parm_b5_partial[2]=temp_coordinate;
		temp_coordinate = (x1 & 0xff);
		parm_b5_partial[3]=temp_coordinate;//set X

		temp_coordinate = (y1 & 0xffff)>>8;
		parm_b5_partial[4]=temp_coordinate;
		temp_coordinate = (y1 & 0xff);
		parm_b5_partial[5]=temp_coordinate;//set Y

		temp_coordinate = ((x2-x1+2) & 0xffff)>>8;
		parm_b5_partial[6]=temp_coordinate;
		temp_coordinate = ((x2-x1+2) & 0xff);
		parm_b5_partial[7]=temp_coordinate;//set W

		temp_coordinate = ((y2-y1+1) & 0xffff)>>8;
		parm_b5_partial[8]=temp_coordinate;
		temp_coordinate = ((y2-y1+1) & 0xff);
		parm_b5_partial[9]=temp_coordinate;//set L

		#if debug_panel
		  pr_err("[YC] X[0x%x 0x%x]- Y[0x%x 0x%x]\n", parm_b5_partial[2], parm_b5_partial[3], parm_b5_partial[4], parm_b5_partial[5]);
		  pr_err("[YC] W[0x%x 0x%x]- L[0x%x 0x%x]\n", parm_b5_partial[6], parm_b5_partial[7], parm_b5_partial[8], parm_b5_partial[9]);
		#endif
		mdss_spi_tx_parameter(parm_b5_partial,sizeof(parm_b5_partial));

	}else{
		mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5));
		//pr_err("[Raymond] Send DRF done \n");
	}
  	}
  else{
  //#else
  //[4101][Raymond] impelment uevent feature - begin
  #if 0
	if(os_mode==1 && gdev_done==1){
		kobject_uevent_env(&g_dev->kobj,
			KOBJ_CHANGE, envp2); // send uevent busy=1
		pr_err("%s[Raymond] send epd busy=1 uevent \n",__func__);
	}
   #endif	
//[4101][Raymond] impelment uevent feature - end	
        //mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5)); //raymond test
  //#endif
  	}
  //>2018/10/08,Yuchen

	//rc = mdss_spi_tx_pixel(tx_buf, ctrl_pdata->byte_pre_frame);
//2018/10/08,Yuchen mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5));
//rc = mdss_spi_tx_pixel(EPD_fb, 36002);
//mdss_spi_tx_parameter(parm25,sizeof(parm25));
	mutex_unlock(&ctrl_pdata->spi_tx_mutex);
#if 1
if(os_mode==0){
	
//raymond implement begin
		//pr_err("%s check busy pin - begin \n",__func__);
		irq_gpio_value =gpio_get_value(ctrl_pdata->disp_busy_gpio);
		if(irq_gpio_value==0){
			while(1){
				ret = 0;
				
				if(!ctrl_pdata->irq_enable){
				ctrl_pdata->irq_enable=true;
				enable_irq(ctrl_pdata->irq);
				}
				
				if(!gpio_get_value(ctrl_pdata->disp_busy_gpio)){
					ret = wait_event_interruptible(ctrl_pdata->busy_wq, !ctrl_pdata->irq_enable);
				}

				if(ctrl_pdata->irq_enable){
					disable_irq_nosync(ctrl_pdata->irq);
					ctrl_pdata->irq_enable=false;
				}

				if(gpio_get_value(ctrl_pdata->disp_busy_gpio))
				break;
			}
		}
		//raymond implement end

}
#endif
done_flash=1;
}

//pr_err("%s --,done_flash=1 \n",__func__);
	return rc;
}
else{  //drop

return rc;
}
}

//[4101][Raymond]Implement EPD update - begin
int update_epd(struct mdss_panel_data *pdata,int flash_time)
{
	//int busy_n;
	int i;
	int rc=0;
	struct spi_panel_data *ctrl = NULL;
	struct mdss_panel_info *pinfo;
	int irq_gpio_value=0;
	int ret;
	int j,k;


	//pr_err("%s ++,flash_time:%d \n",__func__,flash_time);
	//pr_err("%s [Raymond] bflash:%d  \n",__func__,bflash);
	cancel_delayed_work_sync(&epd_pof_work);
	//[4101][Raymond]three seconds Power Off command Implementation - end

	pinfo = &pdata->panel_info;
	ctrl = container_of(pdata, struct spi_panel_data,
				panel_data);
	//pr_err("%s ++ 1,flash_time:%d \n",__func__,flash_time);
   	EPD_fb_4bit[0] = 0x10;
   	EPD_fb_4bit[1] = 0x03;
	EPD_fb_4bit_partial[0]=0x10;
	EPD_fb_4bit_partial[1]=0x03;

   	kobject_uevent_env(&g_dev->kobj,
			KOBJ_CHANGE, envp2); // send uevent busy=1
	pr_err("%s send epd busy=1 uevent \n",__func__);

	
	if(EPD_POF==1){
	mdss_spi_tx_parameter(PON_REG,sizeof(PON_REG)); //PON
	//raymond implement begin
		//pr_err("%s check busy pin - begin \n",__func__);
		irq_gpio_value =gpio_get_value(ctrl->disp_busy_gpio);
		if(irq_gpio_value==0){
			while(1){
				ret = 0;
				
				if(!ctrl->irq_enable){
				ctrl->irq_enable=true;
				enable_irq(ctrl->irq);
				}
				
				if(!gpio_get_value(ctrl->disp_busy_gpio)){
					ret = wait_event_interruptible(ctrl->busy_wq, !ctrl->irq_enable);
				}

				if(ctrl->irq_enable){
					disable_irq_nosync(ctrl->irq);
					ctrl->irq_enable=false;
				}

				if(gpio_get_value(ctrl->disp_busy_gpio))
				break;
			}
		}
		//pr_err("%s check busy pin - end \n",__func__);
		//raymond implement end
	EPD_POF=0;
	}
	mutex_lock(&ctrl->spi_tx_mutex);
	//pr_err("%s write pixel to register ++ \n",__func__);

	if(pu_en==0){  //full update
		//pr_err("%s full write pixel to register \n",__func__);
		mdss_spi_tx_parameter(parm_dtmw_full,sizeof(parm_dtmw_full)); //DTMW
   		mdss_spi_tx_parameter(EPD_fb_4bit, TOTAL_PIXEL_ARY_CNT + 2); //write pixel to register
   		mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5)); //DRF
		}
	else{ //partial update
		//pr_err("%s partial write pixel to register \n",__func__);

		mdss_spi_tx_parameter(parm_dtmw_partial,sizeof(parm_dtmw_partial)); //DTMW
		for(j=0;j<pu_w;j++){
			for(k=0;k<(pu_l/2);k++){
				EPD_fb_4bit_partial[(((j*pu_l)/2)+k)+2] = EPD_fb_4bit[( ((600-(pu_y+pu_l))/2) + (((480-pu_x-pu_w)*600)/2) +k) + (300*j) ];
			}
		}
   		mdss_spi_tx_parameter(EPD_fb_4bit_partial, (((pu_w*pu_l)/2) + 2)); //write pixel to register
		mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5)); //DRF
		}
	//pr_err("%s write pixel to register -- \n",__func__);
	mutex_unlock(&ctrl->spi_tx_mutex);
	
	//raymond implement begin
	irq_gpio_value =gpio_get_value(ctrl->disp_busy_gpio);
	if(irq_gpio_value==0){
		while(1){
			ret = 0;
			if(!ctrl->irq_enable){
				ctrl->irq_enable=true;
				//pr_err("%s enable_irq \n",__func__);
				enable_irq(ctrl->irq);
			}
			if(!gpio_get_value(ctrl->disp_busy_gpio)){
				ret = wait_event_interruptible(ctrl->busy_wq, !ctrl->irq_enable);
			}

			if(ctrl->irq_enable){
				disable_irq_nosync(ctrl->irq);
				ctrl->irq_enable=false;
			}

			if(gpio_get_value(ctrl->disp_busy_gpio))
				break;
		}
	}
	//raymond implement end

	if(flash_time>1){ //need update twice or more
		for(i=1; i<flash_time;i++){
		mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5)); //DRF

		//raymond implement begin
		irq_gpio_value =gpio_get_value(ctrl->disp_busy_gpio);
		if(irq_gpio_value==0){
			while(1){
				ret = 0;
				
				if(!ctrl->irq_enable){
				ctrl->irq_enable=true;
				enable_irq(ctrl->irq);
				}
				
				if(!gpio_get_value(ctrl->disp_busy_gpio)){
					//busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
					ret = wait_event_interruptible(ctrl->busy_wq, !ctrl->irq_enable);
				}

				if(ctrl->irq_enable){
					disable_irq_nosync(ctrl->irq);
					ctrl->irq_enable=false;
				}

				if(gpio_get_value(ctrl->disp_busy_gpio))
				break;
			}
		}
		//raymond implement end
		}
	}

	bflash=0;
	schedule_delayed_work(&epd_pof_work,
			msecs_to_jiffies(3000));

	kobject_uevent_env(&g_dev->kobj,
		KOBJ_CHANGE, envp); // send uevent busy=0
	pr_err("%s send epd busy=0 uevent \n",__func__);

	//pr_err("%s --\n",__func__);
	return rc;
   
}
//[4101][Raymond]Implement EPD update - end

//<2019/03/20,Yuchen-[4101]deal with recovery UI issue
int mdss_spi_panel_kickoff_recovery(struct mdss_panel_data *pdata,
			char *buf, int len, int dma_stride, bool bDelay)
{
	struct spi_panel_data *ctrl_pdata = NULL;
	char *tx_buf;
	int rc = 0;
	int panel_yres;
	int panel_xres;
	int padding_length = 0;
	int actual_stride = 0;
	int byte_per_pixel = 0;
	int scan_count = 0;
	int busy_n;
	bool bNeedUpdate = false;
	char xor_chr;//for different color device
	//<2018/10/08,Yuchen-[4101]partial update
	char parm_b5_partial[]={0x12, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
	char temp_coordinate;
	int x1,x2,y1,y2;
	//>2018/10/08,Yuchen

	if(!rcy_mode){//not to handle if not in recovery mode
		pr_err("%s: [YC] not in recovery mode.\n", __func__);//temp
		return -EINVAL;
	}

	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,panel_data);
	tx_buf = ctrl_pdata->tx_buf;
	panel_xres = ctrl_pdata->panel_data.panel_info.xres;
	panel_yres = ctrl_pdata->panel_data.panel_info.yres;
	byte_per_pixel = 2;//recovery FB- RGB565 only 2 bits
	actual_stride = panel_xres * byte_per_pixel;
	padding_length = dma_stride - actual_stride;

	pr_err("[YC]: panel_xres=%d, panel_yres=%d, panel_info.bpp=%d\n",
               panel_xres, panel_yres, ctrl_pdata->panel_data.panel_info.bpp);
	pr_err("[YC]: actual_stride=%d, padding_length=%d\n",
               actual_stride, padding_length);
	/* drop the same FB */
	while (scan_count < panel_yres) {
		if(!bNeedUpdate){
			char * pX;
			char * pB;
			const char * pBound = tx_buf + scan_count * actual_stride + actual_stride;
			for(pX=tx_buf + scan_count * actual_stride,
				pB=buf + scan_count * (actual_stride + padding_length)
				;
				pX<pBound
				;
				pX++,pB++){
					if((*pX)^(*pB)){
						bNeedUpdate = true;
						break;
					}
			}
		}
		if(bNeedUpdate)break;
		scan_count++;
	}

	if(bNeedUpdate){
		////if update EPD less than 3s then cancel POF timer
		//cancel_delayed_work_sync(&epd_pof_work);

		//copy new FB pixel
		scan_count = 0;
		while (scan_count < panel_yres) {
			memcpy((tx_buf + scan_count * actual_stride),
				(buf + scan_count * (actual_stride + padding_length)),
					actual_stride);

			scan_count++;
		}

		mutex_lock(&ctrl_pdata->spi_tx_mutex);

		while(1){
			busy_n=check_busy_n_status(ctrl_pdata->disp_busy_gpio);
			if(busy_n==1) break;
		}
		//if(EPD_POF==1){//previous did not do POF so here no need PON
			mdss_spi_tx_parameter(PON_REG,sizeof(PON_REG)); //PON
			msleep(80);
			while(1){
				busy_n=check_busy_n_status(ctrl_pdata->disp_busy_gpio);
				if(busy_n==1) break;
			}
			//EPD_POF=0;
		//}

		//[4101]Fine tune performance begin
		{
			char *temp_tx_buf_DataPointW;
			char *temp_tx_buf_DataPointH1;
			char *temp_tx_buf_DataPointH2;
			char *temp_EPD_fb_4bit_DataPointW;
			char *temp_EPD_fb_4bit_DataPointH;
			int width = 0;
			int height = 0;
			char temp_2Pixels;
			char temp_Pixel1;
			char temp_Pixel2;
			bool temp_width_changed = false;

			//<2018/10/08,Yuchen-[4101]find region
			x1 = 600;//start width
			x2 = 0;//end width
			y1 = 480;//start height
			y2 = 0;//end height
			//>2018/10/08,Yuchen

			if(color_id==2){ //white phone
				xor_chr=0xFF;//inverse
			}else{
				xor_chr=0x00;
			}

			EPD_fb_4bit[0] = 0x10;
			EPD_fb_4bit[1] = 0x03;
			temp_tx_buf_DataPointW = tx_buf;
			temp_EPD_fb_4bit_DataPointW = &EPD_fb_4bit[143999 + 2];
			for(width = 598; width >= 0; width -= 2){ // process 2 pixles in 1 loop
				temp_tx_buf_DataPointH1 = temp_tx_buf_DataPointW;
				temp_tx_buf_DataPointH2 = temp_tx_buf_DataPointW + actual_stride;//yuchen (1920 -> actual_stride)
				temp_EPD_fb_4bit_DataPointH = temp_EPD_fb_4bit_DataPointW;
				for(height = 479; height >= 0; height--){
					// 2 pixels of each line
					//RGB565
					temp_Pixel1 = ((temp_tx_buf_DataPointH1[0] & 0xF0));//catch red 4bit only
					temp_Pixel2 = ((temp_tx_buf_DataPointH2[0] & 0xF0));//catch red 4bit only
					temp_2Pixels = ( temp_Pixel2 | (temp_Pixel1 >> 4)) ^ xor_chr;//try inverse
					// check dirty update
					if(temp_EPD_fb_4bit_DataPointH[0] != temp_2Pixels){
						temp_EPD_fb_4bit_DataPointH[0] = temp_2Pixels;
						temp_width_changed = true;
						//--update region---
						if( height < y1) y1 = height;
						if( height > y2) y2 = height;
					}
					temp_tx_buf_DataPointH1 += byte_per_pixel;//yuchen (4 -> byte_per_pixel)
					temp_tx_buf_DataPointH2 += byte_per_pixel;//yuchen (4 -> byte_per_pixel)
					temp_EPD_fb_4bit_DataPointH -= 300;
				}

				if(temp_width_changed){
					if( width < x1) x1 = width; //find smallest point
					if( width > x2) x2 = width; //find largest point
					temp_width_changed = false;
				}
				temp_tx_buf_DataPointW += (actual_stride*2);//yuchen (every two stride)
				temp_EPD_fb_4bit_DataPointW--;
			}
			mdss_spi_tx_parameter(EPD_fb_4bit, TOTAL_PIXEL_ARY_CNT + 2);
		}//Fine tune End

		//<2018/10/08,Yuchen-[4101]partial update
		x1 = (x1-EXT_UPDATE_PIXEL)<0   ? 0   : (x1-EXT_UPDATE_PIXEL);
		x2 = (x2+EXT_UPDATE_PIXEL)>600 ? 600 : (x2+EXT_UPDATE_PIXEL);
		y1 = (y1-EXT_UPDATE_PIXEL)<0   ? 0   : (y1-EXT_UPDATE_PIXEL);
		y2 = (y2+EXT_UPDATE_PIXEL)>480 ? 480 : (y2+EXT_UPDATE_PIXEL);

		if( (x1 < x2) && (y1 < y2) ){
			#if debug_panel
				pr_err("[YC] partial update x1[%d]- x2[%d], y1[%d]-y2[%d]\n", x1, x2, y1, y2);
			#endif
			temp_coordinate = (x1 & 0xffff)>>8;
			parm_b5_partial[2]=temp_coordinate;
			temp_coordinate = (x1 & 0xff);
			parm_b5_partial[3]=temp_coordinate;//set X

			temp_coordinate = (y1 & 0xffff)>>8;
			parm_b5_partial[4]=temp_coordinate;
			temp_coordinate = (y1 & 0xff);
			parm_b5_partial[5]=temp_coordinate;//set Y

			temp_coordinate = ((x2-x1+2) & 0xffff)>>8;
			parm_b5_partial[6]=temp_coordinate;
			temp_coordinate = ((x2-x1+2) & 0xff);
			parm_b5_partial[7]=temp_coordinate;//set W

			temp_coordinate = ((y2-y1+1) & 0xffff)>>8;
			parm_b5_partial[8]=temp_coordinate;
			temp_coordinate = ((y2-y1+1) & 0xff);
			parm_b5_partial[9]=temp_coordinate;//set L

			#if debug_panel
				pr_err("[YC] X[0x%x 0x%x]- Y[0x%x 0x%x]\n", parm_b5_partial[2], parm_b5_partial[3], parm_b5_partial[4], parm_b5_partial[5]);
				pr_err("[YC] W[0x%x 0x%x]- L[0x%x 0x%x]\n", parm_b5_partial[6], parm_b5_partial[7], parm_b5_partial[8], parm_b5_partial[9]);
			#endif
			mdss_spi_tx_parameter(parm_b5_partial,sizeof(parm_b5_partial));
		}else{
			mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5));
		}
		//>2018/10/08,Yuchen

		mutex_unlock(&ctrl_pdata->spi_tx_mutex);
		msleep(480);
		while(1){
			busy_n=check_busy_n_status(ctrl_pdata->disp_busy_gpio);
			if(busy_n==1) break;
		}
		//schedule_delayed_work(&epd_pof_work,msecs_to_jiffies(3000));//[Raymond]three seconds Power Off command Implementation
		mdss_spi_tx_parameter(POF_REG,sizeof(POF_REG)); //POF
	}//if bNeedUpdate

	return rc;
}
//>2019/03/20,Yuchen

static int mdss_spi_read_panel_data(struct mdss_panel_data *pdata,
		u8 reg_addr, u8 *data, u8 len)
{
	int rc = 0;
	struct spi_panel_data *ctrl_pdata = NULL;
pr_err("%s ++\n",__func__);
	ctrl_pdata = container_of(pdata, struct spi_panel_data,
		panel_data);

	mutex_lock(&ctrl_pdata->spi_tx_mutex);
	//gpio_direction_output(ctrl_pdata->disp_dc_gpio, 0);
	rc = mdss_spi_read_data(reg_addr, data, len);
	//gpio_direction_output(ctrl_pdata->disp_dc_gpio, 1);
	mutex_unlock(&ctrl_pdata->spi_tx_mutex);
pr_err("%s --\n",__func__);
	return rc;
}

//raymond test b
	char parm1[]={0x01, 0x03 ,0x04 ,0x00 ,0x00};
	char parm2[]={0x00, 0x25, 0x00};
	char parm3[]={0x26, 0x82};
	char parm4[]={0x03, 0x03};
	char parm5[]={0x07, 0xEF, 0xEF, 0x28};
	char parm6[]={0xE0, 0x02};
	char parm7[]={0x30, 0x0E};
	char parm8[]={0x41, 0x00};
	char parm9[]={0x50, 0x01, 0x22};
	char parm10[]={0x60, 0x3F, 0x09, 0x2D};
	char parm11[]={0x61, 0x02, 0x60, 0x01, 0xE0};
	char parm12[]={0xE0, 0x02};
	//char parm13[]={0x82, 0x4A};//vcom -3.7v
	//ar parm13[]={0x82, 0x48};//vcom -3.6v
	char parm13[]={0x82, 0x31};//vcom -2.45
	char para_vcom[2]={0x82, 0x31}; //vcom -2.45
	//char parm13[]={0x80, 0x51};//AMV
	//char parm13[]={0x82, 0x14};//vcom
//Upload_Temperature_LUT b
       //char parm14[]={0x40};
       //LUTC
       char LUTC[65];
      char parm15[65]={0x20,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00};
	   //LUTD
	   char LUTD[16386];
       char parm16[16386]={0x21,0x2B, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
0x20, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x02, 0x02, 0x00, 0x00, 0x02,
0x20, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20,
0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00, 0x20,
0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
0x20, 0x00, 0x00, 0x00, 0x80, 0x20, 0x20, 0x22, 0x20, 0x00, 0x20, 0x02, 0x02, 0x00, 0x02, 0x0A,
0x20, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20,
0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x02, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00, 0x20,
0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x02,
0x20, 0x00, 0x00, 0x00, 0x80, 0x20, 0x20, 0x22, 0x20, 0x00, 0x20, 0x02, 0x82, 0x00, 0xA2, 0x0A,
0x22, 0x00, 0x20, 0x0A, 0x8A, 0x00, 0x20, 0x00, 0x28, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x20,
0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x22, 0x20, 0x20, 0x20, 0x20, 0x02, 0x20, 0x00, 0x00, 0x20,
0x00, 0x08, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x88, 0x00, 0x00, 0x02,
0x28, 0x20, 0x00, 0x00, 0x80, 0x20, 0x20, 0x22, 0xA0, 0x00, 0x22, 0x0A, 0xA2, 0x02, 0xA2, 0x2A,
0xA2, 0x00, 0x20, 0x0A, 0xAA, 0x00, 0x20, 0x22, 0x28, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x20,
0x20, 0x00, 0x22, 0x20, 0x20, 0x28, 0x22, 0x20, 0x20, 0x28, 0x22, 0x02, 0xA0, 0x00, 0x00, 0x20,
0x00, 0x08, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x2A, 0x20, 0x00, 0x02, 0x88, 0x00, 0x00, 0x02,
0x28, 0x20, 0x00, 0x00, 0x80, 0x22, 0x22, 0x22, 0xA0, 0x00, 0x22, 0x2A, 0xA2, 0x2A, 0xA2, 0x2A,
0xA2, 0x00, 0x28, 0x0A, 0xAA, 0x00, 0x22, 0x2A, 0x28, 0x00, 0x08, 0x20, 0x28, 0x00, 0x20, 0x22,
0x28, 0x00, 0x22, 0x22, 0x28, 0x28, 0x22, 0x22, 0x28, 0x28, 0x22, 0x02, 0xA8, 0x08, 0x00, 0x20,
0x00, 0x08, 0x28, 0x20, 0x08, 0x00, 0x00, 0x00, 0x2A, 0x20, 0x00, 0x02, 0xA8, 0x00, 0x00, 0x02,
0xA8, 0x22, 0x00, 0x00, 0x80, 0x22, 0x22, 0x22, 0xA0, 0x00, 0x22, 0x2A, 0xAA, 0x2A, 0xA2, 0x2A,
0xA2, 0x00, 0x28, 0x0A, 0xAA, 0x00, 0x2A, 0x2A, 0xA8, 0x00, 0x08, 0x22, 0xA8, 0x00, 0x2A, 0x22,
0xA8, 0x00, 0x2A, 0x22, 0x28, 0x28, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x22, 0xA8, 0x08, 0x20, 0x22,
0x20, 0x08, 0x28, 0x20, 0x08, 0x00, 0x20, 0x00, 0x2A, 0x20, 0x00, 0x0A, 0xA8, 0x00, 0x00, 0x0A,
0xA8, 0x22, 0x00, 0x00, 0x80, 0x22, 0x22, 0x2A, 0xA0, 0x00, 0x22, 0x2A, 0xAA, 0x2A, 0xA2, 0xAA,
0xA2, 0x00, 0x28, 0x0A, 0xAA, 0x00, 0x2A, 0x2A, 0xA8, 0x00, 0x28, 0x2A, 0xA8, 0x00, 0x2A, 0x22,
0xA8, 0x00, 0x2A, 0x2A, 0x28, 0x28, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x2A, 0xA8, 0x08, 0x22, 0x22,
0x20, 0x08, 0x28, 0x22, 0x28, 0x00, 0x20, 0x08, 0x2A, 0x20, 0x20, 0x0A, 0xA8, 0x00, 0x20, 0x0A,
0xA8, 0x22, 0x88, 0x08, 0x80, 0x22, 0x22, 0xAA, 0xA0, 0x00, 0xA2, 0x2A, 0xAA, 0x2A, 0xA2, 0xAA,
0xA2, 0x08, 0x2A, 0x0A, 0xAA, 0x08, 0x2A, 0x2A, 0xA8, 0x00, 0x28, 0x2A, 0xA8, 0x00, 0x2A, 0x22,
0xA8, 0x00, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x22,
0x2A, 0x28, 0x2A, 0x22, 0x28, 0x00, 0x20, 0x88, 0x29, 0x20, 0x20, 0x0A, 0xAA, 0x00, 0x20, 0x0A,
0xA8, 0x22, 0x88, 0x28, 0x80, 0x2A, 0xA2, 0xAA, 0xA8, 0x00, 0xA2, 0x2A, 0xAA, 0x2A, 0xAA, 0xAA,
0xAA, 0x08, 0x2A, 0x2A, 0xAA, 0x08, 0x2A, 0x2A, 0xA8, 0x00, 0x2A, 0x2A, 0xAA, 0x00, 0x2A, 0x2A,
0xA8, 0x00, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x2A, 0xA8, 0x28, 0x2A, 0x22,
0xAA, 0x28, 0x2A, 0x22, 0xAA, 0x08, 0x28, 0x88, 0x29, 0x20, 0x20, 0x0A, 0xAA, 0x20, 0x20, 0x8A,
0xAA, 0x22, 0x88, 0xAA, 0x80, 0x2A, 0xA2, 0xAA, 0xA8, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA,
0xAA, 0x08, 0xAA, 0xAA, 0xAA, 0x08, 0x2A, 0x2A, 0xA8, 0x20, 0x2A, 0x2A, 0xAA, 0x20, 0x2A, 0x2A,
0xAA, 0x00, 0x2A, 0x2A, 0xA8, 0x2A, 0x2A, 0x2A, 0xA8, 0x2A, 0x2A, 0xAA, 0xA8, 0x28, 0x2A, 0x22,
0xAA, 0x28, 0x2A, 0x22, 0xAA, 0x28, 0x28, 0xA8, 0x29, 0x28, 0x20, 0x0A, 0xAA, 0x28, 0x20, 0x8A,
0xAA, 0x22, 0x88, 0xAA, 0x80, 0x2A, 0xAA, 0xAA, 0xA8, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA,
0xAA, 0x08, 0xAA, 0xAA, 0xAA, 0x28, 0xAA, 0xAA, 0xA8, 0x28, 0x2A, 0x2A, 0xAA, 0x28, 0x2A, 0x2A,
0xAA, 0x00, 0xAA, 0x2A, 0xA8, 0x2A, 0xAA, 0x2A, 0xA8, 0x2A, 0xAA, 0xAA, 0xA8, 0x28, 0x2A, 0x22,
0xAA, 0x28, 0x2A, 0x22, 0xAA, 0x28, 0x28, 0xA8, 0x29, 0x28, 0xA0, 0x8A, 0xA5, 0x28, 0xA0, 0x8A,
0x96, 0x22, 0x88, 0xAA, 0x88, 0x2A, 0xAA, 0xAA, 0xA8, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA,
0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0x28, 0xAA, 0xAA, 0xA8, 0x28, 0x2A, 0xAA, 0xAA, 0x28, 0x2A, 0xAA,
0xAA, 0x00, 0xAA, 0x2A, 0xAA, 0x2A, 0xAA, 0x2A, 0xAA, 0x2A, 0xAA, 0xAA, 0xA8, 0x28, 0x2A, 0x22,
0xAA, 0x28, 0x2A, 0x22, 0xAA, 0x28, 0xA8, 0xAA, 0x25, 0x28, 0xA0, 0x8A, 0xA5, 0x28, 0xA8, 0x89,
0x95, 0x21, 0x68, 0xAA, 0x08, 0x2A, 0xAA, 0xAA, 0xA8, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA,
0xA9, 0x28, 0xAA, 0xAA, 0xA9, 0x28, 0xAA, 0xAA, 0xA8, 0x2A, 0x2A, 0xAA, 0xAA, 0x28, 0x2A, 0xAA,
0xAA, 0x20, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA, 0xAA, 0x28, 0x2A, 0xAA,
0xAA, 0x24, 0x2A, 0x2A, 0xA6, 0x28, 0xA8, 0xAA, 0x95, 0x28, 0xA0, 0xA9, 0x95, 0x28, 0xA8, 0xA9,
0x95, 0x11, 0x68, 0xAA, 0x68, 0x2A, 0xAA, 0xAA, 0xA8, 0x28, 0xAA, 0xAA, 0xA9, 0x2A, 0xAA, 0xAA,
0xA9, 0x28, 0xAA, 0xAA, 0xA9, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0x2A, 0xAA, 0xAA, 0x28, 0x2A, 0xAA,
0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA, 0xAA, 0x2A, 0xAA, 0xAA, 0xAA, 0x28, 0x2A, 0x8A,
0xA9, 0x24, 0x2A, 0x1A, 0xA5, 0x28, 0xAA, 0xAA, 0x95, 0x18, 0xA8, 0xA9, 0x55, 0x28, 0xA8, 0xA9,
0x95, 0x11, 0x64, 0x66, 0x68, 0x2A, 0xAA, 0xAA, 0xA8, 0x2A, 0xAA, 0xAA, 0xA9, 0x2A, 0xAA, 0xAA,
0xA9, 0x2A, 0xAA, 0xAA, 0xA9, 0x28, 0xAA, 0xAA, 0xAA, 0x2A, 0x2A, 0xAA, 0xAA, 0x28, 0xAA, 0x9A,
0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0x1A, 0xAA, 0xAA, 0xAA, 0x1A, 0xAA, 0xAA, 0xAA, 0x28, 0x2A, 0x8A,
0xA9, 0x26, 0x2A, 0x1A, 0xA5, 0x28, 0xAA, 0x2A, 0x95, 0x18, 0xA8, 0xA5, 0x55, 0x28, 0xA8, 0x25,
0x95, 0x11, 0x64, 0x55, 0x54, 0x2A, 0xAA, 0xAA, 0xA8, 0x2A, 0xAA, 0xAA, 0xA9, 0x2A, 0xAA, 0xAA,
0xA9, 0x2A, 0xAA, 0xAA, 0xA9, 0x2A, 0xAA, 0xAA, 0xAA, 0x2A, 0x2A, 0xAA, 0x9A, 0x28, 0xAA, 0x9A,
0xAA, 0x28, 0xAA, 0x9A, 0x9A, 0x1A, 0xAA, 0x9A, 0x9A, 0x1A, 0xAA, 0xAA, 0x9A, 0x26, 0x2A, 0x9A,
0xA9, 0x2A, 0x26, 0x9A, 0xA5, 0x28, 0x9A, 0x66, 0x95, 0x18, 0x98, 0x25, 0x55, 0x28, 0x9A, 0x65,
0x55, 0x11, 0x64, 0x55, 0x54, 0x2A, 0xAA, 0xAA, 0xA8, 0x2A, 0xAA, 0xAA, 0xA9, 0x2A, 0xAA, 0xAA,
0x89, 0x2A, 0xAA, 0xAA, 0xA5, 0x2A, 0xAA, 0xAA, 0x96, 0x2A, 0xAA, 0x9A, 0x95, 0x28, 0xAA, 0x9A,
0x95, 0x28, 0x8A, 0x9A, 0x96, 0x2A, 0xA9, 0x9A, 0x96, 0x2A, 0xAA, 0xAA, 0x96, 0x26, 0x2A, 0x9A,
0x95, 0x2A, 0x16, 0x9A, 0x95, 0x26, 0x96, 0x56, 0x95, 0x14, 0x5A, 0x65, 0x55, 0x14, 0x5A, 0x65,
0x55, 0x11, 0x66, 0x55, 0x54, 0x1A, 0xAA, 0xAA, 0x98, 0x2A, 0xAA, 0xAA, 0xA9, 0x2A, 0xAA, 0xAA,
0x99, 0x2A, 0xAA, 0xAA, 0xA5, 0x2A, 0xAA, 0x9A, 0x96, 0x2A, 0xA6, 0x9A, 0x95, 0x28, 0xAA, 0x9A,
0x95, 0x28, 0x99, 0x9A, 0x96, 0x2A, 0xA9, 0x9A, 0x96, 0x2A, 0x99, 0xA9, 0x96, 0x26, 0x25, 0x99,
0x95, 0x2A, 0x15, 0x99, 0x95, 0x16, 0x56, 0x56, 0x95, 0x16, 0x5A, 0x55, 0x55, 0x16, 0x56, 0x55,
0x55, 0x19, 0x56, 0x55, 0x54, 0x1A, 0xAA, 0x9A, 0x98, 0x2A, 0xAA, 0xA9, 0xA9, 0x1A, 0x68, 0xA5,
0x99, 0x2A, 0xAA, 0xA4, 0xA5, 0x2A, 0xAA, 0x98, 0x96, 0x26, 0xA6, 0x98, 0x95, 0x24, 0xA5, 0x99,
0x95, 0x28, 0x95, 0x99, 0x95, 0x06, 0x95, 0x95, 0x95, 0x06, 0x95, 0x99, 0x96, 0x26, 0x15, 0x99,
0x95, 0x16, 0x15, 0x99, 0x95, 0x16, 0x55, 0x55, 0x95, 0x16, 0x56, 0x55, 0x55, 0x16, 0x56, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x2A, 0x8A, 0x98, 0x98, 0x2A, 0x8A, 0xA9, 0xA9, 0x19, 0x69, 0x95,
0x99, 0x2A, 0x96, 0xA5, 0x95, 0x2A, 0x95, 0x95, 0x96, 0x26, 0xA6, 0x95, 0x95, 0x26, 0x95, 0x99,
0x95, 0x2A, 0x95, 0x95, 0x95, 0x06, 0x95, 0x95, 0x95, 0x06, 0x95, 0x55, 0x55, 0x16, 0x95, 0x59,
0x95, 0x16, 0x15, 0x99, 0x55, 0x15, 0x55, 0x55, 0x95, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x2A, 0x9A, 0x99, 0x98, 0x2A, 0x99, 0x95, 0x95, 0x19, 0x59, 0x55,
0x99, 0x26, 0x96, 0xA5, 0x55, 0x26, 0x95, 0x95, 0x96, 0x26, 0x95, 0x55, 0x55, 0x26, 0x95, 0x55,
0x55, 0x2A, 0x55, 0x95, 0x95, 0x15, 0x55, 0x95, 0x55, 0x15, 0x55, 0x55, 0x55, 0x16, 0x95, 0x59,
0x55, 0x16, 0x95, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x09, 0x99, 0x59, 0x98, 0x2A, 0x59, 0x95, 0x55, 0x25, 0x59, 0x55,
0x55, 0x26, 0x55, 0x55, 0x55, 0x26, 0x55, 0x55, 0x55, 0x06, 0x95, 0x55, 0x55, 0x06, 0x95, 0x55,
0x55, 0x26, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x16, 0x95, 0x55,
0x55, 0x15, 0x95, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x15, 0x59, 0x51, 0x54, 0x26, 0x59, 0x55, 0x55, 0x25, 0x55, 0x55,
0x54, 0x26, 0x55, 0x55, 0x55, 0x26, 0x55, 0x55, 0x55, 0x15, 0x95, 0x55, 0x55, 0x16, 0x55, 0x55,
0x55, 0x26, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x95, 0x55,
0x55, 0x15, 0x95, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x15, 0x59, 0x55, 0x54, 0x26, 0x55, 0x55, 0x55, 0x25, 0x55, 0x55,
0x54, 0x26, 0x55, 0x55, 0x55, 0x16, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x16, 0x55, 0x55,
0x55, 0x16, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x55, 0x95, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55, 0x56, 0x16, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x16, 0x55, 0x55,
0x55, 0x16, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x55, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55, 0x55, 0x95, 0x55, 0x55,
0x55, 0x15, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x55, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55,
0x54, 0x55, 0x55, 0x55, 0x54, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x56, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55, 0x56, 0x55, 0x55, 0x55,
0x56, 0x55, 0x55, 0x55, 0x44, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x56, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55, 0x54, 0x15, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x5A, 0x55, 0x55, 0x55, 0x5A, 0x55, 0x55, 0x55, 0x5A, 0x55, 0x55, 0x55,
0x5A, 0x55, 0x55, 0x55, 0x44, 0x95, 0x55, 0x55, 0x46, 0x95, 0x55, 0x55, 0x56, 0x95, 0x55, 0x55,
0x56, 0x15, 0x55, 0x55, 0x5A, 0x15, 0x55, 0x55, 0x58, 0x15, 0x55, 0x55, 0x5A, 0x15, 0x55, 0x55,
0x5A, 0x15, 0x55, 0x55, 0x58, 0x15, 0x55, 0x55, 0x58, 0x15, 0x55, 0x55, 0x58, 0x95, 0x55, 0x55,
0x5A, 0x15, 0x55, 0x55, 0x5A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55,
0x6A, 0x55, 0x55, 0x55, 0x68, 0x95, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55, 0x5A, 0x95, 0x55, 0x55,
0x6A, 0x15, 0x55, 0x55, 0x5A, 0x15, 0x55, 0x55, 0x68, 0x19, 0x55, 0x55, 0x6A, 0x19, 0x55, 0x55,
0x6A, 0x15, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55, 0x68, 0x95, 0x55, 0x55,
0x6A, 0x95, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55,
0x6A, 0x55, 0x15, 0x55, 0x68, 0x95, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55,
0x6A, 0x95, 0x55, 0x55, 0x6A, 0x15, 0x55, 0x55, 0x6A, 0x19, 0x55, 0x55, 0x6A, 0x19, 0x55, 0x55,
0x6A, 0x15, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55,
0x6A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x55,
0x6A, 0x55, 0x15, 0x55, 0x68, 0x95, 0x55, 0x15, 0x6A, 0x95, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x51,
0x6A, 0x95, 0x55, 0x51, 0x6A, 0x95, 0x55, 0x65, 0x6A, 0x15, 0x59, 0x15, 0x6A, 0x15, 0x55, 0x05,
0x6A, 0x15, 0x55, 0x55, 0x6A, 0x95, 0x55, 0x51, 0x6A, 0x95, 0x55, 0x15, 0x6A, 0x55, 0x59, 0x15,
0x6A, 0x55, 0x59, 0x15, 0x2A, 0x55, 0x55, 0x55, 0x6A, 0x55, 0x55, 0x56, 0x2A, 0x55, 0x55, 0x56,
0x6A, 0x56, 0x15, 0x12, 0x68, 0x55, 0x95, 0x16, 0x2A, 0x55, 0x95, 0x12, 0x2A, 0x55, 0x95, 0x12,
0x2A, 0x95, 0x99, 0x12, 0x2A, 0x95, 0x99, 0x12, 0x2A, 0x15, 0x95, 0x12, 0x2A, 0x15, 0x99, 0x02,
0x2A, 0x19, 0x99, 0x12, 0x2A, 0x55, 0x99, 0x12, 0x2A, 0x55, 0x99, 0x12, 0x2A, 0x59, 0x95, 0x16,
0x2A, 0x55, 0x95, 0x12, 0x2A, 0x56, 0x54, 0x9A, 0x2A, 0x56, 0x54, 0x9A, 0xAA, 0x56, 0x55, 0x9A,
0xAA, 0x56, 0x10, 0x9A, 0xA8, 0x51, 0x51, 0x8A, 0xAA, 0x51, 0x51, 0x9A, 0xAA, 0x55, 0x51, 0x9A,
0xAA, 0x91, 0x55, 0xAA, 0xAA, 0x95, 0x55, 0x8A, 0xAA, 0x91, 0x51, 0x8A, 0xAA, 0x91, 0x54, 0x8A,
0xAA, 0x95, 0x54, 0x8A, 0xAA, 0x55, 0x54, 0x8A, 0xAA, 0x55, 0x54, 0x9A, 0xAA, 0x55, 0x50, 0x9A,
0xAA, 0x55, 0x50, 0x9A, 0xAA, 0x56, 0x08, 0x8A, 0xAA, 0x56, 0x08, 0x8A, 0xAA, 0x56, 0x08, 0x8A,
0xAA, 0x59, 0x18, 0x9A, 0xA8, 0x51, 0x1A, 0xAA, 0xAA, 0x52, 0x1A, 0xAA, 0xAA, 0x56, 0x1A, 0xAA,
0xAA, 0x52, 0x0A, 0x9A, 0xAA, 0x5A, 0x1A, 0x8A, 0xAA, 0x51, 0x1A, 0xAA, 0xAA, 0x52, 0x1A, 0xAA,
0xAA, 0x52, 0x1A, 0xAA, 0xAA, 0x56, 0x1A, 0xAA, 0xAA, 0x56, 0x1A, 0xAA, 0xAA, 0x51, 0x1A, 0xAA,
0xAA, 0x55, 0x0A, 0xAA, 0xAA, 0x55, 0x0A, 0xAA, 0xAA, 0x55, 0x0A, 0xAA, 0xAA, 0x55, 0x0A, 0xAA,
0xAA, 0x55, 0x0A, 0xAA, 0xA8, 0x52, 0x2A, 0xAA, 0xAA, 0x52, 0x2A, 0xAA, 0xAA, 0x56, 0x2A, 0xAA,
0xAA, 0x52, 0x2A, 0xAA, 0xAA, 0x56, 0x2A, 0xAA, 0xAA, 0x52, 0x2A, 0xAA, 0xAA, 0x52, 0x2A, 0xAA,
0xAA, 0x52, 0x2A, 0xAA, 0xAA, 0x56, 0x2A, 0xAA, 0xAA, 0x56, 0x2A, 0xAA, 0xAA, 0x52, 0x2A, 0xAA,
0xAA, 0x56, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x41, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA,
0xAA, 0x50, 0x0A, 0xAA, 0xA8, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x55, 0x2A, 0xAA,
0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA,
0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA, 0xAA, 0x51, 0x2A, 0xAA,
0xAA, 0x59, 0x2A, 0xAA, 0xAA, 0x58, 0xAA, 0xAA, 0xAA, 0x48, 0xAA, 0xAA, 0xAA, 0x58, 0xAA, 0xAA,
0xAA, 0x58, 0xAA, 0xAA, 0xA8, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A,
0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A,
0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A, 0xA2, 0x5A, 0x82, 0x6A,
0xA2, 0x4A, 0x82, 0x6A, 0xA2, 0x6A, 0x82, 0x6A, 0xA2, 0x4A, 0x82, 0x6A, 0xA2, 0x6A, 0x82, 0x6A,
0xA2, 0x6A, 0xA2, 0x6A, 0xA0, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05,
0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05,
0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05,
0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05, 0x02, 0x42, 0x85, 0x05,
0x02, 0x42, 0x85, 0x05, 0x00, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15,
0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15,
0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15,
0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15, 0x54, 0x41, 0x10, 0x15,
0x54, 0x41, 0x10, 0x15, 0x54, 0x44, 0x60, 0x2A, 0x88, 0x45, 0x60, 0x2A, 0x88, 0x45, 0x60, 0x2A,
0x88, 0x45, 0x60, 0x2A, 0x88, 0x45, 0x60, 0x2A, 0x88, 0x44, 0x60, 0x2A, 0x88, 0x45, 0x60, 0x2A,
0x88, 0x45, 0x60, 0x2A, 0x88, 0x45, 0x60, 0x2A, 0x88, 0x45, 0x60, 0x2A, 0x88, 0x44, 0x60, 0x2A,
0x88, 0x44, 0x60, 0x2A, 0x88, 0x44, 0x60, 0x2A, 0x88, 0x44, 0x60, 0x2A, 0x88, 0x44, 0x60, 0x2A,
0x88, 0x44, 0x60, 0x2A, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
 };
//LUTR
char LUTR[257];
char parm16_1[257]={0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//Upload_Temperature_LUT e
//EPD_DTM2_Initial b
        char parm17[]={0x83, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
	//char parm18[36002] = { 0x00 }; //black
	//char parm18_1[36002] = { 0xFF }; //white
//EPD_DTM2_Initial e
	char parm19[]={0x04}; //PON
	//char parm20[]={02}; //POF

//EPD white b
     char parm21[]={0x83, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
     //char parm22[36002] = { 0x00 }; //black
     //char parm22_1[36002] ; //white
     char parm23[]={0xE0, 0x02};
     char parm24[]={0x04}; //PON
     char parm25[]={0x12, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};//DRF
     char parm26[]={02}; //POF
//EPD white e
//EPD black b
     char parm_b1[]={0x83, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0};
     char parm_b2[TOTAL_PIXEL_ARY_CNT+2] = { 0x00 }; //black //2018/10/08,Yuchen- [4101] 144002 -> TOTAL_PIXEL_ARY_CNT+2
     char parm_dtm2[TOTAL_PIXEL_ARY_CNT+2] = { 0x00 }; //DTM2
     char parm_b3[]={0xE0, 0x02};
     char parm_b4[]={0x04}; //PON
     //char parm_b5[]={0x12, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x02, 0xE0};
     char parm_b6[]={02}; //POF
//EPD black e
//raymond test e
//[4101][Raymond]Dynamic change waveform mode - brgin
static int change_waveform_mode(struct mdss_panel_data *pdata, unsigned int Mode)
{
	unsigned int t=0;
	int tsc_t;
	int busy_n,i;
	int ret =0;
	unsigned int offset = 0;
	unsigned int segment = 0;
	struct spi_panel_data *ctrl = NULL;
	struct mdss_panel_info *pinfo;
	//struct file *file;
	//const struct firmware *cfg = NULL;
	unsigned int frame;
	int LUTD_Count;

#if 1
pr_err("%s ++\n",__func__);
#endif
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}
	pinfo = &pdata->panel_info;
	ctrl = container_of(pdata, struct spi_panel_data,
				panel_data);

	mdss_spi_tx_parameter(PON_REG,sizeof(PON_REG)); //PON
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	if(busy_n==1)
		break;
	}

	//mutex_lock(&ctrl->spi_tx_mutex);
	ret=mdss_spi_read_epd_data(TSC,ctrl->act_status_value,2);
	//mutex_unlock(&ctrl->spi_tx_mutex);
	if(ret<0)
		pr_err("%s: Read TSC register returned error\n", __func__);

	for (i = 0; i < 2; i++) {
	pr_err("Read EPD TSC REG:0x40[%d] = 0x%x\n",i, ctrl->act_status_value[i]);
	}

	if(ctrl->act_status_value[1]>=128)
		t=((ctrl->act_status_value[0]*2)+1);
	else
		t=(ctrl->act_status_value[0]*2);
	pr_err("%s:1 Read TSC register t=%d \n", __func__,t);

	//pr_err("%s:1 Read TSC register temperature=%d \n", __func__,(int)(t/2));


	//t=(int) ((ctrl->act_status_value[0]&0xFF) | (( ctrl->act_status_value[1]&0x80) >>8) ) ;
	//pr_err("%s:2 Read TSC register t=%d \n", __func__,t);

      //segment value
      tsc_t=t/2;
	if (tsc_t<3)
		segment = 0;
	else if (tsc_t>=3&&tsc_t<6)
		segment = 1;
	else if (tsc_t>=6&&tsc_t<9)
		segment = 2;
	else if (tsc_t>=9&&tsc_t<12)
		segment = 3;
	else if (tsc_t>=12&&tsc_t<15)
		segment = 4;
	else if (tsc_t>=15&&tsc_t<18)
		segment = 5;
	else if (tsc_t>=18&&tsc_t<21)
		segment = 6;
	else if (tsc_t>=21&&tsc_t<24)
		segment = 7;
	else if (tsc_t>=24&&tsc_t<27)
		segment = 8;
	else if (tsc_t>=27&&tsc_t<30)
		segment = 9;
	else if (tsc_t>=30&&tsc_t<33)
		segment = 10;
	else if (tsc_t>=33&&tsc_t<38)
		segment = 11;
	else if (tsc_t>=38&&tsc_t<43)
		segment = 12;
	else if (tsc_t>=43&&tsc_t<50)
		segment = 13;
	else if (tsc_t>=50)
		segment = 13;

      if(ctrl->act_status_value[0]>=128) //Negative temperature
      	{
      	 pr_err("%s: Negative temperature \n", __func__);
      	  segment=0;
      	}
	pr_err("%s: temperature=%d, segment=%d \n", __func__,tsc_t,segment);

	switch(Mode){
		case INIT:
			offset = 16705*segment;
		break;
		case A2:
			offset = 16705*(14+segment);
		break;
		case DU:
			offset = 16705*(28+segment);
		break;
		case GL16:
			offset = 16705*(42+segment);
		break;
		case GC16:
			offset = 16705*(56+segment);
		break;
		default:
			offset = 16705*(42+segment);
		break;
	}
      pr_err("%s: Mode=%d, offset=%d \n", __func__,Mode,offset);
	if(first_request==1){ //only request once
	pr_err("%s  request config file %s\n",__func__,WF_FILE_NAME);
	ret = request_firmware(&wf_file_cfg,WF_FILE_NAME,ctrl->dev);
	if (ret < 0) { // if request fail, waveform mode is GL mode
		pr_err("%s Failure to request config file %s\n",
			__func__,WF_FILE_NAME);
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
	mdss_spi_tx_parameter(parm15,sizeof(parm15)); //LUTC
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
	mdss_spi_tx_parameter(parm16,sizeof(parm16)); //LUTD
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
	mdss_spi_tx_parameter(parm16_1,sizeof(parm16_1));//LUTR
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
	return ret;
	}
	}

pr_err("fw: %02X, %02X, %02X, %02X, %02X, %02X \n",
			wf_file_cfg->data[0],wf_file_cfg->data[1],wf_file_cfg->data[2],wf_file_cfg->data[3840],wf_file_cfg->data[3841],wf_file_cfg->data[3842]);

       //LUTD
       frame = wf_file_cfg->data[offset] +1;
	LUTD_Count = frame * 64;
	pr_err("frame=%d, LUTD_Count=%d \n",frame,LUTD_Count);
	LUTD[0]=0x21;
	pr_err("LUTD[0]=0x%X, ",LUTD[0]);
       for(i=0;i<=LUTD_Count;i++){
      		LUTD[i+1]= wf_file_cfg->data[i+offset];
			//if(i<100 || i>LUTD_Count-100)
			 //pr_err("LUTD[%d]=0x%X, ",i+1,LUTD[i+1]);
       }

//pr_err("LUTD[0]=0x%X , LUTD[1]=0x%X ,offset=%d \n",LUTD[0],LUTD[1],offset);
	  //LUTC
	  offset+=16385;
	  LUTC[0]=0x20;
	  pr_err("LUTC[0]=0x%X, ",LUTC[0]);
	  for(i=0;i<64;i++){
      		LUTC[i+1]= wf_file_cfg->data[i+offset];
			// pr_err("LUTC[%d]=0x%X, ",i+1,LUTC[i+1]);
       }
//pr_err("LUTC[0]=0x%X , LUTC[1]=0x%X,offset=%d \n",LUTC[0],LUTC[1],offset);
	  //LUTR
	  offset+=64;
	  LUTR[0]=0x22;
	  pr_err("LUTR[0]=0x%X, ",LUTR[0]);
	  for(i=0;i<256;i++){
      		LUTR[i+1]= wf_file_cfg->data[i+offset];
			 //pr_err("LUTR[%d]=0x%X, ",i+1,LUTR[i+1]);
       }
//pr_err("LUTR[0]=0x%X , LUTR[1]=0x%X, offset=%d \n",LUTR[0],LUTR[1],offset);
//mutex_lock(&ctrl->spi_tx_mutex);
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
	  mdss_spi_tx_parameter(LUTC,sizeof(LUTC)); //LUTC
	  //mdss_spi_tx_parameter(LUTC,65); //LUTC
	  //mdss_spi_tx_parameter(parm15,sizeof(parm15)); //LUTC
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}

	mdss_spi_tx_parameter(LUTD,sizeof(LUTD)); //LUTD
	//mdss_spi_tx_parameter(LUTD,LUTD_Count+1); //LUTD
	//mdss_spi_tx_parameter(parm16,sizeof(parm16)); //LUTD
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}

	mdss_spi_tx_parameter(LUTR,sizeof(LUTR)); //LUTR
	//mdss_spi_tx_parameter(LUTR,257); //LUTR
	//mdss_spi_tx_parameter(parm16_1,sizeof(parm16_1));//LUTR
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
//mutex_unlock(&ctrl->spi_tx_mutex);
	//release_firmware(cfg);
       first_request=0;
	#if 1
pr_err("%s --\n",__func__);
#endif
	return 0;
}
//[4101][Raymond]Dynamic change waveform mode - end

struct mdss_panel_data *g_pdata = NULL;

//[4101][Raymond]three seconds Power Off command Implementation - begin
static void epd_pof_cmd(struct work_struct *data)
{

	//pr_err("%s ++,POF\n",__func__);
	mdss_spi_tx_parameter(POF_REG,sizeof(POF_REG)); //POF
	EPD_POF=1;
	//pr_err("%s --\n",__func__);
}

//[4101][Raymond]three seconds Power Off command Implementation - end

static int mdss_spi_panel_on(struct mdss_panel_data *pdata)
{
	struct spi_panel_data *ctrl = NULL;
	struct mdss_panel_info *pinfo;
	int busy_n;
	int i;
	//[4101][Raymond]off charging icon - begin
  char xor_chr=0x00;//2019/02/23,Yuchen
	int h_space, w_space;//2019/02/23,Yuchen distance from life-top
	//[4101][Raymond]off charging icon - end
	//int ret=0;
#if 0
	parm18[0] = 0x13;
	parm18[1] = 0x00;
	parm18_1[0] = 0x13;
	parm18_1[1] = 0x00;
	parm22_1[0] = 0x10;
	parm22_1[1] = 0x00;
#endif
//[4101][Raymond]Fix screen abnormal in suspend - begin
//bflash=1;
panel_suspend=0;
//[4101][Raymond]Fix screen abnormal in suspend - begin
	parm_dtm2[0] = 0x13;
	parm_dtm2[1] = 0x03;
	bRemoteContentInconsistant = true;
	g_pdata = pdata;
#if 1
pr_err("%s ++\n",__func__);
#endif
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}
	pinfo = &pdata->panel_info;
	ctrl = container_of(pdata, struct spi_panel_data,
				panel_data);
//memset(parm22_1,0xFF,36002);
//memset(parm22_1,0xFF,18001); //half
//memset(parm22_1,0xFF,40002); //double
	//parm22_1[0] = 0x10;
	//parm22_1[1] = 0x00;
//pr_err("parm1 size:%d \n",sizeof(parm1));
//pr_err("parm15 size:%d \n",sizeof(parm15));
#if 1
mdss_spi_tx_parameter(parm1,sizeof(parm1));
mdss_spi_tx_parameter(parm2,sizeof(parm2));
mdss_spi_tx_parameter(parm3,sizeof(parm3));
mdss_spi_tx_parameter(parm4,sizeof(parm4));
mdss_spi_tx_parameter(parm5,sizeof(parm5));
mdss_spi_tx_parameter(parm6,sizeof(parm6));
mdss_spi_tx_parameter(parm7,sizeof(parm7));
mdss_spi_tx_parameter(parm8,sizeof(parm8));
mdss_spi_tx_parameter(parm9,sizeof(parm9));
mdss_spi_tx_parameter(parm10,sizeof(parm10));
mdss_spi_tx_parameter(parm11,sizeof(parm11));
mdss_spi_tx_parameter(parm12,sizeof(parm12));
//mdss_spi_tx_parameter(parm13,sizeof(parm13));
mdss_spi_tx_parameter(para_vcom,sizeof(para_vcom)); //write vcom
for(i=0;i<sizeof(para_vcom);i++)
{
pr_err("%s para_vcom[%d]=0x%X \n",__func__,i,para_vcom[i]);
}

//mdss_spi_tx_parameter(parm14,sizeof(parm14));
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}
#if 0
pr_err("%s ++ , param15 \n",__func__);
mdss_spi_tx_parameter(parm15,sizeof(parm15)); //LUTC
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}
mdss_spi_tx_parameter(parm16,sizeof(parm16)); //LUTD
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}
mdss_spi_tx_parameter(parm16_1,sizeof(parm16_1));//LUTR
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}
#else
pr_err("%s ++ , read bin \n",__func__);
if(chg_mode==0){ // normal mode
	change_waveform_mode(&ctrl->panel_data,wf_mode);
}else{ // off charging mode
	if(show_count==0){
	change_waveform_mode(&ctrl->panel_data,0);
	mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5));
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	//pr_err("pon busy_n status:%d \n", busy_n);
	if(busy_n==1)
	break;
	}
	}
	change_waveform_mode(&ctrl->panel_data,wf_mode);
}
//change_waveform_mode(g_pdata,wf_mode);
//change_waveform_mode(&ctrl->panel_data,3);
#endif
mdss_spi_tx_parameter(parm17,sizeof(parm17));

mdss_spi_tx_parameter(parm21,sizeof(parm21));

//[4101][Raymond] Fix LCM abnormal issue - begin
mdss_spi_tx_parameter(parm_dtm2,sizeof(parm_dtm2)); //DTM2 black
//[4101][Raymond] Fix LCM abnormal issue - end
mdss_spi_tx_parameter(parm23,sizeof(parm23));
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}
mdss_spi_tx_parameter(parm24,sizeof(parm24)); //PON
//mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
//mdelay(1000);
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}
#endif

//[4101][Raymond] impelment uevent feature - begin
if(os_mode==1 && gdev_done==1){
	kobject_uevent_env(&g_dev->kobj,
		KOBJ_CHANGE, power1); // send uevent busy=0
	pr_err("%s send power state =1 uevent \n",__func__);
}
//[4101][Raymond] impelment uevent feature - end


//[4101][Raymond]off charging icon - begin
if(chg_mode==1){ // off charging mode
  if( chg_mode_show==1){
		//<2019/02/23, Yuchen-[4101] use macro to insert pixel
		if(color_id==2){ //white phone
			memset(EPD_Macro_Insert_4bit,0xFF,144002);
			xor_chr=0x00;
		}else{//black phone
			memset(EPD_Macro_Insert_4bit,0x00,144002);
			xor_chr=0xff;//inverse
		}
		EPD_Macro_Insert_4bit[0] = 0x10;
		EPD_Macro_Insert_4bit[1] = 0x03;
		h_space=136;//initial position
		w_space=146;//initial position
		//[4101][Raymond]picture of the battery full for off-charging -begin
		if(battery_full==1){ //full chg
              Insert_pixel(xor_chr,white_full_chg,full_chg_WIDTH,full_chg_HEIGHT,h_space,w_space);
			}
		else{
		Insert_pixel(xor_chr,white_offchg_rgb,offchg_WIDTH,offchg_HEIGHT,h_space,w_space);
			}
		//[4101][Raymond]picture of the battery full for off-charging -end
		//>2019/02/23, Yuchen
	mdss_spi_tx_parameter(EPD_Macro_Insert_4bit,TOTAL_PIXEL_ARY_CNT+2);//2019/02/23,Yuchen
	mdss_spi_tx_parameter(parm_b5,sizeof(parm_b5));
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
if(busy_n==1)
	break;
}
chg_mode_show=0;
  	}

show_count=show_count+1;

}
//[4101][Raymond]off charging icon - end
	pinfo->blank_state = MDSS_PANEL_BLANK_UNBLANK;

#if 1
pr_err("%s --\n",__func__);
#endif
	return 0;
}


static int mdss_spi_panel_off(struct mdss_panel_data *pdata)
{
	struct spi_panel_data *ctrl = NULL;
	struct mdss_panel_info *pinfo;
	//int i;
	//int ret=0;
	int busy_n;
	//int rc = 0;
//[4101][Raymond]Fix screen abnormal in suspend - begin	
if(os_mode==1){
	bflash=0;
}
panel_suspend=1;
//[4101][Raymond]Fix screen abnormal in suspend - end
memset(EPD_fb_4bit,0xFF,144002); //
	parm_b2[0] = 0x10;
	parm_b2[1] = 0x03;
	EPD_fb_4bit[0] = 0x10;
	EPD_fb_4bit[1] = 0x03;

#if 1
pr_err("%s ++\n",__func__);
#endif

	bRemoteContentInconsistant = true;
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	pinfo = &pdata->panel_info;
	ctrl = container_of(pdata, struct spi_panel_data,
				panel_data);
//[4101][Raymond]three seconds Power Off command Implementation - begin
//pr_err("%s cancel_delayed_work_sync \n",__func__);
cancel_delayed_work_sync(&epd_pof_work);
//[4101][Raymond]three seconds Power Off command Implementation - end
if(chg_mode==0){ //normal boot
//[4101][Raymond]fix JIRA PRJ4101-17- begin
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
if(busy_n==1)
	break;
}
//[4101][Raymond]Fix JIRA PRJ4101-17- end
mdss_spi_tx_parameter(PON_REG,sizeof(PON_REG)); //PON
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
if(busy_n==1)
	break;
}
//mdss_spi_tx_parameter(parm_b1,sizeof(parm_b1));
//<2018/10/09,Yuchen-[4101] handle power off case

//#if DYN_PARTIAL_UPDATE
has_pre_frame=false;
//#endif

//>2018/10/09,Yuchen
//[4101][Raymond] improve ghost image when system suspend - begin
if(!rcy_mode){//2019/03/20,Yuchen
change_waveform_mode(&ctrl->panel_data,4);
mdss_spi_tx_parameter(parm_dtmw_full,sizeof(parm_dtmw_full)); //DTMW
mdss_spi_tx_parameter(EPD_fb_4bit,sizeof(EPD_fb_4bit));//white
//[4101][Raymond] impelment uevent feature - begin
if(os_mode==1 && gdev_done==1){
	kobject_uevent_env(&g_dev->kobj,
		KOBJ_CHANGE, envp2); // send uevent busy=1
	pr_err("%s send epd busy=1 uevent \n",__func__);
}
//[4101][Raymond] impelment uevent feature - end
mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
if(busy_n==1)
	break;
}
}//2019/03/20,Yuchen
//[Arima][Raymond]color of screen to match color id in suspend -begin
if(color_id!=2){ //!=white
	mdss_spi_tx_parameter(parm_dtmw_full,sizeof(parm_dtmw_full)); //DTMW
	mdss_spi_tx_parameter(parm_b2,sizeof(parm_b2));//black
	mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	if(busy_n==1)
		break;
	}
}
else{ //white
		mdss_spi_tx_parameter(parm_dtmw_full,sizeof(parm_dtmw_full)); //DTMW
		mdss_spi_tx_parameter(EPD_fb_4bit,sizeof(EPD_fb_4bit));//white
		mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
		while(1){
		busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
		if(busy_n==1)
		break;
		}
	}
//[Arima][Raymond]color of screen to match color id in suspend -end	
//[4101][Raymond] improve ghost image when system suspend - end
//[4101][Raymond] impelment uevent feature - begin
if(os_mode==1 && gdev_done==1){
	kobject_uevent_env(&g_dev->kobj,
		KOBJ_CHANGE, envp); // send uevent busy=0
	pr_err("%s[Raymond] send epd busy=0 uevent \n",__func__);
	kobject_uevent_env(&g_dev->kobj,
		KOBJ_CHANGE, power0); // send uevent busy=0
	pr_err("%s[Raymond] send power state =0 uevent \n",__func__);
}
//[4101][Raymond] impelment uevent feature - end
mdss_spi_tx_parameter(parm26,sizeof(parm26)); //POF
while(1){
busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
//pr_err("pon busy_n status:%d \n", busy_n);
if(busy_n==1)
	break;
}

	pinfo->blank_state = MDSS_PANEL_BLANK_BLANK;

}else{ //off charging mode
if(show_count>=3){
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	if(busy_n==1)
		break;
	}
	mdss_spi_tx_parameter(PON_REG,sizeof(PON_REG)); //PON
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	if(busy_n==1)
		break;
	}
//[4101][Raymond] improve ghost image when system suspend - begin
	change_waveform_mode(&ctrl->panel_data,4);
	mdss_spi_tx_parameter(EPD_fb_4bit,sizeof(EPD_fb_4bit));//white
	mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	if(busy_n==1)
		break;
	}
//[Arima][Raymond]color of screen to match color id in offcharging -begin	
	if(color_id!=2){ //!=white
		mdss_spi_tx_parameter(parm_b2,sizeof(parm_b2));//black
		mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
		while(1){
		busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
		if(busy_n==1)
		break;
		}
	}
	else{ //white
		mdss_spi_tx_parameter(EPD_fb_4bit,sizeof(EPD_fb_4bit));//white
		mdss_spi_tx_parameter(parm25,sizeof(parm25)); //DRF
		while(1){
		busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
		if(busy_n==1)
		break;
		}
	}
//[Arima][Raymond]color of screen to match color id in offcharging -end	
//[4101][Raymond] improve ghost image when system suspend - end
	mdss_spi_tx_parameter(parm26,sizeof(parm26)); //POF
	while(1){
	busy_n=check_busy_n_status(ctrl->disp_busy_gpio);
	if(busy_n==1)
	break;
	}
	chg_mode_show=1;
}
}

pr_err("%s --\n",__func__);
#if debug_panel
	pr_debug("%s:-\n", __func__);
#endif
	return 0;
}

static void mdss_spi_put_dt_vreg_data(struct device *dev,
	struct dss_module_power *module_power)
{
#if debug_panel
pr_err("%s ++\n",__func__);
#endif

	if (!module_power) {
		pr_err("%s: invalid input\n", __func__);
		return;
	}

	if (module_power->vreg_config) {
		devm_kfree(dev, module_power->vreg_config);
		module_power->vreg_config = NULL;
	}
	module_power->num_vreg = 0;
#if debug_panel
pr_err("%s --\n",__func__);
#endif
}


static int mdss_spi_get_panel_vreg_data(struct device *dev,
			struct dss_module_power *mp)
{
	int i = 0, rc = 0;
	u32 tmp = 0;
	struct device_node *of_node = NULL, *supply_node = NULL;
	struct device_node *supply_root_node = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (!dev || !mp) {
		pr_err("%s: invalid input\n", __func__);
		rc = -EINVAL;
		return rc;
	}

	of_node = dev->of_node;

	mp->num_vreg = 0;

	supply_root_node = of_get_child_by_name(of_node,
				"qcom,panel-supply-entries");

	for_each_available_child_of_node(supply_root_node, supply_node) {
		mp->num_vreg++;
	}
	if (mp->num_vreg == 0) {
		pr_debug("%s: no vreg\n", __func__);
		goto novreg;
	} else {
		pr_debug("%s: vreg found. count=%d\n", __func__, mp->num_vreg);
	}

	mp->vreg_config = kcalloc(mp->num_vreg, sizeof(struct dss_vreg),
				GFP_KERNEL);

	if (NULL != mp->vreg_config) {
		for_each_available_child_of_node(supply_root_node,
				supply_node) {
			const char *st = NULL;
			/* vreg-name */
			rc = of_property_read_string(supply_node,
				"qcom,supply-name", &st);
			if (rc) {
				pr_err("%s: error reading name. rc=%d\n",
					__func__, rc);
				goto error;
			}
			snprintf(mp->vreg_config[i].vreg_name,
				ARRAY_SIZE((mp->vreg_config[i].vreg_name)),
					"%s", st);
			/* vreg-min-voltage */
			rc = of_property_read_u32(supply_node,
				"qcom,supply-min-voltage", &tmp);
			if (rc) {
				pr_err("%s: error reading min volt. rc=%d\n",
					__func__, rc);
				goto error;
			}
			mp->vreg_config[i].min_voltage = tmp;

			/* vreg-max-voltage */
			rc = of_property_read_u32(supply_node,
				"qcom,supply-max-voltage", &tmp);
			if (rc) {
				pr_err("%s: error reading max volt. rc=%d\n",
					__func__, rc);
				goto error;
			}
			mp->vreg_config[i].max_voltage = tmp;

			/* enable-load */
			rc = of_property_read_u32(supply_node,
				"qcom,supply-enable-load", &tmp);
			if (rc) {
				pr_err("%s: error read enable load. rc=%d\n",
					__func__, rc);
				goto error;
			}
			mp->vreg_config[i].load[DSS_REG_MODE_ENABLE] = tmp;

			/* disable-load */
			rc = of_property_read_u32(supply_node,
				"qcom,supply-disable-load", &tmp);
			if (rc) {
				pr_err("%s: error read disable load. rc=%d\n",
					__func__, rc);
				goto error;
			}
			mp->vreg_config[i].load[DSS_REG_MODE_DISABLE] = tmp;

			/* pre-sleep */
			rc = of_property_read_u32(supply_node,
				"qcom,supply-pre-on-sleep", &tmp);
			if (rc) {
				pr_debug("%s: error read pre on value\n",
						__func__);
				rc = 0;
			} else {
				mp->vreg_config[i].pre_on_sleep = tmp;
			}

			rc = of_property_read_u32(supply_node,
				"qcom,supply-pre-off-sleep", &tmp);
			if (rc) {
				pr_debug("%s: error read pre off value\n",
						__func__);
				rc = 0;
			} else {
				mp->vreg_config[i].pre_off_sleep = tmp;
			}

			/* post-sleep */
			rc = of_property_read_u32(supply_node,
				"qcom,supply-post-on-sleep", &tmp);
			if (rc) {
				pr_debug("%s: error read post on value\n",
						__func__);
				rc = 0;
			} else {
				mp->vreg_config[i].post_on_sleep = tmp;
			}

			rc = of_property_read_u32(supply_node,
				"qcom,supply-post-off-sleep", &tmp);
			if (rc) {
				pr_debug("%s: error read post off value\n",
						__func__);
				rc = 0;
			} else {
				mp->vreg_config[i].post_off_sleep = tmp;
			}

			++i;
		}
	}
	#if debug_panel
	pr_err("%s --\n",__func__);
	#endif
	return rc;
error:
	kfree(mp->vreg_config);
	mp->vreg_config = NULL;

novreg:
	mp->num_vreg = 0;

	return rc;

}

static int mdss_spi_panel_parse_cmds(struct device_node *np,
		struct spi_panel_cmds *pcmds, char *cmd_key)
{
	const char *data;
	int blen = 0, len;
	char *buf, *bp;
	struct spi_ctrl_hdr *dchdr;
	int i, cnt;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	data = of_get_property(np, cmd_key, &blen);
	if (!data) {
		pr_err("%s: failed, key=%s\n", __func__, cmd_key);
		return -ENOMEM;
	}

	buf = kcalloc(blen, sizeof(char), GFP_KERNEL);
	//buf = kcalloc(blen, sizeof(u16), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	memcpy(buf, data, blen);

	/* scan dcs commands */
	bp = buf;
	len = blen;
	cnt = 0;
	while (len >= sizeof(*dchdr)) {
		dchdr = (struct spi_ctrl_hdr *)bp;
		if (dchdr->dlen > len) {
			pr_err("%s: dtsi parse error, len=%d",
				__func__, dchdr->dlen);
			goto exit_free;
		}
		bp += sizeof(*dchdr);
		len -= sizeof(*dchdr);
		bp += dchdr->dlen;
		len -= dchdr->dlen;
		cnt++;
	}

	if (len != 0) {
		pr_err("%s: dcs_cmd=%x len=%d error",
				__func__, buf[0], len);
		goto exit_free;
	}

	pcmds->cmds = kcalloc(cnt, sizeof(struct spi_cmd_desc),
						GFP_KERNEL);
	if (!pcmds->cmds)
		goto exit_free;

	pcmds->cmd_cnt = cnt;
	pcmds->buf = buf;
	pcmds->blen = blen;

	bp = buf;
	len = blen;
	for (i = 0; i < cnt; i++) {
		dchdr = (struct spi_ctrl_hdr *)bp;
		len -= sizeof(*dchdr);
		bp += sizeof(*dchdr);
		pcmds->cmds[i].dchdr = *dchdr;
		pcmds->cmds[i].command = bp;
		pcmds->cmds[i].parameter = bp + sizeof(char);
		bp += dchdr->dlen;
		len -= dchdr->dlen;
	}

	pr_debug("%s: dcs_cmd=%x, len=%d, cmd_cnt=%d\n", __func__,
		pcmds->buf[0], pcmds->blen, pcmds->cmd_cnt);
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return 0;

exit_free:
	kfree(buf);
	return -ENOMEM;
}
static int mdss_spi_panel_parse_reset_seq(struct device_node *np,
		u32 rst_seq[MDSS_SPI_RST_SEQ_LEN], u32 *rst_len,
		const char *name)
{
	int num = 0, i;
	int rc;
	struct property *data;
	u32 tmp[MDSS_SPI_RST_SEQ_LEN];
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	*rst_len = 0;
	data = of_find_property(np, name, &num);
	num /= sizeof(u32);
	if (!data || !num || num > MDSS_SPI_RST_SEQ_LEN || num % 2) {
		pr_err("%s:%d, error reading %s, length found = %d\n",
			__func__, __LINE__, name, num);
	} else {
		rc = of_property_read_u32_array(np, name, tmp, num);
		if (rc)
			pr_err("%s:%d, error reading %s, rc = %d\n",
				__func__, __LINE__, name, rc);
		else {
			for (i = 0; i < num; ++i)
				rst_seq[i] = tmp[i];
			*rst_len = num;
		}
	}
#if debug_panel
	pr_err("%s --\n",__func__);
#endif
	return 0;
}

static bool mdss_send_panel_cmd_for_esd(struct spi_panel_data *ctrl_pdata)
{
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (ctrl_pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return false;
	}

	mutex_lock(&ctrl_pdata->spi_tx_mutex);
	mdss_spi_panel_on(&ctrl_pdata->panel_data);
	mutex_unlock(&ctrl_pdata->spi_tx_mutex);
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return true;
}

static bool mdss_spi_reg_status_check(struct spi_panel_data *ctrl_pdata)
{
	int ret = 0;
	int i = 0;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (ctrl_pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return false;
	}

	pr_debug("%s: Checking Register status\n", __func__);

	ret = mdss_spi_read_panel_data(&ctrl_pdata->panel_data,
					ctrl_pdata->panel_status_reg,
					ctrl_pdata->act_status_value,
					ctrl_pdata->status_cmds_rlen);
	if (ret < 0) {
		pr_err("%s: Read status register returned error\n", __func__);
	} else {
		for (i = 0; i < ctrl_pdata->status_cmds_rlen; i++) {
			pr_debug("act_value[%d] = %x, exp_value[%d] = %x\n",
					i, ctrl_pdata->act_status_value[i],
					i, ctrl_pdata->exp_status_value[i]);
			if (ctrl_pdata->act_status_value[i] !=
					ctrl_pdata->exp_status_value[i])
				return false;
		}
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return true;
}

static void mdss_spi_parse_esd_params(struct device_node *np,
		struct spi_panel_data	*ctrl)
{
	u32 tmp;
	int rc;
	struct property *data;
	const char *string;
	struct mdss_panel_info *pinfo = &ctrl->panel_data.panel_info;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	pinfo->esd_check_enabled = of_property_read_bool(np,
		"qcom,esd-check-enabled");

	if (!pinfo->esd_check_enabled){
		ctrl->act_status_value = kzalloc(sizeof(u8) *
				(1 + 1), GFP_KERNEL);
		pr_err("%s esd_check_enabled nor enable \n",__func__);
		return;
		}
	ctrl->status_mode = SPI_ESD_MAX;

	rc = of_property_read_string(np,
			"qcom,mdss-spi-panel-status-check-mode", &string);
	if (!rc) {
		if (!strcmp(string, "reg_read")) {
			ctrl->status_mode = SPI_ESD_REG;
			ctrl->check_status =
				mdss_spi_reg_status_check;
		} else if (!strcmp(string, "send_init_command")) {
			ctrl->status_mode = SPI_SEND_PANEL_COMMAND;
			ctrl->check_status =
				mdss_send_panel_cmd_for_esd;
				return;
		} else {
			pr_err("No valid panel-status-check-mode string\n");
			pinfo->esd_check_enabled = false;
			return;
		}
	}

	rc = of_property_read_u8(np, "qcom,mdss-spi-panel-status-reg",
				&ctrl->panel_status_reg);
	if (rc) {
		pr_warn("%s:%d, Read status reg failed, disable ESD check\n",
				__func__, __LINE__);
		pinfo->esd_check_enabled = false;
		return;
	}

	rc = of_property_read_u32(np, "qcom,mdss-spi-panel-status-read-length",
					&tmp);
	if (rc) {
		pr_warn("%s:%d, Read reg length failed, disable ESD check\n",
				__func__, __LINE__);
		pinfo->esd_check_enabled = false;
		return;
	}

	ctrl->status_cmds_rlen = (!rc ? tmp : 1);

	ctrl->exp_status_value = kzalloc(sizeof(u8) *
				 (ctrl->status_cmds_rlen + 1), GFP_KERNEL);
	ctrl->act_status_value = kzalloc(sizeof(u8) *
				(ctrl->status_cmds_rlen + 1), GFP_KERNEL);

	if (!ctrl->exp_status_value || !ctrl->act_status_value) {
		pr_err("%s: Error allocating memory for status buffer\n",
						__func__);
		pinfo->esd_check_enabled = false;
		return;
	}

	data = of_find_property(np, "qcom,mdss-spi-panel-status-value", &tmp);
	tmp /= sizeof(u8);
	if (!data || (tmp != ctrl->status_cmds_rlen)) {
		pr_err("%s: Panel status values not found\n", __func__);
		pinfo->esd_check_enabled = false;
		memset(ctrl->exp_status_value, 0, ctrl->status_cmds_rlen);
	} else {
		rc = of_property_read_u8_array(np,
			"qcom,mdss-spi-panel-status-value",
			ctrl->exp_status_value, tmp);
		if (rc) {
			pr_err("%s: Error reading panel status values\n",
				__func__);
			pinfo->esd_check_enabled = false;
			memset(ctrl->exp_status_value, 0,
				ctrl->status_cmds_rlen);
		}
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
}

static int mdss_spi_panel_parse_dt(struct device_node *np,
		struct spi_panel_data	*ctrl_pdata)
{
	u32 tmp;
	int rc;
	const char *data;
	struct mdss_panel_info *pinfo = &(ctrl_pdata->panel_data.panel_info);
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	pinfo->cont_splash_enabled = of_property_read_bool(np,
					"qcom,cont-splash-enabled");

	rc = of_property_read_u32(np, "qcom,mdss-spi-panel-width", &tmp);
	if (rc) {
		pr_err("%s: panel width not specified\n", __func__);
		return -EINVAL;
	}
	pinfo->xres = (!rc ? tmp : 240);

	rc = of_property_read_u32(np, "qcom,mdss-spi-panel-height", &tmp);
	if (rc) {
		pr_err("%s:panel height not specified\n", __func__);
		return -EINVAL;
	}
	pinfo->yres = (!rc ? tmp : 320);

	rc = of_property_read_u32(np,
		"qcom,mdss-pan-physical-width-dimension", &tmp);
	pinfo->physical_width = (!rc ? tmp : 0);
	rc = of_property_read_u32(np,
		"qcom,mdss-pan-physical-height-dimension", &tmp);
	pinfo->physical_height = (!rc ? tmp : 0);
	rc = of_property_read_u32(np, "qcom,mdss-spi-panel-framerate", &tmp);
	pinfo->spi.frame_rate = (!rc ? tmp : 30);
	rc = of_property_read_u32(np, "qcom,mdss-spi-h-front-porch", &tmp);
	pinfo->lcdc.h_front_porch = (!rc ? tmp : 6);
	rc = of_property_read_u32(np, "qcom,mdss-spi-h-back-porch", &tmp);
	pinfo->lcdc.h_back_porch = (!rc ? tmp : 6);
	rc = of_property_read_u32(np, "qcom,mdss-spi-h-pulse-width", &tmp);
	pinfo->lcdc.h_pulse_width = (!rc ? tmp : 2);
	rc = of_property_read_u32(np, "qcom,mdss-spi-h-sync-skew", &tmp);
	pinfo->lcdc.hsync_skew = (!rc ? tmp : 0);
	rc = of_property_read_u32(np, "qcom,mdss-spi-v-back-porch", &tmp);
	pinfo->lcdc.v_back_porch = (!rc ? tmp : 6);
	rc = of_property_read_u32(np, "qcom,mdss-spi-v-front-porch", &tmp);
	pinfo->lcdc.v_front_porch = (!rc ? tmp : 6);
	rc = of_property_read_u32(np, "qcom,mdss-spi-v-pulse-width", &tmp);
	pinfo->lcdc.v_pulse_width = (!rc ? tmp : 2);


	rc = of_property_read_u32(np, "qcom,mdss-spi-bpp", &tmp);
	if (rc) {
		pr_err("%s: bpp not specified\n", __func__);
		return -EINVAL;
	}
	pinfo->bpp = (!rc ? tmp : 16);

	pinfo->pdest = DISPLAY_1;

	ctrl_pdata->bklt_ctrl = SPI_UNKNOWN_CTRL;
	data = of_get_property(np, "qcom,mdss-spi-bl-pmic-control-type", NULL);
	if (data) {
		if (!strcmp(data, "bl_ctrl_wled")) {
			led_trigger_register_simple("bkl-trigger",
				&bl_led_trigger);
			pr_debug("%s: SUCCESS-> WLED TRIGGER register\n",
				__func__);
			ctrl_pdata->bklt_ctrl = SPI_BL_WLED;
		} else if (!strcmp(data, "bl_gpio_pulse")) {
			led_trigger_register_simple("gpio-bklt-trigger",
				&bl_led_trigger);
			pr_debug("%s: SUCCESS-> GPIO PULSE TRIGGER register\n",
				__func__);
			ctrl_pdata->bklt_ctrl = SPI_BL_WLED;
		} else if (!strcmp(data, "bl_ctrl_pwm")) {
			ctrl_pdata->bklt_ctrl = SPI_BL_PWM;
			ctrl_pdata->pwm_pmi = of_property_read_bool(np,
					"qcom,mdss-spi-bl-pwm-pmi");
			rc = of_property_read_u32(np,
				"qcom,mdss-spi-bl-pmic-pwm-frequency", &tmp);
			if (rc) {
				pr_err("%s: Error, panel pwm_period\n",
					 __func__);
				return -EINVAL;
			}
			ctrl_pdata->pwm_period = tmp;
			if (ctrl_pdata->pwm_pmi) {
				ctrl_pdata->pwm_bl = of_pwm_get(np, NULL);
				if (IS_ERR(ctrl_pdata->pwm_bl)) {
					pr_err("%s: Error, pwm device\n",
							__func__);
					ctrl_pdata->pwm_bl = NULL;
					return -EINVAL;
				}
			} else {
				rc = of_property_read_u32(np,
					"qcom,mdss-spi-bl-pmic-bank-select",
								 &tmp);
				if (rc) {
					pr_err("%s: Error, lpg channel\n",
						__func__);
					return -EINVAL;
				}
				ctrl_pdata->pwm_lpg_chan = tmp;
				tmp = of_get_named_gpio(np,
					"qcom,mdss-spi-pwm-gpio", 0);
				ctrl_pdata->pwm_pmic_gpio = tmp;
				pr_debug("%s: Configured PWM bklt ctrl\n",
								 __func__);
			}
		}
	}
	rc = of_property_read_u32(np, "qcom,mdss-brightness-max-level", &tmp);
	pinfo->brightness_max = (!rc ? tmp : MDSS_MAX_BL_BRIGHTNESS);
	rc = of_property_read_u32(np, "qcom,mdss-spi-bl-min-level", &tmp);
	pinfo->bl_min = (!rc ? tmp : 0);
	rc = of_property_read_u32(np, "qcom,mdss-spi-bl-max-level", &tmp);
	pinfo->bl_max = (!rc ? tmp : 255);
	ctrl_pdata->bklt_max = pinfo->bl_max;


	mdss_spi_panel_parse_reset_seq(np, pinfo->rst_seq,
					&(pinfo->rst_seq_len),
					"qcom,mdss-spi-reset-sequence");

	mdss_spi_panel_parse_cmds(np, &ctrl_pdata->on_cmds,
		"qcom,mdss-spi-on-command");

	mdss_spi_panel_parse_cmds(np, &ctrl_pdata->off_cmds,
		"qcom,mdss-spi-off-command");

	mdss_spi_parse_esd_params(np, ctrl_pdata);
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return 0;
}

static void mdss_spi_panel_pwm_cfg(struct spi_panel_data *ctrl)
{
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (ctrl->pwm_pmi)
		return;

	ctrl->pwm_bl = pwm_request(ctrl->pwm_lpg_chan, "lcd-bklt");
	if (ctrl->pwm_bl == NULL || IS_ERR(ctrl->pwm_bl)) {
		pr_err("%s: Error: lpg_chan=%d pwm request failed",
				__func__, ctrl->pwm_lpg_chan);
	}
	ctrl->pwm_enabled = 0;
#if debug_panel
pr_err("%s --\n",__func__);
#endif
}

static void mdss_spi_panel_bklt_pwm(struct spi_panel_data *ctrl, int level)
{
	int ret;
	u32 duty;
	u32 period_ns;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (ctrl->pwm_bl == NULL) {
		pr_err("%s: no PWM\n", __func__);
		return;
	}
//[4101][Raymond]Modify front light brightness begin
if(level>10)
{
level=10;
}
if(level==1)
{
level=0;
}
//[4101][Raymond]Modify front light brightness end
	if (level == 0) {
		if (ctrl->pwm_enabled) {
			ret = pwm_config_us(ctrl->pwm_bl, level,
					ctrl->pwm_period);
			if (ret)
				pr_err("%s: pwm_config_us() failed err=%d.\n",
						__func__, ret);
			pwm_disable(ctrl->pwm_bl);
		}
		ctrl->pwm_enabled = 0;
		return;
	}

	duty = level * ctrl->pwm_period;
	duty /= ctrl->bklt_max;

	pr_err("%s: bklt_ctrl=%d pwm_period=%d pwm_gpio=%d pwm_lpg_chan=%d duty=%d level=%d \n",
			__func__, ctrl->bklt_ctrl, ctrl->pwm_period,
				ctrl->pwm_pmic_gpio, ctrl->pwm_lpg_chan,duty,level);

	if (ctrl->pwm_period >= USEC_PER_SEC) {
		ret = pwm_config_us(ctrl->pwm_bl, duty, ctrl->pwm_period);
		if (ret) {
			pr_err("%s: pwm_config_us() failed err=%d\n",
					__func__, ret);
			return;
		}
	} else {
		period_ns = ctrl->pwm_period * NSEC_PER_USEC;
		ret = pwm_config(ctrl->pwm_bl,
				level * period_ns / ctrl->bklt_max,
				period_ns);
		if (ret) {
			pr_err("%s: pwm_config() failed err=%d\n",
					__func__, ret);
			return;
		}
	}

	if (!ctrl->pwm_enabled) {
		ret = pwm_enable(ctrl->pwm_bl);
		if (ret)
			pr_err("%s: pwm_enable() failed err=%d\n", __func__,
				ret);
		ctrl->pwm_enabled = 1;
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
}

static void mdss_spi_panel_bl_ctrl(struct mdss_panel_data *pdata,
							u32 bl_level)
{
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (bl_level) {
		mdp3_res->bklt_level = bl_level;
		mdp3_res->bklt_update = true;
	} else {
		mdss_spi_panel_bl_ctrl_update(pdata, bl_level);
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
}

#if defined(CONFIG_FB_MSM_MDSS_SPI_PANEL) && defined(CONFIG_SPI_QUP)
void mdss_spi_panel_bl_ctrl_update(struct mdss_panel_data *pdata,
							u32 bl_level)
{
	struct spi_panel_data *ctrl_pdata = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	if ((bl_level < pdata->panel_info.bl_min) && (bl_level != 0))
		bl_level = pdata->panel_info.bl_min;

	switch (ctrl_pdata->bklt_ctrl) {
	case SPI_BL_WLED:
		led_trigger_event(bl_led_trigger, bl_level);
		break;
	case SPI_BL_PWM:
		mdss_spi_panel_bklt_pwm(ctrl_pdata, bl_level);
		break;
	default:
		pr_err("%s: Unknown bl_ctrl configuration %d\n",
			__func__, ctrl_pdata->bklt_ctrl);
		break;
	}
#if debug_panel
	pr_err("%s --\n",__func__);
#endif
}
#endif

static int mdss_spi_panel_init(struct device_node *node,
	struct spi_panel_data	*ctrl_pdata,
	bool cmd_cfg_cont_splash)
{
	int rc = 0;
	static const char *panel_name;
	struct mdss_panel_info *pinfo;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (!node || !ctrl_pdata) {
		pr_err("%s: Invalid arguments\n", __func__);
		return -ENODEV;
	}

	pinfo = &ctrl_pdata->panel_data.panel_info;

	pr_debug("%s:%d\n", __func__, __LINE__);
	pinfo->panel_name[0] = '\0';
	panel_name = of_get_property(node, "qcom,mdss-spi-panel-name", NULL);
	if (!panel_name) {
		pr_info("%s:%d, Panel name not specified\n",
						__func__, __LINE__);
	} else {
		pr_debug("%s: Panel Name = %s\n", __func__, panel_name);
		strlcpy(&pinfo->panel_name[0], panel_name, MDSS_MAX_PANEL_LEN);
	}
	rc = mdss_spi_panel_parse_dt(node, ctrl_pdata);
	if (rc) {
		pr_err("%s:%d panel dt parse failed\n", __func__, __LINE__);
		return rc;
	}

	ctrl_pdata->byte_pre_frame = pinfo->xres * pinfo->yres * pinfo->bpp/8;

	ctrl_pdata->tx_buf = kmalloc(ctrl_pdata->byte_pre_frame, GFP_KERNEL);

	if (!cmd_cfg_cont_splash)
		pinfo->cont_splash_enabled = false;

	pr_info("%s: Continuous splash %s\n", __func__,
		pinfo->cont_splash_enabled ? "enabled" : "disabled");

	pinfo->dynamic_switch_pending = false;
	pinfo->is_lpm_mode = false;
	pinfo->esd_rdy = false;

	ctrl_pdata->on = mdss_spi_panel_on;
	ctrl_pdata->off = mdss_spi_panel_off;
	ctrl_pdata->panel_data.set_backlight = mdss_spi_panel_bl_ctrl;
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return 0;
}

static int mdss_spi_get_panel_cfg(char *panel_cfg,
				struct spi_panel_data	*ctrl_pdata)
{
	int rc;
	struct mdss_panel_cfg *pan_cfg = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (!ctrl_pdata)
		return MDSS_PANEL_INTF_INVALID;

	pan_cfg = ctrl_pdata->mdss_util->panel_intf_type(MDSS_PANEL_INTF_SPI);
	if (IS_ERR(pan_cfg)) {
		return PTR_ERR(pan_cfg);
	} else if (!pan_cfg) {
		panel_cfg[0] = 0;
		return 0;
	}

	pr_debug("%s:%d: cfg:[%s]\n", __func__, __LINE__,
		 pan_cfg->arg_cfg);
	ctrl_pdata->panel_data.panel_info.is_prim_panel = true;
	rc = strlcpy(panel_cfg, pan_cfg->arg_cfg,
		     sizeof(pan_cfg->arg_cfg));
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return rc;
}

static int mdss_spi_panel_regulator_init(struct platform_device *pdev)
{
	int rc = 0;

	struct spi_panel_data *ctrl_pdata = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (!pdev) {
		pr_err("%s: invalid input\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = platform_get_drvdata(pdev);
	if (!ctrl_pdata) {
		pr_err("%s: invalid driver data\n", __func__);
		return -EINVAL;
	}

	rc = msm_dss_config_vreg(&pdev->dev,
		ctrl_pdata->panel_power_data.vreg_config,
		ctrl_pdata->panel_power_data.num_vreg, 1);
	if (rc)
		pr_err("%s: failed to init vregs for %s\n",
			__func__, "PANEL_PM");
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return rc;

}
#if 0
static irqreturn_t spi_panel_te_handler(int irq, void *data)
{
	struct spi_panel_data *ctrl_pdata = (struct spi_panel_data *)data;
	static int count = 2;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (!ctrl_pdata) {
		pr_err("%s: SPI display not available\n", __func__);
		return IRQ_HANDLED;
	}
	complete(&ctrl_pdata->spi_panel_te);

	if (ctrl_pdata->vsync_client.handler && !(--count)) {
		ctrl_pdata->vsync_client.handler(ctrl_pdata->vsync_client.arg);
		count = 2;
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return IRQ_HANDLED;
}
#endif
void mdp3_spi_vsync_enable(struct mdss_panel_data *pdata,
				struct mdp3_notification *vsync_client)
{
	int updated = 0;
	struct spi_panel_data *ctrl_pdata = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	ctrl_pdata = container_of(pdata, struct spi_panel_data,
				panel_data);

	if (vsync_client) {
		if (ctrl_pdata->vsync_client.handler != vsync_client->handler) {
			ctrl_pdata->vsync_client = *vsync_client;
			updated = 1;
		}
	} else {
		if (ctrl_pdata->vsync_client.handler) {
			ctrl_pdata->vsync_client.handler = NULL;
			ctrl_pdata->vsync_client.arg = NULL;
			updated = 1;
		}
	}

	if (updated) {
		if (vsync_client && vsync_client->handler)
			enable_spi_panel_te_irq(ctrl_pdata, true);
		else
			enable_spi_panel_te_irq(ctrl_pdata, false);
	}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
}

static struct device_node *mdss_spi_pref_prim_panel(
		struct platform_device *pdev)
{
	struct device_node *spi_pan_node = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	pr_debug("%s:%d: Select primary panel from dt\n",
					__func__, __LINE__);
	spi_pan_node = of_parse_phandle(pdev->dev.of_node,
					"qcom,spi-pref-prim-pan", 0);
	if (!spi_pan_node)
		pr_err("%s:can't find panel phandle\n", __func__);
#if debug_panel
pr_err("%s --\n",__func__);
#endif
	return spi_pan_node;
}

static int spi_panel_device_register(struct device_node *pan_node,
				struct spi_panel_data *ctrl_pdata)
{
	int rc;
	struct mdss_panel_info *pinfo = &(ctrl_pdata->panel_data.panel_info);
	struct device_node *spi_ctrl_np = NULL;
	struct platform_device *ctrl_pdev = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	pinfo->type = SPI_PANEL;

	spi_ctrl_np = of_parse_phandle(pan_node,
				"qcom,mdss-spi-panel-controller", 0);
	if (!spi_ctrl_np) {
		pr_err("%s: SPI controller node not initialized\n", __func__);
		return -EPROBE_DEFER;
	}

	ctrl_pdev = of_find_device_by_node(spi_ctrl_np);
	if (!ctrl_pdev) {
		of_node_put(spi_ctrl_np);
		pr_err("%s: SPI controller node not find\n", __func__);
		return -EPROBE_DEFER;
	}

	rc = mdss_spi_panel_regulator_init(ctrl_pdev);
	if (rc) {
		pr_err("%s: failed to init regulator, rc=%d\n",
						__func__, rc);
		return rc;
	}

	pinfo->panel_max_fps = mdss_panel_get_framerate(pinfo ,
		FPS_RESOLUTION_HZ);
	pinfo->panel_max_vtotal = mdss_panel_get_vtotal(pinfo);

	ctrl_pdata->disp_te_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
		"qcom,platform-te-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->disp_te_gpio))
		pr_err("%s:%d, TE gpio not specified\n",
						__func__, __LINE__);
//raymond test b
	ctrl_pdata->disp_busy_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
		"qcom,platform-busy-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->disp_busy_gpio))
		pr_err("%s:%d, busy gpio not specified\n",
						__func__, __LINE__);
//raymond test e

	ctrl_pdata->disp_dc_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
		"qcom,platform-spi-dc-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->disp_dc_gpio))
		pr_err("%s:%d, SPI DC gpio not specified\n",
						__func__, __LINE__);

	ctrl_pdata->rst_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
			 "qcom,platform-reset-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->rst_gpio))
		pr_err("%s:%d, reset gpio not specified\n",
						__func__, __LINE__);

	//raymond test b
ctrl_pdata->vcc_en_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
			 "qcom,platform-vcc-en-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->vcc_en_gpio))
		pr_err("%s:%d, vcc en gpio not specified\n",
						__func__, __LINE__);
//[4101][Raymond] decrease power consumption in suspend - begin
	ctrl_pdata->ls_en_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
			 "qcom,platform-ls-en-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->ls_en_gpio))
		pr_err("%s:%d, ls en gpio not specified\n",
						__func__, __LINE__);

	ctrl_pdata->boost_en_gpio = of_get_named_gpio(ctrl_pdev->dev.of_node,
			 "qcom,platform-boost-en-gpio", 0);
	if (!gpio_is_valid(ctrl_pdata->boost_en_gpio))
		pr_err("%s:%d, boost en gpio not specified\n",
						__func__, __LINE__);
	//raymond test e
//[4101][Raymond] decrease power consumption in suspend - end
	ctrl_pdata->panel_data.event_handler = mdss_spi_panel_event_handler;

	if (ctrl_pdata->bklt_ctrl == SPI_BL_PWM)
		mdss_spi_panel_pwm_cfg(ctrl_pdata);

	ctrl_pdata->ctrl_state = CTRL_STATE_UNKNOWN;

	if (pinfo->cont_splash_enabled) {
		rc = mdss_spi_panel_power_ctrl(&(ctrl_pdata->panel_data),
			MDSS_PANEL_POWER_ON);
		if (rc) {
			pr_err("%s: Panel power on failed\n", __func__);
			return rc;
		}
		if (ctrl_pdata->bklt_ctrl == SPI_BL_PWM)
			ctrl_pdata->pwm_enabled = 1;
		pinfo->blank_state = MDSS_PANEL_BLANK_UNBLANK;
		ctrl_pdata->ctrl_state |=
			(CTRL_STATE_PANEL_INIT | CTRL_STATE_MDP_ACTIVE);
	} else {
		pinfo->panel_power_state = MDSS_PANEL_POWER_OFF;
	}

	rc = mdss_register_panel(ctrl_pdev, &(ctrl_pdata->panel_data));
	if (rc) {
		pr_err("%s: unable to register SPI panel\n", __func__);
		return rc;
	}

	pr_debug("%s: Panel data initialized\n", __func__);
#if debug_panel
	pr_err("%s --\n",__func__);
#endif
	return 0;
}


/**
 * mdss_spi_find_panel_of_node(): find device node of spi panel
 * @pdev: platform_device of the spi ctrl node
 * @panel_cfg: string containing intf specific config data
 *
 * Function finds the panel device node using the interface
 * specific configuration data. This configuration data is
 * could be derived from the result of bootloader's GCDB
 * panel detection mechanism. If such config data doesn't
 * exist then this panel returns the default panel configured
 * in the device tree.
 *
 * returns pointer to panel node on success, NULL on error.
 */
static struct device_node *mdss_spi_find_panel_of_node(
		struct platform_device *pdev, char *panel_cfg)
{
	int len, i;
	int ctrl_id = pdev->id - 1;
	char panel_name[MDSS_MAX_PANEL_LEN] = "";
	char ctrl_id_stream[3] =  "0:";
	char *stream = NULL, *pan = NULL;
	struct device_node *spi_pan_node = NULL, *mdss_node = NULL;
#if debug_panel
pr_err("%s ++\n",__func__);
#endif
	len = strlen(panel_cfg);
	if (!len) {
		/* no panel cfg chg, parse dt */
		pr_err("%s:%d: no cmd line cfg present\n",
			 __func__, __LINE__);
		goto end;
	} else {
		if (ctrl_id == 1)
			strlcpy(ctrl_id_stream, "1:", 3);

		stream = strnstr(panel_cfg, ctrl_id_stream, len);
		if (!stream) {
			pr_err("controller config is not present\n");
			goto end;
		}
		stream += 2;

		pan = strnchr(stream, strlen(stream), ':');
		if (!pan) {
			strlcpy(panel_name, stream, MDSS_MAX_PANEL_LEN);
		} else {
			for (i = 0; (stream + i) < pan; i++)
				panel_name[i] = *(stream + i);
			panel_name[i] = 0;
		}

		pr_debug("%s:%d:%s:%s\n", __func__, __LINE__,
			 panel_cfg, panel_name);

		mdss_node = of_parse_phandle(pdev->dev.of_node,
					     "qcom,mdss-mdp", 0);
		if (!mdss_node) {
			pr_err("%s: %d: mdss_node null\n",
			       __func__, __LINE__);
			return NULL;
		}

		spi_pan_node = of_find_node_by_name(mdss_node,
						    panel_name);
		if (!spi_pan_node) {
			pr_err("%s: invalid pan node, selecting prim panel\n",
			       __func__);
			goto end;
		}
#if debug_panel
pr_err("%s --\n",__func__);
#endif
		return spi_pan_node;
	}
end:
	if (strcmp(panel_name, NONE_PANEL))
		spi_pan_node = mdss_spi_pref_prim_panel(pdev);
	of_node_put(mdss_node);
	return spi_pan_node;
}

//[4101][LCM][RaymondLin] Add LCM_vendor file node for PCBA function test begin
extern int seq_printf(struct seq_file *m, const char *f, ...);
extern int single_open(struct file *, int (*)(struct seq_file *, void *), void *);
extern ssize_t seq_read(struct file *, char __user *, size_t, loff_t *);
extern loff_t seq_lseek(struct file *, loff_t, int);
extern int single_release(struct inode *, struct file *);
static int proc_lcm_vendor_show(struct seq_file *m, void *v)
{
    int lcm_id=1;
    //int LCM_ID_PIN=80;
    //lcm_id = mt_get_gpio_in(GPIO_LCM_ID_PIN);

    if(lcm_id == 1)
    {
    	seq_printf(m, "E-ink EPD");
    }
    else
    {
    	seq_printf(m, "EPD");
    }

    return 0;
}
static int proc_lcm_vendor_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_lcm_vendor_show, NULL);
}

static const struct file_operations proc_lcm_vendor_fops = {
    .open  = proc_lcm_vendor_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};
//[4101][LCM][RaymondLin] Add LCM_vendor file node for PCBA function test end

static int proc_epd_wf_show(struct seq_file *m, void *v)
{
pr_err("%s ++\n",__func__);
    	//seq_printf(m, "GL16");
seq_printf(m, "%d",wf_mode);
pr_err("%s --\n",__func__);
    return 0;
}

static ssize_t proc_epd_wf_write(struct file *file, const char __user *buffer,
				    size_t count, loff_t *pos)
{
	char mode[]="0x00000000";

	u8 input_value;

pr_err("%s ++\n",__func__);

	if (copy_from_user(mode, buffer, sizeof(mode))) return count;

      if (sscanf(mode, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}
	#if 1
	if(input_value ==0){
            	pr_err("[R]%s mode=0 \n",__func__);
		change_waveform_mode(g_pdata,0);
		}
	else if(input_value ==1){
 		pr_err("[R]%s mode=1 \n",__func__);
		change_waveform_mode(g_pdata,1);
		}
	else if(input_value ==2){
		pr_err("[R]%s mode=2 \n",__func__);
		change_waveform_mode(g_pdata,2);
		}
	else if(input_value ==3){
		pr_err("[R]%s mode=3 \n",__func__);
		change_waveform_mode(g_pdata,3);
		}
	else if(input_value ==4){
		pr_err("[R]%s mode=4 \n",__func__);
		change_waveform_mode(g_pdata,4);
		}
	else{
		input_value =3;
		change_waveform_mode(g_pdata,3);
		pr_err("[R]%s default mode=3 \n",__func__);
		}
	#endif
	wf_mode = input_value;

pr_err("%s, mode=%s ,input_value=%d --\n",__func__,mode,input_value);
	return count;
	//return 0;
}

static int proc_epd_wf_open(struct inode *inode, struct file *file)
{
pr_err("%s ++\n",__func__);
    return single_open(file, proc_epd_wf_show, NULL);
pr_err("%s --\n",__func__);
}

static const struct file_operations proc_epd_wf_fops = {
    .open  = proc_epd_wf_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = proc_epd_wf_write,
    .read = seq_read,
};

static int proc_epd_pu_en_show(struct seq_file *m, void *v)
{
pr_err("%s ++\n",__func__);
    	//seq_printf(m, "GL16");
seq_printf(m, "%d",pu_en);
pr_err("%s --\n",__func__);
    return 0;
}

static ssize_t proc_epd_pu_en_write(struct file *file, const char __user *buffer,
				    size_t count, loff_t *pos)
{
	char mode[]="0x00000000";

	u8 input_value;
	//u16 input_value;

pr_err("%s ++\n",__func__);

	if (copy_from_user(mode, buffer, sizeof(mode))) return count;

      if (sscanf(mode, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}
	pu_en = input_value;
	if(pu_en==1){
		drx_x3 = (pu_x / 256) ;
		drx_x4 = (pu_x % 256) ;
		drx_y5 = (pu_y / 256) ;
		drx_y6 = (pu_y % 256) ;
		drx_w7 = (pu_w / 256) ;
		drx_w8 = (pu_w % 256) ;
		drx_l9 = (pu_l / 256) ;
		drx_l10 = (pu_l % 256) ;

		}
	else{ //not enable
       	drx_x3=0x00;
		drx_x4=0x00;
		drx_y5=0x00;
		drx_y6=0x00;
		drx_w7=0x02;
		drx_w8=0x58;
		drx_l9=0x01;
		drx_l10=0xE0;
		}

pr_err("%s, mode=%s ,input_value=%d --\n",__func__,mode,input_value);
	return count;
	//return 0;
}


static int proc_epd_pu_en_open(struct inode *inode, struct file *file)
{
pr_err("%s ++\n",__func__);
    return single_open(file, proc_epd_pu_en_show, NULL);
pr_err("%s --\n",__func__);
}

static const struct file_operations proc_epd_pu_en_fops = {
    .open  = proc_epd_pu_en_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = proc_epd_pu_en_write,
    .read = seq_read,
};

static int proc_epd_pu_x_show(struct seq_file *m, void *v)
{
pr_err("%s ++\n",__func__);
    	//seq_printf(m, "GL16");
seq_printf(m, "%d",pu_x);
pr_err("%s --\n",__func__);
    return 0;
}

static ssize_t proc_epd_pu_x_write(struct file *file, const char __user *buffer,
				    size_t count, loff_t *pos)
{
	char mode[]="0x00000000";

	u16 input_value;

pr_err("%s ++\n",__func__);

	if (copy_from_user(mode, buffer, sizeof(mode))) return count;

      if (sscanf(mode, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	pu_x = input_value;

pr_err("%s, mode=%s ,input_value=%d --\n",__func__,mode,input_value);
	return count;
	//return 0;
}


static int proc_epd_pu_x_open(struct inode *inode, struct file *file)
{
pr_err("%s ++\n",__func__);
    return single_open(file, proc_epd_pu_x_show, NULL);
pr_err("%s --\n",__func__);
}

static const struct file_operations proc_epd_pu_x_fops = {
    .open  = proc_epd_pu_x_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = proc_epd_pu_x_write,
    .read = seq_read,
};

static int proc_epd_pu_y_show(struct seq_file *m, void *v)
{
pr_err("%s ++\n",__func__);
    	//seq_printf(m, "GL16");
seq_printf(m, "%d",pu_y);
pr_err("%s --\n",__func__);
    return 0;
}

static ssize_t proc_epd_pu_y_write(struct file *file, const char __user *buffer,
				    size_t count, loff_t *pos)
{
	char mode[]="0x00000000";

	u16 input_value;

pr_err("%s ++\n",__func__);

	if (copy_from_user(mode, buffer, sizeof(mode))) return count;

      if (sscanf(mode, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	pu_y = input_value;

pr_err("%s, mode=%s ,input_value=%d --\n",__func__,mode,input_value);
	return count;
	//return 0;
}


static int proc_epd_pu_y_open(struct inode *inode, struct file *file)
{
pr_err("%s ++\n",__func__);
    return single_open(file, proc_epd_pu_y_show, NULL);
pr_err("%s --\n",__func__);
}

static const struct file_operations proc_epd_pu_y_fops = {
    .open  = proc_epd_pu_y_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = proc_epd_pu_y_write,
    .read = seq_read,
};

static int proc_epd_pu_w_show(struct seq_file *m, void *v)
{
pr_err("%s ++\n",__func__);
    	//seq_printf(m, "GL16");
seq_printf(m, "%d",pu_w);
pr_err("%s --\n",__func__);
    return 0;
}

static ssize_t proc_epd_pu_w_write(struct file *file, const char __user *buffer,
				    size_t count, loff_t *pos)
{
	char mode[]="0x00000000";

	u16 input_value;

pr_err("%s ++\n",__func__);

	if (copy_from_user(mode, buffer, sizeof(mode))) return count;

      if (sscanf(mode, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	pu_w = input_value;

pr_err("%s, mode=%s ,input_value=%d --\n",__func__,mode,input_value);
	return count;
	//return 0;
}

static int proc_epd_pu_w_open(struct inode *inode, struct file *file)
{
pr_err("%s ++\n",__func__);
    return single_open(file, proc_epd_pu_w_show, NULL);
pr_err("%s --\n",__func__);
}

static const struct file_operations proc_epd_pu_w_fops = {
    .open  = proc_epd_pu_w_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = proc_epd_pu_w_write,
    .read = seq_read,
};

static int proc_epd_pu_l_show(struct seq_file *m, void *v)
{
pr_err("%s ++\n",__func__);
    	//seq_printf(m, "GL16");
seq_printf(m, "%d",pu_l);
pr_err("%s --\n",__func__);
    return 0;
}

static ssize_t proc_epd_pu_l_write(struct file *file, const char __user *buffer,
				    size_t count, loff_t *pos)
{
	char mode[]="0x00000000";

	u16 input_value;

pr_err("%s ++\n",__func__);

	if (copy_from_user(mode, buffer, sizeof(mode))) return count;

      if (sscanf(mode, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	pu_l = input_value;

pr_err("%s, mode=%s ,input_value=%d --\n",__func__,mode,input_value);
	return count;
	//return 0;
}

static int proc_epd_pu_l_open(struct inode *inode, struct file *file)
{
pr_err("%s ++\n",__func__);
    return single_open(file, proc_epd_pu_l_show, NULL);
pr_err("%s --\n",__func__);
}

static const struct file_operations proc_epd_pu_l_fops = {
    .open  = proc_epd_pu_l_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = proc_epd_pu_l_write,
    .read = seq_read,
};

static ssize_t epd_wf_mode_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u8 input_value;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}

	 if(input_value ==0){
            	pr_err("[R]%s mode=0 \n",__func__);
			if(input_value==wf_mode){
				pr_err("[R]%s waveform mode already is 0 \n",__func__);
				}
			else{
				wf_mode = input_value;
				change_waveform_mode(g_pdata,0);
				}
		}
	else if(input_value ==1){
            	pr_err("[R]%s mode=1 \n",__func__);
			if(input_value==wf_mode){
				pr_err("[R]%s waveform mode already is 1 \n",__func__);
				}
			else{
				wf_mode = input_value;
				change_waveform_mode(g_pdata,1);
				}
		}
	else if(input_value ==2){
            	pr_err("[R]%s mode=2 \n",__func__);
			if(input_value==wf_mode){
				pr_err("[R]%s waveform mode already is 2 \n",__func__);
				}
			else{
				wf_mode = input_value;
				change_waveform_mode(g_pdata,2);
				}
		}
	else if(input_value ==3){
            	pr_err("[R]%s mode=3 \n",__func__);
			if(input_value==wf_mode){
				pr_err("[R]%s waveform mode already is 3 \n",__func__);
				}
			else{
				wf_mode = input_value;
				change_waveform_mode(g_pdata,3);
				}
		}
	else if(input_value ==4){
            	pr_err("[R]%s mode=4 \n",__func__);
			if(input_value==wf_mode){
				pr_err("[R]%s waveform mode already is 4 \n",__func__);
				}
			else{
				wf_mode = input_value;
				change_waveform_mode(g_pdata,4);
				}
		}
	else{
		input_value =3;
		pr_err("[R]%s default mode=3 \n",__func__);
		wf_mode = input_value;
		change_waveform_mode(g_pdata,3);
		}

pr_err("%s --\n",__func__);
	return count;
}


static ssize_t epd_get_wf_mode(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "wf_mode=%d \n",wf_mode);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_pu_en_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u8 input_value;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}

	 pu_en = input_value;
	if(pu_en==1){
		pu_rx=pu_x;
		pu_ry=pu_y;
		DYN_PARTIAL_UPDATE=0;
		pu_ry=479-(pu_w+pu_x);
		pu_rx=599-(pu_l+pu_y);
		if(pu_ry<0)
			pu_ry=0;
		if(pu_ry>1023)
			pu_ry=1023;
		if(pu_rx<0)
			pu_rx=0;
		if(pu_rx>1023)
			pu_rx=1023;
		drx_x3 = (pu_rx / 256) ;
		drx_x4 = (pu_rx % 256) ;
		drx_y5 = (pu_ry / 256) ;
		drx_y6 = (pu_ry % 256) ;
		drx_w7 = (pu_l / 256) ;
		drx_w8 = (pu_l % 256) ;
		drx_l9 = (pu_w / 256) ;
		drx_l10 = (pu_w % 256) ;
		//[4101]Dynamic partial update begin
		parm_b5[2]=drx_x3;
		parm_b5[3]=drx_x4;
		parm_b5[4]=drx_y5;
		parm_b5[5]=drx_y6;
		parm_b5[6]=drx_w7;
		parm_b5[7]=drx_w8;
		parm_b5[8]=drx_l9;
		parm_b5[9]=drx_l10;
		parm_dtmw_partial[1]=drx_x3;
		parm_dtmw_partial[2]=drx_x4;
		parm_dtmw_partial[3]=drx_y5;
		parm_dtmw_partial[4]=drx_y6;
		parm_dtmw_partial[5]=drx_w7;
		parm_dtmw_partial[6]=drx_w8;
		parm_dtmw_partial[7]=drx_l9;
		parm_dtmw_partial[8]=drx_l10;
		//[1401]Dynamic partial update end
		
		}
	else{ //not enable
		//DYN_PARTIAL_UPDATE=1;
		has_pre_frame=false;
       	drx_x3=0x00;
		drx_x4=0x00;
		drx_y5=0x00;
		drx_y6=0x00;
		drx_w7=0x02;
		drx_w8=0x58;
		drx_l9=0x01;
		drx_l10=0xE0;
		//[4101]Dynamic partial update begin
		parm_b5[2]=drx_x3;
		parm_b5[3]=drx_x4;
		parm_b5[4]=drx_y5;
		parm_b5[5]=drx_y6;
		parm_b5[6]=drx_w7;
		parm_b5[7]=drx_w8;
		parm_b5[8]=drx_l9;
		parm_b5[9]=drx_l10;
		parm_dtmw_partial[1]=drx_x3;
		parm_dtmw_partial[2]=drx_x4;
		parm_dtmw_partial[3]=drx_y5;
		parm_dtmw_partial[4]=drx_y6;
		parm_dtmw_partial[5]=drx_w7;
		parm_dtmw_partial[6]=drx_w8;
		parm_dtmw_partial[7]=drx_l9;
		parm_dtmw_partial[8]=drx_l10;
		//[1401]Dynamic partial update end
		}

pr_err("%s partial_update_en=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_pu_en(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "partial_update_en=%d \n",pu_en);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_pu_x_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u16 input_value;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hu", &input_value) != 1) {
		return -EINVAL;
	}

	 if(input_value>479)
		input_value=479;
	//input_value=479 -input_value;
	pu_x = input_value;

pr_err("%s partial_update_x=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_pu_x(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "partial_update_x=%d \n",pu_x);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_pu_y_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u16 input_value;
	int remainder;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	//[4101][Raymond] Fixed jira PRJ4101-292 - begin
	remainder = input_value%4;
	if(remainder!=0)
		input_value = input_value -(4-remainder);
	
	//[4101][Raymond] Fixed jira PRJ4101-292 - end	 
	
	 if(input_value>599)
		input_value=599;
	//input_value=599 -input_value;
	pu_y = input_value;

pr_err("%s partial_update_y=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_pu_y(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "partial_update_y=%d \n",pu_y);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_pu_w_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u16 input_value;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	 if(input_value>480)
		input_value=480;
	//input_value=479 -input_value;
	pu_w = input_value;

pr_err("%s partial_update_w=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_pu_w(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "partial_update_w=%d \n",pu_w);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_pu_l_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u16 input_value;
	int remainder;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hu", &input_value) != 1) {
		return -EINVAL;
	}
	//[4101][Raymond] Fixed jira PRJ4101-292 - begin
	remainder = input_value%4;
	if(remainder!=0)
		input_value = input_value+(4-remainder);
	
	//[4101][Raymond] Fixed jira PRJ4101-292 - end	 
	 if(input_value>600)
		input_value=600;
	//input_value=599 -input_value;
	pu_l = input_value;

pr_err("%s partial_update_L=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_pu_l(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "partial_update_L=%d \n",pu_l);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_Bflash_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u8 input_value;
	pr_err("%s ++\n",__func__);

	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}
//[4101][Raymond]impelment PRJ4101-128 Bug 2a -begin
	 //bflash = input_value;
	if(input_value==1){ //flash image
		//DYN_PARTIAL_UPDATE=0;
		bflash=1;
		done_flash=0;
		//raymond test image begin
		update_epd(g_pdata,bflash);
		//raymond test image end
		}
	else if(input_value==2){ //flash image
		//DYN_PARTIAL_UPDATE=0;
		bflash=2;
		done_flash=0;
		//raymond test image begin
		update_epd(g_pdata,bflash);
		//raymond test image end
		}
	else if(input_value==3){ //flash image
		//DYN_PARTIAL_UPDATE=0;
		bflash=3;
		done_flash=0;
		//raymond test image begin
		update_epd(g_pdata,bflash);
		//raymond test image end
		}
	else{ //not flash image
		//DYN_PARTIAL_UPDATE=1;
		//has_pre_frame=false;
		bflash=0;
		}
//[4101][Raymond]impelment PRJ4101-128 Bug 2a -end
pr_err("%s bflash=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_Bflash(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "bflash=%d \n",bflash);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_os_mode_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{

	u8 input_value;
	pr_err("%s ++\n",__func__);
//[4101][Raymond] Implement call back feature - begin	
	g_dev = dev;
	gdev_done=1;
//[4101][Raymond] Implement call back feature - end
	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}

	 os_mode = input_value;
	if(os_mode==1){ // light phone os
		DYN_PARTIAL_UPDATE=0;
		bflash=0;
		os_mode=1;
		}
	else{ //android os
		DYN_PARTIAL_UPDATE=1;
		bflash=1;
		os_mode=0;

		}

pr_err("%s os_mode=%d --\n",__func__,input_value);

	return count;
}

static ssize_t epd_get_os_mode(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "os_mode=%d \n",os_mode);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

static ssize_t epd_done_flash_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	return count;
}

static ssize_t epd_get_done_flash(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "%d\n",done_flash);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}

//[4101][Raymond]picture of the battery full for off-charging -begin
static ssize_t epd_battery_full_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	u8 input_value;
	pr_err("%s ++\n",__func__);

//[4101][Raymond] Implement call back feature - begin	
	g_dev = dev;
	gdev_done=1;
//[4101][Raymond] Implement call back feature - end
	 //sscanf(buf, "%d %d", &data[0], &data[1]);
	     if (sscanf(buf, "%hhu", &input_value) != 1) {
		return -EINVAL;
	}
	if(input_value==0 ||input_value==1){	 
	battery_full=input_value;
		}
	else{
 	pr_err("%s,out of range \n",__func__);
		}
	pr_err("%s battery_full=%d --\n",__func__,input_value);	
	return count;
}

static ssize_t epd_get_battery_full(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	pr_err("%s ++\n",__func__);
	ret = snprintf(buf, PAGE_SIZE, "%d\n",battery_full);
	pr_err("%s -- ret =%d \n",__func__,ret);
	return ret;
}
//[4101][Raymond]picture of the battery full for off-charging -end
static DEVICE_ATTR(wf_mode, 0664, epd_get_wf_mode, epd_wf_mode_store);
static DEVICE_ATTR(partial_update_en, 0664, epd_get_pu_en, epd_pu_en_store);
static DEVICE_ATTR(partial_update_x, 0664, epd_get_pu_x, epd_pu_x_store);
static DEVICE_ATTR(partial_update_y, 0664, epd_get_pu_y, epd_pu_y_store);
static DEVICE_ATTR(partial_update_w, 0664, epd_get_pu_w, epd_pu_w_store);
static DEVICE_ATTR(partial_update_l, 0664, epd_get_pu_l, epd_pu_l_store);
static DEVICE_ATTR(Bflash, 0664, epd_get_Bflash, epd_Bflash_store);
static DEVICE_ATTR(os_mode, 0664, epd_get_os_mode, epd_os_mode_store);
static DEVICE_ATTR(done_flash, 0664, epd_get_done_flash, epd_done_flash_store);
//[4101][Raymond]picture of the battery full for off-charging -begin
static DEVICE_ATTR(battery_full, 0664, epd_get_battery_full, epd_battery_full_store);
//[4101][Raymond]picture of the battery full for off-charging -end
//static DEVICE_ATTR(strobe, 0664, NULL, led_strobe_type_store);

static struct attribute *epd_attrs[] = {
	&dev_attr_wf_mode.attr,
	&dev_attr_partial_update_en.attr,
	&dev_attr_partial_update_x.attr,
	&dev_attr_partial_update_y.attr,
	&dev_attr_partial_update_w.attr,
	&dev_attr_partial_update_l.attr,
	&dev_attr_Bflash.attr,
	&dev_attr_os_mode.attr,
	&dev_attr_done_flash.attr,
	&dev_attr_battery_full.attr, //[4101][Raymond]picture of the battery full for off-charging
	NULL
};

const struct attribute_group epd_attr_group = {
	.attrs = epd_attrs,
};

static int mdss_spi_panel_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct spi_panel_data	*ctrl_pdata;
	struct mdss_panel_cfg *pan_cfg = NULL;
	struct device_node *spi_pan_node = NULL;
	bool cmd_cfg_cont_splash = true;
	char panel_cfg[MDSS_MAX_PANEL_LEN];
	struct mdss_util_intf *util;
	const char *ctrl_name;
	struct proc_dir_entry *dir, *res;
	int r=0;
	signed int irqn;

#if 1
pr_err("%s ++\n",__func__);
#endif
	util = mdss_get_util_intf();
	if (util == NULL) {
		pr_err("Failed to get mdss utility functions\n");
		return -ENODEV;
	}

	if (!util->mdp_probe_done) {
		pr_err("%s: MDP not probed yet\n", __func__);
		return -EPROBE_DEFER;
	}

	if (!pdev->dev.of_node) {
		pr_err("SPI driver only supports device tree probe\n");
		return -ENOTSUPP;
	}

	pan_cfg = util->panel_intf_type(MDSS_PANEL_INTF_DSI);
	if (IS_ERR(pan_cfg)) {
		pr_err("%s: return MDSS_PANEL_INTF_DSI\n", __func__);
		return PTR_ERR(pan_cfg);
	} else if (pan_cfg) {
		pr_err("%s: DSI is primary\n", __func__);
		return -ENODEV;
	}

	ctrl_pdata = platform_get_drvdata(pdev);
	if (!ctrl_pdata) {
		ctrl_pdata = devm_kzalloc(&pdev->dev,
					  sizeof(struct spi_panel_data),
					  GFP_KERNEL);
		if (!ctrl_pdata) {
			pr_err("%s: FAILED: cannot alloc spi panel\n",
			       __func__);
			rc = -ENOMEM;
			goto error_no_mem;
		}
		platform_set_drvdata(pdev, ctrl_pdata);
	}

	ctrl_pdata->mdss_util = util;

	ctrl_name = of_get_property(pdev->dev.of_node, "label", NULL);
	if (!ctrl_name)
		pr_err("%s:%d, Ctrl name not specified\n",
			__func__, __LINE__);
	else
		pr_err("%s: Ctrl name = %s\n",
			__func__, ctrl_name);


	rc = of_platform_populate(pdev->dev.of_node,
				  NULL, NULL, &pdev->dev);
	if (rc) {
		dev_err(&pdev->dev,
			"%s: failed to add child nodes, rc=%d\n",
			__func__, rc);
		goto error_no_mem;
	}

	rc = mdss_spi_panel_pinctrl_init(pdev);
	if (rc)
		pr_warn("%s: failed to get pin resources\n", __func__);

	rc = mdss_spi_get_panel_vreg_data(&pdev->dev,
					&ctrl_pdata->panel_power_data);
	if (rc) {
		dev_err(&pdev->dev,
			"%s: failed to get panel vreg data, rc=%d\n",
			__func__, rc);
		goto error_vreg;
	}

	/* SPI panels can be different between controllers */
	rc = mdss_spi_get_panel_cfg(panel_cfg, ctrl_pdata);
	if (!rc)
		/* spi panel cfg not present */
		pr_warn("%s:%d:spi specific cfg not present\n",
			__func__, __LINE__);

	/* find panel device node */
	spi_pan_node = mdss_spi_find_panel_of_node(pdev, panel_cfg);
	if (!spi_pan_node) {
		pr_err("%s: can't find panel node %s\n", __func__, panel_cfg);
		goto error_pan_node;
	}

	cmd_cfg_cont_splash = true;

	rc = mdss_spi_panel_init(spi_pan_node, ctrl_pdata, cmd_cfg_cont_splash);
	if (rc) {
		pr_err("%s: spi panel init failed\n", __func__);
		goto error_pan_node;
	}

	rc = spi_panel_device_register(spi_pan_node, ctrl_pdata);
	if (rc) {
		pr_err("%s: spi panel dev reg failed\n", __func__);
		goto error_pan_node;
	}

	ctrl_pdata->panel_data.event_handler = mdss_spi_panel_event_handler;


	init_completion(&ctrl_pdata->spi_panel_te);
	mutex_init(&ctrl_pdata->spi_tx_mutex);
	already_probe=true;//2018/10/23,Yuchen
#if 0
	rc = devm_request_irq(&pdev->dev,
		gpio_to_irq(ctrl_pdata->disp_te_gpio),
		spi_panel_te_handler, IRQF_TRIGGER_RISING,
		"TE_GPIO", ctrl_pdata);
	if (rc) {
		pr_err("TE request_irq failed.\n");
		return rc;
	}
#endif
#if 1
	init_waitqueue_head(&ctrl_pdata->busy_wq);
	rc = gpio_request(ctrl_pdata->disp_busy_gpio, "disp_busy");
	if (rc) {
		pr_err("disp_busy: gpio request failed\n");
		goto error_busy_request;
	}

	rc = gpio_direction_input(ctrl_pdata->disp_busy_gpio);
	if (rc < 0) {
		pr_err("disp_busy: gpio direction_input failed\n");
		goto error_busy_request;
	}

	rc =irqn = gpio_to_irq(ctrl_pdata->disp_busy_gpio);
	if(rc<0){
		pr_err("disp_busy: gpio to irq failed\n");
		goto error_busy_request;
	}

	//r = request_irq(irq, epd_irq_handler,
			  //IRQF_TRIGGER_HIGH, "EPD", ctrl_pdata);
	ctrl_pdata->irq_enable = true;		  
	r=devm_request_irq(&pdev->dev,irqn,epd_irq_handler,IRQF_TRIGGER_RISING,"EPD_IRQ",ctrl_pdata);	  
	if (r) {
		pr_err("disp_busy: devm_request_irq fail \n");
		goto error_busy_request;
		}
	
	disable_irq(irqn);
	//epd_disable_irq(ctrl_pdata);
	ctrl_pdata->irq = irqn;
	ctrl_pdata->irq_enable = false;
#endif
//[4101][Raymond]three seconds Power Off command Implementation - begin
	INIT_DELAYED_WORK(&epd_pof_work, epd_pof_cmd);
//[4101][Raymond]three seconds Power Off command Implementation - end
/*[4101][Raymond] implement EPD vcom read write command  begin*/
	{
		const char *vcom_buf = NULL;
		vcom_buf = strstr(saved_command_line, "androidboot.lcm_vcom=");
		if (vcom_buf) {

			char temp_buf[128]={0};

			 u8 vcom_1;
			 u8 vcom_2;
			 int vcom_int;
			int vcom_int_2;

                     pr_err("%s(%d): vcom_buf = %s \n", __func__, __LINE__,vcom_buf);

			sscanf(vcom_buf, "%s%hhu.%hhu", temp_buf, &vcom_1, &vcom_2);
			sscanf(vcom_buf, "androidboot.lcm_vcom=-%d.%d", &vcom_int, &vcom_int_2);

			//pr_err("%s(%d): temp_buf=%s  vcom_1=%d, vcom_2=%d \n", __func__, __LINE__, temp_buf,vcom_1,vcom_2);
			//pr_err("%s(%d): vcom_int=%d, vcom_int_2=%d \n", __func__, __LINE__, vcom_int,vcom_int_2);

                    if(temp_buf[21] == '-'){
                        pr_err("%s(%d): write vcom, temp_buf[21]=%c \n", __func__, __LINE__,temp_buf[21]);
			   vcom_value = ((vcom_int*100) + vcom_int_2) /5 ;
			   para_vcom[1]=vcom_value;
			   pr_err("%s(%d): vcom=-%d.%d ,para_vcom[0]=0x%X, para_vcom[1]=0x%X \n", __func__, __LINE__, vcom_int,vcom_int_2,para_vcom[0],para_vcom[1]);
                    	}
		else{
                        pr_err("%s(%d): no write vcom, use default vcom \n", __func__, __LINE__);
			}

		}
	}
/*[4101][Raymond] implement EPD vcom read write command  end*/
//[4101][Raymond]off charging icon - begin
	{
		const char *color_id_buf = NULL;
		color_id_buf = strstr(saved_command_line, "androidboot.color_id=");
		if (color_id_buf) {

			int color_int;
			sscanf(color_id_buf, "androidboot.color_id=%d", &color_int);
			if(color_int==2){ //white
					pr_err("%s(%d): white color \n", __func__, __LINE__);
					color_id=2;
				}
			else{ //black
					pr_err("%s(%d): black color \n", __func__, __LINE__);
					color_id=1;
				}


		}
		else{
        			pr_err("%s(%d): black color \n", __func__, __LINE__);
				color_id=1;

			}
	}

	{
		const char *bootmode_buf = NULL;
		bootmode_buf = strstr(saved_command_line, "androidboot.mode=charger");
		if (bootmode_buf) {

                     pr_err("%s(%d): off charging mode \n", __func__, __LINE__);
			chg_mode=1;
		}
		else{
        		pr_err("%s(%d): Normal boot\n", __func__, __LINE__);
			chg_mode=0;
			}
	}
//[4101][Raymond]off charging icon - end
	//<2019/02/25,Yuchen-[4101] add for recovery mode
	{
		const char *bootmode_recov_buf = NULL;
		bootmode_recov_buf = strstr(saved_command_line, "androidboot.rcymode");
		if(bootmode_recov_buf){
			pr_err("[YC]%s(%d): recovery mode \n", __func__, __LINE__);
			//2019/03/20,Yuchen erase workaround chg_mode=2;//for recovery only
			rcy_mode=true;//2019/03/20,Yuchen-[4101]deal with recovery UI issue & disable workaround
		}
	}
	//>2019/02/25,Yuchen

	pr_err("proc_create lcm_vendor \n");
//[4101][LCM][RaymondLin] Add LCM_vendor file node for PCBA function test begin
    proc_create("lcm_vendor", 0, NULL, &proc_lcm_vendor_fops);
//[4101][LCM][RaymondLin] Add LCM_vendor file node for PCBA function test end

	pr_err("proc_create epd/wf_mode \n");
	dir = proc_mkdir("epd", NULL);
	if (!dir)
		return -ENOMEM;

	res = proc_create("epd/wf_mode", 0, NULL, &proc_epd_wf_fops);
	if (!res)
		return -ENOMEM;
	res = proc_create("epd/partial_update_en", S_IWUSR | S_IRUGO, NULL, &proc_epd_pu_en_fops);
	if (!res)
		return -ENOMEM;
	res = proc_create("epd/partial_update_x", S_IWUSR | S_IRUGO, NULL, &proc_epd_pu_x_fops);
	if (!res)
		return -ENOMEM;
	res = proc_create("epd/partial_update_y", S_IWUSR | S_IRUGO, NULL, &proc_epd_pu_y_fops);
	if (!res)
		return -ENOMEM;
	res = proc_create("epd/partial_update_w", S_IWUSR | S_IRUGO, NULL, &proc_epd_pu_w_fops);
	if (!res)
		return -ENOMEM;
	res = proc_create("epd/partial_update_l", S_IWUSR | S_IRUGO, NULL, &proc_epd_pu_l_fops);
	if (!res)
		return -ENOMEM;
#if 1
pr_err("%s --\n",__func__);
#endif
	pr_debug("%s: spi panel  initialized\n", __func__);
	return 0;

error_busy_request:
      gpio_free(ctrl_pdata->disp_busy_gpio);
error_pan_node:
	of_node_put(spi_pan_node);
error_vreg:
	mdss_spi_put_dt_vreg_data(&pdev->dev,
			&ctrl_pdata->panel_power_data);
error_no_mem:
	devm_kfree(&pdev->dev, ctrl_pdata);
	return rc;
}


static const struct of_device_id mdss_spi_panel_match[] = {
	{ .compatible = "qcom,mdss-spi-display" },
	{},
};

static struct platform_driver this_driver = {
	.probe = mdss_spi_panel_probe,
	.driver = {
		.name = "spi_panel",
		.owner  = THIS_MODULE,
		.of_match_table = mdss_spi_panel_match,
	},
};

static int __init mdss_spi_display_init(void)
{
	int ret;
#if 1
pr_err("%s ++\n",__func__);
#endif
	ret = platform_driver_register(&this_driver);
#if 1
pr_err("%s --\n",__func__);
#endif
	return 0;
}
module_init(mdss_spi_display_init);

MODULE_DEVICE_TABLE(of, mdss_spi_panel_match);
MODULE_LICENSE("GPL v2");
