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
// Header for all etel driver strategies classes.
// Each class corresponds to an entry in TEtelInput
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef ETELDRIVERSTRATEGIES_H
#define ETELDRIVERSTRATEGIES_H

#include "meteldriverstrategy.h"


/**
Strategy to prepare etel for subsequent manipulations with pdp contexts.
*/
NONSHARABLE_CLASS(TOpenStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);

	};

/**
Strategy to delete pdp context.
*/
NONSHARABLE_CLASS(TContextDeleteStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);

	};
	
/**
Strategy to set QoS for pdp context.
*/
NONSHARABLE_CLASS(TSetQoSStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};
	
/**
Strategy to set tft for pdp context.
*/
NONSHARABLE_CLASS(TSetTftStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};

/**
Strategy to change tft for pdp context.
*/
NONSHARABLE_CLASS(TChangeTftStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};

/**
Strategy to create primary pdp context.
*/
NONSHARABLE_CLASS(TCreate1ryPdpContextStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};

/**
Strategy to create secondary pdp context.
*/
NONSHARABLE_CLASS(TCreate2ryPdpContextStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};

/**
Strategy to activate pdp context, primary or secondary.
*/
NONSHARABLE_CLASS(TActivatePdpStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};

/**
Strategy to retrieve negotiated qos from TSY for secondary pdp context
*/
NONSHARABLE_CLASS(TGetNegQoSStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};


/**
Strategy to modify an active PDP context, primary or secondary.
*/
NONSHARABLE_CLASS(TModifyActiveStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};
/*
Strategy to create mbms pdp context.
*/
NONSHARABLE_CLASS(TCreateMbmsPdpContextStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};

/*
Strategy to create mbms pdp context.
*/
NONSHARABLE_CLASS(TActivateMbmsPdpStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};
	
NONSHARABLE_CLASS(TMbmsSessionUpdateStrategy) : public MEtelDriverStrategy
	{
public:
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus);
	virtual void NotifyFsm(CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus );
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext);
	};	
	
	
#endif // ETELDRIVERSTRATEGIES_H
