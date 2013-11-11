// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the Simulator TSY Data Call functionality.  
// The Call classes process the Call-based requests made by ETel clients 
// and passed down to the TSY by the ETel Server.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimDataCallTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimDataCall.h"
#include "CSimPhone.h"

CSimDataCall* CSimDataCall::NewL(CSimLine* aLine,const TDesC& aName, CSimPhone* aPhone)
/**
 * Standard two phase constructor.
 *
 * @param aLine pointer to the Line object.
 * @param aName name of the call to be constructed
 * @return CSimDataCall  pointer to the data call object created
 * @leave Leaves if no memory or object is not created for any reason
 */
	{
	CSimDataCall* dataCall=new(ELeave) CSimDataCall(aLine,aName,aPhone);
	TCleanupItem newCallDataClose(CloseCall,dataCall);
	CleanupStack::PushL(newCallDataClose);
	dataCall->ConstructL();
	CleanupStack::Pop();
	return dataCall;
	}

CSimDataCall::CSimDataCall(CSimLine* aLine,const TName& aName, CSimPhone* aPhone)
	: CSimCall(aLine,aName,aPhone), iCommPortLoaned(EFalse)
/**
 * Trivial constructor. Calls CSimCall to initialise its members
 */
	{
	iCaps=Caps();
	}

