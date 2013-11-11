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

#ifndef __CFMSGS_INL__
#define __CFMSGS_INL__

inline TCFCommsMessage::TCFCommsMessage(TCodes aCode)
/**
Constructor for basic comms message type.
@param aCode Code identifying the specific message type.
@see TCodes
@publishedPartner
@released
*/
	{
	Code(aCode);
	}

inline TCFCommsMessageId::TCFCommsMessageId(TCFCommsMessage::TCodes aCode, TId aIdentifier):
		TCFCommsMessage(aCode)
/**
Constructor for comms message type with Id.
@param aCode Code identifying the specific message type.
@param aIdentifier Unique code identifying the specific message instance.
@see TId
@see TCodes
@publishedPartner
@released
*/
	{
	TParams* params = reinterpret_cast<TParams*>(TCFCommsMessage::ParamBlock());
	params->iIdentifier=aIdentifier;
	}

inline TUint TCFCommsMessageId::Identifier() const
/**
@return Identifier from message.
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(TCFCommsMessage::ParamBlock());
	return params->iIdentifier;
	}

inline TCFCommsMessageResult::TCFCommsMessageResult(TCFCommsMessage::TCodes aCode, TId aIdentifier, TInt aReturnCode):
	TCFCommsMessageId(aCode, aIdentifier)
/**
Constructor for comms message type with result
@param aCode Code identifying the specific message type.
@param aIdentifier Unique code identifying the specific message instance.
@param aReturnCode Code detailing whether the operation was a success or had some error. \n
Can be one of the system wide error codes or one specific to the module.
@see TId
@see TCodes
@publishedPartner
@released
*/
	{
	TCFCommsMessageResult::TParams* params = 
		reinterpret_cast<TCFCommsMessageResult::TParams*>(TCFCommsMessageId::ParamBlock());
	params->iReturnCode=aReturnCode;
	}

inline TInt TCFCommsMessageResult::ReturnCode() const
/**
@return The return code contained in the message.
@publishedPartner
@released
*/
	{
	const TCFCommsMessageResult::TParams* params = 
		reinterpret_cast<const TCFCommsMessageResult::TParams*>(ParamBlock());
	return params->iReturnCode;
	}

inline TCFDiscoverMsg::TCFDiscoverMsg(TId aIdentifier, TBool aReset, TUint aSize, TCFSubModuleName* aSubModuleNames):
	TCFCommsMessageId(TCFCommsMessage::ECodeDiscover, aIdentifier)
/**
Constructor for discovery comms message.
@param aIdentifier Unique code identifying the specific message instance.
@param aReset When ETrue requesting the receiver of the message to start \n
with the first submodulename (like restarting an enumeration). \n
If EFalse start from the first unreported submodulename.
@param aSize Number of submodulenames that can be written to the iSubModuleNames array.
@param aSubModuleNames Array to which a list of names can be written by the receiver, has room for iSize elements.
@see TId
@see TCFModuleName
@publishedPartner
@released
*/
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iReset=aReset;
	params->iSize=aSize;
	params->iSubModuleNames=aSubModuleNames;
	}

inline TUint TCFDiscoverMsg::Size() const
/**
@return Size of array provided in message.
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iSize;
	}

inline TCFSubModuleName* TCFDiscoverMsg::SubModuleNames() const
/**
@return Pointer to array provided in message.
@see TCFSubModuleName
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iSubModuleNames;
	}

inline TBool TCFDiscoverMsg::Reset() const
/**
@return Whether the message was a reset (start from first) request or not.
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iReset;
	}

inline TCFDiscoverRespMsg::TCFDiscoverRespMsg(TId aIdentifier, TInt aReturnCode, TBool aMoreToWrite):
	TCFCommsMessageResult(TCFCommsMessage::ECodeDiscoverResp, aIdentifier, aReturnCode)
/**
Constructor for discovery response comms message.
@param aIdentifier Unique code identifying the specific message instance.
@param aReturnCode Code detailing whether the operation was a success or had some error. \n
Can be one of the system wide error codes or one specific to the module.
@param aMoreToWrite ETrue if the module has more submodules to report.
@see TId
@see TCodes
@publishedPartner
@released
*/
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iMoreToWrite = aMoreToWrite;
	}


inline TBool TCFDiscoverRespMsg::MoreToWrite() const
/**
@return Whether there are more sub-module names to be written back as part of the discovery process.
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iMoreToWrite;
	}

inline TCFBindMsg::TCFBindMsg(	TId aIdentifier,
								RCFChannel::TMsgQueues aInputQueues,
								RCFChannel::TMsgQueues aOutputQueues,
								const TCFSubModuleAddress* aSubModule1,
								const TCFSubModuleAddress* aSubModule2,
								TCFBindType aType):
								TCFCommsMessageId(TCFCommsMessage::ECodeBind, aIdentifier)
/**
Constructor for bind comms message type
@param aIdentifier Unique code identifying the specific message instance.
@param aInputQueues Comms Channel on which to listen for messages.
@param aOutputQueues Comms Channel on which to send messages.
@param aSubModule1 Address of the submodule in this module which is to be bound to.
@param aSubModule2 Aaddress of the remote submodule to be bound to.
@param aType TType of binding requested.
@see RCFChannel::TMsgQueues
@see TCFSubModuleAddress
@see TCFBindType
@see TId
@publishedPartner
@released
*/
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iInputQueues=aInputQueues;
	params->iOutputQueues=aOutputQueues;
	params->iSubModule1=aSubModule1;
	params->iSubModule2= aSubModule2;
	params->iType= aType;
	}
		
