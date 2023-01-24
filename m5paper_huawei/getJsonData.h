
/**
  * @file getJsonData.h
  * 
  * Helper function to communicate with the IoBroker.
  */
#pragma once

#include <ArduinoJson.h>


/* ***************************************************************************** */
/* *** Get JSON DATA Values() ***************************************************** */
/* ***************************************************************************** */


void GetHTTPValues(MyData &myData)
{
String payload;
HTTPClient http;
  
  http.begin(URL);
  int httpCode = http.GET();
  // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.print("[HTTP] GET... code:");
      Serial.println(httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        //Serial.println(payload);
        //payload2 = payload.replace('&quot;','\"');
        payload.replace("&quot;","\"");
        payload.replace("{\"data\":\"","");
        payload.replace("\",\"success\":true,\"failCode\":0}","");
        
        Serial.println(payload);
      }
    } else {
      Serial.print("[HTTP] GET... failed, error:");
      Serial.println(http.errorToString(httpCode).c_str());
    }
  http.end();  

  DynamicJsonDocument doc(20 * 1024);

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonArray dayly_list  = doc["powerCurve"]["activePower"];
      for (int i = 0; i < MAX_FORECAST; i++) {
         if (i < dayly_list.size()) {
            myData.huawei.historyYeld[i]  = dayly_list[i].as<float>();
            if (myData.huawei.historyYeld[i] > myData.huawei.maxYeld) {
               myData.huawei.maxYeld = myData.huawei.historyYeld[i];
            }
         }
      }

  myData.huawei.realTimePower = doc["realKpi"]["realTimePower"];
  myData.huawei.yearEnergy = doc["realKpi"]["yearEnergy"];
  myData.huawei.cumulativeEnergy = doc["realKpi"]["cumulativeEnergy"];
  myData.huawei.dailyEnergy = doc["realKpi"]["dailyEnergy"];
  myData.huawei.monthEnergy = doc["realKpi"]["monthEnergy"];

  myData.huawei.co2 = doc["socialContribution"]["co2Reduction"];
  myData.huawei.coal = doc["socialContribution"]["standardCoalSavings"];
  myData.huawei.tree = doc["socialContribution"]["equivalentTreePlanting"];


}
