/*
 * FB.c
 *
 *  Created on: 2016年7月14日
 *      Author: root
 */

#include"FB.h"


/*
static int s_i32FBWidth;
static int s_i32FBHeight;
int InitFBDevice(uint8_t **ppu8FBBuf,uint32_t *pu32FBBufSize)
{
	int32_t i32FBFd;
	uint8_t *pu8FBBuf;
	static struct fb_var_screeninfo sVar;

	*ppu8FBBuf = MAP_FAILED;
	*pu32FBBufSize = 0;

	i32FBFd = open(FBDEVICE, O_RDWR);
	if(i32FBFd < 0){
		printf("Open FB fail\n");
		return -1;
	}

	printf("fb open successful\n");
	if (ioctl(i32FBFd, FBIOGET_VSCREENINFO, &sVar) < 0) {
		printf("ioctl FBIOGET_VSCREENINFO \n");
		close(i32FBFd);
		return -1;
	}

	s_i32FBWidth = sVar.xres;
	s_i32FBHeight = sVar.yres;

	pu8FBBuf = mmap( NULL, s_i32FBWidth*s_i32FBHeight*2 , PROT_READ|PROT_WRITE, MAP_SHARED, i32FBFd, 0 );
	if ((unsigned char*)-1 == pu8FBBuf) {
		close(i32FBFd);
		return -1;
	}
	*pu32FBBufSize = s_i32FBWidth*s_i32FBHeight*2;

	ioctl(i32FBFd, IOCTL_LCD_DISABLE_INT, 0);


	ioctl(i32FBFd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_YCBYCR);
	//2012-0808 wmemset((wchar_t *)pu8FBBuf, 0x80108010, (s_i32FBWidth*s_i32FBHeight*2)/4);

	ioctl(i32FBFd, IOCTL_LCD_ENABLE_INT, 0);
	*ppu8FBBuf = pu8FBBuf;
	return i32FBFd;
}

void ClearFB(int32_t i32FBFd,uint8_t *pu8FBBufAddr)
{
	ioctl(i32FBFd, IOCTL_LCD_DISABLE_INT, 0);
	wmemset((wchar_t *)pu8FBBufAddr, 0x80008000, (s_i32FBWidth*s_i32FBHeight*2)/4);
	ioctl(i32FBFd, IOCTL_LCD_ENABLE_INT, 0);
}
*/
