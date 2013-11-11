/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NULL
#define NULL (void*)0
#endif 

int32 IPEmz_sYUV420toY2VY1U(tBaseVideoFrame* yuv420Frame, tBaseVideoFrame* yuv422Frame, int16 stride);
int32 IPEmz_sYUV420toUY1VY2(tBaseVideoFrame* yuv420Frame, tBaseVideoFrame* yuv422Frame, int16 stride);

/*
*******************************************************************************
Name            : IPEmz_gYUV420toY2VY1U
Description     : This fuction converts image from yuv420 to YUYV interleaved format
Parameter       : yuv420Frame :contains the decoded image of jpeg stream in yuv420 format
          yuyvFrame:this is user passed buffer to store the output in YUYV
              interleaved format
Return Value    : int32
*******************************************************************************
*/

int32 IPEmz_sYUV420toY2VY1U(tBaseVideoFrame* yuv420Frame, tBaseVideoFrame* yuv422Frame, int16 stride)
{
    int32   lLumWidth, lLumHeight;  
    uint8   *lLumPtr1;
    uint8   *lCrPtr;
    uint8   *lCbPtr;
  	int32   lY1, lCr, lCb,lTemp;
    int32   i,j;
  	uint32  *lOutyuvBufferPtr;
  	uint8		*pOutputLinePtr;
    
  	lLumWidth = yuv420Frame->width;	
	lLumHeight = yuv420Frame->height;
	
	if (stride < lLumWidth<<1)
        return E_ERROR_ARGUMENT;
	    
	lLumPtr1 = yuv420Frame->lum+lLumWidth*(lLumHeight-1);
  lCbPtr = yuv420Frame->cb+(lLumWidth>>1)*((lLumHeight>>1)-1);
  lCrPtr = yuv420Frame->cr+(lLumWidth>>1)*((lLumHeight>>1)-1);
	
	lOutyuvBufferPtr = (uint32*) (yuv422Frame->lum+((lLumHeight-1)*(stride)));	
	pOutputLinePtr= (uint8*)lOutyuvBufferPtr;
 	     
  for(i = 0; i < lLumHeight ; i ++)
	{
		j=(lLumWidth >> 1) + 1;
		while(--j)
		{
			lY1    = *lLumPtr1++; //Y1
			lCr	   = *lCrPtr++;			
			lTemp  = *lLumPtr1++; //Y2
			lCb    = *lCbPtr++;
		
			lTemp |= (lY1 << 16);			  
			lCr |= (lCb<<16);	//Cb and Cr are common			
			*lOutyuvBufferPtr++ = (uint32)(lTemp|(lCr<<8));
	    }
	    
		if(i&1)
		{		
			lCbPtr	 -= lLumWidth;
			lCrPtr	 -= lLumWidth;			
		}
		else
		{
			lCbPtr	 -= lLumWidth/2;
			lCrPtr	 -= lLumWidth/2;			
		}

		lLumPtr1 -= lLumWidth*2;	
		pOutputLinePtr -= stride;
 	  lOutyuvBufferPtr = (uint32*) pOutputLinePtr;
	}
	
  return E_SUCCESS;
}

/*
*******************************************************************************
Name            : IPEmz_gYUV420toUY1VY2
Description     : This fuction converts image from yuv420 to YUYV interleaved format
Parameter       : yuv420Frame :contains the decoded image of jpeg stream in yuv420 format
          yuyvFrame:this is user passed buffer to store the output in YUYV
              interleaved format
Return Value    : int32
*******************************************************************************
*/
#if (defined ARM_V5) || (defined ARM_V5E) || (defined __WINSCW__)
int32 IPEmz_sYUV420toUY1VY2(tBaseVideoFrame* yuv420Frame, tBaseVideoFrame* yuv422Frame, int16 stride)
{
    int32   lLumWidth, lLumHeight;  
    uint8   *lLumPtr;
    uint8   *lCrPtr;
    uint8   *lCbPtr;
  	int32   lY1, lCr, lCb,lTemp;
    int32   i,j;
	  uint32  *lOutyuvBufferPtr;
	  uint8		*pOutputLinePtr;
    
  	lLumWidth = yuv420Frame->width; 
  	lLumHeight = yuv420Frame->height;

    if (stride < lLumWidth<<1)
        return E_ERROR_ARGUMENT;
    
    lLumPtr = yuv420Frame->lum+lLumWidth*(lLumHeight-1);
    lCbPtr = yuv420Frame->cb+(lLumWidth>>1)*((lLumHeight>>1)-1);
    lCrPtr = yuv420Frame->cr+(lLumWidth>>1)*((lLumHeight>>1)-1);
	
	  lOutyuvBufferPtr = (uint32*) (yuv422Frame->lum+((lLumHeight-1)*(stride)));
	  pOutputLinePtr= (uint8*)lOutyuvBufferPtr;

    for(i = 0; i < lLumHeight ; i ++)
	  {
		j=(lLumWidth >> 1) + 1;

		while (--j)
		{
			lCr    = *lCrPtr++;
			lTemp  = *lLumPtr++; //Y1
			lCb    = *lCbPtr++;			
			lY1    = *lLumPtr++; //Y2
				
			lCb   |= (lCr << 16);	
			lTemp |= (lY1 << 16);	
			*lOutyuvBufferPtr++ = (uint32)((lTemp << 8)|(lCb));
		}
	    
		if(i&1)
		{		
			lCbPtr	 -= lLumWidth;
			lCrPtr	 -= lLumWidth;			
		}
		else
		{
			lCbPtr	 -= lLumWidth >> 1;
			lCrPtr	 -= lLumWidth >> 1;
		}

		lLumPtr -= (lLumWidth << 1);
		pOutputLinePtr -= stride;
 	  lOutyuvBufferPtr = (uint32*) pOutputLinePtr;
	}
  return E_SUCCESS;
}

