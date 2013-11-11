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
//

#include <bluetooth/logger.h>

#include "l2capSigPacketConfigure.h"

#include "l2capSignalHandler.h"
#include "L2CapChannelConfig.h"

#include "l2util.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//					CONFIGURE REQUEST COMMAND 


//Pass in a TL2ConfigOptions packets.  Any options set to valid will be negotiated with there set values
HConfigureRequest* HConfigureRequest::New(TL2CAPPort aRemotePort, CL2CapChannelConfig& aConfig,
		  	               				  TUint8 aRTXTimerDuration,
	                                      TUint16 aERTXTimerDuration)

	{
	LOG_STATIC_FUNC
	HConfigureRequest* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KConfigRequestEmptyLength));
	if(rerr == KErrNone)
		{
		cmd = new HConfigureRequest(buf, aRTXTimerDuration, aERTXTimerDuration);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EConfigureRequest);
			cmd->SetID(KUninitialisedID);
						
			cmd->SetDestinationCID(aRemotePort);
			cmd->SetFlags(KNoConfigurationFlags);

			// Add the options
			rerr = cmd->AddOptionsToCommand(aConfig);
			if(rerr != KErrNone)
				{
				delete cmd;
				cmd = NULL;
				}
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}
	
/**
Verifies that the buffer contains a structurally correct command. 
This does not ensure that the content of the command is semantically valid.

@param aCommmand A new HL2CapCommand if the buffer contains a structurally valid command.
@param aBuffer The buffer containing the command
@return KErrNone if the command if created.
		KErrNoMemory if the command structure is valid but cannot be created.
		KErrCorrupt if the command structure is invalid.
*/
TInt HConfigureRequest::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	TInt length = aBuffer.Length();
	if(length < KConfigRequestEmptyLength)
		{
		// Dodge length!
		return KErrCorrupt;
		}
		
	if(TL2CapConfigParamOptions::VerifyOptionsStructure(KOptionsOffset, aBuffer))
		{
		// Top!  The structure's fine, go ahead and create the command.
		aCommand = new HConfigureRequest(aBuffer);
		if(aCommand)
			{
			return KErrNone;
			}
		else
			{
			return KErrNoMemory;
			}
		}
	else
		{
		// We'll not have any of this nonsense!
		return KErrCorrupt;
		}	
	}
	
HConfigureRequest::HConfigureRequest(RMBufChain& aCommand,
	  	               				 TUint8 aRTXTimerDuration,
	                                 TUint16 aERTXTimerDuration)
 : HL2CapCommand(aCommand, aRTXTimerDuration, aERTXTimerDuration)
	{
	LOG_FUNC
	}

TBool HConfigureRequest::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleConfigureRequest(this))
		{
		// The command has been handled.  Delete it.
		delete this;
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

TInt HConfigureRequest::ParseOptions(TL2CapConfigParamOptions& aConfigOptions, RMBufChain& aUnknownOptions) const
	{
	LOG_FUNC
	return aConfigOptions.ParseOptions(KOptionsOffset, iCommand, aUnknownOptions);
	}

TInt HConfigureRequest::AddOptionsToCommand(CL2CapChannelConfig& aConfig)
	{
	LOG_FUNC
	TInt rerr = TL2CapConfigParamOptions::AddConfigRequestOptions(aConfig, KOptionsOffset, iCommand);
	if(rerr == KErrNone)
		{
		WriteDataLength();
		}
	return rerr;
	}


//					CONFIGURE RESPONSE COMMAND 
//Constructs a command response ready for transmission
HConfigureResponse* HConfigureResponse::New(TUint8 aId, TL2CAPPort aRemotePort, TConfigFlags aFlags, TConfigResponseResult aResult) 
	{
	LOG_STATIC_FUNC
	HConfigureResponse* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KConfigResponseEmptyLength));
	if(rerr == KErrNone)
		{
		cmd = new HConfigureResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EConfigureResponse);
			cmd->SetID(aId);
						
			cmd->SetSourceCID(aRemotePort);
			cmd->SetFlags(aFlags);
			cmd->SetResults(aResult);
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}

