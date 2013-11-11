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
// Implements the avdtp signalling session
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpSignallingSession.h"
#include "avdtpSignallingChannel.h"
#include "avdtp.h"
#include "avdtpsap.h"
#include "avdtpStream.h"
#include "avdtpLogicalChannelFactory.h"
#include "avdtpConfigurators.h"
#include "avdtputil.h"
#include "avdtpDirectChannel.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

#ifdef _DEBUG
PANICCATEGORY("avdtpsigse");
#endif

CSignallingSession* CSignallingSession::NewLC(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP)
	{
	LOG_STATIC_FUNC
	CSignallingSession* s = new (ELeave) CSignallingSession(aProtocol, aSAP);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}
	
CSignallingSession* CSignallingSession::NewL(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP)
	{
	LOG_STATIC_FUNC
	CSignallingSession* s = CSignallingSession::NewLC(aProtocol, aSAP);
	CleanupStack::Pop();
	return s;		
	}
	
CSignallingSession::CSignallingSession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP)
: CTransportSession(aProtocol, aSAP, ESignalling),
  iLocalSEPs(_FOFF(CLocalSEP, iSignallingSessionLink))
	{
	LOG_FUNC	
	}

CSignallingSession::~CSignallingSession()
	{
	LOG_FUNC
	DestroyLocalSEPs();
	ClearSignallingChannel();
	delete iSEPConfigurator;
	}

	
void CSignallingSession::ConstructL()
	{
	LOG_FUNC
	CTransportSession::ConstructL();
	}

TInt CSignallingSession::ActiveOpen()
	{
	LOG_FUNC
	// esock will protect us from an accidental double connect
	TInt ret = KErrNone;
	if (iSignallingChannel)
		{
		if (!iSignallingChannel->IsListening())
			{
			// already got a suitable SC - probably due to a passive connection
			// just say it's done (we're already attached)
			iSAP.Ready();
			}
		else
			{
			// signalling channel is listening - detach from it, user has decided to connect
			// drop through afterwards
			ClearSignallingChannel();
			}
		}
	// not else as ClearSignallingChannel will NULL iSignallingChannel
	if (!iSignallingChannel)
		{
		iSignallingChannel = iProtocol.GetSignallingChannel(RemoteAddress());
		if (iSignallingChannel)
			{
			ret = iSignallingChannel->AttachSignallingUser(*this);
			}
		else
			{
			ret = KErrNoMemory;
			}
		}

	return ret;
	}
	
void CSignallingSession::ClearSignallingChannel()
	{
	LOG_FUNC
	if (iSignallingChannel)
		{
		iSignallingChannel->DetachSignallingUser(*this);	
		}
	iSignallingChannel = NULL;
	}
	
void CSignallingSession::DestroyLocalSEPs()
	{
	LOG_FUNC
	while (!iLocalSEPs.IsEmpty())
		{
		CLocalSEP* sep = iLocalSEPs.First();
		sep->iSignallingSessionLink.Deque();
		// inform codman - it'll clear bits as necessary since it reference coutns
		iProtocol.CodMan().RemoveCodService(sep->Info().IsSink() ? EMajorServiceRendering :
																	EMajorServiceCapturing);
		delete sep;		
		}
	}
	
void CSignallingSession::DoShutdown()
	{
	LOG_FUNC

	TDblQueIter<CLocalSEP> iter(iLocalSEPs);
	while(iter)
		{
		iter++->SetInUse(EFalse);
		}
		
	ClearSignallingChannel();
	
	if (iSAPShutdown == ENormal)
		{
		iSAP.CanClose();
		}
	else
		{
		// base class has detached us so that we own ourselves
		// nothing more to do, apart from die.
		delete this;
		}
	}

TInt CSignallingSession::Send(RMBufChain& /*aData*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	// not supported in signalling - only ioctls and opts allowed
	return 0;
	}

TInt CSignallingSession::GetData(RMBufChain& /*aData*/)
	{
	LOG_FUNC
	return KErrNotSupported;
	}
	