void CSimDataCall::ConstructL()
/**
 * Second phase of 2-Phase Constructor
 * Retrieves all the pausing duration tags from the config file
 *
 * @param aName name of the data call to be constructed
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_1, "Starting to parse Data Call config parameters...");

	iDiallingPause=iLine->CfgFile()->ItemValue(KDiallingPauseDuration,KDefaultDiallingPauseDuration);
	iConnectingPause=iLine->CfgFile()->ItemValue(KConnectingPauseDuration,KDefaultConnectingPauseDuration);
	iDisconnectingPause=iLine->CfgFile()->ItemValue(KDisconnectingPauseDuration,KDefaultDisconnectingPauseDuration);
	iAnswerIncomingPause=iLine->CfgFile()->ItemValue(KAnswerIncomingPauseDuration,KDefaultAnswerIncomingPauseDuration);
	iRemoteHangupPause=iLine->CfgFile()->ItemValue(KRemoteHangupPauseDuration,KDefaultRemoteHangupPauseDuration);

// Read in the CommPort setup

	const CTestConfigItem* item=iLine->CfgFile()->Item(KDataCallCaps);
	if(item)
		{
		TPtrC8 speedCaps, protocolCaps, serviceCaps, qosCaps, codingCaps, asymmetryCaps;
		TPtrC8 rlpVersionCaps, v42bisCaps;
		TBool   hscsdSupport, userInitUpgrade;
		TInt mClass, MaxRxTimeslots, MaxTxTimeslots, totalRxTxTimeslots;
		TUint8 digit = 0;
		
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,speedCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SPEEDCAPS returned %d (element no. %d) from tag %s.",ret,0,KDataCallCaps);
			}
		else
			if(AsciiToNum(speedCaps, digit)==KErrNone)
				iMobileCallCaps.iSpeedCaps = digit;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,protocolCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCAPS returned %d (element no. %d) from tag %s.",ret,1,KDataCallCaps);
			}
		else
			if(AsciiToNum(protocolCaps, digit)==KErrNone)
				iMobileCallCaps.iProtocolCaps = digit;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,serviceCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element SERVICECAPS returned %d (element no. %d) from tag %s.",ret,2,KDataCallCaps);
			}
		else
			if(AsciiToNum(serviceCaps, digit)==KErrNone)
				iMobileCallCaps.iServiceCaps = digit;

		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,qosCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element QOSCAPS returned %d (element no. %d) from tag %s.",ret,3,KDataCallCaps);
			}
		else
			if(AsciiToNum(qosCaps, digit)==KErrNone)
				iMobileCallCaps.iQoSCaps = digit;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,hscsdSupport);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element HSCSDSUPPORT returned %d (element no. %d) from tag %s.",ret,4,KDataCallCaps);
			}
		else
			iMobileCallCaps.iHscsdSupport = hscsdSupport;
	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,mClass);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element MCLASS returned %d (element no. %d) from tag %s.",ret,5,KDataCallCaps);
			}
		else
			iMobileCallCaps.iMClass = mClass;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,MaxRxTimeslots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXRXTIMESLOTS returned %d (element no. %d) from tag %s.",ret,6,KDataCallCaps);
			}
		else
			iMobileCallCaps.iMaxRxTimeSlots = MaxRxTimeslots;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,MaxTxTimeslots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXTXTIMESLOTS returned %d (element no. %d) from tag %s.",ret,7,KDataCallCaps);
			}
		else
			iMobileCallCaps.iMaxTxTimeSlots = MaxTxTimeslots;

		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,totalRxTxTimeslots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element TOTALRXTXTIMESLOTS returned %d (element no. %d) from tag %s.",ret,8,KDataCallCaps);
			}
		else
			iMobileCallCaps.iTotalRxTxTimeSlots = totalRxTxTimeslots;

		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,codingCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element CODINGCAPS returned %d (element no. %d) from tag %s.",ret,9,KDataCallCaps);
			}
		else
			if(AsciiToNum(codingCaps, digit)==KErrNone)
				iMobileCallCaps.iCodingCaps = digit;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,asymmetryCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element ASYMMETRYCAPS returned %d (element no. %d) from tag %s.",ret,10,KDataCallCaps);
			}
		else
			if(AsciiToNum(asymmetryCaps, digit)==KErrNone)
				iMobileCallCaps.iAsymmetryCaps = digit;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,11,userInitUpgrade);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element USERINITUPGRADE returned %d (element no. %d) from tag %s.",ret,11,KDataCallCaps);
			}
		else
			iMobileCallCaps.iUserInitUpgrade = userInitUpgrade;

		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,rlpVersionCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element RLPVERSIONCAPS returned %d (element no. %d) from tag %s.",ret,10,KDataCallCaps);
			}
		else
			if(AsciiToNum(rlpVersionCaps, digit)==KErrNone)
				iMobileCallCaps.iRLPVersionCaps = digit;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,v42bisCaps);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element V42BISCAPS returned %d (element no. %d) from tag %s.",ret,10,KDataCallCaps);
			}
		else
			if(AsciiToNum(v42bisCaps, digit)==KErrNone)
				iMobileCallCaps.iV42bisCaps  = digit;
		}
		else //Default Values for Caps
			{
			iMobileCallCaps.iSpeedCaps=RMobileCall::KCapsSpeedAutobauding;
			iMobileCallCaps.iProtocolCaps=RMobileCall::KCapsProtocolV32;
			iMobileCallCaps.iServiceCaps=(RMobileCall::KCapsDataCircuitAsynchronous | RMobileCall::KCapsPacketAccessSyncUDI);
			iMobileCallCaps.iQoSCaps=RMobileCall::KCapsTransparentPreferred;
			iMobileCallCaps.iHscsdSupport=(TBool)(ETrue);
			iMobileCallCaps.iMClass=1;
			iMobileCallCaps.iMaxRxTimeSlots=5;
			iMobileCallCaps.iMaxTxTimeSlots=5;
			iMobileCallCaps.iTotalRxTxTimeSlots=10;
			iMobileCallCaps.iCodingCaps=(RMobileCall::KCapsAiurCoding48 | RMobileCall::KCapsAiurCoding96);
			iMobileCallCaps.iAsymmetryCaps=(RMobileCall::EAsymmetryUplink);
			iMobileCallCaps.iUserInitUpgrade=(TBool)(ETrue);
			iMobileCallCaps.iRLPVersionCaps = 1;
			iMobileCallCaps.iV42bisCaps = 1;
		}

	TInt count = iLine->CfgFile()->ItemCount(KDataRLPparams);
	TMobileCallRLPItem iMobileCallRLPItem;
	iMobileCallRLPList = new(ELeave) CArrayFixFlat<TMobileCallRLPItem>(5);

	if(count!=0)
		{
		for(TInt i=0;i<count;i++)
			{

			item = iLine->CfgFile()->Item(KDataRLPparams,i);
			if(!item)
				break;

			TInt iRlpVersion,iIWSMax,iIWSMin,iMWSMax,iMWSMin;
			TInt iT1Max,iT1Min,iN2Max,iN2Min,iT4Max,iT4Min;

			TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iRlpVersion);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_16, "WARNING - CONFIGURATION FILE PARSING - Reading element IRLPVERSION returned %d (element no. %d) from tag %s.",ret,0,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iRlpVersion = iRlpVersion;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iIWSMax);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_17, "WARNING - CONFIGURATION FILE PARSING - Reading element IIWSMAX returned %d (element no. %d) from tag %s.",ret,0,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iIWSMax = iIWSMax;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iIWSMin);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_18, "WARNING - CONFIGURATION FILE PARSING - Reading element IIWSMIN returned %d (element no. %d) from tag %s.",ret,1,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iIWSMin = iIWSMin;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,iMWSMax);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_19, "WARNING - CONFIGURATION FILE PARSING - Reading element IMWSMAX returned %d (element no. %d) from tag %s.",ret,2,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iMWSMax = iMWSMax;
			

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,iMWSMin);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_20, "WARNING - CONFIGURATION FILE PARSING - Reading element IMWSMIN returned %d (element no. %d) from tag %s.",ret,3,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iMWSMin = iMWSMin;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,iT1Max);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_21, "WARNING - CONFIGURATION FILE PARSING - Reading element IT1MAX returned %d (element no. %d) from tag %s.",ret,4,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iT1Max = iT1Max;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,iT1Min);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_22, "WARNING - CONFIGURATION FILE PARSING - Reading element IT1MIN returned %d (element no. %d) from tag %s.",ret,5,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iT1Min = iT1Min;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,iN2Max);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_23, "WARNING - CONFIGURATION FILE PARSING - Reading element IN2MAX returned %d (element no. %d) from tag %s.",ret,6,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iN2Max = iN2Max;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,iN2Min);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_24, "WARNING - CONFIGURATION FILE PARSING - Reading element IN2MIN returned %d (element no. %d) from tag %s.",ret,7,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iN2Min = iN2Min;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,iT4Max);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_25, "WARNING - CONFIGURATION FILE PARSING - Reading element IT4MAX returned %d (element no. %d) from tag %s.",ret,8,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iT4Max = iT4Max;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,iT4Min);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_26, "WARNING - CONFIGURATION FILE PARSING - Reading element IT4MIN returned %d (element no. %d) from tag %s.",ret,9,KDataRLPparams);
				}
			else
				iMobileCallRLPItem.iMobileCallRLP.iT4Min = iT4Min;
			
			iMobileCallRLPList->AppendL(iMobileCallRLPItem);		
			}
		}
	else 
		{
		iMobileCallRLPItem.iRlpVersion=1;
		iMobileCallRLPItem.iMobileCallRLP.iIWSMax=10;
		iMobileCallRLPItem.iMobileCallRLP.iIWSMin=5;
		iMobileCallRLPItem.iMobileCallRLP.iMWSMax=8;
		iMobileCallRLPItem.iMobileCallRLP.iMWSMin=4;
		iMobileCallRLPItem.iMobileCallRLP.iT1Max=4;
		iMobileCallRLPItem.iMobileCallRLP.iT1Min=2;
		iMobileCallRLPItem.iMobileCallRLP.iN2Max=12;
		iMobileCallRLPItem.iMobileCallRLP.iN2Min=1;
		iMobileCallRLPItem.iMobileCallRLP.iT4Max=10;
		iMobileCallRLPItem.iMobileCallRLP.iT4Min=2;
		iMobileCallRLPList->AppendL(iMobileCallRLPItem);
		}
	
	item=iLine->CfgFile()->Item(KDynamicHSCSDInfo);
	if(item)
		{		
		TPtrC8 iAiur,iCodings;
		TInt iRxTimeSlots,iTxTimeSlots;
		
		TUint8 digit=0;
	
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iAiur);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_27, "WARNING - CONFIGURATION FILE PARSING - Reading element IAIUR returned %d (element no. %d) from tag %s.",ret,0,KDynamicHSCSDInfo);
			}
		else
			if(AsciiToNum(iAiur, digit)==KErrNone)
				iHscsdInfo.iAiur = (RMobileCall::TMobileCallAiur) digit;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iRxTimeSlots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_28, "WARNING - CONFIGURATION FILE PARSING - Reading element IRXTIMESLOTS returned %d (element no. %d) from tag %s.",ret,1,KDynamicHSCSDInfo);
			}
		else
			iHscsdInfo.iRxTimeSlots = iRxTimeSlots;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,iTxTimeSlots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_29, "WARNING - CONFIGURATION FILE PARSING - Reading element ITXTIMESLOTS returned %d (element no. %d) from tag %s.",ret,2,KDynamicHSCSDInfo);
			}
		else
			iHscsdInfo.iTxTimeSlots = iTxTimeSlots;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,iCodings);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_30, "WARNING - CONFIGURATION FILE PARSING - Reading element ICODINGS returned %d (element no. %d) from tag %s.",ret,3,KDynamicHSCSDInfo);
			}
		else
			if(AsciiToNum(iCodings, digit)==KErrNone)
				iHscsdInfo.iCodings = (RMobileCall::TMobileCallTchCoding) digit;
	}
	else 
		{
		iHscsdInfo.iAiur=RMobileCall::EAiurBps9600;
		iHscsdInfo.iRxTimeSlots=5;
		iHscsdInfo.iTxTimeSlots=10;
		iHscsdInfo.iCodings=RMobileCall::ETchCoding48;
	}

	item=iLine->CfgFile()->Item(KCommSetup);
	if(item)
		{
		TPtrC8 portName,csyName;
		TInt err=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,csyName);	// The 3rd parameter (0) represents the index of the variable on the config file line
		if (err!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_31, "WARNING - CONFIGURATION FILE PARSING - Reading element CSYNAME returned %d (element no. %d) from tag %s.",err,0,KCommSetup);
			iCsyName.Copy(KDefaultCsyName);
			}
		else
			iCsyName.Copy(csyName);

		err=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,portName);		// The 3rd parameter (1) represents the index of the variable on the config file line
		if (err!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_32, "WARNING - CONFIGURATION FILE PARSING - Reading element PORTNAME returned %d (element no. %d) from tag %s.",err,1,KCommSetup);
			iPortName.Copy(KDefaultPortName);
			}
		else
			iPortName.Copy(portName);

		TInt dataRate,handshake;
		err=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,dataRate);		// The 3rd parameter (2) represents the index of the variable on the config file line
		if (err!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_33, "WARNING - CONFIGURATION FILE PARSING - Reading element DATARATE returned %d (element no. %d) from tag %s.",err,2,KCommSetup);
			iConfig.iRate=KDefaultCommPortRate;
			}
		else
			iConfig.iRate=(TBps)dataRate;

		err=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,handshake);		// The 3rd parameter (3) represents the index of the variable on the config file line
		if (err!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_34, "WARNING - CONFIGURATION FILE PARSING - Reading element HANDSHAKE returned %d (element no. %d) from tag %s.",err,3,KCommSetup);
			iConfig.iHandshake=KDefaultHandshake;
			}
		else
			iConfig.iHandshake=(TUint)handshake;
		}
	else
		{
		iCsyName.Copy(KDefaultCsyName);
		iPortName.Copy(KDefaultPortName);
		iConfig.iRate=KDefaultCommPortRate;
		iConfig.iHandshake=KDefaultHandshake;
		}

// Read in the Bearer Service information
	item=iLine->CfgFile()->Item(KBearerService);
	if(item)
		{
		TInt bearerCaps,bearerSpeed;
		TInt err=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,bearerCaps);	// The 3rd parameter (0) represents the index of the variable on the config file line
		if (err!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_35, "WARNING - CONFIGURATION FILE PARSING - Reading element BEARERCAPS returned %d (element no. %d) from tag %s.",err,0,KBearerService);
			iBearerService.iBearerCaps=KDefaultBearerCaps;
			}
		else
			iBearerService.iBearerCaps=bearerCaps;
		
		err=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,bearerSpeed);		// The 3rd parameter (1) represents the index of the variable on the config file line
		if (err!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_36, "WARNING - CONFIGURATION FILE PARSING - Reading element BEARERSPEED returned %d (element no. %d) from tag %s.",err,1,KBearerService);
			iBearerService.iBearerSpeed=KDefaultBearerSpeed;
			}
		else
			iBearerService.iBearerSpeed=(RCall::TCallBearerSpeed)bearerSpeed;
		
		}
	else
		{
		iBearerService.iBearerCaps=KDefaultBearerCaps;
		iBearerService.iBearerSpeed=KDefaultBearerSpeed;
		}

	iTimer=CSimTimer::NewL(iLine->iPhone);
	iNtRas=CSimNtRas::NewL(iLine->iPhone);
	CSimCall::ConstructL();
	
	//If present read in remote party info tag
 	TPtrC8 callingname, remotenumber;
 	TInt delay=0;
 	
 	item=iLine->CfgFile()->Item(KNotifyRemotePartyInfo);
 	
 	TInt ret;
 	
 	if (item)
 		{
 		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,delay);
 		if(ret!=KErrNone)
 			{
 			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_37, "WARNING - CONFIGURATION FILE PARSING - Reading element DELAY returned %d (element no. %d) from tag %s.",ret,0,KNotifyRemotePartyInfo);
 			}
 		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,callingname);
 		if(ret!=KErrNone)
 			{
 			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_38, "WARNING - CONFIGURATION FILE PARSING - Reading element CALLINGNAME returned %d (element no. %d) from tag %s.",ret,1,KNotifyRemotePartyInfo);
 			}
 		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,remotenumber);
 		if(ret!=KErrNone)
 			{
 			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_39, "WARNING - CONFIGURATION FILE PARSING - Reading element REMOTENUMBER returned %d (element no. %d) from tag %s.",ret,2,KNotifyRemotePartyInfo);
 			}
 		
 		iNotifyRemotePartyInfoTimer->iDelay = delay;
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iDirection = RMobileCall::EDirectionUnknown;
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iCallingName.Copy(callingname);
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteNumber.iTelNumber.Copy(remotenumber);	
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;					
 		}
 	else
 		{
 		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
 		}
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CONSTRUCTL_40, "...Finished parsing Data Call config parameters...");
	}

CSimDataCall::~CSimDataCall()
/**
 * Destroy all the objects constructed.
 * CSimTimer and CSimSysAgent objects are destroyed here
 */
	{
	delete iNtRas;
	delete iTimer;
	if(iAnswerIncomingCall.iNotifyPending)
		iLine->ResetAutoAnswerCallObject(this);
	if(iMobileCallRLPList)
		{
		iMobileCallRLPList->Delete(0,iMobileCallRLPList->Count());
		delete iMobileCallRLPList; 
		}
	}