/**
Verifies that the buffer contains a structurally correct command. 
This does not ensure that the content of the command is semantically valid.

@param aCommmand A new HL2CapCommand if the buffer contains a structurally valid command.
@param aBuffer The buffer containing the command
@return KErrNone if the command if created.
		KErrNoMemory if the command structure is valid but cannot be created.
		KErrCorrupt if the command structure is invalid.
*/
TInt HConfigureResponse::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC	
	if(aBuffer.Length() < KConfigResponseEmptyLength)
		{
		// Dodge length!
		return KErrCorrupt;
		}
		
	if(TL2CapConfigParamOptions::VerifyOptionsStructure(KOptionsOffset, aBuffer))
		{
		// Top!  The structure's fine, go ahead and create the command.
		aCommand = new HConfigureResponse(aBuffer);
		if(aCommand)
			{
			return KErrNone;
			}
		else
			{
			return KErrNoMemory;
			}
		}
	else
		{
		// We'll not have any of this nonsense!
		return KErrCorrupt;
		}	
	}

HConfigureResponse::HConfigureResponse(RMBufChain& aCommand) 
 : HL2CapCommand(aCommand)
	{
	LOG_FUNC
	}

TBool HConfigureResponse::ProcessCommand(CL2CapSignalHandler& aSignalHandler)	
	{
	LOG_FUNC
	if(aSignalHandler.HandleConfigureResponse(this))
		{
		// The command has been handled.  Delete it.
		delete this;
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
//Put this here to allow base function to be protected, ie not all commands expose functionality
TInt HConfigureResponse::ParseOptions(TL2CapConfigParamOptions& aConfigOptions, RMBufChain& aUnknownOptions) const
	{
	LOG_FUNC
	return aConfigOptions.ParseOptions(KOptionsOffset, iCommand, aUnknownOptions);
	}
																									
TInt HConfigureResponse::AddOptionsToCommand(CL2CapChannelConfig& aConfig, TConfigResponseResult aResult)
	{
	LOG_FUNC
	TInt rerr = TL2CapConfigParamOptions::AddConfigResponseOptions(aConfig, KOptionsOffset, iCommand, aResult);
	if(rerr == KErrNone)
		{
		WriteDataLength();
		}
	return rerr;
	}

void HConfigureResponse::AddUnknownOptionsToCommand(RMBufChain& aUnknownOptions)
	{
	LOG_FUNC
	iCommand.Append(aUnknownOptions);
	WriteDataLength();	
	}


TL2CapConfigParamOptions::TL2CapConfigParamOptions()
 :	iMtuSet(EFalse),
	iMtu(ML2CapConfigurationOption::ESpecDefaultValue),
	iFlushTimeoutSet(EFalse),
	iFlushTimeout(ML2CapConfigurationOption::ESpecDefaultValue),
	iFecSet(EFalse),
	iQosSet(EFalse),
	iQos(ML2CapConfigurationOption::ESpecDefaultValue)
	{
	LOG_FUNC
	}

TBool TL2CapConfigParamOptions::operator==(const TL2CapConfigParamOptions& aThat)
	{
	LOG_FUNC
	// The exactly same options must be included in both sets...
	if (iMtuSet != aThat.iMtuSet ||
		iFlushTimeoutSet != aThat.iFlushTimeoutSet ||
		iFecSet != aThat.iFecSet ||
		iQosSet != aThat.iQosSet)
		{
		return EFalse;
		}
	else
		{
		// ... and the included option's values must be the same.
		return (!iMtuSet || iMtu == aThat.iMtu) &&
			   (!iFlushTimeoutSet || iFlushTimeout == aThat.iFlushTimeout) &&
			   (!iFecSet || iFec == aThat.iFec) &&
			   (!iQosSet || iQos == aThat.iQos);
		}
	}

TInt TL2CapConfigParamOptions::ParseOptions(TUint8 aOptionOffset, const RMBufChain& aCommand, RMBufChain& aUnknownOptions)
	{
	LOG_FUNC
	TUint8 optionDataLen = 0;
	TInt startOfCurrentOption = aOptionOffset;
	TInt commandLength = aCommand.Length();
	TInt rerr = KErrNone;

	// Parse the command options.  Ensure that there is at least an option header
	// left to read.
	while(startOfCurrentOption + KOptionHeaderLength <= commandLength && 
	      (rerr == KErrNone || rerr == KErrConfigUnknownOptions))
		{
		TBuf8<KOptionHeaderLength> headerBuf(KOptionHeaderLength);
		aCommand.CopyOut(headerBuf, startOfCurrentOption);

		TUint8 optionType = headerBuf[KTypeOffset];
		optionDataLen = headerBuf[KLengthOffset];

		// Switch on the option type. Option type could have hint bit set, remove it
		switch(optionType & ~KOptionTypeIsHintMask)
			{
			case EConfigOptionTypeMTU:
				{
				TBuf8<KMTUOptionDataLen> mtuBuf(KMTUOptionDataLen);
				aCommand.CopyOut(mtuBuf, startOfCurrentOption + KDataOffset);

				TMTUOption mtu(LittleEndian::Get16(mtuBuf.Ptr()));
				SetMtu(mtu);
				}
				break;

			case EConfigOptionTypeFlushTimeoutDuration:
				{
				TBuf8<KFlushOptionDataLen> flushTimeoutBuf(KFlushOptionDataLen);
				aCommand.CopyOut(flushTimeoutBuf, startOfCurrentOption + KDataOffset);

				TFlushTimeoutDurationOption flushTimeoutDuration(LittleEndian::Get16(flushTimeoutBuf.Ptr()));
				SetFlushTimeout(flushTimeoutDuration);
				}
				break;
				
			case EConfigOptionTypeQOS:
				{
				TBuf8<KQOSOptionDataLen> qosBuf(KQOSOptionDataLen);
				aCommand.CopyOut(qosBuf, startOfCurrentOption + KDataOffset);
				
				TQualityOfServiceOption qos(static_cast<TQOSServiceType>(qosBuf[KQOSServiceTypeOffset]),
				                            LittleEndian::Get32(qosBuf.Ptr()+KTokenRateOffset),
											LittleEndian::Get32(qosBuf.Ptr()+KTokenBucketSizeOffset),
											LittleEndian::Get32(qosBuf.Ptr()+KPeakBandwidthOffset),
											LittleEndian::Get32(qosBuf.Ptr()+KLatencyOffset),
											LittleEndian::Get32(qosBuf.Ptr()+KDelayVariationOffset));
				SetQos(qos);
				}
				break;
			
			case EConfigOptionTypeRTxAndFEC:
				{
				TBuf8<KFECOptionDataLen> fecBuf(KFECOptionDataLen);
				aCommand.CopyOut(fecBuf, startOfCurrentOption + KDataOffset);
			
				TRetransmissionAndFlowControlOption fec(static_cast<TL2CapChannelMode>(fecBuf[KLinkModeOffset]),
														fecBuf[KTxWindowSizeOffset],
														fecBuf[KMaxTransmitOffset],
														LittleEndian::Get16(fecBuf.Ptr() + KRetransmissionTimeoutOffset),
														LittleEndian::Get16(fecBuf.Ptr() + KMonitorTimeoutOffset),
														LittleEndian::Get16(fecBuf.Ptr() + KMaximumPDUSizeOffset));
				SetFec(fec);
				}
				break;

			case EConfigOptionTypeFcs:
				// Special case. Even though we don't support the 'FCS option' option
				// (i.e. we always do FCS), this can be silently ignored because of
				// its specific semantics: FCS is only disabled on a channel on which
				// both ends include this option with the value = 'No FCS'. Since we
				// will never do that, the value we receive from the peer has no effect
				// and can be ignored instead of responding with 'Unknown Option',
				// even though 'FCS Option' is 0 in our supported feature mask - some
				// people still send the option even in this case.
				break;
				
			default:
				{
				// Store the unknown options into the buffer argument.
				// If the hint bit is set (MSB of option type field) ignore the unknown
				// parameter.
				if(!(optionType & KOptionTypeIsHintMask))
					{
					// This is not a hint option - add it to the list of unknown options
					// to be sent in the subsequent response.
					RMBufChain unknownOption;
					TRAP(rerr, aCommand.CopyL(unknownOption, startOfCurrentOption, KOptionHeaderLength + optionDataLen));
					if(rerr == KErrNone)
						{
						aUnknownOptions.Append(unknownOption);
						rerr = KErrConfigUnknownOptions;
						}
					}
				}
				break;
			}

		// Move to the next option.
		startOfCurrentOption += (optionDataLen + KOptionHeaderLength);
		}
	return rerr;
	}

/*static*/ TInt TL2CapConfigParamOptions::AddConfigResponseOptions(CL2CapChannelConfig& aConfig, TUint8 aOptionOffset, RMBufChain& aOptions, TConfigResponseResult aResult)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;
	TInt currentCommandIndex = 0;
	// General rule is that we include accepted options only in positive responses.
	TBool isUnacceptableParams = (aResult == EConfigUnacceptableParams);

	const TL2CapConfigurationOptionGroupBase::TOptionConfigStatus mtuStatus = aConfig.OutgoingMTU().ConfigOptionStatus();
	const TL2CapConfigurationOptionGroupBase::TOptionConfigStatus flushToStatus = aConfig.IncomingFlushTimeout().ConfigOptionStatus();
	const TL2CapConfigurationOptionGroupBase::TOptionConfigStatus qosStatus = aConfig.IncomingQOS().ConfigOptionStatus();
	const TL2CapConfigurationOptionGroupBase::TOptionConfigStatus fecStatus = aConfig.FecNegotiator().OutgoingConfigOptionStatus();

	__ASSERT_DEBUG(isUnacceptableParams ||
					(mtuStatus != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding &&
					 flushToStatus != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding &&
					 qosStatus != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding &&
					 fecStatus != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding),
				   Panic(EL2CapConstructingPositiveConfigResponseWithUnresolvedOptionStatus));

	// The BT spec (sec 5.1) specifies that positive Config Responses will always
	// contain the MTU that is to be used on the channel.
	if (!(isUnacceptableParams && mtuStatus != TL2CapConfigurationOptionGroupBase::EOptionConfigFailed))
		{
		TRAP(rerr, currentCommandIndex += AddMtuOptionL(aConfig.OutgoingMTU().Preferred().MTU(), 
											aOptionOffset + currentCommandIndex, 
											aOptions));
		}

	if (rerr == KErrNone &&
		((isUnacceptableParams && flushToStatus == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed) ||
		 (!isUnacceptableParams && aConfig.IncomingFlushTimeout().NeedToIncludeInPositiveConfigResponse())))
		{
		TRAP(rerr, currentCommandIndex += AddFlushOptionL(aConfig.IncomingFlushTimeout().Preferred().FlushTimeoutDuration(), 
											aOptionOffset + currentCommandIndex, 
											aOptions));
		}

	if (rerr == KErrNone &&
		((isUnacceptableParams && qosStatus == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed) ||
		 (!isUnacceptableParams && aConfig.IncomingQOS().NeedToIncludeInPositiveConfigResponse())))
		{
		TRAP(rerr, currentCommandIndex += AddQosOptionL(aConfig.IncomingQOS().Preferred(), 
											aOptionOffset + currentCommandIndex,
											aOptions));
		}

	if (rerr == KErrNone &&
		((isUnacceptableParams && fecStatus == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed) ||
		 (!isUnacceptableParams && aConfig.FecNegotiator().NeedToIncludeInPositiveConfigResponse())))
		{
		TRAP(rerr, currentCommandIndex += AddFecOptionL(aConfig.FecNegotiator().OutgoingPreferred(),
											aOptionOffset + currentCommandIndex,
											aOptions));
		}

	return rerr;
	}

/*static*/ TInt TL2CapConfigParamOptions::AddConfigRequestOptions(CL2CapChannelConfig& aConfig, TUint8 aOptionOffset, RMBufChain& aOptions)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;
	TInt currentCommandIndex = 0;
	
	// Config Request parameters.
	
	if(aConfig.IncomingMTU().ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigComplete && rerr == KErrNone)
		{		
		TRAP(rerr, currentCommandIndex += AddMtuOptionL(aConfig.IncomingMTU().Preferred().MTU(), 
											aOptionOffset + currentCommandIndex, 
											aOptions));
		}

	if(aConfig.OutgoingFlushTimeout().ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigComplete && rerr == KErrNone)
		{		
		TRAP(rerr, currentCommandIndex += AddFlushOptionL(aConfig.OutgoingFlushTimeout().Preferred().FlushTimeoutDuration(), 
											  aOptionOffset + currentCommandIndex, 
											  aOptions));
		}

	if(aConfig.OutgoingQOS().ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigComplete && rerr == KErrNone)
		{			
		TRAP(rerr, currentCommandIndex += AddQosOptionL(aConfig.OutgoingQOS().Preferred(), 
											aOptionOffset + currentCommandIndex,
											aOptions));
		}
		
	if(aConfig.FecNegotiator().IncomingConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigComplete && rerr == KErrNone)
		{
		TRAP(rerr, currentCommandIndex += AddFecOptionL(aConfig.FecNegotiator().IncomingPreferred(),
											aOptionOffset + currentCommandIndex,
											aOptions));
		}

	return rerr;		
	}

