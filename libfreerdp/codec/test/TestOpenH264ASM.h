int freerdp_image_copy_yuv420p_to_xrgb(unsigned char* pDstData, int nDstStep, int nXDst, int nYDst,
		int nWidth, int nHeight, unsigned char* pSrcData[3], int nSrcStep[2], int nXSrc, int nYSrc);

extern int freerdp_image_yuv_to_xrgb_asm(unsigned char *pDstData,unsigned char **pSrcData,int nWidth,int nHeight,int istride0,int istride1);

extern int check_ssse3();
extern int freerdp_image_yuv420p_to_xrgb(unsigned char *pDstData,unsigned char **pSrcData,int nWidth,int nHeight,int istride0,int istride1);