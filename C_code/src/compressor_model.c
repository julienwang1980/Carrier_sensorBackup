//*************************************************************************
//*************************************************************************
/**
 * \file		compressor_model.c
 *
 * \brief		The model of the compressor
 *
 * \copyright	CARRIER CONFIDENTIAL & PROPRIETARY
 *				COPYRIGHT, CARRIER CORPORATION, 2020
 *				UNPUBLISHED WORK, ALL RIGHTS RESERVED
 *
 * \author		Julien Wang
*/
//*************************************************************************
//*************************************************************************
#include "compressor_model.h"
#include <stdio.h>
#include <math.h>


//-------------------------------------------------------------------------------------------------
/**
 * \const	COE_32[]
 * \brief   This array defines the coefficient of compressor model
 */
//-------------------------------------------------------------------------------------------------
static const float COE_32[] = {	97.067,		-177.99,		297.6,		20.081,		11.098,		-1.8449,		0.44883,	0,
								0,			0.65281,		0,			0,			0.096619,	-0.029134,		0.011636,	-0.11126,
								0.073423,	-0.024061,		2.4395,		0.029512,	-119.08,	-85.79,			12.689,		-0.00026992,
								0.00047164,	-0.00019762,	0.3311,		-0.53155,	0.18157,	0.0000024884,	390.25,		-150.24
							};



//-------------------------------------------------------------------------------------------------
/**
 * \def
 * \brief		Define the calculation of intermediate coefficients by initial coefficients.
*/
//-------------------------------------------------------------------------------------------------
#define PR(pd, ps) ((pd)/(ps))	//pd:discharge pressure, ps:suction pressure
#define SR(compSpeed) ((compSpeed)/(COMPSPEED_RATED))

#define COE_A(compSpeed) (COE_32[0]+COE_32[1]*sqrt(SR(compSpeed))+COE_32[2]*SR(compSpeed))
#define COE_B(compSpeed) (COE_32[3]+COE_32[4]*pow(SR(compSpeed), 2)+COE_32[5]*pow(SR(compSpeed), 4))
#define COE_C(compSpeed) (COE_32[6]+COE_32[7]*SR(compSpeed)+COE_32[8]*pow(SR(compSpeed), 2))
#define COE_D(compSpeed) (COE_32[9]+COE_32[10]*sqrt(SR(compSpeed))+COE_32[11]*SR(compSpeed))
#define COE_Y1(compSpeed) (COE_32[12]+COE_32[13]*SR(compSpeed)+COE_32[14]*pow(SR(compSpeed), 2))
#define COE_Y2(compSpeed) (COE_32[15]+COE_32[16]*SR(compSpeed)+COE_32[17]*pow(SR(compSpeed), 2))
#define COE_F(compSpeed) (COE_Y1(compSpeed)-COE_Y2(compSpeed))/(pow(COE_32[18], COE_D(compSpeed))-\
							pow(COE_32[19], COE_D(compSpeed)))
