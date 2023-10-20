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
#ifndef _SENSOR_PREDICT_H_   	        				// Re-include guard
#define _SENSOR_PREDICT_H_	    		        		// Re-include guard


//-------------------------------------------------------------------------------------------------
/**
 * \def
 * \brief
 */
//-------------------------------------------------------------------------------------------------
#define FW (0.8)

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
// float pred_Tdis(float p_suc, float t_suc, float p_dis, float compSpeed, int tau, float T_interval);


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
float pred_Pdis(float p_suc, float t_suc, float t_dis, float compSpeed);



void sensor_pre_test(void);
#endif                                      // re-include guard
