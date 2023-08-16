#ifndef H_GD
#define H_GD

#include <gd.h> 
#include <gdfontl.h>

#include "../jsonextension.h"

namespace core
{

class gd
{
	public:
	    gd() {};
	    ~gd() {};

	    static void dashboard(Json::Value data)
	    {
      // https://byrushan.com/projects/material-admin/app/2.6.1/angular/#/home
      // http://libgd.github.io/manuals/2.2.3/files/gdft-c.html 
        
         Json::Value meta = core::json::file("../src/platform/gd/dashboard-meta.json");
     
         /* Declare the image */
         gdImagePtr im;
         /* Declare output files */
         FILE *pngout;
         /* Declare color indexes */
         int color;
         int background;

         /* Allocate the image: 64 pixels across by 64 pixels tall */
         im = gdImageCreate(meta["size"]["width"].asInt(),meta["size"]["height"].asInt());

         /* Allocate the color black (red, green and blue all minimum).
            Since this is the first color in a new image, it will
            be the background color. */
         background = gdImageColorAllocate(im, 33, 150, 243);
         color = gdImageColorAllocate(im, 255, 255, 255);


         int brect[8];
         char *font_family = new char[100];
         char *text = new char[100];
         double font_size;
         int x, y;

      // Рисуем легенду
         memcpy(font_family,meta["legend"]["font"].asString().c_str(),100);
         x = meta["legend"]["x"].asInt();
         y = meta["legend"]["y"].asInt();
         font_size = meta["legend"]["font_size"].asInt();
         memcpy(text,data["legend"].asString().c_str(),100);
         gdImageStringFT(im,brect,color,font_family,font_size,0.0,x,y,text);

      // Рисуем значение
         memcpy(font_family,meta["value"]["font"].asString().c_str(),100);
         x = meta["value"]["x"].asInt();
         y = meta["value"]["y"].asInt();
         font_size = meta["value"]["font_size"].asInt();
         memcpy(text,data["value"].asString().c_str(),100);
         gdImageStringFT(im,brect,color,font_family,font_size,0.0,x,y,text);

      // Min & Max   
         int min = 0;
         int max = 0;

         for(auto it = data["diagram"].begin(); it != data["diagram"].end(); it++)
         {
            int val = (*it).asInt();
            if(min > val)
            {
                min = val;
            }

            if(max < val)
            {
                max = val;
            }
         }

         int range = max - min;
         double step = 60. / (double) range;

         int start = 220;

         for(auto it = data["diagram"].begin(); it != data["diagram"].end(); it++)
         {
            int val = max - (*it).asInt();
            int size = val * step;

            gdImageFilledRectangle(im, start + 2, size + 20, start + 8,80, color);
            start = start + 10;
         }



        /*gdImageFilledRectangle(im, 222,20, 228,80, color);
        gdImageFilledRectangle(im, 232,40, 238,80, color);
        gdImageFilledRectangle(im, 242,25, 248,80, color);
        gdImageFilledRectangle(im, 252,50, 258,80, color);
        gdImageFilledRectangle(im, 262,37, 268,80, color);
        gdImageFilledRectangle(im, 272,30, 278,80, color);
        gdImageFilledRectangle(im, 282,42, 288,80, color);*/


        pngout = fopen("test.png", "wb");
        gdImagePng(im, pngout);
        fclose(pngout);

        /* Destroy the image in memory. */
        gdImageDestroy(im);
  	    	
	    }




};

}
#endif