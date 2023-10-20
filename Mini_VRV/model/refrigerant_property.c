//*************************************************************************
//*************************************************************************
/**
 * \file		refrigerant_property.c
 *
 * \brief		The formula for calculating the physical properties
 * \brief		of refrigerant applies only to R410A
 *
 * \copyright	CARRIER CONFIDENTIAL & PROPRIETARY
 *				COPYRIGHT, CARRIER CORPORATION, 2020
 *				UNPUBLISHED WORK, ALL RIGHTS RESERVED
 *
 * \author		Julien Wang
*/
//*************************************************************************
//*************************************************************************
#include "refrigerant_property.h"
#include <stdio.h>
#include <math.h>


//-------------------------------------------------------------------------------------------------
/**
 * \var     COE[]
 * \brief   This array defines the coefficient of implicit equations for
 * 			superheated thermodynamic properties of R410A.
 */
//-------------------------------------------------------------------------------------------------
static const double COE[] = {	0.0169347786859482,			-0.0000391263315032514,		0.0000000436416993794122,
								1.75371690212062,			-0.0204274840559141,		0.0000393230641090647,
								-0.0000000457868739196494,	-1.20806074268803,			0.00655504316587795,
								-0.00000837195897399936,	0.00000000979938358453164};


//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_t_sat()
 *
 * \brief		Calculated saturation temperature.
 *				t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
 *
 * \param[in]	p = Pressure in kPa.
 *
 * \return		saturation temperature in ℃.