// stuff forwarded from SAP
TInt CSignallingSession::SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption)
	{
	LOG_FUNC
	TInt ret = KErrNotSupported;

	if (aLevel == KSolBtAVDTPSignalling)
		{
		switch (aName)
			{
			case EAwaitPassiveSignallingChannel:
				{
				if (iIsListening && iSignallingChannel && iSignallingChannel->IsListening())
					{
					ret = KErrAlreadyExists;
					}
				else if (iSignallingChannel && !iSignallingChannel->IsListening())
					{
					ret = KErrInUse;
					}
				else
					{					
	                iSignallingChannel=iProtocol.FindListeningSignallingChannel();
					// For ACP - triggered when client is ready for listening
					// (would be nice to be "automatic", but needs client to be happy they've registered all their seps
					if (!iSignallingChannel)
						{
	    				iSignallingChannel = iProtocol.CreateSignallingChannelForListening();
						}
					
					if (iSignallingChannel)
						{
						ASSERT_DEBUG(iSignallingChannel->IsListening());
						ret = iSignallingChannel->AttachSignallingUser(*this);
						if (ret == KErrNone)
							{
							iIsListening = ETrue;
							}
						else
							{
							// if we didn't manage to tell anyone else
							// then no one else knows so actually we're best
							// off cleaning up after ourselves.
							iSignallingChannel->DetachSignallingUser(*this);
							iSignallingChannel = NULL;
							}							
						}
					else
						{
						ret = KErrNoMemory;
						}
					}
				break;
				}

			case EStopAwaitPassiveSignallingChannel:
				{
				if (!iSignallingChannel)
					{
    				ret = KErrNotReady;
					}

				else
					{
					iIsListening = EFalse;
					ClearSignallingChannel();
					}
				break;
				}
			
			// also for reconfigure - the configurator decides
			case EStartConfiguringRemote:
				{
				if (iSEPConfigurator)
					{
					ret = KErrInUse;
					}
				else
					{
					// Check the integrity of the descriptor					
					if (!aOption.Ptr() || aOption.Length() != sizeof(TInternalSelectRemoteSEP))
						{
						ret = KErrBadDescriptor;
						}
					else
						{
						// get Remote SEID we're configuring, and the binding local SEP SEID
						// avdtp spec makes us have to do the binding now
						TInternalSelectRemoteSEP select = *reinterpret_cast<const TInternalSelectRemoteSEP*>(aOption.Ptr());

						if (!select.iLocalSEID.IsLocal())
							{
							ret = KErrArgument;
							}
						else
							{							
							CLocalSEP* localSEP = FindLocalSEP(select.iLocalSEID);
							if (!localSEP)
								{
								// client supplied bad SEID
								ret = KErrArgument;
								}
							else
								{
								// don't support reconfiguring of an active stream - client must suspend
								CAVStream* stream = localSEP->Stream();
								if (stream && stream->IsStreaming())
									{
									ret = KErrInUse;
									}
								else
									{
									TRAP(ret, iSEPConfigurator = CRemoteSEPConfigurator::NewL(*this,
																  iProtocol,
																  select.iLocalSEID,
																  select.iRemoteSEID,
																  stream ? ETrue : EFalse));
									}
								}
							}
						}
					}
				break;
				}
			case EStartConfiguringLocal:
				{
				if (iSEPConfigurator)
					{
					ret = KErrInUse;
					}
				else
					{
					// Check the integrity of the descriptor					
					if (!aOption.Ptr() || aOption.Length() != sizeof(TSEID))
						{
						ret = KErrBadDescriptor;
						}
					else
						{
						// get Local SEID we're configuring
						TSEID localseid = *reinterpret_cast<const TSEID*>(aOption.Ptr());
						TRAP(ret, iSEPConfigurator = CLocalSEPConfigurator::NewL(*this, iProtocol, localseid));
						}
					}
				break;
				}
			
			case EAddCapabilitySelection:
				{
				if (iSEPConfigurator)
					{
					ret = iSEPConfigurator->AddCapability(aOption);
					}
				else
					{
					ret = KErrNotReady;
					}
				break;
				}
				
			case ESendConfigurationResponse:
				{
				if (iSignallingChannel)
					{
					// Check the integrity of the descriptor					
					if (!aOption.Ptr() || aOption.Length() != sizeof(TAvdtpInternalConfigurationResponse))
						{
						ret = KErrBadDescriptor;
						}
					else
						{
						const TAvdtpInternalConfigurationResponse rsp = *reinterpret_cast<const TAvdtpInternalConfigurationResponse*>(aOption.Ptr());
						// check SEID is ok
						CLocalSEP* sep = FindLocalSEP(rsp.iSEID);
						if (sep)
							{						
	 						// need to tell sep of the outcome so it can clear its state
	 						// this needs to occur for config and reconfig
	 						// first param is true if result is kerrnone- i.e. all ok, false otherwise
	 						TRAP(ret, sep->CommitPendingConfigurationL(rsp.iResult == KErrNone,rsp.iIsReconfigure));
							if (!rsp.iIsReconfigure)
								{
								if (rsp.iResult == KErrNone)
									{
									if (ret==KErrNone)
										{
										ret = iSignallingChannel->SendSetConfigurationAccept(rsp.iTransactionLabel);
										if (ret==KErrNone)
											{
											// time to spawn that (ACP) stream
											TRAP(ret, sep->ExpectStreamL(RemoteAddress(), *this, iProtocol));
											}
										}
									}
								else
									{
									ret = iSignallingChannel->SendSetConfigurationReject(rsp.iTransactionLabel,
																			   AvdtpInternalUtils::SymbianErrorToAvdtpError(rsp.iResult),
																			   rsp.iCategory);
									}
								}
							else
								{
								if (!sep->InUse())
									{
									// almost assertable since we shouldnt have forwarded the indication if not
									// but we don't want a DoS if bad client
									ret = KErrNotReady;
									}
								else
									{
									if (rsp.iResult == KErrNone)
										{
										ret = iSignallingChannel->SendReconfigureAccept(rsp.iTransactionLabel);
										}
									else
										{
										ret = iSignallingChannel->SendReconfigureReject(rsp.iTransactionLabel,
																			   AvdtpInternalUtils::SymbianErrorToAvdtpError(rsp.iResult),
																			   rsp.iCategory);
										}
									}
								}
							if (ret==KErrNone && rsp.iResult == KErrNone && !rsp.iIsReconfigure)
								{
								// now in use, and from AVDTP spec sect6.3 we now do not expect other transactions until stream started
								// not point resetting for reconfigure
								sep->SetInUse(ETrue);
								}
							}
						else
							{
							ret = KErrArgument;
							}
						}
					}
				else
					{
					ret = KErrDisconnected;
					}
				break;
				}
				
			case ESendStartResponse:
				{
				if (iSignallingChannel)
					{
					// Check the integrity of the descriptor					
					if (!aOption.Ptr() || aOption.Length() != sizeof(TAvdtpInternalStartResponse))
						{
						ret = KErrBadDescriptor;
						}
					else
						{
						const TAvdtpInternalStartResponse rsp = *reinterpret_cast<const TAvdtpInternalStartResponse*>(aOption.Ptr());
						// check SEID is ok
						CLocalSEP* sep = FindLocalSEP(rsp.iSEID);
						
						if (sep && sep->Stream() && rsp.iResult == KErrNone)
							{
							ret = iSignallingChannel->SendStartAccept(rsp.iTransactionLabel);
							if (ret==KErrNone)
								{
								sep->Stream()->Started();
								}
							}
						else if (sep && !(sep->Stream()))
							{
							ret = KErrUnknown;
							}
						else if (sep)
							{
							// The AVDTP error code (spec... ACP to INT error code table) is offset by -18045
							ret = iSignallingChannel->SendStartReject(rsp.iTransactionLabel, AvdtpInternalUtils::SymbianErrorToAvdtpError(rsp.iResult), rsp.iSEID);
							}
						else
							{
							ret = KErrArgument;
							}
						}
					}
				else
					{
					ret = KErrDisconnected;
					}
				break;
				}
			case ESendSuspendResponse:
				{
				if (iSignallingChannel)
					{
					// Check the integrity of the descriptor					
					if (!aOption.Ptr() || aOption.Length() != sizeof(TAvdtpInternalSuspendResponse))
						{
						ret = KErrBadDescriptor;
						}
					else
						{
						const TAvdtpInternalSuspendResponse rsp = *reinterpret_cast<const TAvdtpInternalSuspendResponse*>(aOption.Ptr());
						// check SEID is ok
						CLocalSEP* sep = FindLocalSEP(rsp.iSEID);
						if (sep && sep->Stream() && rsp.iResult == KErrNone)
							{
							ret = iSignallingChannel->SendSuspendAccept(rsp.iTransactionLabel);
							if (ret==KErrNone)
								{
								sep->Stream()->Suspended();
								}
							}
						else if (sep && !(sep->Stream()))
							{
							ret = KErrUnknown;
							}
						else if (sep)
							{
							// The AVDTP error code (spec... ACP to INT error code table) is offset by -18045
							ret = iSignallingChannel->SendSuspendReject(rsp.iTransactionLabel, AvdtpInternalUtils::SymbianErrorToAvdtpError(rsp.iResult), rsp.iSEID);
							}
						else
							{
							ret = KErrArgument;
							}
						}
					}
				else
					{
					ret = KErrDisconnected;
					}
				break;
				}
			case ESendSecurityControlResponse:
				{
				if (iSignallingChannel)
					{
					// Check the integrity of the descriptor					
					if (!aOption.Ptr() || aOption.Length() != sizeof(TAvdtpInternalSecurityControlResponse))
						{
						ret = KErrBadDescriptor;
						}
					else
						{
						const TAvdtpInternalSecurityControlResponse secRsp = *reinterpret_cast<const TAvdtpInternalSecurityControlResponse*>(aOption.Ptr());
						// check SEID is ok
						CLocalSEP* sep = FindLocalSEP(secRsp.iSEID);
						if (!sep)
							{
							ret = KErrArgument;
							}
						else
							{
							if(secRsp.iResult == KErrNone)
								{
								const TDesC8* rspInfo = secRsp.iSecurityControlInfo.Length() ? &secRsp.iSecurityControlInfo : NULL;
								ret = iSignallingChannel->SendSecurityControlAccept(secRsp.iTransactionLabel, rspInfo);
								}
							else
								{
								ret = iSignallingChannel->SendSecurityControlReject(secRsp.iTransactionLabel,
																		AvdtpInternalUtils::SymbianErrorToAvdtpError(secRsp.iResult));
								}
							}
						}
					}
				else
					{
					ret = KErrDisconnected;
					}
				break;
				}
			case EShutdown:
				{
				ret=KErrNone;
				TDblQueIter<CLocalSEP> iter(iLocalSEPs);
				while(iter)
					{
					if (iter++->Stream()) 
						{
						ret=KErrNotReady;
						}
					}
				if (ret==KErrNone)
					{
					iter.SetToFirst();
					while(iter)
						{
						iter++->SetInUse(EFalse);
						}

					ClearSignallingChannel();
					iSAP.SessionDisconnect();
					}
				break;
				}
			}
		}
	return ret;
	}
	
	
	