/* static */ TInt TL2CapConfigParamOptions::AddMtuOptionL(TInt16 aMtu, TUint8 aOptionOffset, RMBufChain& aOptions)
	{
	LOG_STATIC_FUNC
	aOptions.AppendL(KOptionHeaderLength + KMTUOptionDataLen);
	
	TBuf8<KOptionHeaderLength + KMTUOptionDataLen> mtuBuf(KOptionHeaderLength + KMTUOptionDataLen);
	
	mtuBuf[KTypeOffset] = EConfigOptionTypeMTU;
	mtuBuf[KLengthOffset] = KMTUOptionDataLen;
	LittleEndian::Put16(&mtuBuf[KDataOffset], aMtu);

	aOptions.CopyIn(mtuBuf, aOptionOffset);
	
	return KOptionHeaderLength + KMTUOptionDataLen;
	}
	
/* static */ TInt TL2CapConfigParamOptions::AddFlushOptionL(TInt16 aFlushTimeout, TUint8 aOptionOffset, RMBufChain& aOptions)
	{
	LOG_STATIC_FUNC
	aOptions.AppendL(KOptionHeaderLength + KFlushOptionDataLen);
	
	TBuf8<KOptionHeaderLength + KFlushOptionDataLen> flushBuf(KOptionHeaderLength + KFlushOptionDataLen);

	flushBuf[KTypeOffset] = EConfigOptionTypeFlushTimeoutDuration;
	flushBuf[KLengthOffset] = KFlushOptionDataLen;
	LittleEndian::Put16(&flushBuf[KDataOffset], aFlushTimeout);

	aOptions.CopyIn(flushBuf, aOptionOffset);
	
	return KOptionHeaderLength + KFlushOptionDataLen;
	}
	
