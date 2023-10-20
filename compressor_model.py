import numpy as np
import pandas as pd


class Compressor_model():

    def __init__(self, Pd, Ps, CompSpeed, ST, CompSpeed_rated, custom_init_coe=None) -> None:
        """
        Create a compressor model

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

        custom_init_coe: list
            Default Value = [55.85, -127.29, 700.04, 1.2347, 0.06207, 0.23527, 1.7695, 0, 0, 1.1473, 0, 0, 0.88358, -0.41507,
                        0.14333, 0.15895, -0.058611, 0.022684, 19.922, 4.6538, -195.28, 712.67, -43.42, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        Returns
        ----------
        mid_coe: list
            Intermediate coefficient
        """
        if isinstance(custom_init_coe, list) and len(custom_init_coe) == 32:
            self.coe_a = custom_init_coe
        else:
            self.coe_a = [55.85, -127.29, 700.04, 1.2347, 0.06207, 0.23527, 1.7695, 0, 0, 1.1473, 0, 0, 0.88358, -0.41507,
                          0.14333, 0.15895, -0.058611, 0.022684, 19.922, 4.6538, -195.28, 712.67, -43.42, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.Pd = Pd
        self.Ps = Ps
        self.CompSpeed = CompSpeed
        self.ST = ST
        self.CompSpeed_rated = CompSpeed_rated
        self.Pr = self.Pd/self.Ps
        self.sr = self.CompSpeed/self.CompSpeed_rated
        self.mid_coe = self._cal_mid_coe()
        self.volume = self._cal_volume()
        self.power = self._cal_power()
        # self.current = self._cal_current()

    def _cal_mid_coe(self):
        """
        Calculation of intermediate coefficients by initial coefficients

        Returns
        ----------
        mid_coe: list
            Intermediate coefficient
        """
        a = self.coe_a[0]+self.coe_a[1]*np.sqrt(self.sr)+self.coe_a[2]*self.sr
        b = self.coe_a[3]+self.coe_a[4]*self.sr**2+self.coe_a[5]*self.sr**4
        c = self.coe_a[6]+self.coe_a[7]*self.sr+self.coe_a[8]*self.sr**2
        d = self.coe_a[9]+self.coe_a[10] * \
            np.sqrt(self.sr)+self.coe_a[11]*self.sr
        y1 = self.coe_a[12]+self.coe_a[13]*self.sr+self.coe_a[14]*self.sr**2
        y2 = self.coe_a[15]+self.coe_a[16]*self.sr+self.coe_a[17]*self.sr**2
        f = (y1-y2)/(self.coe_a[18]**d-self.coe_a[19]**d)
        e = y1-f*self.coe_a[18]**d
        g = self.coe_a[20]+self.coe_a[21]*self.sr**2+self.coe_a[22]*self.sr**4
        q = self.coe_a[23]+self.coe_a[24]*self.sr+self.coe_a[25]*self.sr**2
        r = self.coe_a[26]+self.coe_a[27]*self.sr**2+self.coe_a[28]*self.sr**4
        s = self.coe_a[29]+self.coe_a[30]*self.sr**2+self.coe_a[31]*self.sr**4

        return {'a': a, 'b': b, 'c': c, 'd': d, 'y1': y1, 'y2': y2, 'f': f, 'e': e, 'g': g, 'q': q, 'r': r, 's': s}

    def _cal_volume(self):
        volume = (self.mid_coe['a']-self.mid_coe['b'] *
                  self.Pr**self.mid_coe['c'])*4.719476965*10**(-4)/60
        return volume

    def _cal_power(self):
        power = ((self.mid_coe['e']+self.mid_coe['f']*self.Pr**self.mid_coe['d'])
                 * self.Ps*0.000145*1000*self.volume/(4.719476965*10**(-4)/60))+self.mid_coe['g']
        return power
        
    def _cal_current(self):
        current = 'TYPE ERROR'
        if isinstance(self.mid_coe['q'], pd.Series):
            index_0  = self.mid_coe['q'][self.mid_coe['q']<=0]
            current = self.power/((self.mid_coe['q']*self.power+self.mid_coe['r'])*self.power+self.mid_coe['s'])
            current[index_0] = 0
        else:
            if self.mid_coe['q'] > 0:
                current= self.power/((self.mid_coe['q']*self.power+self.mid_coe['r'])*self.power+self.mid_coe['s'])
            else:
                current = 0
        return current