void CSignallingSession::Ioctl(TUint aLevel, TUint aName, const TDesC8* aOption)
	{
	LOG_FUNC
	TInt res = KErrNotSupported;
	
	if (aLevel == KSolBtAVDTPSignalling)	
		{
		const TUint8* optionPtr=NULL;
		if (aOption)
			{
			optionPtr = aOption->Ptr();
			}
			
		switch (aName)
			{
			case EDiscoverSEPs:
				{
				res=iSignallingChannel ? iSignallingChannel->SendDiscoverSEPs(*this) : KErrNotReady;
				break;
				}
			case EGetCapabilities:
				{
				// Check the integrity of the descriptor					
				if (!optionPtr || aOption->Length() != sizeof(TSEID))
					{
					res = KErrBadDescriptor;
					}
				else
					{
					TSEID seid = *reinterpret_cast<const TSEID*>(optionPtr);
					
					if (seid.IsLocal())
						{
						// meaningless to try to associate with a remote sep at this stage
						// getting local capabilities not supported
						res = KErrArgument;
						}
					else
						{						
						// client doesn't need to have populated remote sep cache at this point, so just try to send
						res=iSignallingChannel ?  iSignallingChannel->SendGetCapabilities(*this, seid) : KErrNotReady;
						}
					}
				break;
				}
			case EAbortStream:
				{
				// Check the integrity of the descriptor					
				if (!optionPtr || aOption->Length() != sizeof(TSEID))
					{
					res = KErrBadDescriptor;
					}
				else
					{
					res = KErrNone;
					// check seid exists
					const TSEID& seid = *reinterpret_cast<const TSEID*>(optionPtr);
					TSEID packetSeid;
					
					if (seid.IsLocal())
						{
						CLocalSEP* sep = FindLocalSEP(seid);
						// check local sep exists
						if (!sep || !(sep->Stream()))
							{
							res = KErrArgument;
							}
						else
							{
							packetSeid = sep->Stream()->RemoteSEID();
							}
						}
					else
						{
						// remote was specified - again, check it'ok
						if (!FindStreamFromRemoteSEID(seid))
							{
							res = KErrArgument;
							}
						else
							{
							packetSeid = seid;
							}
						}
					if (res==KErrNone)
						{
						res = SendAbort(packetSeid);
						}
					}
				break;
				}
			case ESelectSEP:
				{
				if (iSEPConfigurator)
					{				
					TInt cfgRes = iSEPConfigurator->Finalise();
					// user doesn't need to know whether we sync or async
					res = cfgRes == KRequestPending ? KErrNone : cfgRes;
					if (cfgRes==KErrNone)
						{
						// can complete the service now
						iSAP.ServiceComplete(NULL);
						}
					delete iSEPConfigurator;
					iSEPConfigurator = NULL;
					}
				else
					{
					res = KErrNotReady;
					}
				break;
				}
			case EStartStreaming:
				{
				if (iSignallingChannel==NULL)
					{
					res = KErrNotReady;
					}
				else
					{
					// Check the integrity of the descriptor					
				if (!optionPtr || aOption->Length() != sizeof(TSEID))
					{
					res = KErrBadDescriptor;
					}
					else
						{
						const TSEID& clientSeid = *reinterpret_cast<const TSEID*>(optionPtr);
						
						// see whether it was a local or remote seid - either is ok
						CAVStream* stream = NULL;
						
						if (clientSeid.IsLocal())
							{
							CLocalSEP* sep = FindLocalSEP(clientSeid);
							if (sep)
								{
								stream = sep->Stream();
								}
							}
						else
							{
							// client supplied remote seid so just check
							stream = FindStreamFromRemoteSEID(clientSeid);
							}
						res = stream ? stream->Start() : KErrUnknown;
						}
					}					
				break;
				}
			case ESuspendStreaming:
				{
				if (iSignallingChannel==NULL)
					{
					res = KErrNotReady;
					}
				else
					{
					// Check the integrity of the descriptor					
				if (!optionPtr || aOption->Length() != sizeof(TSEID))
					{
					res = KErrBadDescriptor;
					}
					else
						{
						const TSEID& clientSeid = *reinterpret_cast<const TSEID*>(optionPtr);
						// see whether it was a local or remote seid - either is ok
						CAVStream* stream = NULL;
						
						if (clientSeid.IsLocal())
							{
							CLocalSEP* sep = FindLocalSEP(clientSeid);
							if (sep)
								{
								stream = sep->Stream();
								}
							}
						else
							{
							// client supplied remote seid so just check
							stream = FindStreamFromRemoteSEID(clientSeid);
							}
						res = stream ? stream->Suspend() : KErrUnknown;
						}
					}
				break;
				}
			case ESendSecurityControl:
				{
				//GAVDP is wishing to send security control command (allowed any time if capable)
				// needs to be remote
				// Check the integrity of the descriptor					
				if (!optionPtr || aOption->Length() != sizeof(TAvdtpInternalSendSecurityControl))
					{
					res = KErrBadDescriptor;
					}
				else
					{
					const TAvdtpInternalSendSecurityControl& security = *reinterpret_cast<const TAvdtpInternalSendSecurityControl*>(optionPtr);
					if (!security.iRemoteSEID.IsLocal())
						{
						// check capability
						if (iSignallingChannel)
							{
							if (iProtocol.RemoteSEPCache().HasCapability(RemoteAddress(),
																		security.iRemoteSEID,
																		EServiceCategoryContentProtection))
								{
								res = iSignallingChannel->SendSecurityControl(*this,
																		security.iRemoteSEID,
																		security.iSecurityControlInfo);
								}
							else
								{
								// remote doesnt support CP
								res = KErrNotSupported;
								}
							}
						else
							{
							res = KErrNotReady;
							}
						}
					else
						{
						res = KErrArgument;
						}
					}
				break;
				}	
			default:
				{
				res = KErrNotSupported;
				}
			}
		if (res!=KErrNone)
			{
			iSAP.Error(res);
			}
		}
	}
	
