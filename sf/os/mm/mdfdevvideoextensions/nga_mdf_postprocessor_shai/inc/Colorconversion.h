/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "common.h"

#define CONST_19075		19075
#define CONST_26748		26748
#define CONST_6563		6563
#define CONST_13621		13621
#define CONST_33802		33802
#define CONST_29374		29374
#define CONST_8731		8731
#define CONST_1747		1747
#define CONST_34603		34603
#define CONST_22973		22973
#define CONST_5637		5637
#define CONST_11699		11699
#define CONST_29032		29032
#define CONST_25801		25801
#define CONST_3835		3835
#define CONST_30397		30397


/************* Start of Chroma2 format ******************************************/

/* For YUV420 Chroma2 input */
// For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV420Chr2toColor64k_Ordered_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_Ordered_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_Ordered_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_Ordered_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_ErrDiff_709_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_ErrDiff_601_5_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_ErrDiff_709_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor64k_ErrDiff_601_5_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV420Chr2toColor16M_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor16M_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor16M_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor16M_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV420Chr2toColor4k_Ordered_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_Ordered_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_Ordered_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_Ordered_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_ErrDiff_709_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_ErrDiff_601_5_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_ErrDiff_709_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor4k_ErrDiff_601_5_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 8:8:8:8
void sEmz_VDec_YUV420Chr2toColor16MU_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor16MU_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor16MU_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr2toColor16MU_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/* End of YUV420 input */




/* Start of YUV422 Chroma2 BE (U1 Y1 V1 Y2) input */
// For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV422BEChr2toColor64k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor64k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV422BEChr2toColor16M_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor16M_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor16M_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor16M_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV422BEChr2toColor4k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor4k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 8:8:8:8
void sEmz_VDec_YUV422BEChr2toColor16MU_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor16MU_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor16MU_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr2toColor16MU_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/* End of YUV422 Chroma2 BE (U1 Y1 V1 Y2) input */


/* Start of YUV422 Chr2 LE (Y2 V1 Y1 U1) input */
// For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV422LEChr2toColor64k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor64k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV422LEChr2toColor16M_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor16M_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor16M_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor16M_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV422LEChr2toColor4k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor4k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 8:8:8:8
void sEmz_VDec_YUV422LEChr2toColor16MU_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor16MU_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor16MU_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr2toColor16MU_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/* End of YUV422 Chr2 LE (Y2 V1 Y1 U1) input */


void sDiffuse(int32 i, int32 j, int32 diff, int32 actWidth, int32 actHeight,
			  uint8 *lLumPtr, int32 lLumWidth, uint8 skip);

/************* End of Chroma2 format ********************************************/


/************* Start of Chroma1 format ******************************************/

/* For YUV420 Chroma1 input */
//For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV420Chr1toColor64k_Ordered_709_FR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_Ordered_601_5_FR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_Ordered_709_RR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_Ordered_601_5_RR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_ErrDiff_709_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_ErrDiff_601_5_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_ErrDiff_709_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor64k_ErrDiff_601_5_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV420Chr1toColor16M_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor16M_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor16M_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor16M_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

//For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV420Chr1toColor4k_Ordered_709_FR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_Ordered_601_5_FR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_Ordered_709_RR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_Ordered_601_5_RR
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow,  tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_ErrDiff_709_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_ErrDiff_601_5_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_ErrDiff_709_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor4k_ErrDiff_601_5_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);


// For XRGB 8:8:8:8
void sEmz_VDec_YUV420Chr1toColor16MU_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor16MU_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor16MU_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr1toColor16MU_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/* For YUV422 BE Chroma1 input */
//For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV422BEChr1toColor64k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor64k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV422BEChr1toColor16M_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor16M_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor16M_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor16M_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV422BEChr1toColor4k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor4k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 8:8:8:8
void sEmz_VDec_YUV422BEChr1toColor16MU_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor16MU_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor16MU_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422BEChr1toColor16MU_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/* For YUV422 LE Chroma1 input */
//For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV422LEChr1toColor64k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor64k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV422LEChr1toColor16M_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor16M_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor16M_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor16M_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV422LEChr1toColor4k_Ordered_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_Ordered_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_Ordered_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_Ordered_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_ErrDiff_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_ErrDiff_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_ErrDiff_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor4k_ErrDiff_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 8:8:8:8
void sEmz_VDec_YUV422LEChr1toColor16MU_709_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor16MU_601_5_FR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor16MU_709_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV422LEChr1toColor16MU_601_5_RR
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/************* End of Chroma1 format ********************************************/



/************* Start of Chroma3 format ******************************************/

/* For YUV420 Chroma3 input */
// For RGB 5:6:5 Ordered and Error Diffusion
void sEmz_VDec_YUV420Chr3toColor64k_Ordered_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_Ordered_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_Ordered_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_Ordered_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_ErrDiff_709_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_ErrDiff_601_5_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_ErrDiff_709_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor64k_ErrDiff_601_5_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 4:4:4:4 Ordered and Error Diffusion
void sEmz_VDec_YUV420Chr3toColor4k_Ordered_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_Ordered_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_Ordered_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_Ordered_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_ErrDiff_709_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_ErrDiff_601_5_FR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_ErrDiff_709_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor4k_ErrDiff_601_5_RR 
		(tBaseVideoFrame* srcImage, uint8* dstImage, 
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For RGB 8:8:8
void sEmz_VDec_YUV420Chr3toColor16M_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor16M_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor16M_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor16M_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

// For XRGB 8:8:8:8
void sEmz_VDec_YUV420Chr3toColor16MU_709_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor16MU_601_5_FR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor16MU_709_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

void sEmz_VDec_YUV420Chr3toColor16MU_601_5_RR 
		(tBaseVideoFrame *srcImage, uint8* dstImage,
		 tWndParam* srcWindow, tWndParam* dstWindow);

/************* End of Chroma3 format ******************************************/

