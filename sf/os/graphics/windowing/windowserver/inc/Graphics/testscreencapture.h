// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef TESTSCREENCAPTURE_H_
#define TESTSCREENCAPTURE_H_

struct TWsScsComposeScreenCommand
	{
	struct TrivialPoint
		{
		/**
		The x co-ordinate.
		*/
		TInt iX;
		/**
		The y co-ordinate.
		*/
		TInt iY;
		};
	struct TrivialRect
		{
		/**
		The x and y co-ordinates of the top left hand corner of the rectangle.
		*/
		TrivialPoint iTl;
		
		/**
		The x and y co-ordinates of the bottom right hand corner of the rectangle.
		*/
		TrivialPoint iBr;
		};
	inline TWsScsComposeScreenCommand(TUint aCommand, const TInt aHandle);
	inline TWsScsComposeScreenCommand(TUint aCommand, const TRect iAttributes);
	inline void ConvertTrivialRect(TRect& aRect);
	
	TUint iCommand;
	union Parameters
	{
	TInt iBitmapHandle;
	TrivialRect iExtent;
	} iParameter;
	};

/**
 * Window Server client interface for screen capture.
 * 
 * All functions in MTestScreenCapture will automatically flush the client-server
 * session buffer as part of their behavior.
 * 
 * @publishedPartner
 * @prototype
 */

class MTestScreenCapture
{
public:
	enum TWsTestScreenCaptureOpcodes
		{
		EWsScsInvalid = 0x00000000,
		EWsScsScreenCompose = 0x00000001,
		EWsScsTranslateExtent = 0x00000002,
		};
	
	enum	
		{
		KUidTestScreenCaptureIf =  0x10286504,
		ETypeId =	 KUidTestScreenCaptureIf 
		};
public:
	/** Retrieves the composited screen content. 
	    A new composition is initiated and the result is copied in 
	    the bitmap created by user.
	    The only bitmap mode supported is EColor16MU.

	    This function always causes a flush of the window server buffer.
	    
 		@param aBitmap retturns the composited screen content.
 		@param aTarget target rectangle to be filled in
		@return KErrNone if successful
		        KErrArgument if the size does not match the composition area size or
		        the display mode is not supported
	*/
	virtual TInt ComposeScreen(const CFbsBitmap& aBitmap) const = 0;
	/** Maps the source rectangle from application coordinate space to a
		target coordinate space. Since there may be scaling involved, the
		target rectangle may be larger or smaller than the source one, or
		even become empty.

	    This function always causes a flush of the window server buffer.
	    
		@param aSource source rectangle
		@param aTarget target rectangle to be filled in
		@return KErrNone if successful, otherwise one of the system-wide error codes
		*/
	virtual TInt TranslateExtent(const TRect& aSource, TRect& aTarget) const = 0;

	/** Retrieves the size of the composition area in pixels.

	    This function always causes a flush of the window server buffer.

		@param aSize returns the composition area size
		@return KErrNone if successful, otherwise one of the system-wide error codes
		*/
	virtual TInt GetCompositedSize(TSize& aSize) const = 0;
};

inline TWsScsComposeScreenCommand::TWsScsComposeScreenCommand(TUint aCommand, const TInt aHandle): 
	iCommand(aCommand)
	{ iParameter.iBitmapHandle = aHandle; }

inline TWsScsComposeScreenCommand::TWsScsComposeScreenCommand(TUint aCommand, const TRect iAttributes): 
	iCommand(aCommand)
	{ 
	iParameter.iExtent.iTl.iX = iAttributes.iTl.iX; 
	iParameter.iExtent.iTl.iY = iAttributes.iTl.iY; 
	iParameter.iExtent.iBr.iX = iAttributes.iBr.iX; 
	iParameter.iExtent.iBr.iY = iAttributes.iBr.iY; 
	}
inline void TWsScsComposeScreenCommand::ConvertTrivialRect(TRect& aRect)
	{
	aRect.iTl.iX = iParameter.iExtent.iTl.iX;
	aRect.iTl.iY = iParameter.iExtent.iTl.iY;
	aRect.iBr.iX = iParameter.iExtent.iBr.iX;
	aRect.iBr.iY = iParameter.iExtent.iBr.iY;
	}

#endif /*TESTSCREENCAPTURE_H_*/
