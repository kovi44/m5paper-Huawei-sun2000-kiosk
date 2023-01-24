/*
   Copyright (C) 2022 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file Display.h
  * 
  * Main class for drawing the content to the e-paper display.
  */
#pragma once
#include "Data.h"
#include "Icons.h"


M5EPD_Canvas canvas(&M5.EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class SolarDisplay
{
protected:
   MyData &myData; //!< Reference to the global data
   int     maxX;   //!< Max width of the e-paper
   int     maxY;   //!< Max height of the e-paper

protected:
   void   DrawCircle            (int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom = 0, int32_t degTo = 360);
   void   DrawIcon              (int x, int y, const uint16_t *icon, int dx = 64, int dy = 64, bool highContrast = false);
   void   DrawGraph             (int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, double yMax, float values[]);
   String FormatString          (String format, double data, int fillLen = 4); 
   void   DrawHead              (int x, int y, int dx, int dy);
   void   DrawHeadVersion       (int x, int y);
   void   DrawHeadUpdated       (int x, int y);
   void   DrawHeadRSSI          (int x, int y);
   void   DrawHeadBattery       (int x, int y);
   void   DrawBody              (int x, int y, int dx, int dy);


   void   DrawRealtimePower       (int x, int y, int dx, int dy);
   void   DrawYeldToday       (int x, int y, int dx, int dy);
   void   DrawYeldMonth       (int x, int y, int dx, int dy);
   void   DrawYeldYear       (int x, int y, int dx, int dy);
   void   DrawTotalYeld      (int x, int y, int dx, int dy);

public:
   SolarDisplay(MyData &md, int x = 960, int y = 540)
      : myData(md)
      , maxX(x)
      , maxY(y)
   {
   }

   void Show();
   void ShowWiFiError(String ssid);
};

/* Draw a circle with optional start and end point */
void SolarDisplay::DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */)
{
   for (int i = degFrom; i < degTo; i++) {
      double radians = i * PI / 180;
      double px      = x + r * cos(radians);
      double py      = y + r * sin(radians);
      
      canvas.drawPixel(px, py, color);
   }
} 

/* Draw one icon from the binary data */
void SolarDisplay::DrawIcon(int x, int y, const uint16_t *icon, int dx /*= 64*/, int dy /*= 64*/, bool highContrast /*= false*/)
{
   for (int yi = 0; yi < dy; yi++) {
      for (int xi = 0; xi < dx; xi++) {
         uint16_t pixel = icon[yi * dx + xi];

         if (highContrast) {
            if (15 - (pixel / 4096) > 0) canvas.drawPixel(x + xi, y + yi, M5EPD_Canvas::G15);
         } else {
            canvas.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
         }
      }
   }
}

/* Draw a graph with x- and y-axis and values */
void SolarDisplay::DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, double yMax, float values[])
// DrawGraph(15, 350, 400, 600, "Active Power (Wh) blebnlebkkekkdskkdsjjdskdjdskdjdskdjskjds",  0, 287, 0, myData.huawei.maxYeld, myData.huawei.historyYeld);
{
  int yMaxRound = round(yMax);
   String yMinString = String(yMin);
   String yMaxString = String(yMaxRound);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   //int    graphDX    = dx - textWidth - 20;
   int    graphDX    = dx - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / (xMax - xMin);
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(2);
   canvas.drawCentreString(title, x + dx / 2, y + dy - 4, 1);
   canvas.setTextSize(1);
   canvas.drawString(yMaxString, x + 5, graphY - 5);   
   canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);   
   //for (int i = 0; i <= (xMax - xMin); i++) {
   //   canvas.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);   
   //}
   
   //canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);   
   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);   
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);         
      }
   }
   for (int i = xMin; i <= xMax; i++) {
      float yValue   = values[i - xMin];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / (xMax - xMin) * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.fillCircle(xPos, yPos, 1, M5EPD_Canvas::G15);
      if (i > xMin) {
         canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);         
      }
      iOldX = xPos;
      iOldY = yPos;
   }
}

/* Printf operation as String. */
String SolarDisplay::FormatString(String format, double data, int fillLen /*= 9*/)
{
   char buff[100];

   memset(buff, 0, sizeof(buff));
   sprintf(buff, format.c_str(), data);

   String fill;
   int    padLen = (int) fillLen - (int) strlen(buff);

   for (int i = 0; i < padLen; i++) {
      fill += ' ';
   }

   return fill + buff;
}

/* ********************************************************************************************* */

