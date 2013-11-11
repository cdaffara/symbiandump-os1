/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include <kernel.h>
#include <mmc.h>
#include <locmedia.h>

#include "mmcldd.h"
#include "d_mmcsdif.h"

/**
 * Do a kernel side control for DMMCStack
 *
 * @param TInt aFunction - Request Function ID
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::StackDoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	MMCSDLOG3(
			"DLddMmcCntrlInterface::StackDoControl aFunction(%d) \
			TTCFunctionId(aFunction).FunctionId()(%d)",
			aFunction, TTCFunctionId(aFunction).FunctionId());
	TInt ret = KErrNone;
	
	if (!iStack)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::StackDoControl - iStack is NULL");		
		return KErrGeneral;
		}
	else if (!iClient)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::StackDoControl - iClient is NULL");		
		return KErrGeneral;
		}

	switch (TTCFunctionId(aFunction).FunctionId())
		{
		case RMMCSDTestControllerInterface::EStack:
			{
			MMCSDLOG("case RMMCSDTestControllerInterface::EStack");
			ret = Kern::ThreadRawWrite(iClient, a1, &iStack, sizeof(iStack));
			}
			break;
		case RMMCSDTestControllerInterface::EStackAdjustPartialRead:
			{
			if (!iCard)
				{
				MMCSDLOG("RMMCSDTestControllerInterface::EStackAdjustPartialRead NULL card\n");
				return KErrNotReady;
				}

			TPartialReadData partialReadData;
			Kern::ThreadRawRead(iClient, a1, &partialReadData,
					sizeof(partialReadData));
			iStack->AdjustPartialRead(iCard, partialReadData.iStart,
					partialReadData.iEnd,
					(TUint32*)&(partialReadData.iPhysStart),
					(TUint32*)&(partialReadData.iPhysEnd));
			ret = Kern::ThreadRawWrite(iClient, a1, &partialReadData,
					sizeof(partialReadData));
			}
			break;
		case RMMCSDTestControllerInterface::EStackAllocSession:
			{
			// just a simple test to check p1 call of DMMCStack::AllocSession
			// the session is tested further within the DLddAsyncRequest class
			// which each uses their own session (allocated from DMMCStack::AllocSession)			
			MMCSDLOG("pre AllocSession");
			DMMCSession* session = iStack->AllocSession(TMMCCallBack());
			MMCSDLOG2("post AllocSession session = %x", session);
			delete session;
			}
			break;
		case RMMCSDTestControllerInterface::EStackBufferInfo:
			{
			MMCSDLOG("RMMCSDTestControllerInterface::EStackBufferInfo");
			TBufferInfo bufInfo;
			iStack->BufferInfo(bufInfo.iBuf, bufInfo.iBufLen,
					bufInfo.iMinorBufLen);
			Kern::ThreadRawWrite(iClient, a1, &bufInfo, sizeof(bufInfo));
			}
			break;
		case RMMCSDTestControllerInterface::EStackGetBufferInfo:
			{
			MMCSDLOG("RMMCSDTestControllerInterface::EStackGetBufferInfo");
			TBufferInfo bufInfo;
			iStack->GetBufferInfo(&bufInfo.iBuf, &bufInfo.iBufLen);
			Kern::ThreadRawWrite(iClient, a1, &bufInfo, sizeof(bufInfo));
			}
			break;

		case RMMCSDTestControllerInterface::EStackCardP:
			{
     		TCardPtr cardPtr;     		
      		ret = Kern::ThreadRawRead(iClient, a1, &cardPtr, sizeof(cardPtr));	 
			if( ret == KErrNone )
				{
		  		MMCSDLOG("Calling RMMCSDTestControllerInterface::EStackCardP");
		  		MMCSDLOG2("cardNumber=%d", cardPtr.iCardNumber);
		  		iCard = iStack->CardP(cardPtr.iCardNumber);
		  		MMCSDLOG2("cardPtr=%x", iCard);
		  		cardPtr.iCardPtr = iCard;
				ret = Kern::ThreadRawWrite(iClient, a1, &cardPtr, sizeof(cardPtr));
				}
			}
			break;

		case RMMCSDTestControllerInterface::EStackDemandPagingInfo:
			{
			DMMCStack::TDemandPagingInfo dpInfo;
			ret = iStack->DemandPagingInfo(dpInfo);

			if (ret == KErrNone)
				{
				TTCDemandPagingInfo info;
				info.iDriveCount = dpInfo.iDriveCount;
				info.iNumPages = dpInfo.iNumPages;
				info.iPagingDriveList = dpInfo.iPagingDriveList;
				info.iPagingType = dpInfo.iPagingType;
				info.iReadShift = dpInfo.iReadShift;
				info.iWriteProtected = dpInfo.iWriteProtected;
				ret = Kern::ThreadRawWrite(iClient, a1, &info, sizeof(info));
				}
			else
				{
				MMCSDLOG2(
						"RMMCSDTestControllerInterface::EStackDemandPagingInfo err(%d)",
						ret);
				}
			}
			break;

		case RMMCSDTestControllerInterface::EStackEffectiveModes:
			{
			TUint32 rval = iStack->EffectiveModes(iStackConfig);
			ret = Kern::ThreadRawWrite(iClient, a1, &rval, sizeof(rval));
			}
			break;

		case RMMCSDTestControllerInterface::EStackHasCardsPresent:
			{
			TBool cardsPresent = iStack->HasCardsPresent();
			ret = Kern::ThreadRawWrite(iClient, a1, &cardsPresent,
					sizeof(cardsPresent));
			}
			break;
		case RMMCSDTestControllerInterface::EStackHasSessionsQueued:
			{
			TBool sessionsQueued = iStack->HasSessionsQueued();
			ret = Kern::ThreadRawWrite(iClient, a1, &sessionsQueued,
					sizeof(sessionsQueued));
			}
			break;
		case RMMCSDTestControllerInterface::EStackInit:
			ret = iStack->Init();
			break;
		case RMMCSDTestControllerInterface::EStackInitStackInProgress:
			{
			TBool inProgress = iStack->InitStackInProgress();
			ret = Kern::ThreadRawWrite(iClient, a1, &inProgress,
					sizeof(inProgress));
			}
			break;
		case RMMCSDTestControllerInterface::EStackMMCSocket:
			{
			MMCSDLOG("case RMMCSDTestControllerInterface::EStackMMCSocket");
			DMMCSocket* socketPtr = iStack->MMCSocket();
			MMCSDLOG3("Socket Ptr (%x)   &socketPtr(%x)", socketPtr,
					&socketPtr);
			ret = Kern::ThreadRawWrite(iClient, a1, &socketPtr,
					sizeof(socketPtr));
			}
			break;
		case RMMCSDTestControllerInterface::EStackMachineInfo:
			{
			TMMCMachineInfo mi;
			iStack->MachineInfo(mi);
			TMachineInfo machineInfo(mi);
			ret = Kern::ThreadRawWrite(iClient, a1, &machineInfo, sizeof(machineInfo));
			}
			break;
		case RMMCSDTestControllerInterface::EStackMaxCardsInStack:
			{
			TUint maxCards = iStack->MaxCardsInStack();
			ret = Kern::ThreadRawWrite(iClient, a1, &maxCards, sizeof(maxCards));
			}
			break;
		case RMMCSDTestControllerInterface::EStackReadPasswordStore:
			{
			TMMCPasswordStore* pps = iStack->PasswordStore();
			TStackPasswordStoreData pwdStoreData;
			pwdStoreData.iPwdStorePtr = pps;
			if (pps)
				{
				TBuf8<KPasswordStoreCIDLength + KMmcPwdLen> formattedPassword;
				ret = pps->ReadPasswordData(formattedPassword);
				MMCSDLOG2("RMMCSDTestControllerInterface::EStackReadPasswordStore formattedPassword=%S", &formattedPassword);
				
				if (ret == KErrNone)
					{
					pwdStoreData.SetFormattedPassword(formattedPassword);
					ret = Kern::ThreadRawWrite(iClient, a1, &pwdStoreData, sizeof(pwdStoreData));
					}
				}
			else
				{
				MMCSDLOG("RMMCSDTestControllerInterface::EStackReadPasswordStore invalid pps");
				ret = KErrGeneral;
				}

			MMCSDLOG3("RMMCSDTestControllerInterface::EStackReadPasswordStore #1 ret(%d) pwdStoreData.iPassword(%S)",
						ret, &pwdStoreData.iPassword);
			}
			break;
		case RMMCSDTestControllerInterface::EStackWritePasswordStore:
			{
			TStackPasswordStoreData pwdStoreData;
			ret = Kern::ThreadRawRead(iClient, a1, &pwdStoreData, sizeof(pwdStoreData));
			TMMCPasswordStore* pps = iStack->PasswordStore();
			pwdStoreData.iPwdStorePtr = pps;
			if (pps)
				{
				TBuf8<KPasswordStoreCIDLength + KMmcPwdLen> formattedPassword;
				if (pwdStoreData.CreateFormattedPassword(formattedPassword, iCard->CID()))
					{
					MMCSDLOG2("RMMCSDTestControllerInterface::EStackWritePasswordStore formattedPassword=%S", &formattedPassword);
					ret = pps->WritePasswordData(formattedPassword);				
					}
				else
					{
					MMCSDLOG("RMMCSDTestControllerInterface::EStackWritePasswordStore - CreateFormattedPassword() failed");
					ret = KErrGeneral;
					}
				}
			else
				{
				MMCSDLOG("RMMCSDTestControllerInterface::EStackWritePasswordStore invalid pps");
				ret = KErrGeneral;
				}
			MMCSDLOG3(
					"RMMCSDTestControllerInterface::EStackWritePasswordStore ret(%d) pwdStoreData.iPassword(%S)",
					ret, &pwdStoreData.iPassword);
			}
			break;
			
		case RMMCSDTestControllerInterface::EStackProgramPeriodInMs:
			{
			TInt periodMs = iStack->ProgramPeriodInMilliSeconds();
			ret
					= Kern::ThreadRawWrite(iClient, a1, &periodMs,
							sizeof(periodMs));
			}
			break;

		case RMMCSDTestControllerInterface::EStackStop:
			{
			if (!iCard)
				{
				MMCSDLOG("RMMCSDTestControllerInterface::EStackStop NULL card\n");
				return KErrNotReady;
				}

			ret = iStack->Stop(iCard);
			}
			break;

		case RMMCSDTestControllerInterface::EStackReportPowerUp:
			iStack->ReportPowerUp();
			break;
			
		case RMMCSDTestControllerInterface::EStackReportPowerDown:
			iStack->ReportPowerDown();
			break;
			
		case RMMCSDTestControllerInterface::EStackConfigureEffectiveModes:
			if (!a1)
				{
				return KErrArgument;
				}
			ret = StackConfigureEffectiveModes((TInt) a1, a2);
			break;

		default:
			ret = KErrNotFound;
			break;
		}
	return ret;
	}

/**
 * Configure the Effectivemodes parameter for all TMMCStackConfig functions
 *
 * @param TInt aEffectiveModesFunctionId - Effective Modes Function ID
 * @param TAny* aParam - Given Effective Modes Function parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::StackConfigureEffectiveModes(
		TInt aEffectiveModesFunctionId, TAny* aParam)
	{
	// used to handle requests to configure the iStackConfig (Effectivemodes)
	// parameter for all TMMCStackConfig functions is max 32 bit value so passes in as value aParam

	TInt r = KErrNone; // all functions return void	
	switch ((RMMCSDTestControllerInterface::TEffectiveModesFunctionId) aEffectiveModesFunctionId)
		{
		case RMMCSDTestControllerInterface::EStackEffectiveModesRemoveMode:
			iStackConfig.RemoveMode((TUint32)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetBusClockInKhz:
			iStackConfig.SetBusClockInKhz((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetBusyTimeOutInMcs:
			iStackConfig.SetBusyTimeOutInMcs((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetCRCRetries:
			iStackConfig.SetCRCRetries((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetDataTimeOutInMcs:
			iStackConfig.SetDataTimeOutInMcs((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetMode:
			iStackConfig.SetMode((TUint32)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetOpCondBusyTimeout:
			iStackConfig.SetOpCondBusyTimeout((TUint16)((TInt)aParam));
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetPollAttempts:
			iStackConfig.SetPollAttempts((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetResponseTimeOutInTicks:
			iStackConfig.SetResponseTimeOutInTicks((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetTicksClockIn:
			iStackConfig.SetTicksClockIn((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetTicksClockOut:
			iStackConfig.SetTicksClockOut((TUint)aParam);
			break;
		case RMMCSDTestControllerInterface::EStackEffectiveModesSetTimeOutRetries:
			iStackConfig.SetTimeOutRetries((TUint)aParam);
			break;
		default:
			r = KErrNotFound;
			break;
		};
	return r;
	}

