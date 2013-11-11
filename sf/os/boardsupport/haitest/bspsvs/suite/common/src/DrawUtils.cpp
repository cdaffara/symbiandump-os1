/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "DrawUtils.h"

//These are for masks and shifts for rgb 332 data format.
const 	TUint Blue_Mask                 =0x03;
const	TUint Green_Mask                =0x1C;
const	TUint Red_Mask                  =0xE0;
const	TUint BBP8_GreenShift           =2;
const	TUint BBP8_RedShift             =5;

//Number of bits per pixel
const TInt	BPP8                    =8;
const TInt	BPP12                   =12;
const TInt	BPP16                   =16;
const TInt	BPP24                   =24;
const TInt	BPP32                   =32;

//16BPP has rgb565 format rrrrrggggggbbbbb.
const TInt 	BPP16RedShift           =13;
const TInt	BPP16GreenShift         =8;
const TInt	BPP16BlueShift          =3;

//12 bpp has rgb 444 format rrrrggggbbbb.
const TInt	BPP12RedShift           =9;
const TInt	BPP12GreenShift         =5;
const TInt	BPP12BlueShift          =2;
	
//24bpp has rgb888 format and 32 bpp has AlRGB 8888 format where Al is alpha.
const TInt	BPP32RedShift           =21;
const TInt	BPP32GreenShift         =13;
const TInt	BPP32BlueShift          =6;
	

void CDrawUtils::ColourFillUtility(TUint8 aColorVal,const TPoint& aPos)
/**
*
*  Utility Function that fills required colour in at the given x,y positions on the screen,
*  in a 16 pixels *	16 pixels lines area 
*
* 		@param 				aColorVal:  The Colour to be filled in .
*							Scenarios:
* 							1) 	4 bits per pixel mode
* 							   	aColorVal contains empty upper
* 							   	4 bits & only contains 4 bits of info, 
* 							2)	8 bits per pixel  mode
* 								all 8 bits in aColorVal are filled and the value,
* 								is taken from the standard  OS palette.
* 							3)	Other modes: all other modes aColorVal
*			 					holds data in rgb 332 format.
*
* 		@param 				aPos  		The position of the pixel 
*
* 
*/
    {
    //get screendriver attributes
    TInt displayMode;
    HAL::Get(HAL::EDisplayMode, displayMode);
    TInt displayMemoryAddress;
    HAL::Get(HAL::EDisplayMemoryAddress, displayMemoryAddress);
    TInt displayOffsetBetweenLines      =displayMode;
    HAL::Get(HAL::EDisplayOffsetBetweenLines, displayOffsetBetweenLines);
    TInt offsetToFirstPixel             =displayMode;
    HAL::Get(HAL::EDisplayOffsetToFirstPixel, offsetToFirstPixel);
    TInt displayBitsPerPixel            =displayMode;
    HAL::Get(HAL::EDisplayBitsPerPixel, displayBitsPerPixel);
	
    TUint       red             =(aColorVal&Red_Mask)>>BBP8_RedShift;	
    TUint       green           =(aColorVal&Green_Mask)>>BBP8_GreenShift;
    TUint       blue            =aColorVal&Blue_Mask;
    TUint8      color8bpp       =aColorVal; 
    TInt        bbp             =displayBitsPerPixel;
  	
    //Shifts to convert RGB332 into a respective formats.
    //12 bpp has rgb format 444, 16 bpp has rgb format 565, 24 bpp has rgb 888 format, 32 bpp has Al rgb 8888 format : Al is alpha 
    TUint color12bpp=(red << BPP12RedShift) | (green << BPP12GreenShift) | (blue << BPP12BlueShift);
    TUint color16bpp=(red << BPP16RedShift) | (green << BPP16GreenShift) | (blue << BPP16BlueShift);
    TUint color32bpp=(red << BPP32RedShift) | (green << BPP32GreenShift) | (blue << BPP32BlueShift);
    
    if( displayBitsPerPixel == BPP12 )
        {
        bbp = BPP16;
        }
    else if ( displayBitsPerPixel == BPP24 )
        {
        bbp = BPP32;
        }
    else
        {
        bbp = displayBitsPerPixel;
        }
		
    TInt baseAddress=displayMemoryAddress + offsetToFirstPixel + aPos.iY * displayOffsetBetweenLines + aPos.iX * bbp / BPP8;
    if ( displayBitsPerPixel==BPP8 )
          {
          TUint8 *pixelPtr=(TUint8*)baseAddress;
          *pixelPtr++=color8bpp;
          *pixelPtr=color8bpp;
          }
    else if ( displayBitsPerPixel == BPP12 )
          {
          TUint16 *pixelPtr=(TUint16*)baseAddress;
          *pixelPtr++=color12bpp;
          *pixelPtr=color12bpp;	
          }
    else if ( displayBitsPerPixel == BPP16 )
          {
          TUint16 *pixelPtr	=(TUint16*)baseAddress;
          *pixelPtr++=color16bpp;
          *pixelPtr=color16bpp;
          }
    else if ( bbp == BPP32 )
          {
          TUint32 *pixelPtr=(TUint32*)baseAddress;
          *pixelPtr++=color32bpp;
          *pixelPtr=color32bpp;
          }
    }
	
		