#ifdef _DEBUG	
void CSignallingSession::CancelIoctl(TUint aLevel, TUint /*aName*/)
#else
void CSignallingSession::CancelIoctl(TUint /*aLevel*/, TUint /*aName*/)
#endif
	{
	LOG_FUNC
	__ASSERT_DEBUG(aLevel==KSolBtAVDTPSignalling, Panic(EAvdtpBadIoctl));
	// none at present	
	}
	
	
TInt CSignallingSession::GetOption(TUint aLevel, TUint aName, TDes8& aOption) const
	{
	LOG_FUNC
	TInt ret = KErrNotSupported;
	
	if (aLevel == KSolBtAVDTPSignalling)
		{
		switch (aName)
			{
			case EGetProposedConfiguration:
				{
				// Check the integrity of the descriptor. The option length is variable, subject to the payload length.
				if (!aOption.Ptr() || aOption.Length() < sizeof(TInternalGetProposedConfiguration))
					{
					ret = KErrBadDescriptor;
					}
				else
					{
					TInternalGetProposedConfiguration& conf = *const_cast<TInternalGetProposedConfiguration*>(reinterpret_cast<const TInternalGetProposedConfiguration*>(aOption.Ptr()));
					CLocalSEP* s = FindLocalSEP(conf.iSEID);
					if (s)
						{
						// Check that the option buffer size is sufficient
						TPtrC8 pendingConf(s->PendingConfiguration());
						if (aOption.MaxLength() < pendingConf.Length())
							{
							ret = KErrUnderflow;
							}
						else
							{
							// would be nice to use a return struct
							// but gets tricky - we don't know how big the option is
							// so can't use TBuf. TPtr falls foul of IPC.
							aOption.Copy(pendingConf);
							ret = KErrNone;
							}
						}
					else
						{
						ret = KErrArgument;
						}
					}
				break;
				}
			
			case EGetSecurityControl:
				{
				// Check the integrity of the descriptor. The option length is variable, subject to the payload length.
				if (!aOption.Ptr() || aOption.Length() < sizeof(TInternalGetSecurityControl))
					{
					ret = KErrBadDescriptor;
					}
				else
					{
					TInternalGetSecurityControl& sec = *const_cast<TInternalGetSecurityControl*>(reinterpret_cast<const TInternalGetSecurityControl*>(aOption.Ptr()));
					CLocalSEP* s = FindLocalSEP(sec.iSEID);
					if (s)
						{
						// Check that the option buffer size is sufficient
						TPtrC8 securityData(s->SecurityControl());
						if (aOption.MaxLength() < securityData.Length())
							{
							ret = KErrUnderflow;
							}
						else
							{
							aOption.Copy(securityData);
							ret = KErrNone;
							}
						}
					else
						{
						ret = KErrArgument;
						}				
					}
				break;
				}
			case EGetAVDTPCapabilityResponse:
				{
				// Check the integrity of the descriptor. The option length is variable, subject to the payload length.
				if (!aOption.Ptr() || aOption.Length() < sizeof(TInternalGetSEPCapability))
					{
					ret = KErrBadDescriptor;
					}
				else
					{
					TInternalGetSEPCapability cap = *reinterpret_cast<const TInternalGetSEPCapability*>(aOption.Ptr());
					TPtrC8 ptr;
					// get the required capability from the SEP cache (in 'air' form)
					TRAP(ret, ptr.Set(iProtocol.RemoteSEPCache().GetCapabilityL(
															RemoteAddress(),
															cap.iSEID,
															cap.iServiceCategory));)
															
					if (ret==KErrNone)
						{
						// Check that the option buffer size is sufficient
						if (aOption.MaxLength() < ptr.Length())
							{
							ret = KErrUnderflow;
							}
						else
							{
							aOption.Copy(ptr);
							}
						}
					}
				break;
				}
				
			case ERegisterSEP:
				{
				// Check the integrity of the descriptor
				if (!aOption.Ptr() || aOption.Length() != sizeof(TAvdtpSEPInfo))
					{
					ret = KErrBadDescriptor;
					}
				else
					{
					TAvdtpSEPInfo info = *reinterpret_cast<const TAvdtpSEPInfo*>(aOption.Ptr());
					// client doesn't manage inUse field - we do
					info.SetInUse(EFalse);
					
					TRAP(ret, const_cast<CSignallingSession*>(this)->CreateLocalSEPL(info));
					
					TPckg<TAvdtpSEPInfo> pckg(info);
					aOption.Copy(pckg);
					}
				break;
				}
				
			case EGetReportingAndRecoveryConfig:
				{
				// Check the integrity of the descriptor
				if (!aOption.Ptr() || aOption.Length() != sizeof(TReportingAndRecovery))
					{
					ret = KErrBadDescriptor;
					}
				else
					{
					TReportingAndRecovery rar;
					TPckg<TReportingAndRecovery> rarBuf(rar);
					rarBuf.Copy(aOption);
					CAVStream* stream = FindStreamFromRemoteSEID(rar.iSEID);
					if(stream == NULL)
						{
						ret = KErrNotFound;
						}
					else
						{
						rar.iReporting = stream->CheckConfigured(EServiceCategoryReporting);
						rar.iRecovery  = stream->CheckConfigured(EServiceCategoryRecovery);
						aOption.Copy(rarBuf);
						ret = KErrNone;
						}
					}
				break;
				}
			}
		}
		
	return ret;
	}
	