/* static */ TInt TL2CapConfigParamOptions::AddQosOptionL(const TQualityOfServiceOption& aQosOption, TUint8 aOptionOffset, RMBufChain& aOptions)
	{
	LOG_STATIC_FUNC
	aOptions.AppendL(KOptionHeaderLength + KQOSOptionDataLen);
	
	TBuf8<KOptionHeaderLength + KQOSOptionDataLen> qosBuf(KOptionHeaderLength + KQOSOptionDataLen);

	qosBuf[KTypeOffset] = EConfigOptionTypeQOS;
	qosBuf[KLengthOffset] = KQOSOptionDataLen;
	qosBuf[KDataOffset] = 0;

	// Now insert each of the QOS options
	qosBuf[KDataOffset + KQOSServiceTypeOffset] = static_cast<TUint8>(aQosOption.ServiceType());
	LittleEndian::Put32(&qosBuf[KDataOffset + KTokenRateOffset], aQosOption.TokenRate());	
	LittleEndian::Put32(&qosBuf[KDataOffset + KTokenBucketSizeOffset], aQosOption.TokenBucketSize());	
	LittleEndian::Put32(&qosBuf[KDataOffset + KPeakBandwidthOffset], aQosOption.PeakBandwidth());	
	LittleEndian::Put32(&qosBuf[KDataOffset + KLatencyOffset], aQosOption.Latency());	
	LittleEndian::Put32(&qosBuf[KDataOffset + KDelayVariationOffset], aQosOption.DelayVariation());	
	
	aOptions.CopyIn(qosBuf, aOptionOffset);	
	
	return KOptionHeaderLength + KQOSOptionDataLen;
	}

