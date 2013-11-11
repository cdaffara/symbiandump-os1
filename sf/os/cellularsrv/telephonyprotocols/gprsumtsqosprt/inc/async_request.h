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
//

#ifndef __ASYNC_REQUEST_H__
#define __ASYNC_REQUEST_H__

#include <networking/umtsnifcontrolif.h>
#include "extension.h"
#include "guqos_log.h"

enum TRequest
	{
	EPendingAny = 0,
	EPendingCreate,
	EPendingDelete,
	EPendingConfig,
	EPendingSetQoS,
	EPendingModifyActive,
	EPendingActivate,
	EPendingPacketFilterAdd,
    EPendingPacketFilterRemove,
    EPendingSblpParameterAdd,       // Request for adding sblp parameter
    EPendingSblpParameterRemove,    // Request for removing sblp parameter
	EPendingTFTRemove,
	EPendingCancel,
	EPendingNone
	};

class CFlowData;
class CPdpContext;
class CNif;

template <class T>
struct SActionStep 
	{
	/**
	* An action of the state machine.
	*
	* Its it up to the action whether parameters are used or not. When the
	* state machine is started, the parameters are both NULL, as there is
	* no preceding event from NIF. Thus, the first action(s) usually ignore
	* the parameters.
	*
	* The aContext parameter is initially NULL and upon each event reply
	* from the NIF, it is filled with the related context. Action can
	* change the current context for the subsequent actions using
	* CRequestBase::SetContext . This change persists for the duration
	* of the current CRequestBase::Run or CRequestBase::Start .
	*
	* @param aContext The context
	* @param aParams  NULL or the parameters of the event from NIF
	*
	* @return
	*	@li EFalse - No action activated, proceed to the next state
	*	@li ETrue - Action activated. Action can be completion and
	*		destruction of the request object. Thus, when action
	*		returns ETrue, the member variables of the current
	*		request MUST NOT BE REFERENCED.
	*/
	TBool (T::*iAction)(CPdpContext* aContext, const TContextParameters* aParams);
	/**
	* Expected result, if action activated.
	* When action activates some NIF Control request, this code specifies the
	* type of the expected result. When the event arrives, this code can be
	* checked in the Run() function.
	*/
	//TRequest iRequest;
	};

// Base class for async requests: separate flow-specific and channel-specific requests?
class CRequestBase : public CBase
	{
protected:
	virtual ~CRequestBase();
public:
	CRequestBase(CNif& aNif);

	virtual void Start();
	void Run(const TRequest aRequest, CPdpContext* aPdpContext, const TContextParameters& aParams);
	virtual TBool Action(CPdpContext* aPdpContext, const TContextParameters* aParams) = 0;

	virtual void Cancel(CFlowData* aFlowData);
	virtual void Cancel(CPdpContext* aContext) = 0;

    TBool IsOk(TInt aResult, CPdpContext *aContext);

    void CompleteAndDestruct(TInt aErrorCode, const TQoSParameters* aParams, const TExtensionData& aExtension=TExtensionData());


	TSglQueLink iLink;
protected:
	virtual void Failure(CPdpContext* aPdpContext, TInt aErrorCode);

	void SetContext(CPdpContext *aContext);
	void SetExpected(const TRequest aRequest);

	TBool ActionRemovePacketFilter(CPdpContext*, const TContextParameters*);

	TBool ActionAddPacketFilter(CPdpContext*, const TContextParameters*);
	TBool DoAddFlowToContext(CPdpContext*, const TContextParameters*);
	TBool ActionCommit(CPdpContext*, const TContextParameters*);
	TBool ActionRequestComplete(CPdpContext*, const TContextParameters*);

	RExtensionData iExtension;		// Extension Data for the completion message
	MQoSNegotiateEvent* iNotify;
	CNif& iNif;

	//
	// The state machine information
	//
	TInt iState;		// Current State number [0...]
	/**
	* Associated flow, or NULL, if none.
	*
	* Some actions may use a flow as an additional input. The validity
	* of this is at responsibility of the request implementation.
	*/
	CFlowData* iFlow;
	/**
	* The context has been modified.
	*
	* iContextModified is TRUE, when the iContext has queued modifications in NIF.
	* This only controls whether ModifyActive can be skipped in ActionCommit.
	*
	* This is currently automaticly set whenever SetExpected is called,
	* and cleared when iContext is changed.
	*/
	TBool iContextModified;
private:
	/**		
	* The expected event completion.
	*
	* This value is defined only when some request has been passed to
	* NIF Control() and then the value is the expected reply event
	* from the NIF.
	*/
	TRequest iExpected;
	/**
	* The current context being acted on (or NULL).
	*
	* This value is defined only when executing the actions (e.g. while
	* executing Run or Start. In other times, it is just left over
	* value that might point to already destroyed context and must
	* never be used.
	*/
	CPdpContext* iContext;
	/**
	* The current context parameters from NIF (or NULL)
	*
	* This value is defined only when executin Run().
	*/
	const TContextParameters* iParams;
#ifdef _LOG
	//
	// For DEBUG / LOGGING purposes only
public:
	const TDesC* iName;	// Name must be set by the constructor of each derived class.
#endif
	};