*/
//-------------------------------------------------------------------------------------------------
float cal_t_sat(float p)
{
	return -2107.935 / (log(p*1000)-21.8205)-256.2377;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_h_sat_gas()
 *
 * \brief		Calculated Saturated gas Enthalpy.
 * 				t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
 *				h_sat_gas = 280998.3+332.614*t_sat-4.699265*t_sat^2-51.2569*10^(-3)*t_sat^3
 *
 * \param[in]	p = Pressure in kPa.
 *
 * \return		Enthalpy of saturated gas in kJ/kg.
*/
//-------------------------------------------------------------------------------------------------
float cal_h_sat_gas(float p)
{
	double t_sat, h_sat_gas;

	/* Calculated saturation temperature */
	t_sat = cal_t_sat(p);
	/* Calculated Saturated gas Enthalpy */
	h_sat_gas = 280998.3+332.614*t_sat-4.699265*pow(t_sat,2)-51.2569*pow(10,-3)*pow(t_sat,3);

	return h_sat_gas;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_h_sh_gas()
 *
 * \brief		Calculated Enthalpy of superheated gas.
 *				t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
 *				h_sat_gas = 280998.3+332.614*t_sat-4.699265*t_sat^2-51.2569*10^(-3)*t_sat^3
 *				h_sh_gas = (1+3.3247*10^(-3)*(t-t_sat)+3.62592*10^(-7)*(t-t_sat)^2+30.40633*10^(-6)*
 *							(t-t_sat)*t_sat-18.47693*10^(-8)*(t-t_sat)^2*t_sat+76.64206*10^(-8)*
 *							(t-t_sat)*(t_sat)^2-60.2765*10^(-10)*(t-t_sat)^2*t_sat^2)*h_sat_gas
 *
 * \param[in]	p = Pressure in kPa.
 * \param[in]	t = Gas temperature in ℃.
 *
 * \return		Enthalpy of saturated gas in kJ/kg.
*/
//-------------------------------------------------------------------------------------------------
float cal_h_sh_gas(float p, float t)
{
	double t_sat, h_sat_gas, h_sh_gas;

	/* Calculated saturation temperature */
	t_sat = cal_t_sat(p);
	/* Calculated Saturated gas Enthalpy */
	h_sat_gas = cal_h_sat_gas(p);
	/* Calculated superheated gas Enthalpy */
	h_sh_gas = 	(1 + 3.3247*pow(10,-3)*(t-t_sat)+3.62592*pow(10,-7)*pow((t-t_sat),2)
					+ 30.40633*pow(10,-6)*(t-t_sat)*t_sat
					- 18.47693*pow(10,-8)*pow((t-t_sat),2)*t_sat
					+ 76.64206*pow(10, -8)*(t-t_sat)*pow((t_sat),2)
					- 60.2765*pow(10,-10)*pow((t-t_sat),2)*pow(t_sat,2))*h_sat_gas;

	return h_sh_gas;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_vol_sat_gas()
 *
 * \brief		Calculated Saturated gas specific volume.
 *				t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
 *				v_sat_gas = EXP((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461*10^(-4)*
 *							t_sat+111.95294*10^(-6)*t_sat^2-31.84587*10^(-7)*t_sat^3)
 *
 * \param[in]	p = Pressure in kPa.rta
 *
 * \return		Saturated gas specific volume in m^3/s.
 */
//-------------------------------------------------------------------------------------------------
float cal_vol_sat_gas(float p)
{
	double t_sat, v_sat_gas;

	/* Calculated saturation temperature */
	t_sat = cal_t_sat(p);
	/* Calculated Saturated gas specific volume */
	v_sat_gas = exp((-11.93809+1873.567/(t_sat+273.15))) * (5.24253-369.32461*pow(10,(-4))*
 						t_sat+111.95294*pow(10,(-6))*pow(t_sat,2)-31.84587*pow(10,(-7))*pow(t_sat,3));

	return v_sat_gas;
}




//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_dens_sh_gas()
 *
 * \brief		Calculated density of superheated gas.
 *				dens_sh_gas =
 *
 * \param[in]	p = Pressure in kPa.rta
 * \param[in]	t = Gas temperature in ℃.
 *
 * \return		Density of superheated gas in kg/m^3.
 */
//-------------------------------------------------------------------------------------------------
float cal_dens_sh_gas(float p, float t)
{
	double t_sat, t_sat_f, dens_sat_gas, coe_A, coe_B, coe_C, coe_D, y, dens_sh_gas;

	/* Calculated saturation temperature */
	t_sat = cal_t_sat(p);
	t_sat_f = t_sat+273.15;
	/* Calculated Density of Saturated gas */
	dens_sat_gas = pow((1/(exp((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461*pow(10,(-4)) *
						t_sat+111.95294*pow(10,(-6))*pow(t_sat,2)-31.84587*pow(10,(-7))*pow(t_sat,3))))
					,(-0.4))+0.75;

	coe_A = -((1+COE[0]*t_sat_f+COE[1]*pow(t_sat_f,2)+COE[2]*pow(t_sat_f,3))/dens_sat_gas +
				(COE[3]+COE[4]*t_sat_f+COE[5]*pow(t_sat_f,2)+COE[6]*pow(t_sat_f,3))/pow(dens_sat_gas,2) +
				(COE[7]+COE[8]*t_sat_f+COE[9]*pow(t_sat_f,2)+COE[10]*pow(t_sat_f,3))/pow(dens_sat_gas,3));
	coe_B = 1+COE[0]*(t+273.15)+COE[1]*pow((t+273.15),2)+COE[2]*pow((t+273.15),3);
	coe_C = COE[3]+COE[4]*(t+273.15)+COE[5]*pow((t+273.15),2)+COE[6]*pow((t+273.15),3);
	coe_D = COE[7]+COE[8]*(t+273.15)+COE[9]*pow((t+273.15),2)+COE[10]*pow((t+273.15),3);
	y = 2*pow((pow(coe_B,2)/(3*pow(coe_A,2))-coe_C/coe_A)/3,(1.0/2)) *
		cos(1.0/3*acos(-(coe_D/coe_A+2*pow(coe_B,3)/(27*pow(coe_A,3))-coe_B*coe_C/(3*pow(coe_A,2))) /
					  (2*pow((pow(coe_B,2)/(3*pow(coe_A,2))-coe_C/coe_A)/3,(3.0/2))))) -
		coe_B/(3*coe_A);

	/* Calculated Density of Superheated gas */
	dens_sh_gas = pow((y-0.75),(-2.5));

	return dens_sh_gas;
}





void refrig_prop_test()
{
	int h_sh_gas_P[] = {1000,1000,1000,1000,1000,200,200,200,200,200,150,150,3000,3000,3000,3000,3000,
					3000,3000,3000,3000,3000,3000,3000,3500,3500,3500,3500,3500,3500,3500,3500,3500,
					3500,3500,3500,4000,4000,4000,4000,4000,4000,4200,4200,4200,4200,4200,4500,4500,
					4500,4600,4600,4600};
	int h_sh_gas_C[] = {10,15,20,25,30,-30,-25,-20,-15,-10,-40,-20,60,65,70,75,80,85,90,95,100,105,110,
						115,60,65,70,75,80,85,90,95,100,105,110,115,65,75,85,95,105,120,65,75,85,100,
						120,70,90,120,70,90,120};

	int dens_sh_gas_P[] = {150,150,150,200,200,200,200,500,500,500,500,1000,1000,1000,1000,1500,1500,
							1500,1500,2500,2500,2500,2500,3500,3500,3500,3500,3500,4000,4000,4000,4000,
							4000,4300,4300,4300,4300,4300,4500,4500,4500,4500,4500,4600,4600,4600,4600};
	float dens_sh_gas_C[] = {-42.3,-38.3,-28.3,-35.9,-31.9,-20,-6.9,-13,-9,7,27,8,12,25,40,22,26,40,60,
							42,46,55,70,61,65,80,95,110,63,67,77,100,120,67,71,85,100,120,69,73,80,100,
							120,70,85,100,120};

	float t_sat, h_sat_gas, h_sh_gas, v_sat_gas, dens_sh_gas;
	int Pressure;

	/* Calculated saturation temperature */
	printf("Calculated saturation temperature:\r\n");
	for (size_t i = 0; i < 90; i++)
	{
		Pressure = 4600-i*50;
		printf("%d		", Pressure);
	}
	printf("\r\n");
	for (size_t i = 0; i < 90; i++)
	{
		Pressure = 4600-i*50;
		t_sat = cal_t_sat(Pressure);
		printf("%f	", t_sat);
	}
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");

	/* Calculated Saturated gas specific volume */
	printf("Calculated Saturated gas specific volume:\r\n");
	for (size_t i = 0; i < 90; i++)
	{
		Pressure = 4600-i*50;
		printf("%d		", Pressure);
	}
	printf("\r\n");
	for (size_t i = 0; i < 90; i++)
	{
		Pressure = 4600-i*50;
		v_sat_gas = cal_vol_sat_gas(Pressure);
		printf("%f	", v_sat_gas);
	}
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");


	/* Calculated Saturated gas Enthalpy */
	printf("Calculated Saturated gas Enthalpy:\r\n");
	for (size_t i = 0; i < 90; i++)
	{
		Pressure = 4600-i*50;
		printf("%d		", Pressure);
	}
	printf("\r\n");
	for (size_t i = 0; i < 90; i++)
	{
		Pressure = 4600-i*50;
		h_sat_gas = cal_h_sat_gas(Pressure);
		printf("%f	", h_sat_gas);
	}
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");

	/* Calculated superheated gas Enthalpy */
	printf("Calculated superheated gas Enthalpy:\r\n");
	for (size_t i = 0; i < 53; i++)
	{
		printf("%d		", h_sh_gas_C[i]);
	}
	printf("\r\n");
	for (size_t i = 0; i < 53; i++)
	{
		h_sh_gas = cal_h_sh_gas(h_sh_gas_P[i], h_sh_gas_C[i]);
		printf("%f	", h_sh_gas);
	}
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");

	/* Calculated superheated gas density */
	printf("Calculated superheated gas density:\r\n");
	for (size_t i = 0; i < 47; i++)
	{
		printf("%.1f		", dens_sh_gas_C[i]);
	}
	printf("\r\n");
	for (size_t i = 0; i < 47; i++)
	{
		dens_sh_gas = cal_dens_sh_gas(dens_sh_gas_P[i], dens_sh_gas_C[i]);
		printf("%f	", dens_sh_gas);
	}
}

