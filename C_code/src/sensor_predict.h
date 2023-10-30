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
 * \param[in]	p_suc_g = suction gas pressure in kPa(gage pressure).
 * \param[in]	t_suc = suction gas temperature in ℃.
 * \param[in]	p_dis_g = discharge gas pressure in kPa(gage pressure).
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		discharge gas temperature in ℃.
*/
//-------------------------------------------------------------------------------------------------
float pred_Tdis(float p_suc_g, float t_suc, float p_dis_g, float compSpeed);



//-------------------------------------------------------------------------------------------------
/**
 * \fn			pred_Tdis_delay()
 *
 * \brief		Predict temperature of discharge gas by first order delay.
 *
 * \param[in]	p_suc_g = suction gas pressure in kPa(gage pressure).
 * \param[in]	t_suc = suction gas temperature in ℃.
 * \param[in]	p_dis_g = discharge gas pressure in kPa(gage pressure).
 * \param[in]	compSpeed = compressor speed in rpm.
 * \param[in]	tau = 时间常数tau在开机前5分钟为300；正常运行阶段为100；关机（压缩机转速为0）为200
 * \param[in]	T_interval = t[i]-t[i-1]：i和i-1时刻的时间间隔
 *
 * \return		discharge gas temperature in ℃.
*/
//-------------------------------------------------------------------------------------------------
float pred_Tdis_delay(float p_suc_g, float t_suc, float p_dis_g, float compSpeed, int tau, float T_interval);


//-------------------------------------------------------------------------------------------------
/**
 * \fn			pred_Pdis_temp()
 *
 * \brief		Predict pressure of discharge gas by temperature.
 *
 * \param[in]	p_suc_g = suction gas pressure in kPa(gage pressure).
 * \param[in]	t_suc = suction gas temperature in ℃.
 * \param[in]	t_dis = discharge gas temperature in ℃.
 * \param[in]	compSpeed = compressor speed in rpm.
 *
 * \return		discharge gas pressure in kPa(gage pressure).
*/
//-------------------------------------------------------------------------------------------------
float pred_Pdis_temp(float p_suc_g, float t_suc, float t_dis, float compSpeed);


//-------------------------------------------------------------------------------------------------
/**
 * \fn			pred_Pdis_curr()
 *
 * \brief		Predict pressure of discharge gas by current.
 *
 * \param[in]	p_suc_g = suction gas pressure in kPa(gage pressure).
 * \param[in]	I_test = the current of driver in amp.
 * \param[in]	compSpeed = compressor speed in rpm.
 * \param[in]	U = the voltage of compressor.
 *
 * \return		discharge gas pressure in kPa(gage pressure).
*/
//-------------------------------------------------------------------------------------------------
float pred_Pdis_curr(float P_suc, float I_test, float compSpeed,  float U);




void sensor_pre_test(void);
#endif                                      // re-include guard