TInt CSimDataCall::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
 * ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
 * for the TSY to process
 * A request handle, request type and request data are passed to the TSY
 *
 * @param aTsyReqHandle
 * @param aIpc IPc number representing the request
 * @param aPackage data for the request
 * @return KErrNone
 */
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_EXTFUNC_1, "CSimDataCall::ExtFunc: IPC Number is %d",aIpc);
	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch (aIpc)
		{
	//
	// No Flow Control NOR Multiple Completion
	//
	case EMobileCallGetMobileCallCaps:
		return GetMobileCallCaps(aTsyReqHandle,aPackage.Des1n());
	
	case EMobileCallGetMobileDataCallCaps:
		return GetMobileDataCallCaps(aTsyReqHandle,aPackage.Des1n());

	case EMobileCallGetMobileDataCallRLPRange:
		return GetMobileDataCallRLPRange(aTsyReqHandle,
				REINTERPRET_CAST(TInt*,dataPtr),
				REINTERPRET_CAST(TDes8*,dataPtr2));

	case EMobileCallSetDynamicHscsdParams:
		return SetDynamicHSCSDParams(aTsyReqHandle,
				REINTERPRET_CAST(RMobileCall::TMobileCallAiur*,dataPtr),
				REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobileCallGetCurrentHscsdInfo:
		return GetCurrentHSCSDInfo(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallGetMobileCallStatus:
		return GetMobileCallStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*,dataPtr));

	case EMobileCallGetMobileCallInfo:
		return GetMobileCallInfo(aTsyReqHandle,aPackage.Des1n());

	//
	// Multiple Completion Services with Immediate Server Repost
	// (Usually Notifications)
	//
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChange(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallNotifyMobileDataCallCapsChange:
		return NotifyMobileDataCapsChange(aTsyReqHandle, aPackage.Des1n());
	
	case EMobileCallNotifyHscsdInfoChange:
		return NotifyHSCSDInfoChange(aTsyReqHandle, aPackage.Des1n());
	
	case EMobileCallNotifyRemotePartyInfoChange:
 		return NotifyRemotePartyInfoChange(aTsyReqHandle, aPackage.Des1n());

	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_EXTFUNC_2, "CSimDataCall::ExtFunc: Unsupported IPC detected - number %d",aIpc);
		break;
		}

	return KErrNotSupported;
	}