#else
__asm int32 IPEmz_sYUV420toUY1VY2(tBaseVideoFrame* yuv420Frame, tBaseVideoFrame* yuv422Frame, int16 stride)
{
        STMFD    r13!,{r4-r11,lr}
        LDR      r12,[r1,#0]
        LDRH     r1,[r0,#0xc]
        LDR      r6,[r0,#4]
        LDR      r5,[r0,#8]
        LDRH     r4,[r0,#0xe]
        LSR      r3,r1,#1
        LDR      lr,[r0,#0]
                
        CMP      r2,r3,LSL #2
        MOVLT    r0,#-16
        STMLTFD   r13!,{r4-r11,pc}
        ADD      r0,r2,r2,LSR #31
        ASR      r0,r0,#1
        SUB      r2,r4,#1
        LSL      r11,r0,#2
        MUL      r0,r3,r2
        ADD      r1,r4,r4,LSR #31
        LSL      r0,r0,#1
        ADD      r0,r0,r0,LSR #31
        BIC      r0,r0,#1
        ADD      lr,r0,lr
        MVN      r0,#0
        ADD      r0,r0,r1,ASR #1
        MLA      r1,r3,r0,r6
        MLA      r0,r3,r0,r5
        ADD      r5,r11,r11,LSR #31
        ASR      r5,r5,#1
        MUL      r2,r5,r2
        ASR      r5,r2,#31
        ADD      r2,r2,r5,LSR #30
        BIC      r2,r2,#3
        ADD      r12,r2,r12
        MOV      r2,#0xff00
        ORR      r10,r2,r2,LSL #16
        MOV      r5,r3
outer_loop
        LDRH     r2,[lr],#2
        LDRB     r6,[r1],#1
        LDRB     r7,[r0],#1
inner_loop
        SUBS     r5,r5,#1
        PKHBT    r9,r2,r2,LSL #8
        PKHBT    r8,r6,r7,LSL #16
        AND      r9,r10,r9,LSL #8
        LDRBNE   r7,[r0],#1
        LDRBNE   r6,[r1],#1
        LDRHNE   r2,[lr],#2
        ORR      r8,r9,r8
        STR      r8,[r12],#4
        BNE      inner_loop
        ANDS     r2,r4,#1
        SUBEQ    r1,r1,r3
        SUBEQ    r0,r0,r3
        SUBNE    r0,r0,r3,LSL #1
        SUBNE    r1,r1,r3,LSL #1
        SUBS     r4,r4,#1
        SUB      lr,lr,r3,LSL #2
        SUB      r12,r12,r11,ASR #1
        SUB	     r12,r12,r3,LSL #2
        MOV      r5,r3
        BNE      outer_loop
        MOV      r0,#1
        LDMFD    r13!, {r4-r11,pc}

      }
#endif

/*
******************************************************************************
Name            : gColorSpaceConvertYUV
Description   : Converts YUV Data to YUV 422 Interleaved format.
Parameter       : yuv420Frame    - Source structure
          yuv422Frame  - Pointer to yuv 422 Interleaved data.
          outClrFmt   - input color format.
Return Value    : int32
******************************************************************************
*/

int32 gColorConvYUVtoYUV422Int (tBaseVideoFrame *yuv420Frame, tBaseVideoFrame* yuv422Frame,
                 uint8 outClrFmt, int stride)             
{
  if( (NULL == yuv420Frame) || (NULL == yuv422Frame) || (NULL == yuv420Frame->lum) ||
    (NULL == yuv420Frame->cb) || (NULL == yuv420Frame->cr))
  {
    return E_OUT_OF_MEMORY;
  }

  switch(outClrFmt)
  {
    case(YUV422INT_LE):
      return IPEmz_sYUV420toY2VY1U(yuv420Frame, yuv422Frame, stride);
      
    case(YUV422INT_BE):
      return IPEmz_sYUV420toUY1VY2(yuv420Frame, yuv422Frame, stride);
        
    default:
      return E_ERROR_ARGUMENT;
  }
}

