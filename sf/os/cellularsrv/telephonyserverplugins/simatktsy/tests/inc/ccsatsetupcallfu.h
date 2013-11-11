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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the SetUpCall
*/

#ifndef CCSATSETUPCALLFU_H
#define CCSATSETUPCALLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSetUpCallFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySetUpCallPCmd0001L();
	void TestNotifySetUpCallPCmd0002L();
	void TestNotifySetUpCallPCmd0004L();

	void TestNotifySetUpCallPCmd0001bL();
	void TestNotifySetUpCallPCmd0001cL();
	void TestNotifySetUpCallPCmd0001dL();
	void TestNotifySetUpCallPCmd0001eL();

private:
	
	/**
	 * data structure 
	 */
	struct TData
		{
		TUint8 iCommandType;
		TInt   iCommandNumber; 
		TUint8 iCommandQualifier;
		TUint8 iSourceDevice;
		TUint8 iDestinationDevice;
		TUint8 iAddress;
		RSat::TSatTelNumber iCallNumber;
		RSat::TCcp iInfoTransferCap;
		RSat::TSubAddress iSubAddress;

		struct TDuration
			{
			TDuration() : iIfUse(EFalse) {}
			TBool iIfUse;
			TUint8 iUnit;
			TUint8 iInterval;
			} iDuration;

		struct TIcon
			{
			TIcon() : iIfUse(EFalse) {}
			TBool iIfUse;
			TUint8 iQualifier;
			TUint8 iIdentifier;
			} iIcon1, iIcon2;
			
		struct TTextAttribute
			{
			enum TColour
				{
				EDarkGreenForeground    = 0x04,
				EDarkGreenBackground    = 0x40,
				EBrightYellowBackground = 0xB0,
				EBrightYellowForeground = 0x0B
				};
			
			enum TColourMode
				{
				EMode1 = EDarkGreenForeground | EBrightYellowBackground,
				EMode2 = EBrightYellowForeground | EDarkGreenBackground
				};
			
			enum TAlignment
				{
				ELeft   = 0x0,
				ECenter = 0x1,
				ERight  = 0x2
				};
			
			enum TFontSize
				{
				ENormal = 0x0,
				ELarge  = 0x4,
				ESmall  = 0x8
				};
			
			enum TStyle
				{
				EBond          = 0x10,
				EItalic        = 0x20,
				EUnderline     = 0x40,
				EStrikethrough = 0x80
				};
			
			TTextAttribute() : iIfUse(EFalse) {}
			TBool iIfUse;
			TUint8 iFormattingPosition;
			TUint8 iFormattingLength;
			TUint8 iFormattingMode;
			TUint8 iColour;
			} iTextAttribute1, iTextAttribute2;
		};
	
	/**
	 * prepare tlv data
	 * @aTlvData - return tlv data
	 * @aData - data structure
	 * @aAlphaIdentifier - alpha identifier
	 * @aSecondAlphaIdentifier - second alpha identifier
	 */
	void PrepareTlvData( TTlv& aTlvData,
			             const TData& aData,
			             const TDesC8& aAlphaIdentifier,
			             const TDesC8& aSecondAlphaIdentifier);
	
	/**
	 * call Set Up Call proactive command
	 * @aSetupCallData - return data
	 * @aData - data structure
	 * @aAlphaIdentifier - alpha identifier
	 * @aResultCode - possible error code
	 * @aSecondAlphaIdentifier - second alpha identifier
	 */
	void ProactiveCommandCallL( RSat::TSetUpCallV6& aSetupCallData,
			                    const TData& aData,
			                    const TDesC8& aAlphaIdentifier,
			                    TInt aResultCode = KErrNone,
			                    const TDesC8& aSecondAlphaIdentifier = KNullDesC8 );
	
	/**
	 * prepare expect data
	 * @aCommandType - type of command
	 */
	void PrepareExpectDataL( TUint8 aCommandType );

	/**
	 * prepare complete data
	 * @aData - data structure
	 * @aAlphaIdentifier - alpha identifier
	 * @aResultCode - possible error code
	 * @aSecondAlphaIdentifier - second alpha identifier
	 */
	void PrepareCompleteDataL( const TData& aData,
			                   const TDesC8& aAlphaIdentifier, 
			                   TInt aResultCode = KErrNone,
			                   const TDesC8& aSecondAlphaIdentifier = KNullDesC8);

	/**
	 * Variables of aSetupCallData class are compared with aCommandQualifier, aAlphaId,
	 * aTypeOfNumber and aNumberPlan
	 * @aSetupCallData - data
	 * @aCommandQualifier - command qualifier
	 * @aAlphaId - alpha identifier
	 * @aNumberPlan - numbering plan
	 */
	void DataCompareL(const RSat::TSetUpCallV6& aSetupCallData,
			                 const TUint8 aCommandQualifier,
			                 const TDesC& aAlphaId,
			                 const RSat::TTypeOfNumber aTypeOfNumber,
			                 const RSat::TNumberingPlan aNumberPlan);

	/**
	 * auxiliary function for testing different text attribute in set up call
	 * @aMode - formatting mode of text attribute
	 * @aColour1 - colour1 of text attribute
	 * @aColour2 - colour2 of text attribute
	 */
	void TestTextAttributeL( const TUint8 aMode, 
			                 const TUint8 aColour1 = CCSatSetUpCallFU::TData::TTextAttribute::EMode1, 
			                 const TUint8 aColour2 = CCSatSetUpCallFU::TData::TTextAttribute::EMode1 );

	}; // class CCSatSetUpCallFU

#endif // CCSATSETUPCALLFU_H

