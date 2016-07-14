/* 
    sample code for H264 for pattern 720x480 input and bitstream output
    This sample code is to do encode 1000 stream frames named "/tmp/dev0.264"
    #./h264_main test.yuv
 */

#include <unistd.h>  
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "FB.h"
#include "V4L.h"




#define VIDEO_PALETTE_YUV420P_MACRO		50		/* YUV 420 Planar Macro */

static int s_i32FBWidth;
static int s_i32FBHeight;

typedef unsigned long long uint64;

#define TEST_ROUND	300


static int s_i32FBWidth;
static int s_i32FBHeight;
static int InitFBDevice(uint8_t **ppu8FBBuf,uint32_t *pu32FBBufSize)
{
	int32_t i32FBFd;
	uint8_t *pu8FBBuf;
	static struct fb_var_screeninfo sVar;

	*ppu8FBBuf = MAP_FAILED;
	*pu32FBBufSize = 0;

	i32FBFd = open(FBDEVICE, O_RDWR);
	if(i32FBFd < 0){
		DEBUG_PRINT("Open FB fail\n");
		return -1;
	}

	printf("fb open successful\n");
	if (ioctl(i32FBFd, FBIOGET_VSCREENINFO, &sVar) < 0) {
		DEBUG_PRINT("ioctl FBIOGET_VSCREENINFO \n");
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

static void ClearFB(int32_t i32FBFd,uint8_t *pu8FBBufAddr)
{
	ioctl(i32FBFd, IOCTL_LCD_DISABLE_INT, 0);
	wmemset((wchar_t *)pu8FBBufAddr, 0x80008000, (s_i32FBWidth*s_i32FBHeight*2)/4);
	ioctl(i32FBFd, IOCTL_LCD_ENABLE_INT, 0);
}



int main(int argc, char **argv)
{
    int tlength=0;
    int i, fcount=0;
    
	uint8_t *pu8PicPtr;
	uint64_t u64TS;
	uint32_t u32PicPhyAdr;       


	int32_t i32FBFd;
	int32_t i32KpdFd;
	uint8_t *pu8FBBufAddr;
	uint32_t u32FBBufSize;

    printf("h264enc main\n");

   // init();
	E_IMAGE_RESOL ePreviewImgResol = eIMAGE_QVGA;
	E_IMAGE_RESOL eEncodeImgResol = eIMAGE_VGA;

	int32_t i32Ret = 0;


	uint32_t u32EncodeImgWidth=0, u32EncodeImgHeight=0;
	uint32_t u32PreviewImgWidth=0, u32PreviewImgHeight=0;

    ePreviewImgResol = eIMAGE_QVGA;
    eEncodeImgResol =   eIMAGE_QVGA;

	if(ePreviewImgResol == eIMAGE_WQVGA){
		u32PreviewImgWidth = 480;
		u32PreviewImgHeight = 272;
	}
	else if(ePreviewImgResol == eIMAGE_VGA){
		u32PreviewImgWidth = 640;
		u32PreviewImgHeight = 480;
	}
	else if(ePreviewImgResol == eIMAGE_QVGA){
		u32PreviewImgWidth = 320;
		u32PreviewImgHeight = 240;
	}

	if(eEncodeImgResol == eIMAGE_QVGA){
		u32EncodeImgWidth = 320;
		u32EncodeImgHeight = 240;
	}else if(eEncodeImgResol == eIMAGE_WQVGA){
		u32EncodeImgWidth = 480;
		u32EncodeImgHeight = 272;
	}else if(eEncodeImgResol == eIMAGE_VGA){
		u32EncodeImgWidth = 640;
		u32EncodeImgHeight = 480;
	}


	DEBUG_PRINT("Set preview image resolution %d \n", ePreviewImgResol);
	DEBUG_PRINT("Set encode image resolution %d \n", eEncodeImgResol);

	// Init frame buffer
	i32FBFd = InitFBDevice(&pu8FBBufAddr, &u32FBBufSize);
	printf("FB = %d\n",  i32FBFd);
	if(i32FBFd < 0){
		//close(i32KpdFd);
		i32Ret = -1;
		printf("Init fb device fail\n");
		goto exit_prog;
	}

	//Init video in
	if(InitV4LDevice(eEncodeImgResol) != ERR_V4L_SUCCESS){
		ioctl(i32FBFd, IOCTL_LCD_DISABLE_INT, 0);
		ioctl(i32FBFd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_RGB565); //2012-0808
		ioctl(i32FBFd, IOCTL_LCD_ENABLE_INT, 0);
		munmap(pu8FBBufAddr, u32FBBufSize);
		close(i32FBFd);
		close(i32KpdFd);
		i32Ret = -1;
		printf("Init V4L device fail\n");
		goto exit_prog;
	}
	printf("Init V4L device pass\n");

	// for stride issue
	printf("Ready to start preview\n");

	StartPreview();


	ioctl(i32FBFd, IOCTL_LCD_ENABLE_INT, 0);

	StopV4LCapture();
	SetV4LEncode(0, u32EncodeImgWidth, u32EncodeImgHeight, VIDEO_PALETTE_YUV420P_MACRO);


	SetV4LViewWindow(s_i32FBWidth, s_i32FBHeight, u32PreviewImgWidth, u32PreviewImgHeight);
	printf("Ready to start capture\n");
	StartV4LCapture();
	printf("Start capturing\n");

	ioctl(i32FBFd, IOCTL_LCD_ENABLE_INT, 0);

    

    fcount = TEST_ROUND;
    for (i=0; i < fcount; i++)
    {
     
	    if(ReadV4LPicture(&pu8PicPtr, &u64TS, &u32PicPhyAdr) == ERR_V4L_SUCCESS){ 
		    // got the planar buffer address ==> u32PicPhyAdr
	    }  
	    TriggerV4LNextFrame();
	    

    }

    printf("Total Frame %d encode Done, total bitstream = %d \n", i, tlength);

    sync();
    
    ClearFB(i32FBFd,pu8FBBufAddr);


	FinializeV4LDevice();
	munmap(pu8FBBufAddr, u32FBBufSize);
	ioctl(i32FBFd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_RGB565);
	close(i32FBFd);
	close(i32KpdFd);
    
exit_prog:

    fflush(stdout);     

    return 0;
}
