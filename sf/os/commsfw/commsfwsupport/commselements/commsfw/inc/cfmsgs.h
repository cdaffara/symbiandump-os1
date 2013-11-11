// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CFMSGS_H__
#define __CFMSGS_H__

#include<commschan.h>
#include<cfshared.h>

namespace CommsFW 
/** Namespace for the Comms Framework.
@publishedPartner 
@released
*/
{

/** The types of binding allowed by the framework. 
@publishedPartner 
@released
*/
enum TCFBindType
    {
    /** The upper half of a hierarchical binding. This is the only
    type of hierarchical binding that is allowed to a CPM marked 
    as server.
    */
    EHierarchicalUpper,
    /** The lower half of a hierarchical binding. It is illegal
    to request a CPM marked as server to be bound as this type.
    */
    EHierarchicalLower,
    /** Unrestricted type of binding. 
    */
	EHorizontal,
    /** Unrestricted type of binding. 
    */
	EOther
    };

/** Message transaction identifier. 
@publishedPartner 
@released
*/
typedef TInt TId; 

class TCFCommsMessage : public TCFMessage
/** Comms messages used by the Comms Framework.
@publishedPartner
@released
*/
	{
public:
	/** Message types defined by the framework. 
	@publishedPartner 
    @released
    */
	typedef enum
		{
		/** Invalid, not a message. 
        */
		ECodeInvalid		=  0,
		/** Bind two sub-modules with a set of channels. 
        */
		ECodeBind			= 10,
		/** Binding completed; response to ECodeBind. 
        */
		ECodeBindComplete	= 11,
		/** Unbind two sub-modules and close the corresponding channels. 
        */
		ECodeUnbind			= 12,
		/** Unbind completed; response to ECodeUnBind. 
        */
		ECodeUnbindComplete	= 13,
		/** Request information such as names of sub-modules. 
        */
		ECodeDiscover		= 14,
		/** Reply to discover request. 
        */
		ECodeDiscoverResp	= 15,
		/** Request for module to shutdown. 
        */
		ECodeShutdown		= 16,
		/** Message from the root server client forwarder to a module
		@see TCFForwardMsg
        */
		ECodeForward		= 17,
		/** Private message flag; CF channel is being used to convey a
		message specific to the binding end-points.
        */
		ECodePrivateFlag	= 128,
		} TCodes;
	inline TCFCommsMessage(TCodes aCode);
	};

class TCFCommsMessageId : public TCFCommsMessage
/** Base class for Comms messages containing an identifier.
@publishedPartner
@released
*/
	{
public:
	typedef struct TParamsTAG 
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** Unique code identifying the specific message instance.
		Often this code is returned in a reply message.
		*/
		TId iIdentifier;
		} TParams;

#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
	enum { KNullMessageId = 0 };
#endif

	inline TCFCommsMessageId(TCFCommsMessage::TCodes aCode, TId aIdentifier);
	inline TUint Identifier() const;
	};

class TCFCommsMessageResult : public TCFCommsMessageId
/** Base class for Comms messages containing a result code and an identifier.
@publishedPartner
@released
*/
	{
public:
	typedef struct TParamsTAG : public TCFCommsMessageId::TParamsTAG 
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** Code detailing whether the operation was a success or had some error.
		Can be one of the system wide error codes or a specific one to the module.
		*/
		TInt iReturnCode;
		} TParams;

	inline TCFCommsMessageResult(TCFCommsMessage::TCodes aCode, TId aIdentifier, TInt aReturnCode);
	inline TInt ReturnCode() const;
	};


class TCFDiscoverMsg : public TCFCommsMessageId
/** Comms Discovery message. Sent to a CPM to discover which
submodules are available for bindings. Can be sent multiple 
times if the CPM has more submodules than can be contained
in one reply.
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeDiscover};

	typedef struct TParamsTAG : public TCFCommsMessageId::TParamsTAG 
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** When ETrue requesting the receiver of the message to start
		with the first submodulename (like restarting an enumeration).
		If EFalse start from the first unreported submodulename.
		*/
		TBool iReset;
		/** Number of submodulenames that can be written to
		the iSubModuleNames array.
		*/
		TUint iSize;
		/** Array to which a list of names can be written, has room
		for iSize elements.
		*/
		TCFSubModuleName* iSubModuleNames;
		} TParams;

	inline TCFDiscoverMsg(TId aIdentifier, TBool aReset, TUint aSize, TCFSubModuleName* aSubModuleNames);
	inline TUint Size() const;
	inline TCFSubModuleName* SubModuleNames() const;
	inline TBool Reset() const;
	};

