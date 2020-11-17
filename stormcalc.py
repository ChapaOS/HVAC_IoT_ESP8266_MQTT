# Anthony Chapa 11/17/2020
# file is a collection of conversion calculations
# Rv00 - 11/17/20 - Initial GitHub Release
# 

import math

def ConvertT(t,x,y):

    if (x == 'K' and y == 'C'):
        t = t - 273.15
    elif (x == 'K' and y == 'F'):
        t = ((t - 273.15)*1.8)+32
    elif  (x == 'C' and y == 'K'):
        t = t + 273.15
    elif (x == 'C' and y == 'F'):
        t = (t*1.8)+32
    elif (x == 'F' and y == 'C'):
        t = (t-32)*(5/9)
    elif (x == 'F' and y == 'K'):
        t = (t-32)*(5/9) + 273.15      
    else:
        return False
    return round(t,12)

                
def ConvertP(p,x,y):
    values = {'hPa' :100,
              'Pa'  :1,
              'inHg':3386.375258,
              'psi' :6894.7572931783}
    a = values[x]
    b = values[y]
    p = (p*a) / b
    return round(p,12)

def SatVapPress(t):#Temp in Kelvin, sat.vap.pressure in Pa
    if t >= 273.15: 
        g0 = -2.8365744E3    * math.pow(t,-2)
        g1 = -6.028076559E3  * math.pow(t,-1)
        g2 =  1.954263612E1  * math.pow(t,0)
        g3 = -2.737830188E-2 * math.pow(t,1)
        g4 =  1.6261698E-5   * math.pow(t,2)
        g5 =  7.0229056E-10  * math.pow(t,3)
        g6 = -1.8680009E-13  * math.pow(t,4)
        g7 =  2.7150305      * math.log(t)
        calsum = g0+g1+g2+g3+g4+g5+g6+g7
        sat = math.exp(calsum)
    else:
        k0 = -5.8666426E3   * math.pow(t,-1)
        k1 =  2.232870244E1 * math.pow(t,0)
        k2 =  1.39387003E-2 * math.pow(t,1)
        k3 = -3.4262402E-5  * math.pow(t,2)
        k4 =  2.7040955E-8  * math.pow(t,3)
        k5 =  6.7063522E-1  * math.log(t)
        calsum = k0+k1+k2+k3+k4+k5
        sat = math.exp(calsum)
    return round(sat,12)

def DewPoint(t,rh):#In:Temp C / Out: Dew Point Temp K
    t = ConvertT(t,'C','K')
    sat = SatVapPress(t)
    sat = (rh / 100)*sat
    if t >= 273.15: 
        c0 =  2.0798233E2  * math.pow(math.log(sat),0)
        c1 = -2.0156028E1  * math.pow(math.log(sat),1)
        c2 =  4.6778925E-1 * math.pow(math.log(sat),2)
        c3 = -9.2288067E-6 * math.pow(math.log(sat),3)
        d0 =  1            * math.pow(math.log(sat),0)
        d1 = -1.3319669E-1 * math.pow(math.log(sat),1)
        d2 =  5.6577518E-3 * math.pow(math.log(sat),2)
        d3 = -7.5172865E-5 * math.pow(math.log(sat),3)
        upper = c0+c1+c2+c3
        lower = d0+d1+d2+d3
        dew = upper/lower
    else:
        c0 =  2.1257969E2  * math.pow(math.log(sat),0)
        c1 = -1.0264612E1  * math.pow(math.log(sat),1)
        c2 =  1.4354796E-1 * math.pow(math.log(sat),2)
        d0 =  1            * math.pow(math.log(sat),0)
        d1 = -8.2871619E-2 * math.pow(math.log(sat),1)
        d2 =  2.3540411E-3 * math.pow(math.log(sat),2)
        d3 = -2.4363951E-5 * math.pow(math.log(sat),3)
        upper = c0+c1+c2
        lower = d0+d1+d2+d3
        dew = upper/lower
    return round(dew,12)

def mslpressure(p,t,rh,h): #inputs(hPa,C,RH,m) out: msl hPa
    t = ConvertT(t,'C','K')
    gn = 9.80665 #m/s/s
    r  = 287.05  #j/kg/k
    ch = .12 #k/hPa
    a  = .0065 #k/m
    kp = .0148275 #k/m
    es = SatVapPress(t)*(rh/100)
    es = ConvertP(es,'Pa','hPa')
    
    upper = (gn/r)*h
    lower = t+((a*h)/2)+ch*es
    p0 = p*math.exp(upper/lower)
    return round(p0,12)

