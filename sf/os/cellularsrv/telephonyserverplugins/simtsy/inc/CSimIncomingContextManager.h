// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CSimIncomingContextManager.cpp
// Implements code to manage incoming PDP contexts.
// 
//

/**
 @file
*/
 
 
#ifndef __CSIMINCOMINGCONTEXTMANAGER_H__
#define __CSIMINCOMINGCONTEXTMANAGER_H__


#include <e32def.h>
#include <e32base.h>

#include "csimtimer.h"
#include "CSimPacketContext.h"
#include "CSimPubSubChange.h"



/**
Drives the incoming PDP contexts.
*/
struct TDelayIncomingContext
{
public:
	TInt	iExtensionId;
		// can be 
		// TPacketDataConfigBase::KConfigGPRS= 0x01  
		// TPacketDataConfigBase::KConfigRel99Rel4= 0x03
		// TPacketDataConfigBase::KConfigRel5= 0x04  
		
	TInt	iIndex;
		// the ExtensionId, Index and index are used to indicate which 
		// PDP context is incoming.
	
	TInt 	iDelay; // After this amount of time an incoming PDP context will be faked.
};

class CSimPhone;

class CTestConfigSection;
class CTestConfigItem;


//Note Initialy only supports GPRS	
NONSHARABLE_CLASS(CSimIncomingContextManager) : CBase
{
	
public :
	CSimIncomingContextManager(CSimPhone* aPhone);
	~CSimIncomingContextManager();

public:
	static CSimIncomingContextManager* NewL(CSimPhone* aPhone, CSimPacketService* aSimPacketService);
	void ConstructL(CSimPacketService* aSimPacketService);
	
	void LoadIncomingContextsL( TPtrC8 aTag );
	void StartFirstIncomingEvent( );
	void NextIncomingEvent( TDes8* aPckg );
		// Call when we want the next timer event, if any to be run.
	void Cancel( );

	void ForcedIncoming(TInt aIndex, TDes8* aPckg);
	TBool IsForcedIncoming(const CSimPubSub::TPubSubProperty aProperty );
		
public:
	TBool IsUseTimerOn() { return iUseTimerOn; }
	void SetUseTimerOn() { iUseTimerOn = ETrue; }
	void SetUseTimerOff() { iUseTimerOn = EFalse; };
		
private:
	static void GetConfigType( const CTestConfigItem& aItem, 
		TInt aIndex, TInt& aType, TInt& aError );
	static void GetContextIndex( const CTestConfigItem& aItem, 
		TInt aIndex, TInt& aContextIndex, TInt& aError );
	static void GetDelay( const CTestConfigItem& aItem, 
		TInt aIndex, TInt& aDelay, TInt& aError );
	
	
	const TContextConfigParam* FindContext( const CArrayFixFlat<TContextConfigParam>* aIncomingContextConfigParams, 
		const TInt aExtensionId, const TInt aIndex );
		
	TBool GetContextInCorrectFormatForEtel(const TContextConfigParam* aContextConfig,
				TDes8* aData);

	const CTestConfigSection* CfgFile();
	
private:
	CSimPacketService* iSimPacketService;
		// Back link to parent class.
		
	CArrayFixFlat<TDelayIncomingContext>* iDelayIncomingContext;
		// Used to drive the timing of the Incoming PDP contexts.
	int iCurrentDelayIndex;
		// Index to the delay/instuction we are currenly executing.


	CSimPhone* iPhone;		//< Pointer to the phone object (The parent of this class)
	CSimTimer* iTimer;		// < Pointer to the owned timer class.

	CSimPubSub::TPubSubProperty iForceIncomingContextChangeProperty;
	CSimPubSub* iForceIncomingContextChange;
		// Used when CSimPubSub::TPubSubProperty(KUidPSSimTsyCategory, KPSSimTsyForceIncomingContextActivation, KPSSimTsyForceIncomingContextActivationType)
		// is changed to force an incoming PDP in C++ code.

	CArrayFixFlat<TContextConfigParam>* iIncomingContextConfigParams;	//< Pointer to a list of valid Context (GPRS/R99/R5) config parameters
		// Note this is a duplicate of the config params in contexts. 
		// It should be noted that potential configs are only avaliable after a PDP has been 
		// created. This will not do as we need it when we get an incoming PDP i.e. 
		// before we even have created a PDP. 
		// Technically there should be only one copy of the TContextConfigParam's and that 
		// should be at this level. But it would take a while to reorganise and test the code.		
		
		// At the moment only GPRS contexts are supported.


private:
	TBool iUseTimerOn;
	
};


#endif	//__CSIMINCOMINGCONTEXT_H__