class TCFDiscoverRespMsg : public TCFCommsMessageResult
/** Comms Discovery Response message. Sent from the CPM to
the Rootserver in response to the TCFDiscoverMsg message.
Contains the completion code for the requested operation.
@see TCFDiscoverMsg
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeDiscoverResp};

	typedef struct TParamsTAG : public TCFCommsMessageResult::TParamsTAG
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** ETrue if the iSubModuleNames array from the TCFDiscoverMsg
		has been filled but there is more submodules to report. Else
		EFalse.
		@see TCFDiscoverMsg
		@see TCFSubModuleName
		*/
		TBool iMoreToWrite;
		} TParams;

	inline TCFDiscoverRespMsg(TId aIdentifier, TInt aReturnCode, TBool aMoreToWrite);
	inline TBool MoreToWrite() const;
	};

class TCFBindMsg : public TCFCommsMessageId
/** Comms Bind message. Sent to the CPM to request it to bind
one of its submodules to another entity in the Comms Process.
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeBind};

	typedef struct TParamsTAG : public TCFCommsMessageId::TParamsTAG
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** Comms Channel on which to listen for messages.
		*/
		RCFChannel::TMsgQueues iInputQueues;
		/** Comms Channel o which to send messages.
		*/
		RCFChannel::TMsgQueues iOutputQueues;
		/** Address of the submodule in this module
		which is to be bound to.
		*/
		const TCFSubModuleAddress* iSubModule1;
		/** The address of the remote submodule to be bound to.
		*/
		const TCFSubModuleAddress* iSubModule2;
		/** Type of binding requested.
		@see TCFBindType
		*/
		TCFBindType iType;
		} TParams;

	inline TCFBindMsg(	TId aIdentifier, 
						RCFChannel::TMsgQueues aInputQueues,
						RCFChannel::TMsgQueues aOutputQueues,
						const TCFSubModuleAddress* aSubModule1,
						const TCFSubModuleAddress* aSubModule2,
						TCFBindType aType);

	inline RCFChannel::TMsgQueues InputQueues() const;
	inline RCFChannel::TMsgQueues OutputQueues() const;
	inline const RCFChannelMsgQueues& ChannelInput() const;
	inline const RCFChannelMsgQueues& ChannelOutput() const;
	inline const TCFSubModuleAddress* SubModule1() const;
	inline const TCFSubModuleAddress* SubModule2() const;
	inline TCFBindType Type() const;
	};

class TCFBindCompleteMsg : public TCFCommsMessageResult
/** Comms Bind Complete message. Sent from the CPM to the Rootserver
in response to a TCFBindMsg message. Contains the completion code
for the requested operation.
@see TCFBindMsg
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeBindComplete};

	inline TCFBindCompleteMsg(TId aIdentifier, TInt aReturnCode);
	};

class TCFUnbindMsg : public TCFCommsMessageId
/** Comms Unbind message. Sent to a CPM to request it to unbind
a binding. Since a submodule can have bindings to several
other submodules it is important to match both ends of the
binding before closing it down.
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeUnbind};

	typedef struct TParamsTAG : public TCFCommsMessageId::TParamsTAG
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** Address of submodule within this module to unbind.
		*/
		const TCFSubModuleAddress* iSubModule1;
		/** Address of remote sumodule to unbind.
		*/
		const TCFSubModuleAddress* iSubModule2;
		/** If ETrue this indicates that the other end of the binding to close
		is not responsive and the binding should be forced to end immediately.
		This module should not expect to be able to have any communication with
        the remote end prior to closing down the channels.
        */
		TBool iPeerIsDead;
		} TParams;

	inline TCFUnbindMsg(TId aIdentifier, 
						const TCFSubModuleAddress* aSubModule1,
						const TCFSubModuleAddress* aSubModule2, 
						TBool aPeerIsDead);

	inline const TCFSubModuleAddress* SubModule1() const;
	inline const TCFSubModuleAddress* SubModule2() const;
	inline TBool PeerIsDead() const;
	};

class TCFUnbindCompleteMsg : public TCFCommsMessageResult
/** Comms Unbind Complete message. Sent from a CPM to the Rootserver
in response to a TCFUnbindMsg message. Contains the completion code
for the requested operation.
@see TCFUnbindMsg
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeUnbindComplete};

	inline TCFUnbindCompleteMsg(TId aIdentifier, TInt aReturnCode);
	};

class TCFShutdownMsg : public TCFCommsMessage
/** Comms Shutdown message. Sent from the Rootserver to the CPM to
indicate to the CPM it should shut down. No response other than the
module thread ending is required.
@publishedPartner
@released
*/
	{
public:

    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeShutdown};

	typedef struct TParamsTAG : public TCFCommsMessageId::TParamsTAG
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** The shutdown behaviour expected.
		@see TCFShutdownType
		*/
		TCFShutdownType iType;
		} TParams;
	
	inline TCFShutdownMsg(TCFShutdownType aType);
	TCFShutdownType Type() const;
	};

#include "cfmsgs.inl" // implementation

} // namespace CommsFW

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cfforwardmsg.h>
#endif


#endif // __CFMSGS_H__

