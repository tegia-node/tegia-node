#ifndef H_GM
#define H_GM

#include <Magick++.h>



class CChart2
{
	public:
	    CChart2() {};
	    ~CChart2() {};

	    static int draw()
	    {


  try {
    InitializeMagick(nullptr);

    // Create base image (white image of 300 by 200 pixels)
    Image image( Geometry(300,200), Color("white") );

    // Set draw options
    image.strokeColor("red"); // Outline color
    image.fillColor("green"); // Fill color
    image.strokeWidth(5);

    // Draw a circle
    image.draw( DrawableCircle(100,100, 50,100) );

    // Draw a rectangle
    image.draw( DrawableRectangle(200,200, 270,170) );

    // Display the result
    image.write("/home/igor/horse.png");
  }
  catch( exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }

  return 0;
  	    	
	    }
};


#endif