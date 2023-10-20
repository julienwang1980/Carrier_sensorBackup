
import numpy as np
import pandas as pd
import numbers


class Property():

    def cal_t_sat(self, pa):
        """
        Calculated saturation temperature.
        t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377

        Parameters
        ----------
        pa : Numerical-> int flot or Series -> np.array, pd.Series
            Pressure in kPa.

        Returns
        ----------
        t_sat: Numerical-> int, flot or Series -> np.array, pd.Series, list
            saturation temperature in ℃
        """
        if isinstance(pa, pd.Series) or isinstance(pa, np.ndarray):
            t_sat = -2107.935 / (np.log(pa*1000)-21.8205)-256.2377
            return t_sat
        if isinstance(pa, list):
            t_sat = (-2107.935 / (np.log(np.array(pa)*1000) -
                     21.8205)-256.2377).tolist()
            return t_sat
        if isinstance(pa, int) or isinstance(pa, float):
            t_sat = (-2107.935 / (np.log(pa*1000)-21.8205)-256.2377).tolist()
            return t_sat

        return 'TYPE ERROR, Not support type'

    def cal_sat_gas_vol(self, pa):
        """
        Calculated Saturated gas specific volume.
        t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
        v_g=EXP((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461*10^(-4)*t_sat+111.95294*10^(-6)*t_sat^2-31.84587*10^(-7)*t_sat^3)

        Parameters
        ----------
        pa : Numerical-> int flot or Series -> np.array, pd.Series
            Pressure in kPa.

        Returns
        ----------
        t_sat: Numerical-> int, flot or Series -> np.array, pd.Series, list
            sSaturated gas specific volume in m^3/s
        """

        if isinstance(pa, pd.Series) or isinstance(pa, np.ndarray):
            t_sat = self.cal_t_sat(pa)
            v_g = np.exp((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461 *
                                                               10**(-4)*t_sat+111.95294*10**(-6)*t_sat**2-31.84587*10**(-7)*t_sat**3)
            return v_g
        if isinstance(pa, list):
            t_sat = self.cal_t_sat(pa)
            t_sat = np.array(t_sat)
            v_g = np.exp((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461 *
                                                               10**(-4)*t_sat+111.95294*10**(-6)*t_sat**2-31.84587*10**(-7)*t_sat**3)
            v_g = v_g.tolist()
            return v_g
        if isinstance(pa, int) or isinstance(pa, float):
            t_sat = self.cal_t_sat(pa)
            v_g = np.exp((-11.93809+1873.567/(t_sat+273.15)))*(5.24253-369.32461 *
                                                               10**(-4)*t_sat+111.95294*10**(-6)*t_sat**2-31.84587*10**(-7)*t_sat**3)
            v_g = v_g.tolist()
            return v_g

        return 'TYPE ERROR, Not support type'

    def cal_h_sat_gas(self, pa):
        """
        Calculated Enthalpy of saturated gas.
        t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
        h_sg=280998.3+332.614*t_sat-4.699265*t_sat^2-51.2569*10^(-3)*t_sat^3

        Parameters
        ----------
        pa : Numerical-> int flot or Series -> np.array, pd.Series
            Pressure in kPa.

        Returns
        ----------
        h_sg: Numerical-> int, flot or Series -> np.array, pd.Series, list
            Calculated Enthalpy of saturated gas in kJ/kg
        """
        if isinstance(pa, pd.Series) or isinstance(pa, np.ndarray):
            t_sat = self.cal_t_sat(pa)
            h_sg = 280998.3+332.614*t_sat-4.699265 * \
                t_sat**2-51.2569*10**(-3)*t_sat**3
            return h_sg
        if isinstance(pa, list):
            t_sat = self.cal_t_sat(pa)
            t_sat = np.array(t_sat)
            h_sg = 280998.3+332.614*t_sat-4.699265 * \
                t_sat**2-51.2569*10**(-3)*t_sat**3
            h_sg = h_sg.tolist()
            return h_sg
        if isinstance(pa, int) or isinstance(pa, float):
            t_sat = self.cal_t_sat(pa)
            h_sg = 280998.3+332.614*t_sat-4.699265 * \
                t_sat**2-51.2569*10**(-3)*t_sat**3
            return h_sg

        return 'TYPE ERROR, Not support type'

    def cal_h_sc_sh(self, pa, t):
        """
        Calculated Enthalpy of superheated/subcooled gas
        t_sat = -2107.935/(ln(pa*1000)-21.8205)-256.2377
        h_sat_gas = 280998.3+332.614*t_sat-4.699265*t_sat^2-51.2569*10^(-3)*t_sat^3
        h_sh_sc = (1+3.3247*10^(-3)*(t-t_sat)+3.62592*10^(-7)*(t-t_sat)^2+30.40633*10^(-6)*
        (t-t_sat)*t_sat-18.47693*10^(-8)*(t-t_sat)^2*t_sat+76.64206*10^(-8)*(t-t_sat)*(t_sat)^2-60.2765*10^(-10)*(t-t_sat)^2*t_sat^2)*h_sg

        Parameters
        ----------
        pa : Numerical-> int flot or Series -> np.array, pd.Series
            Pressure in kPa.
        t : Numerical-> int flot or Series -> np.array, pd.Series
            Gas temperature in ℃
        Returns
        ----------
        h_sg: Numerical-> int, flot or Series -> np.array, pd.Series, list
            Calculated Enthalpy of saturated gas in kJ/kg
        """
        if isinstance(pa, type(t)):
            pass
        else:
            return 'TYPE ERROR, The type of parameters are not same'
        if isinstance(pa, pd.Series) or isinstance(pa, np.ndarray):
            t_sat = self.cal_t_sat(pa)
            h_sg = self.cal_h_sat_gas(pa)
            deta_t = t - t_sat
            h_sh_sc = (1+3.3247*10**(-3)*deta_t+3.62592*10**(-7)*deta_t**2+30.40633*10**(-6)*deta_t*t_sat-18.47693 *
                       10**(-8)*deta_t**2*t_sat+76.64206*10**(-8)*deta_t*(t_sat)**2-60.2765*10**(-10)*deta_t**2*t_sat**2)*h_sg
            return h_sh_sc
        if isinstance(pa, list):
            t = np.array(t)
            t_sat = np.array(self.cal_t_sat(pa))
            h_sg = np.array(self.cal_h_sat_gas(pa))
            deta_t = t - t_sat
            h_sh_sc = (1+3.3247*10**(-3)*deta_t+3.62592*10**(-7)*deta_t**2+30.40633*10**(-6)*deta_t*t_sat-18.47693 *
                       10**(-8)*deta_t**2*t_sat+76.64206*10**(-8)*deta_t*(t_sat)**2-60.2765*10**(-10)*deta_t**2*t_sat**2)*h_sg
            h_sh_sc = h_sh_sc.tolist()
            return h_sh_sc

        if isinstance(pa, int) or isinstance(pa, float):
            t_sat = self.cal_t_sat(pa)
            h_sg = self.cal_h_sat_gas(pa)
            deta_t = t - t_sat
            h_sh_sc = (1+3.3247*10**(-3)*deta_t+3.62592*10**(-7)*deta_t**2+30.40633*10**(-6)*deta_t*t_sat-18.47693 *
                       10**(-8)*deta_t**2*t_sat+76.64206*10**(-8)*deta_t*(t_sat)**2-60.2765*10**(-10)*deta_t**2*t_sat**2)*h_sg
            return h_sh_sc

        return 'TYPE ERROR, Not support type'


if __name__ == '__main__':

    test_data_list = [4000, 3950, 3750, 3500]
    test_data_t_list = [90,90,90,100]
    test_data_df = pd.DataFrame(columns=['test'], data=test_data_list)['test']
    test_data_np = np.array(test_data_list)
    test_data_number = test_data_list[0]

    test_type = test_data_number
    pt = Property()
    ts = pt.cal_t_sat(test_type)
    vg = pt.cal_sat_gas_vol(test_type)
    h_s = pt.cal_h_sat_gas(test_type)
    h_sh_sc = pt.cal_h_sc_sh(test_type,test_data_t_list[0])
    print(ts)
    print(vg)
    print(h_s)
    print(h_sh_sc)