TInt CSimDataCall::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc The IPC number of the request that must be cancelled.  Note: this is not the
 *             IPC number of the cancel request itself.
 * @param aTsyReqHandle The TSY Request Handle of the request to be cancelled.
 */
	{
	switch(aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChangeCancel(aTsyReqHandle);

	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChangeCancel(aTsyReqHandle);
	
	case EMobileCallNotifyMobileDataCallCapsChange:
		return NotifyMobileDataCapsChangeCancel(aTsyReqHandle);

	case EMobileCallSetDynamicHscsdParams:
		return SetDynamicHSCSDParamsCancel(aTsyReqHandle);
	
	case EMobileCallGetMobileDataCallCaps:
		return GetMobileDataCallCapsCancel(aTsyReqHandle);

	case EMobileCallGetMobileDataCallRLPRange:
		return GetMobileDataCallRLPRangeCancel(aTsyReqHandle);

	case EMobileCallNotifyHscsdInfoChange:
		return NotifyHSCSDInfoChangeCancel(aTsyReqHandle);
	
	case EMobileCallNotifyRemotePartyInfoChange:
 		return NotifyRemotePartyInfoChangeCancel();

	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_CANCELSERVICE_1, "CSimDataCall::CancelService: No match for IPC, defering to base function");
		break;
		}
	return CCallBase::CancelService(aIpc,aTsyReqHandle);
	}

