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


#ifndef T_CFBSDRAWDEVICE_DATA_H
#define T_CFBSDRAWDEVICE_DATA_H


//	User Includes
#include "datawrapperbase.h"
#include "displayconstants.h"
//System includes
#include <bitdraw.h>
#include <gdi.h>

//FORWARD DECLATATION
class CParameterAgentPlugin;

/**
 * Test Active Notification class
 *
 */

class CT_CFbsDrawDeviceData : public CDataWrapperBase
	{
public:
	
	~CT_CFbsDrawDeviceData();

	static	CT_CFbsDrawDeviceData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
protected:	
	CT_CFbsDrawDeviceData();
	void ConstructL();
	
	void DestroyData();
	void DoCmdNewScreenDeviceL(const TTEFSectionName& aSection);
	void DoCmdDestructor();
	void DoCmdInitScreen();
	void DoCmdSetDisplayMode();
	void DoCmdSetAutoUpdate(const TTEFSectionName& aSection);
	void DoCmdOrientationsAvailable();
	void DoCmdSetOrientation(const TTEFSectionName& aSection);	
	void DoCmdWriteLineL(const TTEFSectionName& aSection);
	void DoCmdPreWriteLine(const TTEFSectionName& aSection);	
	void DoCmdWriteRgb(const TTEFSectionName& aSection);	
	void DoCmdWriteRgbMulti(const TTEFSectionName& aSection);
	void DoCmdWriteBinaryLineL(const TTEFSectionName& aSection);
	void DoCmdReadLineL(const TTEFSectionName& aSection);	
	void DoCmdWriteRGBAlphaLineL(const TTEFSectionName& aSection);
	void DoCmdShadowArea(const TTEFSectionName& aSection);
	void DoCmdSetShadowMode(const TTEFSectionName& aSection);
	void DoCmdWriteBinaryLineVerticalL(const TTEFSectionName& aSection);
	void DoCmdWriteBinaryL(const TTEFSectionName& aSection);
	void Clear(TRgb aColor);
	TInt LongWidth(TInt aWidth,TDisplayMode aDispMode);	
	void FillBuffer(TUint8* aBuffer,TInt aByteSize,TDisplayMode aDispMode);
	TInt ByteSize();
	void SetShadowAndFadingMode(TInt shadowMode,TInt blackPoint, TInt whitePoint);
	TInt ReadRect(TRect& r, const TTEFSectionName& aSection);
	TBool InitRectangles();
	void ReadRectL(HBufC& string, RArray<TInt>& aResult);
	
private:
	CFbsDrawDevice* iDrawDevice;
	CGraphicsContext::TDrawMode iDMode;
	TDisplayMode iDispMode;
	TInt iOrientation;
	TSize iSize;
	TPostShadowMode iPostBlendShadow;
	RArray<TRect> iTestRect;
	TInt iNumTestRects;
	
	static const TEnumEntryTable iEnumShadowModes[];
	static const TEnumEntryTable iEnumDisplayModes[];
	static const TEnumEntryTable iEnumDrawModes[];
	static const TEnumEntryTable iEnumOrientationModes[];
	};


#endif // T_CFBSDRAWDEVICE_DATA_H