/* static */ TInt TL2CapConfigParamOptions::AddFecOptionL(const TRetransmissionAndFlowControlOption& aFecOption, TUint8 aOptionOffset, RMBufChain& aOptions)
	{
	LOG_STATIC_FUNC
	aOptions.AppendL(KOptionHeaderLength + KFECOptionDataLen);
	
	TBuf8<KOptionHeaderLength + KFECOptionDataLen> fecBuf(KOptionHeaderLength + KFECOptionDataLen);

	fecBuf[KTypeOffset] = EConfigOptionTypeRTxAndFEC;	
	fecBuf[KLengthOffset] = KFECOptionDataLen;
	
	fecBuf[KDataOffset] = aFecOption.LinkModeAsUnsignedByte();	
	fecBuf[KDataOffset + KTxWindowSizeOffset] = aFecOption.TxWindowSize();	
	fecBuf[KDataOffset + KMaxTransmitOffset] = aFecOption.MaxTransmit();	
	LittleEndian::Put16(&fecBuf[KDataOffset + KRetransmissionTimeoutOffset], aFecOption.RetransmissionTimeout());	
	LittleEndian::Put16(&fecBuf[KDataOffset + KMonitorTimeoutOffset], aFecOption.MonitorTimeout());	
	LittleEndian::Put16(&fecBuf[KDataOffset + KMaximumPDUSizeOffset], aFecOption.MaximumPDUSize());	

	aOptions.CopyIn(fecBuf, aOptionOffset);
	
	return KOptionHeaderLength + KFECOptionDataLen;
	}