// Remove unneeded TFT's from the context
class CClose : public CRequestBase
	{
	~CClose();
public:
	static CClose* New(CPdpContext& aContext);

	virtual void Start();
	virtual TBool Action(CPdpContext* aContext, const TContextParameters* aParams)
		{return (this->*iActionList[iState].iAction)(aContext, aParams); }

	virtual void Cancel(CPdpContext* aContext);

protected:
	CClose(CPdpContext& aContext);

private:
	static const SActionStep<CClose> iActionList[];
	CPdpContext& iClosing;
	};
	
// Context deletion requst
class CDeleteRequest : public CRequestBase
	{
public:
	static CDeleteRequest* NewL(CPdpContext* aContext);
	
	virtual void Start();
	// This request has no "action list", it's all done in Start()
	virtual TBool Action(CPdpContext*, const TContextParameters*) {return ETrue; }
	virtual void Cancel(CPdpContext* aContext);
protected:
	CDeleteRequest(CPdpContext* aContext);
private:
	CPdpContext *iDelete;
	};

// Intermediate shared class for requests that negotiate QoS
class CNegotiationBase : public CRequestBase
	{
protected:
	~CNegotiationBase() {}
public:
	void SetParameters(const TQoSParameters& aParams, CExtensionPolicy& aPolicy);
    void SetParametersFlowExtn(CExtensionPolicy &aPolicy);
protected:
 	CNegotiationBase(CNif& aNif);
	void Failure(CPdpContext* aPdpContext, TInt aErrorCode);


	TBool ActionAddSblpParameter(CPdpContext*, const TContextParameters*);
	TBool ActionSetQoS(CPdpContext*, const TContextParameters*);
	TBool ActionNegotiationComplete(CPdpContext *, const TContextParameters*);

    TBool iSblp;
	RFlowExtensionParams  iFlowExtnParams;
	TQoSParameters	iGeneric;
	TQoSRequested	iUmts;
	};


class COpenChannel : public CNegotiationBase
	{
	~COpenChannel();
public:
	static COpenChannel* New(TUint aChannelId, CFlowData &aFlow, MQoSNegotiateEvent* aNotify);


	virtual void Start();
	virtual TBool Action(CPdpContext* aContext, const TContextParameters* aParams)
		{return (this->*iActionList[iState].iAction)(aContext, aParams); }

	virtual void Failure(CPdpContext* aPdpContext, TInt aErrorCode);
	virtual void Cancel(CPdpContext* aContext);

protected:
	COpenChannel(TInt aChannelId, CFlowData& aFlow, MQoSNegotiateEvent* aNotify);

	TBool ActionNewContext(CPdpContext*, const TContextParameters*);
	TBool DoRememberCreatedContext(CPdpContext*, const TContextParameters*);

private:

	static const SActionStep<COpenChannel> iActionList[];

	const TInt iChannelId;
	CPdpContext* iNewContext;
	};


class CNegotiateChannel : public CNegotiationBase
	{
	~CNegotiateChannel();
public:
	static CNegotiateChannel* New(CPdpContext* aContext, MQoSNegotiateEvent* aNotify);

	virtual void Start();
	virtual TBool Action(CPdpContext* aContext, const TContextParameters* aParams)
		{return (this->*iActionList[iState].iAction)(aContext, aParams); }
	virtual void Cancel(CPdpContext* aContext);

protected:
	CNegotiateChannel(CPdpContext* aContext, MQoSNegotiateEvent* aNotify);
	void ConstructL();

private:
	static const SActionStep<CNegotiateChannel> iActionList[];
	CPdpContext* iChannel;
	};

class CJoinRequest : public CRequestBase
	{
	~CJoinRequest();
public:
	static CJoinRequest* New(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify);

	virtual void Start();
	virtual TBool Action(CPdpContext* aContext, const TContextParameters* aParams)
		{return (this->*iActionList[iState].iAction)(aContext, aParams); }

	virtual void Failure(CPdpContext* aPdpContext, TInt aErrorCode);
	virtual void Cancel(CPdpContext* aContext);

protected:
	CJoinRequest(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify);

	TBool DoStartWithTargetContext(CPdpContext *, const TContextParameters*);


private:
	static const SActionStep<CJoinRequest> iActionList[];
	CPdpContext* iChannel;
	};

class CLeaveRequest : public CRequestBase
	{
	~CLeaveRequest();
public:
	static CLeaveRequest* New(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify);

	virtual void Start();
	virtual TBool Action(CPdpContext* aContext, const TContextParameters* aParams)
		{return (this->*iActionList[iState].iAction)(aContext, aParams); }

	virtual void Failure(CPdpContext* aPdpContext, TInt aErrorCode);
	virtual void Cancel(CPdpContext* aContext);

protected:
	CLeaveRequest(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify);
	void ConstructL();

private:
	static const SActionStep<CLeaveRequest> iActionList[];
	CPdpContext* iChannel;
	};

#endif
