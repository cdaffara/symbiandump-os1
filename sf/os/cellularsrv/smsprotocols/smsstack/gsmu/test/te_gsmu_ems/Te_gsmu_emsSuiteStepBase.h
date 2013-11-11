/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
*/

#ifndef __TE_GSMU_EMS_STEP_BASE__
#define __TE_GSMU_EMS_STEP_BASE__

#include <test/testexecutestepbase.h>
// Please add your include here if you have
#include <fbs.h>
#include <etelmm.h>
#include <e32math.h>
#include "gsmumsg.h"
#include "gsmubuf.h"
#include "gsmuset.h"
#include "smsuaddr.h"
#include "emsinformationelement.h"
#include "emsformatie.h"
#include "emssoundie.h"
#include "emspictureie.h"
#include "emsanimationie.h"
#include "emsuserpromptie.h"
#include "emsobjectdistributionie.h"

// MACROS taken from old test code
#define KSmsMaxEMSLength 64000
#define KMaxSmsBufferLength 39015

_LIT(KPicsMBM,"\\Ems\\pics.mbm");
_LIT(KCDrive,"C:");
_LIT(KZDrive,"Z:");
_LIT(KAnimsMBM,"\\Ems\\anims.mbm");
_LIT(KPromptMsg, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
_LIT(KLongFormatMsg, "0123456789012345678 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");


_LIT8(KBarbieMelody,"MELODY:*3#g3e3#g3*5#c3*4a2r2#f3#d3#f3b3#g2#f3e3r2#c3#f#c2r2#f3e3#g2\r\n");
_LIT8(KOasisMelody,"MELODY:c2.c3c2c2c2f2f2d2#d2.#d3#d2#d2#d2#g2#g2c2.c3c2c2c2f2f2d2#d2.#d3#d2#d2#d2#g2g2c2.c3c2c2c2f2f\r\n");
_LIT8(KItchyMelody,"BEAT:127\r\nMELODY:*5c3a3*5r2*5c3*5a3*5r2*5c3a3*5c3a3*5c3*5a3*5r2*5r3*5c3*5e3*5r3*5e3*5f3*5g3*5r2*5d3*5c3*5d2*5f3*5#a2*5a2\r\n");
_LIT8(KTooBigMelody,"BEAT:127\r\nMELODY:*5c3a3*5r2*5c3*5a3*5r2*5c3a3*5c3a3*5c3*5a3*5r2*5r3*5c3*5e3*5r3*5e3*5f3*5g3*5r2*5d3*5c3*5d2*5f3*5#a2*5a2*5c3*5e3*5r3*5e3*5f3*5g3*5r2*5d3*5c3*5d2*5f3*5#a2*5a2\r\n");

_LIT(KText, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123");


/**
 *  The reason to have a new step base is that it is very much possible
 *  that the all individual test steps have project related common variables
 *  and members
 *  and this is the place to define these common variable and members.
 */
class CTe_gsmu_emsSuiteStepBase : public CTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;

	// Missing functions and definitions copied from the old test code
	RFs iFs;

	CSmsMessage* CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType= CSmsPDU::ESmsSubmit);
	TInt LoadBitmap(CFbsBitmap& aBitmap,const TDesC& aName,TUint aId);
	CSmsMessage* CreateVariableLengthSmsMessageLC(TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
													  TInt aLength);
	void CompareEmsElementsL(CSmsMessage& aOriginalMessage,CSmsMessage& aOtherMessage);
	TBool CompareEmsElementL(const CEmsInformationElement& aOriginalElement,RPointerArray<const CEmsInformationElement>& aOtherElementsArray);
	HBufC8* CreatePictureBufferL(const CFbsBitmap& aBitmap);
	};

#endif // __TE_GSMU_EMS_STEP_BASE__
