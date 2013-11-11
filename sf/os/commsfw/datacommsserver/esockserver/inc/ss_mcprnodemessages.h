// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology 
*/

#ifndef SS_MCPRNODEMESSAGES_H_INCLUDED
#define SS_MCPRNODEMESSAGES_H_INCLUDED

#include <comms-infras/ss_nodemessages.h>
#include <connpref.h>
#include <comms-infras/metadata.h>
#include <comms-infras/ss_nodemessages_selector.h>

namespace ESock
{

//
/** TDeferredSelectionPrefsExt

@internalTechnology
*/
class TDeferredSelectionPrefsExt : public Meta::SMetaData
	{
public:
	enum {EUid = 0x10272C79, ETypeId = 3};

public:
	IMPORT_C explicit TDeferredSelectionPrefsExt(TUid aTierId, const TConnPref& aPrefs);

	EXPORT_DATA_VTABLE_AND_FN

	TConnPref iPrefs;
	TUid iTierId;
	};

//
//TPromptingSelectionPrefsExt - this is only a workaround to handle the prompting
//AccessPoints on the IPProto level during re-selection
class TPromptingSelectionPrefsExt : public Meta::SMetaData
/**
@internalTechnology
@prototype
*/
	{
public:
	enum {EUid = 0x10272C79, ETypeId = 4};

public:
	IMPORT_C explicit TPromptingSelectionPrefsExt(TUid aTierId, const TConnPref& aPrefs);

	EXPORT_DATA_VTABLE_AND_FN

	TConnPref iPrefs;
	TUid iTierId;
	
	// This bool supports detection of whether or not prompting is in progress
	// It replaces the usage of the existence or non-existence of the extension in
	// the container to determine the equivalent meaning. This is required since
	// it is no longer permisable to arbitrarily remove extensions 
	TBool iPromptingInProgress;
	};


/**Safe CF Message carriers (east-west)
Deriving from Messages::TSignatureBase so that we have all of the error handling for free
TSafeMessageCarrierBase assumes the message id, the activity id and the sender from the message it carries.

@internalAll
*/
class TSafeMessageCarrierBase : public Messages::TSignatureBase
	{
protected:
	static const TInt KMaxCarrierSize = 48; //Max size of a TSafeMessageCarrier, less the iMessageBuf size
	static const TInt KMaxCarriedMessageSize = 100; //Min(CommsFW::KMaxInlineMessageSize, Messages::TSignalBase::KMaxInlineMessageSize) - KMaxCarrierSize;

protected: //Not meant to be used directly by design!
	explicit TSafeMessageCarrierBase() {}
    explicit TSafeMessageCarrierBase(const Messages::TSignatureBase& aMessage);

	EXPORT_DATA_VTABLE_AND_FN

protected:
	void DispatchCarriedMessageL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aNodeId);
	IMPORT_C virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

private:
	TBuf8<KMaxCarriedMessageSize> iMessageBuf;
	Messages::TSignatureBase* iMessage; //Local reference only, not stored, not sent, used on the receving end
	};

/**Safe CF Message carriers (east-west)

@internalAll
*/
class TSafeRequestCarrierEast : public TSafeMessageCarrierBase
	{
public:
	DECLARE_MVIP_CTR(TSafeRequestCarrierEast)

protected: //Not meant to be used directly by design!
	explicit TSafeRequestCarrierEast() {} //Not meant to be used directly by design!
    IMPORT_C explicit TSafeRequestCarrierEast(const Messages::TSignatureBase& aMessage); //Not meant to be used directly by design!

	EXPORT_DATA_VTABLE_AND_FN

protected:
	IMPORT_C virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	};

/**Safe CF Message carriers (east-west)

@internalAll
*/
class TSafeResponseCarrierWest : public TSafeMessageCarrierBase
	{
public:
	DECLARE_MVIP_CTR(TSafeResponseCarrierWest)

protected: //Not meant to be used directly by design!
	explicit TSafeResponseCarrierWest() {} //Not meant to be used directly by design!
    IMPORT_C explicit TSafeResponseCarrierWest(const Messages::TSignatureBase& aMessage, const Messages::TRuntimeCtxId& aRecipient); //Not meant to be used directly by design!

	EXPORT_DATA_VTABLE_AND_FN

protected:
	IMPORT_C virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);

private:
	Messages::TNodeId iRecipient;
	};

/**

@internalAll
*/
class TCFSafeMessage
	{
public:
	static void RegisterL();
	static void DeRegister();

	//Safe CF Message carriers (east-west)
	template <class TMESSAGE>
	class TRequestCarrierEast : public TSafeRequestCarrierEast
		{
	public:
	    explicit TRequestCarrierEast(const TMESSAGE& aMessage)
	    :	TSafeRequestCarrierEast(aMessage)
	    	{
	    	__ASSERT_COMPILE(KMaxCarriedMessageSize>=(sizeof(TMESSAGE)+TSignalBase::KMaxUnstoredOverhead)); //External representation should not be greater
	    	}
		};

	//Safe CF Message carriers (east-west)
	template <class TMESSAGE>
	class TResponseCarrierWest : public TSafeResponseCarrierWest
		{
	public:
	    explicit TResponseCarrierWest(const TMESSAGE& aMessage, const Messages::TRuntimeCtxId& aRecipient)
	    :	TSafeResponseCarrierWest(aMessage, aRecipient)
	    	{
	    	__ASSERT_COMPILE(KMaxCarriedMessageSize>=(sizeof(TMESSAGE)+TSignalBase::KMaxUnstoredOverhead)); //External representation should not be greater
	    	}
		};
	};


} //namespace ESock

#endif //SS_MCPRNODEMESSAGES_H_INCLUDED

