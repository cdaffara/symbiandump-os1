// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// etel driver factory
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef CETELDRIVERFACTORY_H
#define CETELDRIVERFACTORY_H

#include <e32base.h>
#include <etelmm.h>
#include <etelpckt.h>

#include "PDPFSM.h"
#include "cservicechangenotifier.h"
#include "eteldriverstrategies.h"
#include "eteldrivernmspace.h"

class CContextElemBase;
class CSpudContextElem;
class CMbmsContextElem;
class CEtelDriverContext;

/** factory (and owner) of spud etel driver objects */
class CEtelDriverFactory : public CBase
	{
public:
	static CEtelDriverFactory* NewL (CPdpFsmInterface& aPdpFsmInterface);
	
	virtual ~CEtelDriverFactory();
	
	virtual void InitL();
	virtual void CreatePdpL (TContextId aPdpId,SpudMan::TPdpContextType aContextType);
	virtual void FreePdp(TContextId aPdpId);
	
	virtual void StartPdpNotifications (TContextId aPdpId);
	virtual void CancelPdpNotifications (TContextId aPdpId);
	virtual void CancelAllPdpNotifications();

	/** accessors */
	TBool HasContext(TContextId aId) const;
	CEtelDriverContext& Context(TContextId aId);
	inline TInt ContextCount() const;
	inline RPacketService& PacketService();
	inline RPhone& Phone();
	inline MEtelDriverStrategy& Strategy(EtelDriver::TEtelDriverStrategy aId) const;
	inline CPdpFsmInterface& PdpFsmInterface() const;

		
	/** map etel input to strategy id */
	inline EtelDriver::TEtelDriverStrategy StrategyId(EtelDriver::TEtelInput aInput) const;

private:
	CEtelDriverFactory (CPdpFsmInterface& aPdpFsmInterface);
	void ConstructL();

private:
	// data common among all pdp contexts
	/** reference to clients CPdpFsmInterface */
	CPdpFsmInterface& 	iPdpFsmInterface;
	/** tel server */
	RTelServer			iTelServer;
	/** phone */
	RPhone				iPhone;
	/** packet service */
	RPacketService 		iPacketService;
	/** service change notify */
	CServiceChangeNotifier iServiceChangeNotifier;
	
	/** global array of Etel Driver Stratergies */
	TFixedArray<MEtelDriverStrategy*, EtelDriver::ESentinelStrategy>	iStrategies;
	/** map of input to strategy id */
	// Dev. note: we might have more Strategies then Inputs
	TFixedArray<EtelDriver::TEtelDriverStrategy, EtelDriver::ESentinelInput> 		iInputToStrategyMap;


	/** array of pdp contexts and notifiers
		Dev. note: primary,  secondary and mbms contexts could be in arbitrary slots
	*/
 	TFixedArray<CContextElemBase*, KMaxPdpContexts>         iContexts;
	/** open strategy */	
	TOpenStrategy 					iOpenStrategy;
	/** set QoS strategy */	
	TSetQoSStrategy 				iSetQoSStrategy;
	/** set TFT strategy */
	TSetTftStrategy					iSetTftStrategy;
	/** change TFT strategy */
	TChangeTftStrategy				iChangeTftStrategy;
	/** create primary pdp context strategy */
	TCreate1ryPdpContextStrategy	iCreate1ryPdpContextStrategy;
	/** create secondary pdp context strategy */
	TCreate2ryPdpContextStrategy 	iCreate2ryPdpContextStrategy;
	/** activate pdp context strategy */
	TActivatePdpStrategy			iActivatePdpStrategy;
	/** retrieve negotiated qos strategy */
	TGetNegQoSStrategy				iGetNegQoSStrategy;
	/** modify active pdp context strategy */
	TModifyActiveStrategy			iModifyActivePdpStrategy;
	/** delete pdp context strategy */
	TContextDeleteStrategy			iContextDeleteStrategy;
	/** create mbms pdp context strategy */
	TCreateMbmsPdpContextStrategy	iCreateMbmsPdpContextStrategy;
	/** activate mbms pdp context strategy */
	TActivateMbmsPdpStrategy		iActivateMbmsPdpStrategy;
	/*  mbms session update strategy  */
	TMbmsSessionUpdateStrategy		iMbmsSessionUpdateStragegy;
	};


// inlines
/** returns count of pdp contexts

@return count of pdp contexts
*/
inline TInt CEtelDriverFactory::ContextCount() const
	{ 
	return iContexts.Count(); 
	}
	
/** accessor 

@return reference to etel RPacketService
*/
inline RPacketService& CEtelDriverFactory::PacketService()
	{ 
	return iPacketService; 
	}
	
/** accessor 

@return reference to etel RPhone
*/
inline RPhone& CEtelDriverFactory::Phone()
	{ 
	return iPhone; 
	}
	
/** accessor 

@return reference to etel driver strategy
*/
inline MEtelDriverStrategy& CEtelDriverFactory::Strategy(EtelDriver::TEtelDriverStrategy aId) const
	{ 
	return *iStrategies[aId]; 
	}
	
/** accessor 

@return reference to pdp fsm interface
*/
inline CPdpFsmInterface& CEtelDriverFactory::PdpFsmInterface() const
	{ 
	return iPdpFsmInterface;
	}

/** map etel input to strategy id 

@param aInput - etel input id
@return id of etel driver strategy 
*/
inline EtelDriver::TEtelDriverStrategy CEtelDriverFactory::StrategyId(EtelDriver::TEtelInput aInput) const
	{ 
	return iInputToStrategyMap[aInput]; 
	}

#endif // CETELDRIVERFACTORY_H