/* Draw the version text.  */
void SolarDisplay::DrawHeadVersion(int x, int y)
{
   canvas.drawString(VERSION, x, y);
}

/* Draw the information when are these data updated. */
void SolarDisplay::DrawHeadUpdated(int x, int y)
{
   String updatedString = "Updated " + getDateTimeString(GetRTCTime());
   
   canvas.drawCentreString(updatedString, x, y, 1);
}

/* Draw the wifi reception strength */
void SolarDisplay::DrawHeadRSSI(int x, int y)
{
   int iQuality = WifiGetRssiAsQualityInt(myData.wifiRSSI);

   canvas.drawRightString(WifiGetRssiAsQuality(myData.wifiRSSI) + "%", x - 2, y - 14, 1);
   
   if (iQuality >= 80) DrawCircle(x + 12, y, 16, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 40) DrawCircle(x + 12, y, 12, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 20) DrawCircle(x + 12, y,  8, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 10) DrawCircle(x + 12, y,  4, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >=  0) DrawCircle(x + 12, y,  2, M5EPD_Canvas::G15, 225, 315); 
}

/* Draw the state of charge. */
void SolarDisplay::DrawHeadBattery(int x, int y)
{
   canvas.drawRect(x, y, 40, 16, M5EPD_Canvas::G15);
   canvas.drawRect(x + 40, y + 3, 4, 10, M5EPD_Canvas::G15);
   for (int i = x; i < x + 40; i++) {
      canvas.drawLine(i, y, i, y + 15, M5EPD_Canvas::G15);
      if ((i - x) * 100.0 / 40.0 > myData.batteryCapacity) {
         break;
      }
   }
   canvas.drawRightString(String(myData.batteryCapacity) + "%", x - 2, y + 1, 1);
}