inline RCFChannel::TMsgQueues TCFBindMsg::InputQueues() const
/**
@return Input queues from message contents.
@see RCFChannel::TMsgQueues
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iInputQueues;
	}

inline RCFChannel::TMsgQueues TCFBindMsg::OutputQueues() const
/**
@return Output queues from message contents.
@see RCFChannel::TMsgQueues
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iOutputQueues;
	}

inline const RCFChannelMsgQueues& TCFBindMsg::ChannelInput() const
/**
@return Input queues from message contents.
@see RCFChannelMsgQueues
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return static_cast<const RCFChannelMsgQueues&>(params->iInputQueues);
	}

inline const RCFChannelMsgQueues& TCFBindMsg::ChannelOutput() const
/**
@return Output queues from message contents.
@see RCFChannelMsgQueues
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return static_cast<const RCFChannelMsgQueues&>(params->iOutputQueues);
	}


inline const TCFSubModuleAddress* TCFBindMsg::SubModule1() const
/**
@return The first sub-module name from message contents.
@see TCFSubModuleAddress
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iSubModule1;
	}

inline const TCFSubModuleAddress* TCFBindMsg::SubModule2() const
/**
@return The second sub-module name from message contents.
@see TCFSubModuleAddress
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iSubModule2;
	}

inline TCFBindType TCFBindMsg::Type() const
/**
@return The bind type from message contents.
@see TCFBindType
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iType;
	}

inline TCFBindCompleteMsg::TCFBindCompleteMsg(TId aIdentifier, TInt aReturnCode):
	TCFCommsMessageResult(TCFCommsMessage::ECodeBindComplete, aIdentifier, aReturnCode)
/**
Constructor for bind complete comms message type.
@param aIdentifier Unique code identifying the specific message instance.
@param aReturnCode Code detailing whether the operation was a success or had some error. \n
Can be one of the system wide error codes or one specific to the module.
@see TId
@see TCodes
@publishedPartner
@released
*/
	{}

inline TCFUnbindMsg::TCFUnbindMsg(TId aIdentifier, const TCFSubModuleAddress* aSubModule1,
								  const TCFSubModuleAddress* aSubModule2, TBool aPeerIsDead):
		TCFCommsMessageId(TCFCommsMessage::ECodeUnbind, aIdentifier)
/**
Constructor for unbind comms message type.
@param aIdentifier Unique code identifying the specific message instance.
@param aSubModule1 Address of submodule within this module to unbind. 
@param aSubModule2 Address of remote sumodule to unbind.
@param aPeerIsDead If ETrue this indicates that the other end of the binding to close \n
is not responsive and the binding should be forced to end immediately. \n
This module should not expect to be able to have any communication with \n
the remote end prior to closing down the channels.
@publishedPartner
@released
*/
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iSubModule1=aSubModule1;
	params->iSubModule2=aSubModule2;
	params->iPeerIsDead=aPeerIsDead;
	}

inline const TCFSubModuleAddress* TCFUnbindMsg::SubModule1() const
/**
@return First sub-module name from message contents.
@see TCFSubModuleAddress
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iSubModule1;
	}

inline const TCFSubModuleAddress* TCFUnbindMsg::SubModule2() const
/**
@return Second sub-module name from message contents.
@see TCFSubModuleAddress
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iSubModule2;
	}

inline TBool TCFUnbindMsg::PeerIsDead() const
/**
@return Whether the peer of the unbinding is still alive.
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iPeerIsDead;
	}

inline TCFUnbindCompleteMsg::TCFUnbindCompleteMsg(TId aIdentifier, TInt aReturnCode):
		TCFCommsMessageResult(TCFCommsMessage::ECodeUnbindComplete, aIdentifier, aReturnCode)
/**
Constructor for unbind complete comms message type.
@param aIdentifier Unique code identifying the specific message instance.
@param aReturnCode Code detailing whether the operation was a success or had some error. \n
Can be one of the system wide error codes or one specific to the module.
@see TId
@publishedPartner
@released
*/
	{}
	
inline TCFShutdownMsg::TCFShutdownMsg(TCFShutdownType aType):TCFCommsMessage(TCFCommsMessage::ECodeShutdown)
/**
Constructor for shutdown comms message type.
@param aType The shutdown behaviour expected.
@see TCFShutdownType
@publishedPartner
@released
*/
	{
	TParams* params = reinterpret_cast<TParams*>(ParamBlock());
	params->iType= aType;
	}

inline TCFShutdownType TCFShutdownMsg::Type() const
/**
Return the bind type from message contents.
@see TCFShutdownType
@publishedPartner
@released
*/
	{
	const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
	return params->iType;
	}

#endif // __CFMSGS_INL__

