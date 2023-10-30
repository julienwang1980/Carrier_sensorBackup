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
#ifndef _COMPRESSPOR_MODEL_H_   					// Re-include guard
#define _COMPRESSPOR_MODEL_H_	    				// Re-include guard


//-------------------------------------------------------------------------------------------------
/**
 * \def
 * \brief
 */
//-------------------------------------------------------------------------------------------------
#define COMPSPEED_RATED (3600)


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
float cal_volume_flow_rate(float pd, float ps, float compSpeed);


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
float cal_power(float pd, float ps, float compSpeed);


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
 * \param[in]	U = the voltage of compressor.
 *
 * \return		current in A.
*/
//-------------------------------------------------------------------------------------------------
float cal_current(float pd, float ps, float compSpeed, float U);


void compressor_model_test(void);

#endif                                      // re-include guard