#define COE_E(compSpeed) COE_Y1(compSpeed)-COE_F(compSpeed)*pow(COE_32[18], COE_D(compSpeed))
#define COE_G(compSpeed) (COE_32[20]+COE_32[21]*pow(SR(compSpeed), 2)+COE_32[22]*pow(SR(compSpeed), 4))
#define COE_Q(compSpeed) (COE_32[23]+COE_32[24]*SR(compSpeed)+COE_32[25]*pow(SR(compSpeed), 2))
#define COE_R(compSpeed) (COE_32[26]+COE_32[27]*pow(SR(compSpeed), 2)+COE_32[28]*pow(SR(compSpeed), 4))
#define COE_S(compSpeed) (COE_32[29]+COE_32[30]*pow(SR(compSpeed), 2)+COE_32[31]*pow(SR(compSpeed), 4))




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_volume_flow_rate()
 *
 * \brief		Calculated volume flow rate.
 * 				volume_flow_rate = (a-b*pr^c)*4.719476965*10^(-4)/60
 *
 * \param[in]	pd = discharge pressure in kPa.
 * \param[in]	ps = suction pressure in kPa.
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		volume flow rate in m^3/s.
*/
//-------------------------------------------------------------------------------------------------
float cal_volume_flow_rate(float pd, float ps, float compSpeed)
{
	float a, b, c, pr;
	float volume_flow_rate;

	/* Calculated intermediate coefficients */
	a = COE_A(compSpeed);
	b = COE_B(compSpeed);
	c = COE_C(compSpeed);
	pr = PR(pd, ps);

	/* Calculated volume flow rate */
	volume_flow_rate = (a-b*pow(pr, c))*4.719476965*pow(10, (-4))/60;
	volume_flow_rate = (volume_flow_rate < 0) ? 0.00000001 : volume_flow_rate;

	return volume_flow_rate;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_power()
 *
 * \brief		Calculated power.
 * 				power = ((e+f*pr^d)*ps*0.000145*1000*volume_flow_rate/(4.719476965*10^(-4)/60))+g
 *
 * \param[in]	pd = discharge pressure in kPa.
 * \param[in]	ps = suction pressure in kPa.
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		power in W.
*/
//-------------------------------------------------------------------------------------------------
float cal_power(float pd, float ps, float compSpeed)
{
	float d, e, f, g, pr;
	float volume_flow_rate, power;

	/* Calculated intermediate coefficients */
	d = COE_D(compSpeed);
	e = COE_E(compSpeed);
	f = COE_F(compSpeed);
	g = COE_G(compSpeed);
	pr = PR(pd, ps);

	/* Calculated volume flow rate */
	volume_flow_rate = cal_volume_flow_rate(pd, ps, compSpeed);
	/* Calculated power */
	power = ((e+f*pow(pr, d))*ps*0.000145*1000*volume_flow_rate/(4.719476965*pow(10, (-4))/60))+g;
	power = (power < 0) ? 0 : power;

	return power;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_current()
 *
 * \brief		Calculated current.
 * 				current = IF(q>0,power/((q*power+s)*power+s),0)
 *
 * \param[in]	pd = discharge pressure in kPa.
 * \param[in]	ps = suction pressure in kPa.
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		current in A.
*/
//-------------------------------------------------------------------------------------------------
float cal_current(float pd, float ps, float compSpeed)
{
	float q, s, r;
	float power, current;

	/* Calculated intermediate coefficients */
	q = COE_Q(compSpeed);
	r = COE_R(compSpeed);
	s = COE_S(compSpeed);
	/* Calculated power */
	power = cal_power(pd, ps, compSpeed);

	/* Calculated current */
	current = power/((q*power+r)*power+s);
	current = (current < 0) ? 0 : current;

	return current;
}



void compressor_model_test(void)
{
	double	Pd = 1883.58288520969, Ps = 480,
			CompSpeed = 3600;

	float sr, pr, COE_A, COE_B, COE_C, COE_D, COE_Y1, COE_Y2, COE_F, COE_E,
			COE_G, COE_Q, COE_R, COE_S;

	float volume_flow_rate, power, current;

	/* Calculated Pr */
	pr = PR(Pd, Ps);
	sr = SR(CompSpeed);
	COE_A = COE_A(CompSpeed);
	COE_B = COE_B(CompSpeed);
	COE_C = COE_C(CompSpeed);
	COE_D = COE_D(CompSpeed);
	COE_Y1 = COE_Y1(CompSpeed);
	COE_Y2 = COE_Y2(CompSpeed);
	COE_F = COE_F(CompSpeed);
	COE_E = COE_E(CompSpeed);
	COE_G = COE_G(CompSpeed);
	COE_Q = COE_Q(CompSpeed);
	COE_R = COE_R(CompSpeed);
	COE_S = COE_S(CompSpeed);
	printf("pr = %f: \r\n", pr);
	printf("sr = %f: \r\n", sr);
	printf("COE_A = %f: \r\n", COE_A);
	printf("COE_B = %f: \r\n", COE_B);
	printf("COE_C = %f: \r\n", COE_C);
	printf("COE_D = %f: \r\n", COE_D);
	printf("COE_Y1 = %f: \r\n", COE_Y1);
	printf("COE_Y2 = %f: \r\n", COE_Y2);
	printf("COE_F = %f: \r\n", COE_F);
	printf("COE_E = %f: \r\n", COE_E);
	printf("COE_G = %f: \r\n", COE_G);
	printf("COE_Q = %f: \r\n", COE_Q);
	printf("COE_R = %f: \r\n", COE_R);
	printf("COE_S = %f: \r\n", COE_S);
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");


	volume_flow_rate = cal_volume_flow_rate(Pd, Ps, CompSpeed);
	power = cal_power(Pd, Ps, CompSpeed);
	current =  cal_current(Pd, Ps, CompSpeed);
	printf("volume flow rate = %f: \r\n", volume_flow_rate);
	printf("power = %f: \r\n", power);
	printf("current = %f: \r\n", current);

}