TInt CSignallingSession::OpenIndication(TSEID aSEID)
	{
	LOG_FUNC
	// now create the full Stream Object
	CLocalSEP* sep = FindLocalSEP(aSEID);
	if (sep && sep->Stream())
		{
		// this will tell channel factory to expect n channels on sending resp
		TRAPD(err, sep->Stream()->AwaitLogicalChannelsL());
		return err;
		}
	else if (sep && !sep->Stream())
		{
		// the remote must have decided that a GetConfig yielded just what they wanted
		// and have proceeded directly with Open
		// this *violates* GAVDP - see p 16
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		}
	else
		{
		return KErrNotFound;
		}
	}
	
	
TInt CSignallingSession::DiscoverIndication(TAvdtpTransactionLabel /*aLabel*/, CAvdtpOutboundSignallingMessage& aDiscoverResponseMessage)
	{
	LOG_FUNC
	// iterate over all idle SEPs returning their capabilities
	// need to return the label back to the Signalling Channel to tie up response

	// this is *far* easier if we process this synchronously
	// otherwise the design is tricky - we'd need to call a primitive bacxk on SigCh
	// and it would need to know our last SEP info etc
	TDblQueIter<CLocalSEP> iter(iLocalSEPs);
	CLocalSEP* sep;
	while(iter)
		{
		sep = iter++;
		
		TRAPD(overFlow, aDiscoverResponseMessage.AddSEPInfoL(sep->Info()));

		if (overFlow)
			{
			// no more will go, try continuing with usecase though
			// so NOT releaving - cos the message is ok
			break;
			}			
		}
	return KErrNone;
	}
	

TInt CSignallingSession::GetCapsIndication(TAvdtpTransactionLabel /*aLabel*/,
										   TSEID aSEID,
										   CAvdtpOutboundSignallingMessage& aGetCapsResponseMessage)
	{
	LOG_FUNC
	// client has set caps now - if they haven't then we will prob just send empty stuff back
	// i.e. we send whatever we've got (but possibly "knowing" this fact
	
	// find the idle SEP with aSEID
	CLocalSEP* sep = FindLocalSEP(aSEID);
	// this uses the same pattern as :DiscoverIndication
	if (!sep)
		{
		return KErrNotFound;
		}
	
	TInt ret = KErrNone;
	
	for (TInt i=0; i<sep->Capabilities().Count(); i++)
		{
		// one at a time - easier for us to cope if we OOM
		if (sep->Capabilities()[i])
			{
			// local sep has capability
			TRAPD(overFlow, aGetCapsResponseMessage.AddSEPCapabilityL(*sep->Capabilities()[i]));
			if (overFlow)
				{
				// no more will go
				// if even the first capability didn't go then we have to forget replying
				if (i==0)
					{
					// there are no caps in the response, so it's meaningless
					ret = KErrUnderflow;		
					}
				// otherwise we can try continuing with usecase though
				// so NOT erroring - cos the message is ok
				break;
				}			
			}
		
		}
	return ret;
	}

TInt CSignallingSession::SetConfigIndication(TAvdtpTransactionLabel aLabel,
											  TSEID aACPSEID,
											  TSEID aINTSEID,
											  RBuf8& aConfig)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}

/*
we check to see if the remote has (wrongly) set capabilities that the idle sep has not
if the remote has then we reject the request
else pass onto gavdp
*/
	
	CCapabilityNotAllowedVisitor* checker =
				new CCapabilityNotAllowedVisitor(sep->Categories());
	
	if (!checker)
		{
		return KErrNoMemory;
		}

	checker->Process(aConfig);
	
	if (!checker->IsValid())
		{
		// signalling channel expects us to put the duff category in the aConfig
		// take the first one (as suggested by spec)
		TPckgBuf<TAvdtpServiceCategory> catBuf(checker->WrongCategory());
		aConfig.Copy(catBuf);
		delete checker;
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadServCategory);
		}

	delete checker;
	checker = NULL;
	
	// update the SEP to contain the pending configuration
	// commited when gavdp says ok
	TInt res = sep->SetPendingConfiguration(aConfig, EFalse); // transfers aConfig ownership
	if (res==KErrNone)
		{
		sep->SetRemoteSEID(aINTSEID);

		// all ok, so pass on an indication to GAVDP as they need to configure too
		TInternalAvdtpConfigurationIndication ind;
		ind.iIndication = EAvdtpSetConfiguration;
		ind.iPayloadLength = aConfig.Length();
		ind.iSEID = aACPSEID;
		ind.iINTSEID = aINTSEID;
		ind.iTransactionLabel = aLabel;

		TPckg<TInternalAvdtpConfigurationIndication> indPckg(ind);
		iSAP.Indication(indPckg);
		}

	return res;
	}
	
TInt CSignallingSession::GetConfigIndication(TAvdtpTransactionLabel /*aLabel*/, TSEID aLocalSEID,
									CAvdtpOutboundSignallingMessage& aGetConfigResponseMessage)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aLocalSEID);
	
	if (!sep)
		{
		return KErrNotFound;
		}

	// remote may not have sent SetConfiguration
	if (sep->PreConfigured())
		{
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		}
		
	TInt ret = KErrNone;
	
	for (TInt i=0; i<sep->Configuration().Count(); i++)
		{
		// one at a time - easier for us to cope if we OOM
		if (sep->Configuration()[i])
			{
			// local sep has capability
			TRAPD(overFlow, aGetConfigResponseMessage.AddSEPCapabilityL(*sep->Configuration()[i]));
			if (overFlow)
				{
				// no more will go
				// if even the first capability didn't go then we have to forget replying
				if (i==0)
					{
					// there are no caps in the response, so it's meaningless
					ret = KErrUnderflow;		
					}
				// otherwise we can try continuing with usecase though
				// so NOT erroring - cos the message is ok
				break;
				}	
			}
		}

	return ret;			
	}
	
