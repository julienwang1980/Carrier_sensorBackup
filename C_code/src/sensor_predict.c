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
#include <dirent.h>
#include <stdlib.h>
#include <string.h>



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
float pred_Tdis(float p_suc_g, float t_suc, float p_dis_g, float compSpeed)
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
	float p_dis;	//discharge gas pressure in kPa_a(absolute pressure)
	float p_suc;	//discharge gas pressure in kPa_a(absolute pressure)

	// gage pressure converte to absolute pressure
	p_dis = p_dis_g + 101.35;
	p_suc = p_suc_g + 101.35;

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
float pre_temp;	//TODO: TEMP
float pred_Tdis_delay(float p_suc_g, float t_suc, float p_dis_g, float compSpeed, int tau, float T_interval)
{
	float t_dis, res;
	// static float pre_temp = 22.2;//TODO: TEMP
	if ((tau != 300) && (tau != 100) && (tau != 200))
	{
		return 0;
	}

	t_dis = pred_Tdis(p_suc_g, t_suc, p_dis_g, compSpeed);

	res = pre_temp+1*(t_dis-pre_temp)*(1-pow(2.718281828459, -(T_interval/tau)));
	pre_temp = res;

	return res;
}




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
float pred_Pdis_temp(float p_suc_g, float t_suc, float t_dis, float compSpeed)
{
	float pd_int1 = 100, pd_int2=4300, pd_int, hd_int;
	float v_flow, power;
	float dens_gas;		//dens_gas:density of scution gas.
	float ts_suc;		//ts:temperature of saturation suction gas
	float ssh;			//ssh:superheated of suction gas
	float mr;			//mr:density and flow rate.
	float h_suc;		//h_suc:enthalpy of suction gas
	float h_dis;		//h_dis:enthalpy of discharge gas
	float vol_sat_gas;	//Saturated gas specific volume
	float p_suc;	//suction gas pressure in kPa_a(absolute pressure)

	// gage pressure converte to absolute pressure
	p_suc = p_suc_g + 101.35;

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
		{
			vol_sat_gas = cal_vol_sat_gas(p_suc);
			if (!vol_sat_gas)
			{
				return 0;
			}
			dens_gas = 1/vol_sat_gas;
		}

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

	return pd_int - 101.35;
}




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
 * \return		discharge gas pressure in kPa.
*/
//-------------------------------------------------------------------------------------------------
float pred_Pdis_curr(float p_suc_g, float I_test, float compSpeed,  float U)
{
	float Pd_int1 = 100, Pd_int2=4300, Pd_int;
	float I;
	float p_suc;	//suction gas pressure in kPa_a(absolute pressure)

	// gage pressure converte to absolute pressure
	p_suc = p_suc_g + 101.35;

	for (size_t i = 0; i < 20; i++)
	{
		Pd_int = (Pd_int1+Pd_int2)/2;

		/* calculating current I */
		I = cal_current(Pd_int, p_suc, compSpeed, U);
		if (fabs(I - I_test) < 0.001)
		{
			return Pd_int - 101.35;
		}
		else
		{
			if ((I - I_test) < 0)
			{
				Pd_int1 = Pd_int;
			}
			else
			{
				Pd_int2 = Pd_int;
			}
		}
	}
	return (Pd_int - 101.35);
}






