
/**
  * @file SHT30.h
  * 
  * Helper function to read the SHT30 environment data.
  */
#pragma once
#include "Data.h"

/* Read the SHT30 environment chip data */
bool GetSHT30Values(MyData &myData)
{
   M5.SHT30.UpdateData();
   if(M5.SHT30.GetError() == 0) {
      myData.sht30Temperatur = (int) M5.SHT30.GetTemperature();
      myData.sht30Humidity   = (int) M5.SHT30.GetRelHumidity();
      return true;
   }
   return false;
}
