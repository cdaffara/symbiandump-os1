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
// mmrcserverphysicaladaptation.h
// 
//

#ifndef MMRCPHYSICALADAPTATION_H_
#define MMRCPHYSICALADAPTATION_H_

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include <a3f/a3f_trace_utils.h>
#include "mmrcclientserver.h"
#include "maudiostreamadaptationobserver.h"

class MLogicalChain;
class MAudioContext;
class CFourCCConvertor;
class CMMRCServerSideAdaptationResponseAO;
class CAudioStreamAdaptation;
class CMMRCServerSession;

//------------------------------------------------------------------------------------
// CMMRCClientSideServerRequestToAdaptationAO
//------------------------------------------------------------------------------------

/**
@internalComponent

This class implements the proxy to the Physical adaptation.
The main purpose of this class is to communicate with the physical resource manager.
*/
NONSHARABLE_CLASS( CMMRCClientSideServerRequestToAdaptationAO ): public CActive, public MAudioStreamAdaptationObserver
	{
public:
	/**
	 * Constructs, and returns a pointer to, a new CMMRCClientSideServerRequestToAdaptationAO object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @return CMMRCClientSideServerRequestToAdaptationAO* A pointer to newly created utlitly object.
	 */
	IMPORT_C static CMMRCClientSideServerRequestToAdaptationAO* NewL( );

	/**
	 * Destructor.
	 */
	~CMMRCClientSideServerRequestToAdaptationAO( );

public:
	void Initialize(CMMRCServerSession* aServerSession,
					CMMRCServerSideAdaptationResponseAO* aCallback,
					RThread* aClientThread,
					const CFourCCConvertor& aFourCCConvertor);

	void Service(TUint aRequestType);

	/**
	 * Callback in case of timeout
	 */
	void ResourceRequestAcknowledgmentTimeout( );

	// from MAudioStreamAdaptationObserver
	void PhysicalAdaptationEvent(TPhysicalEvent aEvent, TInt aError);
	void StateEvent(TInt aReason, TAudioState aNewState);
	void AddProcessingUnitComplete(TUid aType, TInt aError);
	void RemoveProcessingUnitComplete(TUid aType, TInt aError);
	void ProcessingFinished();
	void FlushComplete(TInt aError);

	//From CActive
	virtual void RunL();
	virtual void DoCancel();

private:
	/**
	 * By default Symbian 2nd phase constructor is private.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 */
	CMMRCClientSideServerRequestToAdaptationAO( );

	void TidyUp();

	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL( );

private:
	//owned
	CAudioStreamAdaptation* iStreamAdaptation;

	//not owned
	RThread* iClientThread;
	CMMRCServerSession* iServerSession;
	CMMRCServerSideAdaptationResponseAO* iMMRCServerSideAdaptationResponseAO;

	MLogicalChain* iLogicalChainRequested;
	MLogicalChain* iLogicalChainLastCommited;
	MAudioContext* iAudioContext;
	TInt iRequestsProcessed;
	TUint iRequestType;
	};

//------------------------------------------------------------------------------------
// CMMRCServerSideAdaptationResponseAO
//------------------------------------------------------------------------------------

/**
@internalComponent

*/
NONSHARABLE_CLASS( CMMRCServerSideAdaptationResponseAO ) : public CActive
	{
public:
	~CMMRCServerSideAdaptationResponseAO();
	static CMMRCServerSideAdaptationResponseAO* NewL();
	void Initialize(CMMRCServerSession* aServerSession, RThread* iServerThread );
	void Service(TMMRCAdaptationToServerRequestResults aEvent, TInt aError);
	void StartAdaptationListening();
private:
	CMMRCServerSideAdaptationResponseAO();
	void ConstructL( );
	void RunL( );
	void DoCancel( );

private:
	//not owned
	CMMRCServerSession* iMMRCServerSession;
	RThread* iServerThread;

	TMMRCAdaptationToServerRequestResults iEvent;
	TInt iError;
	};

#endif /*MMRCPHYSICALADAPTATION_H_*/
