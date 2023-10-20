//*************************************************************************
//*************************************************************************
/**
 * \file		sensor_predict.c
 *
 * \brief		The prediction of the sensor.
 *
 * \copyright	CARRIER CONFIDENTIAL & PROPRIETARY
 *				COPYRIGHT, CARRIER CORPORATION, 2020
 *				UNPUBLISHED WORK, ALL RIGHTS RESERVED
 *
 * \author		Julien Wang
*/
//*************************************************************************
//*************************************************************************
#include "sensor_predict.h"
#include "refrigerant_property.h"
#include "compressor_model.h"
#include <stdio.h>
#include <math.h>


//-------------------------------------------------------------------------------------------------
/**
 * \fn			pred_Tdis()
 *
 * \brief		Predict temperature of discharge gas.
 *
 * \param[in]	p_suc = suction gas pressure in kPa.
 * \param[in]	t_suc = suction gas temperature in ℃.
 * \param[in]	p_dis = discharge gas pressure in kPa.
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		discharge gas temperature in ℃.
*/
//-------------------------------------------------------------------------------------------------
float pred_Tdis(float p_suc, float t_suc, float p_dis, float compSpeed)
{
	float volume_flow_rate, power;
	float z_fw, coe_a, coe_b, coe_c;
	float ts_suc;	//ts:temperature of saturation suction gas
	float ts_dis;	//ts_dis:temperaturs of saturation discharge gas
	float dens_gas;	//dens_gas:density of scution gas.
	float mr;	//mr:density and flow rate.
	float h_suc;	//h_suc:enthalpy of suction gas
	float h_dis;	//h_dis:enthalpy of discharge gas
	float hs_dis;	//hs_dis:enthalpy of saturation discharge gas
	float ssh;	//ssh:superheated of suction gas
	float t_dis;	//t_dis:temperaturs of discharge gas

	/* Calculated volume flow rate. */
	volume_flow_rate = cal_volume_flow_rate(p_dis, p_suc, compSpeed);

	/* Calculated power */
	power = cal_power(p_dis, p_suc, compSpeed);

	/* Calculated saturation temperature. */
	ts_suc = cal_t_sat(p_suc);

	/* Calculated superheated of suction gas */
	ssh = t_suc - ts_suc;

	/* Calculated density of suction gas. */
	if (ssh > 1)
		dens_gas = cal_dens_sh_gas(p_suc, t_suc);
	else
		dens_gas = 1/cal_vol_sat_gas(p_suc);

	/* Calculated compressor density and flow rate. */
	mr = volume_flow_rate*dens_gas;

	/* Calculated enthalpy of suction gas */
	if (ssh > 1)
		h_suc = cal_h_sh_gas(p_suc, t_suc);
	else
		h_suc = cal_h_sat_gas(p_suc);

	/* Calculated enthalpy of discharge gas */
	if (ssh < 2)
		z_fw = 0.2 * ssh + 0.6;
	else
		z_fw = 1;
	h_dis = (power * FW * z_fw) / mr + h_suc;

	/* calculate coefficient of coe_a,coe_b,coe_c. */
	/* temperaturs of discharge saturation gas */
	ts_dis = cal_t_sat(p_dis);
	/*
		3.62592×10^(-7)-18.47693×10^(-8)×ts_dis-60.2765×10^(-10)×ts_dis^2
	*/
	coe_a = 3.62592*pow(10,(-7))-18.47693*pow(10,(-8))*
			ts_dis-60.2765*pow(10,(-10))*pow(ts_dis,2);
	/*
		3.3247×10^(-3)-2×3.62592×10^(-7)×ts_dis+30.40633×10^(-6)×ts_dis
		+2×18.47693×10^(-8)×ts_dis^2+76.64206×10^(-8)×ts_dis^2
		+2×60.2765×10^(-10)×ts_dis^3
	*/
	coe_b = 3.3247*pow(10,(-3))-2*3.62592*pow(10,(-7))*ts_dis+30.40633*pow(10,(-6))*
			ts_dis+2*18.47693*pow(10,(-8))*pow(ts_dis,2)+76.64206*pow(10,(-8))*
			pow(ts_dis,2)+2*60.2765*pow(10,(-10))*pow(ts_dis,3);
	/*
		1-3.3247×10^(-3)×ts_dis+3.62592×10^(-7)×ts_dis^2-30.40633×10^(-6)
		×ts_dis^2-18.47693×10^(-8)×ts_dis^3-76.64206×10^(-8)×ts_dis^3-60.2765
		×10^(-10)×ts_dis^4-h_dis/hs_dis
	*/
	hs_dis = cal_h_sat_gas(p_dis);
	coe_c = 1-3.3247*pow(10,(-3))*ts_dis+3.62592*pow(10,(-7))*pow(ts_dis,2)-30.40633*
			pow(10,(-6))*pow(ts_dis,2)-18.47693*pow(10,(-8))*pow(ts_dis,3)-76.64206*
			pow(10,(-8))*pow(ts_dis,3)-60.2765*pow(10,(-10))*pow(ts_dis,4)-h_dis/hs_dis;

	/* calculate temperature of discharge gas.(-b+SQRT(b^2-4*a*c))/(2*a) */
	float sqrt_num;
	sqrt_num = pow(coe_b,2)-4*coe_a*coe_c;
	if (sqrt_num >= 0)
		t_dis = (-coe_b+sqrt(sqrt_num))/(2*coe_a);
	else
		t_dis = 150;

	return t_dis;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			pred_Pdis()
 *
 * \brief		Predict pressure of discharge gas.
 *
 * \param[in]	p_suc = suction gas pressure in kPa.
 * \param[in]	t_suc = suction gas temperature in ℃.
 * \param[in]	t_dis = discharge gas temperature in ℃.
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		discharge gas pressure in kPa.
*/
//-------------------------------------------------------------------------------------------------
float pred_Pdis(float p_suc, float t_suc, float t_dis, float compSpeed)
{
	float pd_int1 = 100, pd_int2=4300, pd_int, hd_int;
	float v_flow, power;
	float dens_gas;	//dens_gas:density of scution gas.
	float ts_suc;	//ts:temperature of saturation suction gas
	float ssh;	//ssh:superheated of suction gas
	float mr;	//mr:density and flow rate.
	float h_suc;	//h_suc:enthalpy of suction gas
	float h_dis;	//h_dis:enthalpy of discharge gas

	for (size_t i = 0; i < 100; i++)
	{
		pd_int = (pd_int1+pd_int2)/2;

		/* Calculated volume flow rate. */
		v_flow = cal_volume_flow_rate(pd_int, p_suc, compSpeed);

		/* Calculated power */
		power = cal_power(pd_int, p_suc, compSpeed);

		/* Calculated saturation temperature. */
		ts_suc = cal_t_sat(p_suc);

		/* Calculated superheated of suction gas */
		ssh = t_suc - ts_suc;

		/* Calculated density of suction gas. */
		if (ssh > 1)
			dens_gas = cal_dens_sh_gas(p_suc, t_suc);
		else
			dens_gas = 1/cal_vol_sat_gas(p_suc);

		/* Calculated compressor density and flow rate. */
		mr = v_flow*dens_gas;

		/* Calculated enthalpy of suction gas */
		if (ssh > 1)
			h_suc = cal_h_sh_gas(p_suc, t_suc);
		else
			h_suc = cal_h_sat_gas(p_suc);

		/* Calculated enthalpy of discharge gas */
		h_dis = (power * FW) / mr + h_suc;

		/* Calculated enthalpy of int discharge gas */
		hd_int = cal_h_sh_gas(pd_int, t_dis);

		/* reset pd_int1 or pd_int2 */
		if (fabs(hd_int - h_dis) < 0.1)
			break;
		else
		{
			if (hd_int < h_dis)
				pd_int2 = pd_int;
			else
				pd_int1 = pd_int;
		}
	}

	return pd_int;
}










void sensor_pre_test(void)
{
	float	Pd = 2152.59, Ps = 1390.88, CompSpeed = 1740, ST = 20.54;
	float t_dis, p_dis;

	t_dis = pred_Tdis(Ps, ST, Pd, CompSpeed);
	p_dis = pred_Pdis(641, -2.23, 56.39, 7080);

}


