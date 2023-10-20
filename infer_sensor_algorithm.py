import numpy as np
import pandas as pd

from compressor_model import Compressor_model
from property import Property
import numbers

class InferSensor():
    def __init__(self) -> None:

        pass

    def cal_density(self, pa, t, coe_init=None):
        """
        Calculated density

        Parameters
        ----------
        pa : Numerical-> int flot or Series -> np.array, pd.Series
            Pressure in kPa.
        t : Numerical-> int flot or Series -> np.array, pd.Series
            Gas temperature in ℃
        Returns
        ----------
        density_gas_sh: Numerical-> int, flot or Series -> np.array, pd.Series
            Density of superheated gas in kg/m^3
        """
        if isinstance(coe_init, list):
            self.coe = coe_init
        else:
            coe = [0.169347786859482*10**(-1), -0.391263315032514*10**(-4), 0.436416993794122*10**(-7), 0.175371690212062*10**1, -0.204274840559141*10**(-1),
                   0.393230641090647*10**(-4), -0.457868739196494*10**(-7), -0.120806074268803*10**1, 0.655504316587795*10**(-2), -0.837195897399936*10**(-5), 0.979938358453164*10**(-8)]

        pt = Property()
        t_sat = pt.cal_t_sat(pa)
        t_sat_f = t_sat+273.15
        t = t+273.15
        # if t_sat_f > t:
        density_gas_sat_2 = 1/(np.exp((-11.93809+1873.567/(t_sat+273.15)))*(
            5.24253-369.32461*10 ** (-4)*t_sat+111.95294*10 ** (-6)*t_sat ** 2-31.84587*10 ** (-7)*t_sat ** 3))

        density_gas_sat = (1/(np.exp((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461*10 **
                           (-4)*t_sat+111.95294*10 ** (-6)*t_sat**2-31.84587*10 ** (-7)*t_sat**3))) ** (-0.4)+0.75

        coe_A = -((1+coe[0]*t_sat_f+coe[1]*t_sat_f**2+coe[2]*t_sat_f**3)/density_gas_sat+(coe[3]+coe[4]*t_sat_f+coe[5]*t_sat_f**2 +
                  coe[6]*t_sat_f**3)/(density_gas_sat**2)+(coe[7]+coe[8]*t_sat_f+coe[9]*t_sat_f**2+coe[10]*t_sat_f**3)/(density_gas_sat**3))
        coe_B = 1+coe[0]*t+coe[1]*t**2+coe[2]*t**3
        coe_C = coe[3]+coe[4]*t+coe[5]*t**2+coe[6]*t**3
        coe_D = coe[7]+coe[8]*t+coe[9]*t**2+coe[10]*t**3
        y = 2*((coe_B ** 2/(3*coe_A**2)-coe_C/coe_A)/3)**(1/2)*np.cos(1/3*np.arccos(-(coe_D/coe_A+2*coe_B**3/27 /
                                                                                      (coe_A**3)-coe_B*coe_C/3/(coe_A**2))/(2*((coe_B**2/3/(coe_A**2)-coe_C/coe_A)/3) ** (3/2))))-coe_B/(3*coe_A)
        density_gas_sh = (y-0.75)**(-2.5)
        if isinstance(density_gas_sh, pd.Series) or isinstance(density_gas_sh, np.ndarray):
            density_gas_sh[np.isnan(
                density_gas_sh)] = density_gas_sat_2[np.isnan(density_gas_sh)]

        return density_gas_sh

    def cal_tdis(self, fw, Pd, Ps, CompSpeed, ST, CompSpeed_rated, custom_init_coe=None):
        """
        Calculated discharge temperature

        Parameters
        ----------
        Pd: Number or Series
            kPa
        Ps: Number
            kPa
        CompSpeed: Number or Series
            rpm
        ST: Number
            ℃
        CompSpeed_rated: Number or Series
            rpm

        Returns
        ----------
        t_dis: Numerical-> int, flot or Series -> np.array, pd.Series
            Discharge temperature in ℃
        """
        pt = Property()
        h_dis = self.cal_hd(fw,Pd,Ps, CompSpeed, ST, CompSpeed_rated)
        t_dis_sat = pt.cal_t_sat(Pd)
        h_dis_sat_gas = 280998.3+332.614*t_dis_sat-4.699265 * \
            t_dis_sat**2-51.2569*10**(-3)*t_dis_sat**3
        coe_a = 3.62592*10**(-7)-18.47693*10**(-8) * \
            t_dis_sat-60.2765*10**(-10)*t_dis_sat**2
        coe_b = 3.3247*10**(-3)-2*3.62592*10**(-7)*t_dis_sat+30.40633*10**(-6)*t_dis_sat+2*18.47693 * \
            10**(-8)*t_dis_sat**2+76.64206*10**(-8) * \
            t_dis_sat**2+2*60.2765*10**(-10)*t_dis_sat**3
        coe_c = 1-3.3247*10**(-3)*t_dis_sat+3.62592*10**(-7)*t_dis_sat**2-30.40633*10**(-6)*t_dis_sat**2-18.47693 * \
            10**(-8)*t_dis_sat**3-76.64206*10**(-8)*t_dis_sat**3 - \
            60.2765*10**(-10)*t_dis_sat**4-h_dis/h_dis_sat_gas
        t_dis = (-coe_b+np.sqrt(coe_b**2-4*coe_a*coe_c))/(2*coe_a)

        return t_dis

    def cal_pd(self, fw, t_dis, Ps, CompSpeed, ST, CompSpeed_rated, custom_init_coe=None):
        """
        Calculated discharge pressure

        Parameters
        ----------
        Pd: Number or Series
            kPa
        Ps: Number
            kPa
        CompSpeed: Number or Series
            rpm
        ST: Number
            ℃
        CompSpeed_rated: Number or Series
            rpm

        Returns
        ----------
        T_dis: Numerical-> int, flot or np.array
            Discharge temperature in ℃
        """
        pt = Property()
        pd_record = []
        if isinstance(t_dis, pd.Series) or isinstance(t_dis, np.ndarray):
            for x_td, x_ps, x_cs, x_st, x_fw, x_cr in zip(t_dis, Ps, CompSpeed, ST, fw, CompSpeed_rated):
                Pd = self.dichotomy_test1(x_td, x_ps, x_cs, x_st, x_fw, x_cr)
                pd_record.append(Pd)
            return np.array(pd_record)
        elif isinstance(t_dis, numbers.Number):
            return self.dichotomy_test1(t_dis, Ps, CompSpeed, ST, fw,CompSpeed_rated)
        
        return 'TYPE ERROR'

    def dichotomy_test1(self, t_dis, Ps, CompSpeed, ST, fw,CompSpeed_rated,pd_left=100, pd_right=4000, eps=0.1, epoch=20):
    
        for i in range(epoch):
            pd_middle = (pd_left + pd_right) / 2
            h_dis = self.cal_hd(fw,pd_middle,Ps, CompSpeed, ST, CompSpeed_rated)
            diffs_mid = self.cal_hd_int(t_dis, pd_middle)-h_dis
            if diffs_mid > 0 :
                pd_left = pd_middle
            else:
                pd_right = pd_middle

            if abs(diffs_mid) < eps:
                return pd_middle

        return pd_middle

    def cal_hd(self, fw, Pd, Ps, CompSpeed, ST, CompSpeed_rated, custom_init_coe=None):
        """
        Calculated discharge enthalpy

        Parameters
        ----------
        Ps: Number
            kPa
        CompSpeed: Number or Series
            rpm
        ST: Number
            ℃
        CompSpeed_rated: Number or Series
            rpm

        Returns
        ----------
        h_dis: Numerical-> int, flot or Series -> np.array, pd.Series
            Discharge enthalpy
        """

        compressor = Compressor_model(
            Pd, Ps, CompSpeed, ST, CompSpeed_rated, custom_init_coe)
        pt = Property()
        volume = compressor.volume
        power = compressor.power
        density_sh = self.cal_density(Ps, ST)
        mass_flow = volume*density_sh
        t_suction_sat = pt.cal_t_sat(Ps)
        h_suction_sat_gas = 280998.3+332.614*t_suction_sat - \
            4.699265*t_suction_sat**2-51.2569*10**(-3)*t_suction_sat**3
        h_suction = (1+3.3247*10 ** (-3)*(ST-t_suction_sat)+3.62592*10 ** (-7)*(ST-t_suction_sat) ** 2+30.40633*10 ** (-6)*(ST-t_suction_sat)*t_suction_sat-18.47693 *
                     10 ** (-8)*(ST-t_suction_sat) ** 2*t_suction_sat+76.64206*10 ** (-8)*(ST-t_suction_sat)*(t_suction_sat) ** 2-60.2765*10 ** (-10)*(ST-t_suction_sat) ** 2*t_suction_sat ** 2)*h_suction_sat_gas
        h_dis = power*fw/mass_flow+h_suction

        return h_dis

    def cal_hd_int(self, t_dis, Pd):
        """
        Calculated discharge enthalpy

        Parameters
        ----------
        t_dis: Number or Series
            Discharge temperature in ℃
        Pd: Number
            kPa

        Returns
        ----------
        h_dis_int: Numerical-> int, flot or Series -> np.array, pd.Series
            Discharge enthalpy 
        """        
        pt = Property()
        t_dis_sat = pt.cal_t_sat(Pd)
        h_dis_sat_gas = 280998.3+332.614*t_dis_sat-4.699265 * \
            t_dis_sat**2-51.2569*10**(-3)*t_dis_sat**3
        h_dis_int = (1+3.3247*10**(-3)*(t_dis-t_dis_sat)+3.62592*10**(-7)*(t_dis-t_dis_sat)**2+30.40633*10**(-6)*(t_dis-t_dis_sat)*t_dis_sat-18.47693*10**(-8)
                     * (t_dis-t_dis_sat)**2*t_dis_sat+76.64206*10**(-8)*(t_dis-t_dis_sat)*(t_dis_sat)**2-60.2765*10**(-10)*(t_dis-t_dis_sat)**2*t_dis_sat**2)*h_dis_sat_gas
        return h_dis_int