TInt CSignallingSession::ReconfigIndication(TAvdtpTransactionLabel aLabel,
															TSEID aACPSEID,
															RBuf8& aNewConfig)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}
		
	if (!sep->Stream())
		{
		// remote has been bad
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		}

/*
we check to see if the remote has (wrongly) set capabilities that the spec forbids
see 8.10.1 avdtp sepc
if the remote has then we reject the request
else pass onto gavdp
*/
	TAvdtpServiceCategories allowed;
	allowed.SetCapability(EServiceCategoryMediaCodec);
	allowed.SetCapability(EServiceCategoryContentProtection);

	CCapabilityNotAllowedVisitor* checker = new CCapabilityNotAllowedVisitor(allowed);
	
	if (!checker)
		{
		return KErrNoMemory;
		}

	checker->Process(aNewConfig);
		
	if (!checker->IsValid())
		{
		// signalling channel expects us to put the duff category in the aConfig
		// take the first one (as suggested by spec)
		TPckg<TUint8> catBuf(checker->WrongCategory());
		aNewConfig.Copy(catBuf);
		delete checker;
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpInvalidCapabilities);
		}

	delete checker;
	checker = NULL;

//take ownership of config buffer
	TInt res = sep->SetPendingConfiguration(aNewConfig, ETrue);	
	
	if (res==KErrNone)
		{
		// all ok, so pass on an indication to GAVDP as they need to reconfigure too
		TInternalAvdtpConfigurationIndication ind;
		ind.iIndication = EAvdtpReconfigure;
		ind.iPayloadLength = aNewConfig.Length();
		ind.iSEID = aACPSEID;
		// to give consistent API for Set and Re-configure , give the INT SEID again
		ind.iINTSEID = sep->Stream()->RemoteSEID();

		ind.iTransactionLabel = aLabel;

		TPckg<TInternalAvdtpAirIndication> indPckg(ind);
		iSAP.Indication(indPckg);
		}
	
	return res;
	}

/** Handle a Start Indication from remote.

The decision about when to issue the start indication to the client is taken by
the stream.

@param aLabel The transaction label of the start command.
@param aACPSEID The (local) seid referring to the stream to be started.
*/
TInt CSignallingSession::StartIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}

	TInt err = KErrUnknown;
	if (sep->Stream())
		{
		// If the stream is in a valid state it will call back to the sig session
		// to cause the indication to be passed to the client.
		err = sep->Stream()->StartIndication(aLabel, aACPSEID);
		}

	if(err == KErrNone)	
		{
		return KErrNone;
		}
	else
		{
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		}
	}
		
TInt CSignallingSession::ReleaseIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}
		
	if (sep->Stream())
		{
		// dont ask GAVDP to confirm
		// we also know that if we have a stream it must be releaseable
		(void)iSignallingChannel->SendReleaseAccept(aLabel);

		sep->SetInUse(EFalse);

		// We try and hand off the channels to be closed.  This is because it is
		// the responsibility of the INT to release these so we can't just 
		// shut them down straight away.  We don't want to have to deal with the 
		// complication of funny stream states and clearup by leaving them with the
		// stream once it's effectively dead.  If it fails however, we just let the 
		// channels go away.
		// This is only done for non-muxed streams, otherwise we just revert to 
		// closing the channels ourselves.  
#ifndef __SYMBIAN_AVDTP_HIDE_MUX
#warning ("Release ACP channel release must be fixed for muxed case")
#endif
		if(!sep->MultiplexingConfigured())
			{
			// Take the channels from the stream to disconnect
			// Note that although some of these may be the same CloseLogicalChannels will
			// deal with correctly closing each channel once.
			TFixedArray<CDirectChannel*, KAvdtpChannelArraySize> channels;
			channels[0] = static_cast<CDirectChannel*>(sep->Stream()->iMediaBinding.iChannel);
			channels[1] = static_cast<CDirectChannel*>(sep->Stream()->iReportingBinding.iChannel);
			channels[2] = static_cast<CDirectChannel*>(sep->Stream()->iRecoveryBinding.iChannel);
			TArray<CDirectChannel*> array = channels.Array();

			TRAP_IGNORE(iProtocol.LogicalChannelFactory().CloseSessionLogicalChannelsL(array, KAvdtpACPReleaseChannelCloseTimeout));	
			}

		// just error the stream - it will tell the user-plane sessions
		// they'll then tell the user
		// so , like open, we don't let user directly see this as an indication
		DestroyStream(sep->Stream(), KErrDisconnected);
		
		// all ok, so pass on an indication to GAVDP
		TInternalAvdtpAirIndication ind;
		ind.iIndication = EAvdtpRelease;
		ind.iPayloadLength = 0;
		ind.iSEID = aACPSEID;
		ind.iTransactionLabel = aLabel; // not used 

		TPckg<TInternalAvdtpAirIndication> indPckg(ind);
		iSAP.Indication(indPckg);
		
		return KErrNone;
		}
	else
		{
		// remote is in bad state
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		}
	}
	
TInt CSignallingSession::SuspendIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}
		
	// check that we think stream suspendable
	if (sep->Stream() && sep->Stream()->IsStreaming())
		{
		// all ok, so pass on an indication to GAVDP
		TInternalAvdtpAirIndication ind;
		ind.iIndication = EAvdtpSuspend;
		ind.iPayloadLength = 0;
		ind.iSEID = aACPSEID;
		ind.iTransactionLabel = aLabel;

		TPckg<TInternalAvdtpAirIndication> indPckg(ind);
		iSAP.Indication(indPckg);
		return KErrNone;
		}
	else
		{
		return SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		}
	}