void sensor_pre_test(void)
{
	float	Pd = 2152.59, Ps = 1390.88, CompSpeed = 1740, ST = 20.54, I_test = 0;
	float t_dis, t_dis_old, p_dis_t, p_dis_a, P_dis;
	int tau;

	DIR *dp;
	struct dirent *dirp;
	char dest_file[128];
	char row[512];
	char *token;
	char *filename;
	float data[30];
	int counter;
	char *dir = "./temp_data/";

	// /* change the dir */
	// chdir(dir);
	// /* open the folder */
	// if ((dp = opendir("./")) == NULL)
	// {
    //     printf("Error opening directory\n");
    // }
	// /* read every files */
	// while ((dirp = readdir(dp)) != NULL)
	// {
	// 	/* useful file */
	// 	if (strstr(dirp->d_name, "tdis.csv") != NULL)
	// 	{
	// 		/* open source file for read data */
	// 		FILE *fpr = fopen(dirp->d_name, "r");
	// 		// printf("dirp->d_name: %s\n", dirp->d_name);

	// 		/* open destination file for save data */
	// 		memset(dest_file, 0, sizeof(dest_file));
	// 		filename = strtok(dirp->d_name, ".");
	// 		memcpy(dest_file, dirp->d_name, strlen(dirp->d_name));
	// 		strncat(dest_file, "_result.csv", 11);
	// 		FILE *fpw = fopen(dest_file, "w");
	// 		// printf("dest_file: %s\n", dest_file);
	// 		/* read first row */
	// 		fgets(row, 512, fpr);
	// 		// printf("Row: %s\n", row);
	// 		/* write first row */
	// 		fprintf(fpw, "%s\n","Ps,ST,Pd,CompSpeed,WORK_MINUTES,T_dis_es,t_dis_old,T_dis_delay,t_dis");
	// 		// printf("Row: %s\n", row);
	// 		/* read data in row and save to data[] */
	// 		fgets(row, 512, fpr);
	// 		token = strtok(row, ",");
	// 		counter = 0;
	// 		while (token != NULL)
	// 		{
	// 			token = strtok(NULL, ",");
	// 			data[counter] = atof(token);
	// 			counter++;
	// 		}
	// 		/* calculate Tdis */
	// 		Pd = data[0];
	// 		Ps = data[1];
	// 		CompSpeed = data[2];
	// 		ST = data[3];
	// 		if (data[14]<0.001)
	// 		{
	// 			tau = 200;
	// 		}
	// 		else if (data[14]<5)
	// 		{
	// 			tau = 300;
	// 		}
	// 		else
	// 		{
	// 			tau = 100;
	// 		}
	// 		t_dis_old = pred_Tdis(Ps, ST, Pd, CompSpeed);
	// 		// printf("t_dis_old = %f", t_dis_old);
	// 		/* write the data to file */
	// 		fprintf(fpw, "%f,%f,%f,%f,%f,%f,%f,%f,%f\n", Ps, ST, Pd, CompSpeed, data[14], data[19], t_dis_old, data[20], t_dis_old);
	// 		pre_temp = t_dis_old;
	// 		/* read other rows */
	// 		while (fgets(row, 512, fpr) != NULL)
	// 		{
	// 			// printf("Row: %s\n", row);
	// 			/* read data in row and save to data[] */
	// 			token = strtok(row, ",");
	// 			counter = 0;
	// 			while (token != NULL)
	// 			{
	// 				token = strtok(NULL, ",");
	// 				data[counter] = atof(token);
	// 				counter++;
	// 			}
	// 			/* calculate Tdis */
	// 			Pd = data[0];
	// 			Ps = data[1];
	// 			CompSpeed = data[2];
	// 			ST = data[3];
	// 			if (data[14]<0.001)
	// 			{
	// 				tau = 200;
	// 			}
	// 			else if (data[14]<5)
	// 			{
	// 				tau = 300;
	// 			}
	// 			else
	// 			{
	// 				tau = 100;
	// 			}
	// 			t_dis_old = pred_Tdis(Ps, ST, Pd, CompSpeed);
	// 			t_dis = pred_Tdis_delay(Ps, ST, Pd, CompSpeed, tau, 2.0);
	// 			/* write the data to file */
	// 			fprintf(fpw, "%f,%f,%f,%f,%f,%f,%f,%f,%f\n", Ps, ST, Pd, CompSpeed, data[14], data[19], t_dis_old, data[20], t_dis);
	// 		}
	// 		/* close file */
	// 		fclose(fpw);
	// 		fclose(fpr);
	// 	}
    // }
	// /* close the folder */
	// closedir(dp);


	// dir = "../temp_data1/";
	// /* change the dir */
	// chdir(dir);
	// /* open the folder */
	// if ((dp = opendir("./")) == NULL)
	// {
    //     printf("Error opening directory\n");
    // }
	// dirp = readdir(dp);
	// /* read every files */
	// while ((dirp = readdir(dp)) != NULL)
	// {
	// 	/* useful file */
	// 	if (strstr(dirp->d_name, "current.csv") != NULL)
	// 	{
	// 		/* open source file for read data */
	// 		FILE *fpr = fopen(dirp->d_name, "r");
	// 		// printf("dirp->d_name: %s\n", dirp->d_name);

	// 		/* open destination file for save data */
	// 		memset(dest_file, 0, sizeof(dest_file));
	// 		filename = strtok(dirp->d_name, ".");
	// 		memcpy(dest_file, dirp->d_name, strlen(dirp->d_name));
	// 		strncat(dest_file, "_result.csv", 11);
	// 		FILE *fpw = fopen(dest_file, "w");
	// 		// printf("dest_file: %s\n", dest_file);
	// 		/* read first row */
	// 		fgets(row, 512, fpr);
	// 		// printf("Row: %s\n", row);
	// 		/* write first row */
	// 		fprintf(fpw, "%s\n","Ps,I_test,CompSpeed,P_dis_es,P_dis");
	// 		// printf("Row: %s\n", row);
	// 		/* read other rows */
	// 		while (fgets(row, 512, fpr) != NULL)
	// 		{
	// 			// printf("Row: %s\n", row);
	// 			/* read data in row and save to data[] */
	// 			token = strtok(row, ",");
	// 			counter = 0;
	// 			while (token != NULL)
	// 			{
	// 				token = strtok(NULL, ",");
	// 				data[counter] = atof(token);
	// 				counter++;
	// 			}
	// 			/* calculate Tdis */
	// 			Ps = data[1];
	// 			CompSpeed = data[2];
	// 			I_test = data[18];
	// 			P_dis = pred_Pdis_curr(Ps, I_test, CompSpeed, 220);
	// 			/* write the data to file */
	// 			fprintf(fpw, "%f,%f,%f,%f,%f\n", Ps, I_test, CompSpeed, data[19], P_dis);
	// 		}
	// 		/* close file */
	// 		fclose(fpw);
	// 		fclose(fpr);
	// 	}
    // }
	// /* close the folder */
	// closedir(dp);

	// p_dis_t = pred_Pdis_temp(641, -2.23, 56.39, 7080);
	p_dis_a = pred_Pdis_curr(1584.304, 0.002282, 0, 220);
	// printf("p_dis_t = %f: \r\n", p_dis_t);
	printf("p_dis_a = %f: \r\n", p_dis_a);
}