void  CDrawUtils::DrawSquareUtility(const TPoint& aTopLeft,const TInt aBoxLength,const TInt aBoxWidth,TUint8 aColorVal)
/**
 *
 * Draw a filled in square  according to the length and width supplied
 *
 * @param 		aTopLeft Pixel position of the top left corner of the box	
 *
 * @param 		aBoxLength length in pixels of the box to be drawn.
 *
 * @param 		aBoxWidth width  in pixels of the box to be drawn.
 *
 * @param 		aColorVal colour of the box
 *
 */
 	{
 	TInt 	offSet		=1;	
	TInt 	xOrigin		=0;
	TPoint 	endPos;
	TPoint	currentPos;
	if( aTopLeft.iX > xOrigin )
		{
		endPos.iX	=aTopLeft.iX + aBoxWidth - offSet;
		}
	else//TopLeft x co-ordinate is zero
		{
		endPos.iX	=aTopLeft.iX + aBoxWidth;
		}
		
	endPos.iY	=aTopLeft.iY;
	currentPos	=aTopLeft;
			
	for(TInt i=0; i<=aBoxLength; i++)
 		{
 		DrawLineUtility(currentPos,endPos,aColorVal);
 		currentPos.iY	=currentPos.iY + offSet;
 		endPos.iY		=endPos.iY + offSet;
 		}	
	}
	
	
void  CDrawUtils::DrawLineUtility(const TPoint& aStartPos, const TPoint& aEndPos,const TUint8 aColorVal)
/**
 *
 * Draw a line  with colour specified 
 *
 * @param 		aStartPos  pixel position to  start line.
 * @param 		aEndPos    pixel position to  end line.
 *
 */	
	{	
 	TPoint 		step		=aEndPos-aStartPos;
 	TBool 		isDiagonal	=EFalse;
 	TInt		origin		=0;
 	if	(( step.iX>origin ) && ( step.iY>origin ))
 		{
 		isDiagonal	=ETrue;
 		}		
 	TPoint 		currentPos=aStartPos;	
 	for(TInt i=0; i<=step.iX; i++)
 		{
 		currentPos.iX	=( aStartPos.iX + i );
 		for(TInt j=0; j<=step.iY; j++)
 			{
 			if	( !isDiagonal )
 				{
 				currentPos.iY	=( aStartPos.iY + j );
 				ColourFillUtility(aColorVal, currentPos);
 				}
 			else //line is diagonal
 				{
 				currentPos.iY	=( aStartPos.iY + i );
 				ColourFillUtility( aColorVal,currentPos);	
 				break;
 				}
 			}
 		}
	}
	
void    CDrawUtils::DrawSquareOutLineUtility(const TPoint& aTopLeft,const TInt aBoxHeight,const TInt aBoxWidth,TUint8 aColorVal)
/**
 *
 * Draw an outline of a square i.e a square that has not been filled in with colour
 * Draw a filled in square  according to the length and width supplied
 *
 * @param 		aTopLeft Pixel position of the top left corner of the box	
 *
 * @param 		aBoxLength length in pixels of the box to be drawn.
 *
 * @param 		aBoxWidth width  in pixels of the box to be drawn.
 *
 *
 */		
	{
	TPoint startPos	=aTopLeft;
	TPoint endPos;
	//top horizontal line
	endPos.iX		=aTopLeft.iX + aBoxWidth;
	endPos.iY		=aTopLeft.iY;
	DrawLineUtility(startPos, endPos, aColorVal);

	//vertical left
	endPos.iX		=aTopLeft.iX;
	endPos.iY		=aTopLeft.iY + aBoxHeight;								
	DrawLineUtility(startPos, endPos, aColorVal);

	//vertical right
	startPos.iX 	=aTopLeft.iX + aBoxWidth;
	startPos.iY		=aTopLeft.iY;
	endPos.iX		=aTopLeft.iX + aBoxWidth;
	endPos.iY		=aTopLeft.iY + aBoxHeight;
	DrawLineUtility(startPos, endPos, aColorVal);
	
	//bottom horizontal									
	startPos.iX 	=aTopLeft.iX;
	startPos.iY 	=aTopLeft.iY + aBoxHeight;
	endPos.iX		=aTopLeft.iX + aBoxWidth;
	endPos.iY		=aTopLeft.iY + aBoxHeight;											
	DrawLineUtility(startPos, endPos, aColorVal);		
	}