TInt CSignallingSession::AbortIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}
	else
		{
		// tell the sep, stream, then the user (only if we have a stream though)
		sep->SetInUse(EFalse);
		
		if (sep->Stream())
			{			
			DestroyStream(sep->Stream(),KErrAbort);		
			}
		// Pass on an indication to GAVDP as they need to know too
		TInternalAvdtpAirIndication ind;
		ind.iIndication = EAvdtpAbort;
		ind.iSEID = aACPSEID;
		ind.iTransactionLabel = aLabel; // not that useful, client cant respond

		TPckg<TInternalAvdtpAirIndication> indPckg(ind);
		iSAP.Indication(indPckg);			
		}
	return KErrNone;			
	}
	
void CSignallingSession::AbortConfirm(TSEID aRemoteSEID)
	{
	LOG_FUNC
	// tell client
	CompleteBasicService(KErrNone);

	// error stream associated with remote
	CAVStream* stream = FindStreamFromRemoteSEID(aRemoteSEID);
	if (stream)
		{
		DestroyStream(stream, KErrAbort);
		}
	}
	
TInt CSignallingSession::SecurityControlIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID, const HBufC8* aSecurityData)
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aACPSEID);
	
	if (!sep)
		{
		// for a SEP not on this session
		return KErrNotFound;
		}
	else
		{
		// pass ownership of HBuf to SEP
		sep->SetSecurityControl(aSecurityData);
				
		// all ok, so pass on an indication to GAVDP as they need to do the actual security control response
		TInternalAvdtpAirIndication ind;
		ind.iIndication = EAvdtpSecurityControl;
		ind.iSEID = aACPSEID;
		ind.iPayloadLength = aSecurityData->Length();
		ind.iTransactionLabel = aLabel;

		TPckg<TInternalAvdtpAirIndication> indPckg(ind);
		iSAP.Indication(indPckg);
		
		return KErrNone;			
		}
	}

void CSignallingSession::SecurityControlConfirm(TInt aResult, TSEID aRemoteSEID, const TDesC8& aResponseData)
 	{
	LOG_FUNC
 	if (aResult==KErrNone)
 		{
 		TPckgBuf<TAvdtpInternalSendSecurityControl> securityControlPckgBuf;
 		securityControlPckgBuf().iRemoteSEID = aRemoteSEID;
 		securityControlPckgBuf().iSecurityControlInfo = aResponseData;
		iSAP.ServiceComplete(&securityControlPckgBuf);
 		}
 	else
 		{
 		iSAP.Error(aResult);
 		}
 	}


void CSignallingSession::DiscoverConfirm(TInt aResult, const TAvdtpInternalDiscoverConfirm* const aConfirm)
	{
	LOG_FUNC
	if (aResult==KErrNone)
		{
		__ASSERT_DEBUG(aConfirm, Panic(EAvdtpSignallingSessionReceivedBadIndication));
		//complete the client request with the number of SEPs
		TPckgBuf<TAvdtpInternalDiscoverConfirm> buf(*aConfirm);
		iSAP.ServiceComplete(&buf);
		}
	else
		{
		//either we were rejected by the peer or there was an internal
		//error in the signalling channel
		iSAP.Error(aResult);
		}
	}


void CSignallingSession::GetCapsConfirm(TInt aResult, TSEID /*aRemoteSEID*/, TAvdtpServiceCatBitMask aSeen)
	{
	LOG_FUNC
	if (aResult==KErrNone)
		{
		TPckgC<TAvdtpServiceCatBitMask> buf(aSeen);
		iSAP.ServiceComplete(&buf);
		}
	else
		{
		iSAP.Error(aResult);
		}
	}

void CSignallingSession::StreamStarted(TSEID aLocalSEID)
	{
	LOG_FUNC
	if (FindLocalSEP(aLocalSEID))
		{
		// now tell client
		CompleteBasicService(KErrNone);
		}
	}

void CSignallingSession::StreamSuspended(TSEID aLocalSEID)
	{
	LOG_FUNC
	if (FindLocalSEP(aLocalSEID))
		{
		// now tell client
		CompleteBasicService(KErrNone);
		}
	}
	
