
# Anthony Chapa 11/17/2020
# This Class is a program to calculate and relay sensor data to WU.com
# Rv00 - 11/17/20 - Initial GitHub Release
# 

import requests
import stormcalc as calc
from datetime import datetime

class WU_Update:
    def __init__(self,sensor_id,sensor_elev,sta_id,sta_pw):
      
        self.sen_id = sensor_id          # Sensor Topic
        self.sen_elev = sensor_elev            # Sensor Elevation (m abv sl)
        self.WU_ID = sta_id                    # WU station ID
        self.WU_PW = sta_pw                    # WU key PW
        self.cyc_time = 60                     # Default seconds time delay between posts
        self.sen_lag = 11                      # Default sensor register length
        
        self.tc_list = []                      # Temp register for Calcs
        self.bp_list = []                      # Press register for Calcs
        self.rh_list = []                      # Rel. Hum. register for Calcs
        self.ep_list = []
        
        self.last_post = 0                     # Epoch time of last post
        

    def post_WUData(self,e_time,temp_F,baro_inmg,humid_per,dewpt_F):

        t = round(temp_F,1)
        p = round(baro_inmg,2)
        rh = round(humid_per,0)
        dp = round(dewpt_F,1)

        WUurl = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?"
        WUid = self.WU_ID
        WUpwd = self.WU_PW
        WUcreds = "ID=" + WUid + "&PASSWORD=" + WUpwd
        date_str = "&dateutc=now"
        action_str = "&action=updateraw"
        datetime_time = datetime.fromtimestamp(e_time)

        try:
            site_url = WUurl+WUcreds+date_str+"&tempf="+str(t)+"&baromin="+str(p)+"&humidity="+str(rh)+"&dewptf="+str(dp)+action_str
            r=requests.get(site_url, timeout=.500)
            #print("Received " + str(r.status_code) + " " + str(r.text))
            print("Time:"+str(datetime_time)+"  Sent:"+ str(r.status_code)+"  Data: T:"+str(t)+"*F P:"+str(p)+"*inHg RH:"+str(rh)+"% DP:"+str(dp)+"*F")
    
        except:
            print("Internet Down, skipping Update" + str(r.status_code) + " " + str(r.text))
            pass
        
        
    def update(self,payload_str):
        sensor, ip, irrs, epoch, temp_c, press_hpa, rh = payload_str.split(',')
        epoch = int(epoch)
        temp_c= float(temp_c) 
        press_hpa=float(press_hpa)
        rh = float(rh)  
        
        if sensor == self.sen_id:     
            self.ep_list.append(epoch)
            self.tc_list.append(temp_c)
            self.bp_list.append(press_hpa) 
            self.rh_list.append(rh)
            
            if len(self.ep_list)>self.sen_lag:
                self.ep_list.pop(0)
                self.tc_list.pop(0)
                self.bp_list.pop(0)
                self.rh_list.pop(0)
            
           
            ep_rng = round(((max(self.ep_list)-min(self.ep_list))/60),3)
            ep_delay = epoch - self.last_post
            
            if (ep_delay >= self.cyc_time):             
                
                tc_avg = sum(self.tc_list) / len(self.tc_list)
                bp_avg = sum(self.bp_list) / len(self.bp_list)
                rh_avg = sum(self.rh_list) / len(self.rh_list)

                temp_f = calc.ConvertT(tc_avg,'C','F')
                msl_hPa = calc.mslpressure(bp_avg,tc_avg,rh_avg,self.sen_elev)
                msl_inHg = calc.ConvertP(msl_hPa,'hPa','inHg')
                dewpt_K = calc.DewPoint(tc_avg,rh_avg)
                dewpt_F = calc.ConvertT(dewpt_K,'K','F')
                

                self.post_WUData(epoch,temp_f,msl_inHg,rh_avg,dewpt_F)
                print("avg. res:",ep_rng,"mins ","last post:",ep_delay,"s","msl_hPa:",round(msl_hPa,2),"msl_inHg:",round(msl_inHg,2)),
                print("tc_avg:",round(tc_avg,2),self.tc_list)
                print("bp_avg:",round(bp_avg,2),self.bp_list)
                self.last_post = epoch
                print("---------------------------------------------------")
                    
        else:
            pass
        

     
            

        
        
        
        