TInt CSimDataCall::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* /*aTelNumber*/)
/**
 * Process a client's dial request.
 *
 * @param aTsyReqHandle
 * @param aCallParams  the call parameters
 * @param aTelNumber The telephone number to dial
 * @return KErrNone
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_DIAL_1, ">>CSimDataCall::Dial");
	iDialRequestHandle=aTsyReqHandle;
	PopulateCallParams(aCallParams);

	TInt err=ActionEvent(ECallEventDial,KErrNone);
	if(err!=KErrNone)
		ReqCompleted(aTsyReqHandle,err);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_DIAL_2, "<<CSimDataCall::Dial");
	return KErrNone;
	}

TInt CSimDataCall::DialCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a dial Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_DIALCANCEL_1, ">>CSimDataCall::DialCancel");
	switch(iState)
		{
	case RMobileCall::EStatusIdle:
	case RMobileCall::EStatusConnected:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	case RMobileCall::EStatusDialling:
	case RMobileCall::EStatusConnecting:
		iTimer->Cancel();
		
		TInt err;
		err = KErrNone;
		TRAP(err, ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
		if (err != KErrNone)
			{
			ReqCompleted(iDialRequestHandle, err);
			}		
		else
			{
			ReqCompleted(iDialRequestHandle, KErrCancel);
			}
		break;

	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_DIALCANCEL_2, "CSimDataCall::DialCancel: No action taken - state: %d",iState);
		break;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_DIALCANCEL_3, "<<CSimDataCall::DialCancel");
	return KErrNone;
	}

TInt CSimDataCall::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams)
/**
* Register a client's interest in answering the next incoming call.
* First register interest in incoming calls with the line, then, if a call
* is already ringing, start the answer procedure.
*
* @param aTsyReqHandle
* @param aCallParams  the call parameters
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ANSWERINCOMINGCALL_1, ">>CSimDataCall::AnswerIncomingCall");
	TInt err=iLine->SetAutoAnswerCallObject(this);
	if(err!=KErrNone)
		{
		ReqCompleted(aTsyReqHandle,err);
		return KErrNone;
		}

	PopulateCallParams(aCallParams);
	iAnswerIncomingCall.iNotifyPending=ETrue;
	iAnswerIncomingCall.iNotifyHandle=aTsyReqHandle;

	TInt ret=KErrNone;
	if(iState==RMobileCall::EStatusRinging)
		ActionEvent(ECallEventAnswerIncoming,KErrNone);
	else if(iLine->iState==RMobileCall::EStatusRinging)
		{
		TRAP(ret,ret=ChangeStateL(RMobileCall::EStatusRinging,EFalse,EFalse));		
		if(ret == KErrNone)
			{
			ret = ActionEvent(ECallEventAnswerIncoming,KErrNone);	
			}
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ANSWERINCOMINGCALL_2, "<<CSimDataCall::AnswerIncomingCall");
	return ret;
	}

TInt CSimDataCall::AnswerIncomingCallCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a AnswerIncomingCall Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ANSWERINCOMINGCALLCANCEL_1, ">>CSimDataCall::AnswerIncomingCallCancel");
	if(iAnswerIncomingCall.iNotifyPending)
		{
		iAnswerIncomingCall.iNotifyPending=EFalse;
		iLine->ResetAutoAnswerCallObject(this);
		ReqCompleted(iAnswerIncomingCall.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ANSWERINCOMINGCALLCANCEL_2, "<<CSimDataCall::AnswerIncomingCallCancel");
	return KErrNone;
	}

TInt CSimDataCall::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* /*aCallParams*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimDataCall::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimDataCall::HangUp(const TTsyReqHandle aTsyReqHandle)
/**
* Process a client's HangUp request.
*
* @param aTsyReqHandle
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_HANGUP_1, ">>CSimDataCall::HangUp");
	iHangUpRequestHandle=aTsyReqHandle;
	TInt err=ActionEvent(ECallEventHangUp,KErrNone);
	if(err!=KErrNone)
		ReqCompleted(aTsyReqHandle,err);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_HANGUP_2, "<<CSimDataCall::HangUp");
	return KErrNone;
	}

TInt CSimDataCall::HangUpCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a HangUp Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_HANGUPCANCEL_1, ">>CSimDataCall::HangUpCancel");
	switch(iState)
		{
	case RMobileCall::EStatusIdle:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	case RMobileCall::EStatusDisconnecting:
		iTimer->Cancel();
		
		TInt err;
		err = KErrNone;
		TRAP(err, ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
		if (err != KErrNone)
			{
			ReqCompleted(iHangUpRequestHandle, err);
			}
		else
			{	
			ReqCompleted(iHangUpRequestHandle, KErrCancel);
			}
		break;

	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_HANGUPCANCEL_2, "CSimDataCall::HangUpCancel: No action taken - state: %d",iState);
		break;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_HANGUPCANCEL_3, "<<CSimDataCall::HangUpCancel");
	return KErrNone;
	}

TInt CSimDataCall::RelinquishOwnership()
	{
	return KErrNotSupported;
	}

TInt CSimDataCall::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aBearerService)
/**
 * Retrieves the bearer service info.  Bearer service information is only valid
 * when the call is active.
 */
	{
	if(iState!=RMobileCall::EStatusConnected)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}

	*aBearerService=iBearerService;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aCallParams)
