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
 @publishedPartner
 @released since tb9.2
*/

#ifndef SYMBIAN_NODEMESSAGES_LEGACY_H
#define SYMBIAN_NODEMESSAGES_LEGACY_H


#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_dispatchers.h>
#include <elements/responsemsg.h>
#include <es_enum.h>

#include <comms-infras/api_ext_list.h>
#include <comms-infras/ss_platsec_apiext.h>

namespace ESock
{

/**
 *
 *  Legacy RMessage2 handling framework classes
 *
 */	
class TCFSigLegacyRMessage2Ext : public Messages::TSignatureBase
	{
public:
	enum
		{
		EUid = 0x1028712F,
		ETypeId = 0
		};
	
	TCFSigLegacyRMessage2Ext();
	TCFSigLegacyRMessage2Ext(const Messages::TNodeSignal::TMessageId& aMessageId, const Den::RSafeMessage& aMessage);

	~TCFSigLegacyRMessage2Ext() {}

	/**
	@return whether the request can be processed under the current context
    */
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& /*aContext*/) { return EFalse; };

    /**
    Forward request to the another node.
    */
	virtual void ForwardL(MeshMachine::TNodeContextBase& /*aContext*/) {};

    /**
	Process the request. What this does is implementation specific.
    */
	virtual void ProcessL(MeshMachine::TNodeContextBase& /*aContext*/) {};

    /**
    Cancel the request
    */
	virtual void Cancel(MeshMachine::TNodeContextBase& /*aContext*/) {};

    /**
	Convert an Error into an error which matches what the client would historically have
	expected.

	For example, a Legacy call may fail in the stack with KErrNotFound if the interface isn't found.
	The client may expect KErrNotReady in this scenario though, so this method is to do that conversion.

	@param aError The error to check
	@return The error which should be returned to the client
    */
	virtual TInt CheckError(MeshMachine::TNodeContextBase& /*aContext*/, TInt aError) { return aError; };	

	
protected:
	/**
	   Utility function for checking for an interface
	*/
	TBool HasInterface(MeshMachine::TNodeContextBase& /*aContext*/, TInt aInterface);
	
	TAny* FetchInterfaceL(MeshMachine::AMMNodeBase& aNode, TInt aInterface);
	TAny* FetchInterfaceL(Messages::ANode& aNode, TInt aInterface);
	void ForwardRequestL(MeshMachine::TNodeContextBase& aContext);
	
public:
	/**
	   Read only version of RSafeMessage to stop users from completing
	   when they shouldn't. The RMessage2 should only ever be completed at the
	   CConnection-level. If you hit these ASSERTs then your code is broken/dangerous
	   since it is vulnerable to double completes. */
	class RReadOnlyRMessage : public Den::RSafeMessage
		{
	private:
		void Complete(TInt /*aReason*/) const { ASSERT(0); }
		void PanicClient(const TDesC& /*aCategory*/, TInt /*aReason*/) const { ASSERT(0); }
		};

	RReadOnlyRMessage iMessage;

    EXPORT_DATA_VTABLE_AND_FN
	};

class TCFSigLegacyRMessage2PlatsecExt : public TCFSigLegacyRMessage2Ext, protected MPlatsecApiExt
	{
public:
	inline TCFSigLegacyRMessage2PlatsecExt() {};
	TCFSigLegacyRMessage2PlatsecExt(const Messages::TNodeSignal::TMessageId& aMessageId, const Den::RSafeMessage& aMessage)
		:	TCFSigLegacyRMessage2Ext(aMessageId, aMessage)
        {
        }

protected:
    // From MPlatsecExtensionApi
    IMPORT_C virtual TInt SecureId(TSecureId& aResult) const;
    IMPORT_C virtual TInt VendorId(TVendorId& aResult) const;
    IMPORT_C virtual TBool HasCapability(const TCapability aCapability) const;
    IMPORT_C virtual TInt CheckPolicy(const TSecurityPolicy& aPolicy) const;
    };

// Response structure for a RMessage2
struct TLegacyRMessage2Response
	{
	enum TResponseType { ENormal, EPanic };
	explicit TLegacyRMessage2Response() : iType(ENormal), iCode(KErrNone) {}
	explicit TLegacyRMessage2Response(TResponseType aType, TInt aCode) : iType(aType), iCode(aCode), iCatagory() {}
	explicit TLegacyRMessage2Response(TResponseType aType, TInt aCode, const TDesC& aCatagory) : iType(aType), iCode(aCode), iCatagory()
		{ iCatagory.Copy(aCatagory); }
	TResponseType iType;
	TInt iCode;
	TBuf<20> iCatagory;
	};

DECLARE_MESSAGE_SIG_1(SigLegacyRMessage2Response, TLegacyRMessage2Response, Response)

/**
   Message interface for Legacy RMessage2 handling
*/
class TCFLegacyMessage
	{
public:

  	enum
 	{
	ECFLegacyRMessage2Processed     = 1,
	ECFMCprGetConnectionSetting     = 101,
	};
	
public:
	static void RegisterL();
	static void DeRegister();


    enum { ERealmId = 0x1028712F }; //UID allocated on 21/11/2007

	/**
	   Legacy RMessage2 has been completed or paniced. 
	*/
	typedef TMessageSigLegacyRMessage2Response<ECFLegacyRMessage2Processed, TCFLegacyMessage::ERealmId> TLegacyRMessage2Processed;
	};


/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements MLinkCprDataAccessExtensionApi.
@internalComponent
*/
class TMCprGetConnectionSetting : public TCFSigLegacyRMessage2PlatsecExt,
								  public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessage::ECFMCprGetConnectionSetting, TCFLegacyMessage::ERealmId>
    {
public:
    enum TConnectionSettingType
        {
        EIntSetting,
        EBoolSetting,
        EDes8Setting,
        EDes16Setting,
        ELongDesSetting
        };
	
public:
    inline TMCprGetConnectionSetting() {};

	TMCprGetConnectionSetting(TConnectionSettingType aSettingType, const Den::RSafeMessage& aMessage)
	    : TCFSigLegacyRMessage2PlatsecExt(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage), iSettingType(aSettingType)
		{
		}

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

public:
    DECLARE_MVIP_CTR(TMCprGetConnectionSetting)
	EXPORT_DATA_VTABLE_AND_FN				   
    TConnectionSettingType iSettingType;
	};


}
#endif // SYMBIAN_NODEMESSAGES_LEGACY_H