/* static */ TBool TL2CapConfigParamOptions::VerifyOptionsStructure(TUint8 aOptionOffset, const RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	TUint8 optionDataLen = 0;
	TInt startOfCurrentOption = aOptionOffset;
	TInt commandLength = aCommand.Length();
	TBuf8<KOptionHeaderLength> headerBuf(KOptionHeaderLength);

	TBool valid = ETrue;
	// Parse the command options.  Ensure that there is at least an option header
	// left to read.
	while(startOfCurrentOption + KOptionHeaderLength <= commandLength && valid)
		{
		aCommand.CopyOut(headerBuf, startOfCurrentOption);
		
		TUint8 optionType = headerBuf[KTypeOffset];
		optionDataLen = headerBuf[KLengthOffset];

		// Switch on the option type. Option type could have hint bit set, remove it
		switch(optionType & ~KOptionTypeIsHintMask)
			{
			case EConfigOptionTypeMTU:
				if(optionDataLen != KMTUOptionDataLen || 
				   startOfCurrentOption + KOptionHeaderLength + optionDataLen > commandLength)
					{
					valid = EFalse;
					}
				break;

			case EConfigOptionTypeFlushTimeoutDuration:
				if(optionDataLen != KFlushOptionDataLen || 
				   startOfCurrentOption + KOptionHeaderLength + optionDataLen > commandLength)
					{
					valid = EFalse;
					}
				break;
				
			case EConfigOptionTypeQOS:
				if(optionDataLen != KQOSOptionDataLen || 
				   startOfCurrentOption + KOptionHeaderLength + optionDataLen > commandLength)
					{
					valid = EFalse;
					}
				break;
			
			case EConfigOptionTypeRTxAndFEC:
				if(optionDataLen != KFECOptionDataLen || 
				   startOfCurrentOption + KOptionHeaderLength + optionDataLen > commandLength)
					{
					valid = EFalse;
					}
				break;

			case EConfigOptionTypeFcs:
				if(optionDataLen != KFcsOptionDataLen || 
			       startOfCurrentOption + KOptionHeaderLength + optionDataLen > commandLength)
					{
					valid = EFalse;
					}
				break;
				
			default:
				// Store the unknown options into the buffer argument.
				if(startOfCurrentOption + KOptionHeaderLength + optionDataLen > commandLength)
					{
					// The option is not correctly formatted.
					valid = EFalse;
					}
				break;
			};
		// Move to the next option.
		startOfCurrentOption += (optionDataLen + KOptionHeaderLength);
		}

	if(startOfCurrentOption != commandLength)
		{
		// Spurious bytes
		valid = EFalse;
		}

	return valid;
	}