void CSignallingSession::StreamAccepted(TSEID aLocalSEID, TSEID aRemoteSEID, TBool aWithReporting, TBool aWithRecovery)
 	{
	LOG_FUNC
 	__ASSERT_DEBUG(aLocalSEID.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	TInternalAvdtpStreamReadyIndication ind;
	ind.iSEID = aLocalSEID;
	ind.iRemoteSEID = aRemoteSEID;
	ind.iReportingPresent = aWithReporting;
	ind.iRecoveryPresent = aWithRecovery;
 	TPckg<TInternalAvdtpStreamReadyIndication> indPckg(ind);
 	iSAP.Indication(indPckg);
 	}
 	
void CSignallingSession::StreamConfigured(TSEID /*aLocalSEID*/, TInt aResult, TAvdtpServiceCategory aFailedCategory)
	{
	LOG_FUNC
	// tell the user in all cases
	TPckgBuf<TAvdtpConfigReject> rejDataBuf;
	rejDataBuf().iError = aResult;
	rejDataBuf().iFailedCategory = aFailedCategory;
	
	// GAVDP will call back to client with Error upcall, so no need to error SAP
	iSAP.ServiceComplete(&rejDataBuf);
	}

/*
This callback is for the stream to inform gavdp of a failure in its state machine
The signalling session SHALL NOT mutate the stream based on this; the stream should
proceed, or die, or error in line with its present state and potential GAVDP recovery of that stream
*/
void CSignallingSession::StreamInitiatedServiceFailed(const CAVStream& /*aStream*/, TInt aError)
	{
	LOG_FUNC
	ServiceError(aError);
	}

/** 
This should be called from the stream when it is ready for a start indication
to be passed up to the client. 
*/	
void CSignallingSession::StreamReadyForStartIndication(const TAvdtpTransactionLabel aLabel, const TSEID aSeid)
	{
	LOG_FUNC
			
	// pass on an indication to GAVDP
	TInternalAvdtpAirIndication ind;
	ind.iIndication = EAvdtpStart;
	ind.iPayloadLength = 0;
	ind.iSEID = aSeid;
	ind.iTransactionLabel = aLabel;

	TPckg<TInternalAvdtpAirIndication> indPckg(ind);

	iSAP.Indication(indPckg);
	}
	
TInt CSignallingSession::SendAbort(TSEID aRemoteSEID)
	{
	LOG_FUNC
	return iSignallingChannel ? iSignallingChannel->SendAbort(*this, aRemoteSEID) : KErrNotReady;
	}
	
/*inline*/void CSignallingSession::CompleteBasicService(TInt aErr)
	{
	LOG_FUNC
	// basic services have no data to return
	aErr ? iSAP.Error(aErr) : iSAP.ServiceComplete(NULL);
	}
	
	
/**
Called as an INT - create the Stream object when we're setting configuration
*/
void CSignallingSession::DoConfigureStreamL(RBuf8& aConfigBuffer,
												CLocalSEP& aLocalSEP,
												TSEID aRemoteSEID,
												TBool aReportingConfigured,
												TBool aRecoveryConfigured)
	{
	LOG_FUNC
	
	// check to see if SignallingChannel went down during configuration
	if (iSignallingChannel)
		{
		TBool streamCreated = EFalse;
		
		// if there is no stream then we create one now
		// a stream would exist if this is a reconfigure
		CAVStream* stream = FindStreamFromRemoteSEID(aRemoteSEID);
		if (!stream)
			{
			TAvdtpSockAddr addr;
			addr.SetBTAddr(RemoteAddress());
			addr.SetSEID(aRemoteSEID);
			stream = &aLocalSEP.CreateStreamL(addr, *this, iProtocol);
		
			streamCreated = ETrue;
			CleanupStack::PushL(stream);
			}
		else
			{
			__ASSERT_DEBUG(stream->RemoteSEID()==aRemoteSEID, Panic(EAvdtpSignallingSessionFoundWrongStream));
			}

		stream->SetConfigurationL(aConfigBuffer, *iSignallingChannel,
									aReportingConfigured, aRecoveryConfigured);
									
		if (streamCreated)
			{
			// SetConfigurationL ok, pop stream if we created one
			CleanupStack::Pop();
			}
		}
	else
		{
		User::Leave(KErrDisconnected);
		}
	}


void CSignallingSession::CreateLocalSEPL(TAvdtpSEPInfo& aSEPInfo)
	{
	LOG_FUNC
	// create an idle sep into queue for later discovery and selection
	CLocalSEP* idleSEP = CLocalSEP::NewL(aSEPInfo, iProtocol.SEIDManager());
	iLocalSEPs.AddFirst(*idleSEP);
	// return the SEID to GAVDP via the reference paramter
	
	// adjust the service class of CoD, using reference counting of codman
	iProtocol.CodMan().RegisterCodService(aSEPInfo.IsSink() ? EMajorServiceRendering :
															   EMajorServiceCapturing);
	}

CLocalSEP* CSignallingSession::FindLocalSEP(TSEID aSEID) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aSEID.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	CLocalSEP* s = NULL;
	TDblQueIter<CLocalSEP> iter(iLocalSEPs);
	
	while (iter)
		{
		s = iter++;
		if (s->SEID()==aSEID)
			{			
			return s;
			}
		}
	return NULL;
	}
	
	
CLocalSEP& CSignallingSession::FindLocalSEPL(TSEID aSEID) const
	{
	LOG_FUNC
	CLocalSEP* sep = FindLocalSEP(aSEID);
	if (!sep)
		{
		User::Leave(KErrNotFound);
		}
	return *sep;
	}

CAVStream* CSignallingSession::FindStreamFromRemoteSEID(const TSEID& aRemoteSEID) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(!aRemoteSEID.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	TDblQueIter<CLocalSEP> iter(iLocalSEPs);
	
	while (iter)
		{
		CAVStream* s = iter++->Stream();
		if (s && s->RemoteSEID()==aRemoteSEID)
			{			
			return s;
			}
		}
	return NULL;
	}

void CSignallingSession::SignallingChannelError(TInt aError)
	{
	LOG_FUNC
	// set all SEPs to be not in use
	TDblQueIter<CLocalSEP> iter(iLocalSEPs);
	while(iter)
		{
		CLocalSEP* sep = iter++;
			
		sep->SetInUse(EFalse);
		// need to inform sessions - so tell stream
		if (sep->Stream())
			{
			DestroyStream(sep->Stream(),aError);
			}
		}
	// leave the channel
	ClearSignallingChannel();
	// tell the user, on all channels
	iSAP.Error(aError);
	iSAP.SessionDisconnect();
	TPckgBuf<TInternalAvdtpErrorIndication> error;
	error().iError = aError;
	iSAP.Indication(error);
	}

// sigsession has two error paths - one by being a channel user, and the other for protocol errors
void CSignallingSession::ServiceError(TInt aError)
	{
	LOG_FUNC
	iSAP.Error(aError);
	}
	
void CSignallingSession::SignallingChannelReady(CSignallingChannel& aNewChannel)
	{
	LOG_FUNC
	// we may have migrated from old signalling channel to the new instance 
	// so cannot use old signalling channel pointer
	iSignallingChannel = &aNewChannel;
	
	SetRemoteAddress(aNewChannel.RemoteAddress());

	if (iIsListening)
		{
		TInternalAvdtpSignallingReadyIndication ind;
		ind.iBuf = aNewChannel.RemoteAddress().Des();
		
		TPckg<TInternalAvdtpSignallingReadyIndication> indPckg(ind);
		iSAP.Indication(indPckg);
		iIsListening = EFalse;
		}
	else
		{
		iSAP.Ready();
		}
	}

void CSignallingSession::DestroyStream(CAVStream* aStream, TInt aError)
	{
	// tell all sessions of the error
	aStream->NotifyUserPlaneTransportSessionsError(NULL, aError);
	// and remove the stream
	delete aStream;
	}

CCapabilityNotAllowedVisitor::CCapabilityNotAllowedVisitor(TAvdtpServiceCategories aLocallySupported)
: iLocallySupported(aLocallySupported), iIsValid(ETrue)
	{
	LOG_FUNC
	}

TBool CCapabilityNotAllowedVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	// need to put aCat into correct bit position
	TAvdtpServiceCategories cat;
	cat.SetCapability(aCat);
	
	if ((cat() ^ iLocallySupported()) & cat())
		{
		// remote has wrongly used a cap we don't support, mark it as dud
		iWrongCategory = aCat;
		iIsValid = EFalse;
		}
	return iIsValid; // will therefore stop at first broken capability
	}
	
TBool CCapabilityNotAllowedVisitor::IsValid() const
	{
	LOG_FUNC
	return iIsValid;
	}

TAvdtpServiceCategory CCapabilityNotAllowedVisitor::WrongCategory() const
	{
	LOG_FUNC
	return iWrongCategory;
	}