/* Draw realtime power info. */
void SolarDisplay::DrawRealtimePower(int x, int y, int dx, int dy)
{
   String   RealtimePower                       = FormatString("%.2f",  myData.huawei.realTimePower,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(RealtimePower,                x +   90, y +  105,1);
   canvas.setTextSize(2); 
   canvas.drawCentreString("Real-Time", x +  90, y +  135,1);
   canvas.drawCentreString("power(kW)", x + 90, y +  155,1);
   DrawIcon(x + 43, y + 10, (uint16_t *) image_data_real_time_power, 88, 88);
}

/* Draw Yeld Today info. */
void SolarDisplay::DrawYeldToday(int x, int y, int dx, int dy)
{
   String   dailyEnergy                       = FormatString("%.2f",  myData.huawei.dailyEnergy,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(dailyEnergy,                x +   90, y +  105,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("Yeld Today", x +  90, y +  135,1);
   canvas.drawCentreString("(kWh)", x + 90, y +  155,1);
   DrawIcon(x + 43, y + 10, (uint16_t *) image_data_yeld_today, 88, 88);
}

/* Draw Yeld Month info. */
void SolarDisplay::DrawYeldMonth(int x, int y, int dx, int dy)
{
   String   monthEnergy                       = FormatString("%.2f",  myData.huawei.monthEnergy,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(monthEnergy,                x +   90, y +  105,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("Yeld this", x +  90, y +  135,1);
   canvas.drawCentreString("Month(kWh)", x + 90, y +  155,1);
   DrawIcon(x + 43, y + 10, (uint16_t *) image_data_yeld_month, 88, 88);
}

/* Draw Yeld Year info. */
void SolarDisplay::DrawYeldYear(int x, int y, int dx, int dy)
{
   String   yearEnergy                       = FormatString("%.2f",  myData.huawei.yearEnergy,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(yearEnergy,                x +   90, y +  105,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("Yeld this", x +  90, y +  135,1);
   canvas.drawCentreString("Year(kWh)", x + 90, y +  155,1);
   DrawIcon(x + 43, y + 10, (uint16_t *) image_data_yeld_year, 88, 88);
}

/* Draw Total Yeld info. */
void SolarDisplay::DrawTotalYeld(int x, int y, int dx, int dy)
{
   String   cumulativeEnergy                       = FormatString("%.2f",  myData.huawei.cumulativeEnergy,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(cumulativeEnergy,                x +   90, y +  105,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("Total Yeld", x +  90, y +  135,1);
   canvas.drawCentreString("(kWh)", x + 90, y +  155,1);
   DrawIcon(x + 43, y + 10, (uint16_t *) image_data_yeld_total, 88, 88);
}


/* Draw the complete head. */
void SolarDisplay::DrawHead(int x, int y, int dx, int dy)
{
   DrawHeadVersion  (x + 5,         y + 13);
   DrawHeadUpdated( (x + (dx / 2)), y + 13);
   DrawHeadRSSI     (x + dx - 130,  y + 26);
   DrawHeadBattery  (x + dx -  49,  y + 11);
}

/* Draw the whole solar information body. */
void SolarDisplay::DrawBody(int x, int y, int dx, int dy)
{
   canvas.drawRect(x, y, dx, dy, M5EPD_Canvas::G15);
  
   //canvas.drawRect(x + 5, y + 5 , dx - 10, dy - 270, M5EPD_Canvas::G15);
   canvas.drawLine(x + 188, y + 40, x + 185, dy - 300, M5EPD_Canvas::G15);
   canvas.drawLine(x + 376, y + 40, x + 376, dy - 300, M5EPD_Canvas::G15);
   canvas.drawLine(x + 564, y + 40, x + 564, dy - 300, M5EPD_Canvas::G15);
   canvas.drawLine(x + 752, y + 40, x + 752, dy - 300, M5EPD_Canvas::G15);
   
   DrawRealtimePower    (x +  5, y +  5, x + 185, dy - 250);
   DrawYeldToday        (x +  5 + 188, y +  5, x + 185 + 188, dy - 250);
   DrawYeldMonth        (x +  5 + 188 + 188, y +  5, x + 185 + 188 + 188, dy - 250);
   DrawYeldYear         (x +  5 + 188 + 188 + 188 , y +  5, x + 185 + 188 + 188 + 188, dy - 250);
   DrawTotalYeld        (x +  5 + 188 + 188 + 188 + 188, y +  5, x + 185 + 188 + 188 + 188, dy - 250);

   DrawGraph(15, 200, 600, 300, "Active Power Chart (kWh) / Today (24h) ",  0, 287, 0, myData.huawei.maxYeld, myData.huawei.historyYeld);

   DrawCircle(760, 300, 85, M5EPD_Canvas::G15, 0, 360); 
   //DrawCircle(760, 300, 86, M5EPD_Canvas::G15, 0, 360); 
   //DrawCircle(760, 300, 87, M5EPD_Canvas::G15, 0, 360);

   DrawCircle(685, 430, 85, M5EPD_Canvas::G15, 0, 360); 
   //DrawCircle(685, 430, 86, M5EPD_Canvas::G15, 0, 360); 
   //DrawCircle(685, 430, 87, M5EPD_Canvas::G15, 0, 360);

   DrawCircle(835, 430, 85, M5EPD_Canvas::G15, 0, 360); 
   //DrawCircle(835, 430, 86, M5EPD_Canvas::G15, 0, 360); 
   //DrawCircle(835, 430, 87, M5EPD_Canvas::G15, 0, 360);

   DrawIcon(734,225, (uint16_t *) image_data_co2, 52, 52);
   DrawIcon(659,360, (uint16_t *) image_data_coal, 52, 52);
   DrawIcon(809,360, (uint16_t *) image_data_trees, 52, 52);

   String   co2                       = FormatString("%.2f",  myData.huawei.co2,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(co2,                760, 290,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("Avoided of", 760, 315,1);
   canvas.drawCentreString("CO2 (kg)", 760, 335,1);

   String   coal                       = FormatString("%.2f",  myData.huawei.coal,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(coal,                685, 420,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("Std. coal", 685, 445,1);
   canvas.drawCentreString("saved", 685, 465,1);

   String   tree                       = FormatString("%.2f",  myData.huawei.tree,                 4);
   canvas.setTextSize(3);
   canvas.drawCentreString(tree,                835, 420,1);
   canvas.setTextSize(2);
   canvas.drawCentreString("trees", 835, 445,1);
   canvas.drawCentreString("planted", 835, 465,1);
}

/* Fill the screen. */
void SolarDisplay::Show()
{
   Serial.println("SolarDisplay::DrawSolarInfo");

   canvas.setTextSize(2);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);
   canvas.createCanvas(maxX, maxY);

   DrawHead(14,  0, maxX - 28, 33);
   DrawBody(14, 34, maxX - 28, maxY - 45);

   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
   delay(2000);
}

/* Show WiFi connewction error. */
void SolarDisplay::ShowWiFiError(String ssid)
{
   Serial.println("SolarDisplay::ShowWiFiError");

   canvas.setTextSize(4);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);
   canvas.createCanvas(maxX, maxY);

   String errMsg = "WiFi error: [" + ssid + "]";
   canvas.drawCentreString(errMsg, maxX / 2, maxY / 2, 1);

   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
}