/**
 * Retrives the call parameters.  This function must handle both core and Multimode
 * extension versions of the function.  Call parameter information is only valid
 * when the call is active.
 */
	{
	if(iState!=RMobileCall::EStatusConnected)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}

	TPckg<RCall::TCallParams>* callParamPckg=(TPckg<RCall::TCallParams>*)aCallParams;
	RCall::TCallParams& callParam=(*callParamPckg)();

	if(callParam.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1)
		{
// If the passed package is MobileParams, then we can just unpackage and copy.
		RMobileCall::TMobileDataCallParamsV1Pckg* mobileCallParamPckg=(RMobileCall::TMobileDataCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV1& mobileCallParam=(*mobileCallParamPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(mobileCallParam);
		if(err != KErrNone)
			{
			ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

		mobileCallParam=iMobileCallParams;
		}
	
	else if(callParam.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1)
		{
		TPckg<RMobileCall::TMobileHscsdCallParamsV1>* mobileCallParamPckg=(TPckg<RMobileCall::TMobileHscsdCallParamsV1>*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV1& mobileCallParam=(*mobileCallParamPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(mobileCallParam);
		if(err != KErrNone)
			{
			ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

		mobileCallParam=iHscsdSettings;
		}
	else
		{
// If, however, its a core (or an unrecognised extension), we must only copy the
// core parameters.
 		callParam.iSpeakerControl=iMobileCallParams.iSpeakerControl;
		callParam.iSpeakerVolume=iMobileCallParams.iSpeakerVolume;
		callParam.iInterval=iMobileCallParams.iInterval;
		callParam.iWaitForDialTone=iMobileCallParams.iWaitForDialTone;
		}

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::LoanDataPort(const TTsyReqHandle aReqHandle,RCall::TCommPort* aCommPort)
/**
 * Loans the comm port to a client for subsequent streaming of data.
 * For the comm port to be loaned the call must be active and the port unloaned.
 */
	{
	if(iState!=RMobileCall::EStatusConnected)
		{
		ReqCompleted(aReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}

	if(iCommPortLoaned)
		{
		ReqCompleted(aReqHandle,KErrEtelPortAlreadyLoaned);
		return KErrNone;
		}

	aCommPort->iCsy.Copy(iCsyName);
	aCommPort->iPort.Copy(iPortName);
	iCommPortLoaned=ETrue;
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::LoanDataPortCancel(const TTsyReqHandle)
/**
 * Cancels the LoanDataPort request.
 * The LoanDataPort function completes synchronously, so there is nothing to cancel.
 */
	{
	return KErrNone;
	}

TInt CSimDataCall::RecoverDataPort(const TTsyReqHandle aReqHandle)
/**
 * Recovers the comm port.
 * The comm port must be loaned in order to recover it.
 */
	{
	if(!iCommPortLoaned)
		{
		ReqCompleted(aReqHandle,KErrEtelPortNotLoanedToClient);
		return KErrNone;
		}

	iNtRas->Terminate();
	iCommPortLoaned=EFalse;
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::ActionEvent(TCallEvent aEvent,TInt aStatus)
/**
* Entry point when an event has occured that may advance the state machine.
* The aEvent parameter describes the event.
*
* This function contains the main state machine for the data call.  The outer layer
* switches on the event type.  Where appropriate, there are inner layer switches
* or conditional statements to handle the different responses that may be required to
* the same event occurring in different states.
*
* @param aEvent The Call event to handle
* @return value represents the error state caused by the attempted state machine jump.
*/
	{
	TInt ret=KErrNone;
	__ASSERT_ALWAYS(iState!=RMobileCall::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));

	switch(aEvent)
		{
	case ECallEventDial:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_1, ">>CSimDataCall::ActionEvent = [ECallEventDial]");
		if(iState==RMobileCall::EStatusIdle)
			{
			TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusDialling,EFalse,EFalse));
			if(ret==KErrNone)
				iTimer->Start(iDiallingPause,this);
			}
		else
			return KErrEtelCallAlreadyActive;
		break;

	case ECallEventHangUp:
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_2, ">>CSimDataCall::ActionEvent = [ECallEventHangUp]");
		switch(iState)
			{
		case RMobileCall::EStatusConnected:
			iCommPortLoaned=EFalse;
			iNtRas->Terminate();
// Note: No "break;" - fall through to the rest of the discconnecting code...
		case RMobileCall::EStatusDialling:
		case RMobileCall::EStatusRinging:
		case RMobileCall::EStatusAnswering:
		case RMobileCall::EStatusConnecting:
			iTimer->Cancel();
			TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusDisconnecting,EFalse,EFalse));
			if(ret==KErrNone)
				iTimer->Start(iDisconnectingPause,this);
			break;
		default:
			return KErrEtelCallNotActive;
			}
		}
		break;

	case ECallEventIncomingCall:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_3, ">>CSimDataCall::ActionEvent = [ECallEventIncomingCall]");
		if(iState==RMobileCall::EStatusIdle)
			{
			if(iAnswerIncomingCall.iNotifyPending)
				{
				TRAP(ret,	ret=ProcessAnswerIncomingCallL());
				}
			else
				{
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusRinging,EFalse,EFalse));
				if(ret!=KErrNone)
					return ret;
				}
			}
		else
			return KErrEtelCallAlreadyActive;
		break;

	case ECallEventAnswerIncoming:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_4, ">>CSimDataCall::ActionEvent = [ECallEventAnswerIncoming]");
		if(iState==RMobileCall::EStatusRinging)
			{
			TRAP(ret, ret=ProcessAnswerIncomingCallL());
			}
		else
			SimPanic(EIllegalStateInconsistancy);		// The state is checked in AnswerIncomingCall, so there's been an inconsistancy if the state is out of line.
		break;

	case ECallEventRemoteHangup:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_5, ">>CSimDataCall::ActionEvent = [ECallEventRemoteHangup]");
		if(iState==RMobileCall::EStatusConnected)
			{
			TRAP(ret, ret=ProcessRemoteHangupL());
			}
		else
			SimPanic(EIllegalStateInconsistancy);		// The state is checked in RemoteHangup, so there's been an inconsistancy if the state is out of line.
		break;

	case ECallEventTimeOut:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_6, ">>CSimVoiceCall::ActionEvent = [ECallEventTimeOut]");
			switch(iState)
				{
			case RMobileCall::EStatusDialling:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_7, ">>CSimDataCall::State = [EStatusDialling]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnecting,EFalse,EFalse));
				if(ret==KErrNone)
					iTimer->Start(iConnectingPause,this);
				return ret;

			case RMobileCall::EStatusConnecting:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_8, ">>CSimDataCall::State = [EStatusConnecting]");
// If the config file has not spec'ed a CSY, then fail the dial...
				if(iCsyName.Length()==0)
					{
					ReqCompleted(iDialRequestHandle,KErrEtelNoCarrier);
					TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
									
					__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalStateInconsistancy));
					return KErrNone;
					}
				ret=iNtRas->Connect(iCsyName,iPortName,iConfig,this);
				if(ret!=KErrNone)
					ReqCompleted(iDialRequestHandle,ret);
				return ret;

			case RMobileCall::EStatusDisconnecting:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_9, ">>CSimDataCall::State = [EStatusDisconnecting]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
				ReqCompleted(iHangUpRequestHandle,ret);
				return ret;

			case RMobileCall::EStatusAnswering:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_10, ">>CSimDataCall::State = [EStatusAnswering]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnected,EFalse,EFalse));
				ReqCompleted(iAnswerIncomingCall.iNotifyHandle,ret);
				return ret;

			default:
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_ACTIONEVENT_11, ">>CSimDataCall::State = [%d]",iState);
				break;
				}
			}
		break;

	case ECallEventNtRasConnected:
		if(aStatus!=KErrNone)
			{
			ReqCompleted(iDialRequestHandle,aStatus);
			TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
			__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalStateInconsistancy));
			return KErrNone;
			}
		else
			{
			TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnected,EFalse,EFalse));
			ChangeDynamicInfo();
			ReqCompleted(iDialRequestHandle,ret);
			return KErrNone;
			}
	default:
		SimPanic(EIllegalStateInconsistancy);
		break;
		}

	return ret;
	}


