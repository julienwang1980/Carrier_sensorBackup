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
#ifndef _REFRIGENTANT_PROPERTY_H_					// Re-include guard
#define _REFRIGENTANT_PROPERTY_H_					// Re-include guard


//-------------------------------------------------------------------------------------------------
/**
 * \def
 * \brief
 */
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
/**
 * \struct
 * \brief
 */
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
/**
 * \enum
 * \brief
 */
//------------------------------------------------------------------------------------------------



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
float cal_t_sat(float p);


//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_h_sat_gas()
 *
 * \brief		Calculated Saturated gas Enthalpy.
 * 				t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
 *				h_sat_gas = 280998.3+332.614*t_sat-4.699265*t_sat^2-51.2569*10^(-3)*t_sat^3
 *
 * \param[in]	pa = Pressure in kPa.
 *
 * \return		Calculated Enthalpy of saturated gas in kJ/kg.
 */
//-------------------------------------------------------------------------------------------------
float cal_h_sat_gas(float p);


//-------------------------------------------------------------------------------------------------
/**
 * \fn			cal_h_sh_gas()
 *
 * \brief		Calculated Enthalpy of superheated gas.
 *				t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
 *				h_sat_gas = 280998.3+332.614*t_sat-4.699265*t_sat^2-51.2569*10^(-3)*t_sat^3
 *				h_h_sh_gassh = (1+3.3247*10^(-3)*(t-t_sat)+3.62592*10^(-7)*(t-t_sat)^2+30.40633*10^(-6)*
 *								(t-t_sat)*t_sat-18.47693*10^(-8)*(t-t_sat)^2*t_sat+76.64206*10^(-8)*
 *								(t-t_sat)*(t_sat)^2-60.2765*10^(-10)*(t-t_sat)^2*t_sat^2)*h_sat_gas
 *
 * \param[in]	p = Pressure in kPa.
 * \param[in]	t = Gas temperature in ℃.
 *
 * \return		Calculated Enthalpy of saturated gas in kJ/kg.
 */
//-------------------------------------------------------------------------------------------------
float cal_h_sh_gas(float p, float t);


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
float cal_vol_sat_gas(float p);


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
float cal_dens_sh_gas(float p, float t);


void refrig_prop_test(void);

#endif                                      // re-include guard
