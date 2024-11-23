#include "fssimplewindow.h"
#include "ysglfontdata.h"


int main(void)
{
	FsOpenWindow(0,0,800,600,1);
	for(;;)
	{
		FsPollDevice();

		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

		glRasterPos2i(100,60);
		YsGlDrawFontBitmap6x7("Font Rendering Sample");

		glRasterPos2i(100,100);
		YsGlDrawFontBitmap12x16("Font Rendering Sample");

		glRasterPos2i(100,160);
		YsGlDrawFontBitmap32x48("Font Rendering Sample");

		FsSwapBuffers(); // FsSwapBuffers() for double-buffered mode, glFlush() for single-buffered mode.
	}
}