void CSimDataCall::TimerCallBack(TInt /*aId*/)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_TIMERCALLBACK_1, ">>CSimDataCall::TimerCallBack");
	TInt err=ActionEvent(ECallEventTimeOut,KErrNone);
	__ASSERT_ALWAYS(err==KErrNone,SimPanic(ETimeOutEventActionFailed));
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_TIMERCALLBACK_2, "<<CSimDataCall::TimerCallBack");
	}

TInt CSimDataCall::ProcessAnswerIncomingCallL()
/**
* Answers an Incoming Call.
* First the call state must be changed to "answering", then the flag indicating
* that an answer incoming call request is no longer pending.  Finally, a new
* call object must be assigned to receive the details of the next incoming call.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_PROCESSANSWERINCOMINGCALLL_1, ">>CSimDataCall::ProcessAnswerIncomingCall");
	TInt ret=ChangeStateL(RMobileCall::EStatusAnswering,EFalse,EFalse);
	if(ret!=KErrNone)
		return ret;
	iTimer->Start(iAnswerIncomingPause,this);
	iAnswerIncomingCall.iNotifyPending=EFalse;
	iLine->ResetAutoAnswerCallObject(this);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_PROCESSANSWERINCOMINGCALLL_2, "<<CSimDataCall::ProcessAnswerIncomingCall");
	return ret;
	}

TInt CSimDataCall::ProcessRemoteHangupL()
/**
* Hangs up a call from the remote end.
* First the call state must be changed to "disconnecting", then the flag indicating
* that a remote hangup request is no longer pending.  Finally, a new
* call object must be assigned to be the next remotely hung up call.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_PROCESSREMOTEHANGUPL_1, ">>CSimDataCall::ProcessRemoteHangupL");
	TInt ret=ChangeStateL(RMobileCall::EStatusDisconnecting,EFalse,EFalse);
	if(ret!=KErrNone)
		return ret;
	iTimer->Start(iRemoteHangupPause,this);
	iLine->ResetRemoteHangupCallObject(this);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATACALL_PROCESSREMOTEHANGUPL_2, "<<CSimDataCall::ProcessRemoteHangupL");
	return ret;
	}


void CSimDataCall::PopulateCallParams(const TDesC8* aCallParams)
/**
 * This function populates the call parameter member variable iMobileCallParams.
 * The function must handle core and multimode extension call parameters.
 * @param aCallParams	A pointer to the call parameter variable.
 */
	{
	TPckg<RCall::TCallParams>* callParamPckg=(TPckg<RCall::TCallParams>*)aCallParams;
	RCall::TCallParams& callParam=(*callParamPckg)();

	if(callParam.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1)
		{
// If the passed package is MobileParams, then we can just unpackage and copy.
		RMobileCall::TMobileDataCallParamsV1Pckg* mobileCallParamPckg=(RMobileCall::TMobileDataCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV1& mobileCallParam=(*mobileCallParamPckg)();

		iHscsdCall=EFalse;
		iMobileCallParams=mobileCallParam;
		}
	else if(callParam.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1)
		{
		TPckg<RMobileCall::TMobileHscsdCallParamsV1>* mobileCallParamPckg=(TPckg<RMobileCall::TMobileHscsdCallParamsV1>*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV1& mobileCallParam=(*mobileCallParamPckg)();

		iHscsdCall=ETrue;
		iHscsdSettings=mobileCallParam;
		}
	else
		{
// If, however, its core (or an unrecognised extension), we must first reset the member
// member variable before copying just the core parameters.
		RMobileCall::TMobileDataCallParamsV1 mobileCallParam;

		iMobileCallParams=mobileCallParam;
		iHscsdCall=EFalse;
 		iMobileCallParams.iSpeakerControl=callParam.iSpeakerControl;
		iMobileCallParams.iSpeakerVolume=callParam.iSpeakerVolume;
		iMobileCallParams.iInterval=callParam.iInterval;
		iMobileCallParams.iWaitForDialTone=callParam.iWaitForDialTone;
		}
	}

void CSimDataCall::NTRasCallBack(TInt aStatus)
/**
 * NTRas callback function.  This function will be called when the NTRAS
 * Server has responded.
 * @param aStatus	Standard error value, indicating the success or failure of the NTRAS
 *					connection.
 */
	{
	TInt err=ActionEvent(ECallEventNtRasConnected,aStatus);
	__ASSERT_ALWAYS(err==KErrNone,SimPanic(EIllegalStateInconsistancy));
	}

TUint CSimDataCall::Caps()
/**
 * Return the current capabilities of this call.
 * @return TUint	Current call capabilities.
 */
	{
	TUint caps=RCall::KCapsData;
	GenerateCoreCallCaps(caps);
	if(iState==RMobileCall::EStatusConnected)
		{
		if(iCommPortLoaned)
			caps|=RCall::KCapsRecoverDataPort;
		else
			caps|=RCall::KCapsLoanDataPort;
		}
	return caps;
	}

TInt CSimDataCall::GetMobileDataCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
/**
* 
* @param aTsyReqHandle specifies the request handle.
* @param aCaps is used to return the retrieved capabilities to the client.
* @return value returns the result of this operation
*/
	{
	TPckg<RMobileCall::TMobileCallDataCapsV1>* mobileCallCapsPckg=(TPckg<RMobileCall::TMobileCallDataCapsV1>*)aCaps;
	RMobileCall::TMobileCallDataCapsV1& mobileCallCaps=(*mobileCallCapsPckg)();	
	
	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(mobileCallCaps);
	if(err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	if(mobileCallCaps.ExtensionId()==KETelExtMultimodeV1)
		{
		mobileCallCaps=iMobileCallCaps;
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNotFound);
		return KErrNotFound;
		}
}

TInt CSimDataCall::GetMobileDataCallCapsCancel(const TTsyReqHandle aTsyReqHandle)
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @return value returns the result of this operation
*/	
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::SetDynamicHSCSDParams(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallAiur* aAiur, TInt* aRxTimeslots)
/**
* 
* @param aTsyReqHandle specifies the request handle.
* @param aAiur is used to dynamically change Aiur for this call
* @param aRxTimeslots is used to dynamically change RxTimeslots for this call
* @return value returns the result of this operation
*/	
	{
	iHscsdInfo.iRxTimeSlots=*aRxTimeslots;
	iHscsdInfo.iAiur=*aAiur;
	iHscsdSettings.iWantedRxTimeSlots=*aRxTimeslots;
	iHscsdSettings.iWantedAiur=*aAiur;

	if(iNotifyInfo.iNotifyPending)
		{
		*(RMobileCall::TMobileCallHscsdInfoV1*)iNotifyInfo.iNotifyData=iHscsdInfo;
		iNotifyInfo.iNotifyPending=EFalse;
		ReqCompleted(iNotifyInfo.iNotifyHandle,KErrNone);
		}
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
}

TInt CSimDataCall::SetDynamicHSCSDParamsCancel(const TTsyReqHandle aTsyReqHandle)
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @return value returns the result of this operation
*/	
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::GetCurrentHSCSDInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
/**
* 
* @param aTsyReqHandle specifies the request handle.
* @param aInfo is used to return the current Hscsd information to the client
* @return value returns the result of this operation
*/		
	{
	TPckg<RMobileCall::TMobileCallHscsdInfoV1>* hscsdInfoPckg=(TPckg<RMobileCall::TMobileCallHscsdInfoV1>*)aInfo;
	RMobileCall::TMobileCallHscsdInfoV1& hscsdInfo=(*hscsdInfoPckg)();	
	
	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(hscsdInfo);
	if(err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	if(hscsdInfo.ExtensionId()==KETelExtMultimodeV1)
		{
		hscsdInfo=iHscsdInfo;
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNotFound);
		return KErrNotFound;
		}
	}

TInt CSimDataCall::NotifyHSCSDInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo)
/**
* 
* @param aTsyReqHandle specifies the request handle.
* @param aInfo is used to return the current Hscsd information when the notification completes
* @return value returns the result of this operation
*/		
	{
	__ASSERT_ALWAYS(!iNotifyInfo.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	iNotifyInfo.iNotifyPending = ETrue;
	iNotifyInfo.iNotifyHandle = aTsyReqHandle;

	TPckg<RMobileCall::TMobileCallHscsdInfoV1>* hscsdInfo = (TPckg<RMobileCall::TMobileCallHscsdInfoV1>*)aHSCSDInfo;
	RMobileCall::TMobileCallHscsdInfoV1& hscsdInfoV1 = (*hscsdInfo)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(hscsdInfoV1);
	if(err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	iNotifyInfo.iNotifyData = &hscsdInfoV1;
	return KErrNone;
	}

TInt CSimDataCall::NotifyHSCSDInfoChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @return value returns the result of this operation
*/	
	{
	if(iNotifyInfo.iNotifyPending)
		{
		iNotifyInfo.iNotifyPending=EFalse;
		ReqCompleted(iNotifyInfo.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimDataCall::NotifyMobileDataCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* /*aCaps*/)
	{
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @return value returns the result of this operation
*/	
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNotSupported;
	}	

TInt CSimDataCall::NotifyMobileDataCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @return value returns the result of this operation
*/	
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDataCall::GetMobileDataCallRLPRange(const TTsyReqHandle aTsyReqHandle, TInt* aRLPVersion, TDes8* aRLPRange)
	{
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @param aRLPVersion specifies the version for RLP range retrieval
* @param aRLPRange is used to return the requested RLP range to the client
* @return value returns the result of this operation
*/
	TInt ret=SearchRetrieveRlp(aRLPVersion,aRLPRange);

	ReqCompleted(aTsyReqHandle,ret);
	return ret;
	}

TInt CSimDataCall::GetMobileDataCallRLPRangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* 
* @param aTsyReqHandle specifies the request handle to be canceled
* @return value returns the result of this operation
*/		
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CSimDataCall::ChangeDynamicInfo() 
/**
*
* Private function to alter the current Hscsd information 
*/		
	{
	if(iHscsdCall)
			{
			// Change to the requested settings
			iHscsdInfo.iAiur=iHscsdSettings.iWantedAiur;
			iHscsdInfo.iRxTimeSlots=iHscsdSettings.iWantedRxTimeSlots;
			iHscsdInfo.iTxTimeSlots=iHscsdSettings.iMaxTimeSlots-iHscsdSettings.iWantedRxTimeSlots;
			iHscsdInfo.iCodings=(RMobileCall::TMobileCallTchCoding) iHscsdSettings.iCodings;
			// Complete a pending notification event for changes in dynamic info of HSCSD
			if(iNotifyInfo.iNotifyPending)
				{
				*(RMobileCall::TMobileCallHscsdInfoV1*)iNotifyInfo.iNotifyData=(RMobileCall::TMobileCallHscsdInfoV1)iHscsdInfo;
				ReqCompleted(iNotifyInfo.iNotifyHandle,KErrNone);
				}
			}
	}

TInt CSimDataCall::SearchRetrieveRlp(TInt* aRLPVersion,TDes8* aRLPRange)
/**
*
* Private function to search the list of available RLP ranges per version
* and return the appropriate range.  
*/
	{
	TInt count = iMobileCallRLPList->Count();
	TMobileCallRLPItem item;

	for(TInt i=0; i<count; i++)
		{
		item=iMobileCallRLPList->At(i);
		if(item.iRlpVersion==*aRLPVersion)
			{
			*(RMobileCall::TMobileDataRLPRangesV1*)aRLPRange=item.iMobileCallRLP;
			return KErrNone;
			}
		}
		return KErrNotFound;
	}

