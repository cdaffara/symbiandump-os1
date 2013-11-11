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



#ifndef __D_MMCSDIF_H__
#define __D_MMCSDIF_H__



inline TInt RMMCSDTestControllerInterface::TestDoControl(TTCFunctionId aFunctionId, TAny *a1, TAny *a2)
	{ return DoControl( aFunctionId.iValue, a1, a2); }

inline void RMMCSDTestControllerInterface::TestDoRequest(TTCFunctionId aFunctionId, TRequestStatus &aStatus, TAny *a1, TAny *a2)
	{ DoRequest( aFunctionId.iValue, aStatus, a1, a2); }

//DoRequests
inline void RMMCSDTestControllerInterface::ReadSector(TRequestStatus &aStatus, TInt aSectNum, TDes8 &aDes)
	{ TestDoRequest(TTCFunctionId(EReadSector, EMmcGeneralTest), aStatus, (TAny*)aSectNum, &aDes);}

inline void RMMCSDTestControllerInterface::WriteSector(TRequestStatus &aStatus,TInt aSectNum,const TDesC8 &aDes)
	{ TestDoRequest(TTCFunctionId(EWriteSector, EMmcGeneralTest), aStatus, (TAny*)aSectNum, (TAny*)&aDes);}

//General
inline TInt RMMCSDTestControllerInterface::ReadCSD(TCSDInfo& aCSDInfo)
	{return(TestDoControl(TTCFunctionId(EReadCSD, EMmcGeneralTest), &aCSDInfo));}

inline TInt RMMCSDTestControllerInterface::ReadExtCSD(TExtendedCSDInfo& aExtCSDInfo)
	{return(TestDoControl(TTCFunctionId(EReadExtCSD, EMmcGeneralTest), &aExtCSDInfo));}

inline TInt RMMCSDTestControllerInterface::CardInfo(TMMCCardInfo& aCardInfo)
	{return(TestDoControl(TTCFunctionId(ECardInfo, EMmcGeneralTest), &aCardInfo));}

inline TInt RMMCSDTestControllerInterface::Card(TAny* aCard)
	{return(TestDoControl(TTCFunctionId(ECard, EMmcGeneralTest), aCard));}

inline TInt RMMCSDTestControllerInterface::CardIsPresent(TBool& aIsPresent)
	{return(TestDoControl(TTCFunctionId(ECardIsPresent, EMmcGeneralTest), &aIsPresent));}

inline TInt RMMCSDTestControllerInterface::CardIsReady(TBool& aIsReady)
	{return(TestDoControl(TTCFunctionId(ECardIsReady, EMmcGeneralTest), &aIsReady));}

inline TInt RMMCSDTestControllerInterface::CardIsLocked(TBool& aIsLocked)
	{return(TestDoControl(TTCFunctionId(ECardIsLocked, EMmcGeneralTest), &aIsLocked));}


//Open Channel
inline TInt RMMCSDTestControllerInterface::Open(TInt aStack,const TVersion& aVer)
		{return(DoCreate(_L("MMCSDTestCtrlInf"),aVer,(TInt)aStack,NULL,NULL));}

inline TVersion RMMCSDTestControllerInterface::VersionRequired() const
	{return(TVersion(EMajorVersionNumber,EMinorVersionNumber,EBuildVersionNumber));}

//DMMCPsu
inline TInt RMMCSDTestControllerInterface::Psu(TAny* aPsu)
	{return(TestDoControl(TTCFunctionId(EPsu, EMmcTestPsu), aPsu));}

inline TInt RMMCSDTestControllerInterface::PsuInfo(TPBusPsuInf& anInfo)
	{return(TestDoControl(TTCFunctionId(EPsuInfo, EMmcTestPsu), &anInfo));}

inline TInt RMMCSDTestControllerInterface::SetPsuState(TInt& aState)
	{return(TestDoControl(TTCFunctionId(EPsuState, EMmcTestPsu), (TAny*)aState));}

inline TInt RMMCSDTestControllerInterface::SetPsuVoltage(TUint& aVol)
	{return(TestDoControl(TTCFunctionId(EPsuVoltage, EMmcTestPsu), (TAny*)aVol));}

inline TInt RMMCSDTestControllerInterface::PsuNumber(TInt& aPsuNum)
	{return(TestDoControl(TTCFunctionId(EPsuNum, EMmcTestPsu), &aPsuNum));}

inline TInt RMMCSDTestControllerInterface::MediaChangeID(TInt& aMCId)
	{return(TestDoControl(TTCFunctionId(EMediaChangeNum, EMmcTestPsu), &aMCId));}

inline TInt RMMCSDTestControllerInterface::PsuCurLimit(TBool& aSafe)
	{return(TestDoControl(TTCFunctionId(EPsuCurLimit, EMmcTestPsu), &aSafe));}

inline TInt RMMCSDTestControllerInterface::PbusState(TInt& aState)
	{return(TestDoControl(TTCFunctionId(EPbusState,EMmcTestPsu),&aState));}

inline TInt RMMCSDTestControllerInterface::VccState(TInt& aVccState)
	{return(TestDoControl(TTCFunctionId(EVccState,EMmcTestPsu),&aVccState));}

inline TInt RMMCSDTestControllerInterface::IsLocked(TBool& aState)
	{return(TestDoControl(TTCFunctionId(EIsLocked,EMmcTestPsu),&aState));}

inline TInt RMMCSDTestControllerInterface::IsOff(TBool& aState)
	{return(TestDoControl(TTCFunctionId(EIsOff,EMmcTestPsu),&aState));}

inline TInt RMMCSDTestControllerInterface::MaxCurrentInMicroAmps(TInt& aMaxCurrent)
	{return(TestDoControl(TTCFunctionId(EMaxCurrentInMicroAmps,EMmcTestPsu),&aMaxCurrent));}

inline TInt RMMCSDTestControllerInterface::ResetInactivityTimer()
	{return(TestDoControl(TTCFunctionId(EResetInactivityTimer,EMmcTestPsu),NULL));}

inline TInt RMMCSDTestControllerInterface::VoltageSupported(TUint& aVoltage)
	{return(TestDoControl(TTCFunctionId(EVoltageSupported,EMmcTestPsu),&aVoltage));}

inline TInt RMMCSDTestControllerInterface::InactivityCount(TInt& aInactivityCount)
	{return(TestDoControl(TTCFunctionId(EMemInactivityCount,EMmcTestPsu),&aInactivityCount));}

inline TInt RMMCSDTestControllerInterface::NotLockedCount(TInt& aNotLockedCount)
	{return(TestDoControl(TTCFunctionId(EMemNotLockedCount,EMmcTestPsu),&aNotLockedCount));}

inline TInt RMMCSDTestControllerInterface::InactivityTimeout(TInt& aInactivityTimeout)
	{return(TestDoControl(TTCFunctionId(EMemInactivityTimeout,EMmcTestPsu),&aInactivityTimeout));}

inline TInt RMMCSDTestControllerInterface::NotLockedTimeout(TInt& aNotLockedTimeout)
	{return(TestDoControl(TTCFunctionId(EMemNotLockedTimeout,EMmcTestPsu),&aNotLockedTimeout));}

inline TInt RMMCSDTestControllerInterface::MemVoltageSupported(TUint& aVoltage)
	{return(TestDoControl(TTCFunctionId(EMemVoltageSupported,EMmcTestPsu),&aVoltage));}

inline TInt RMMCSDTestControllerInterface::MemMaxCurrentInMicroAmps(TInt& aMaxCurrent)
	{return(TestDoControl(TTCFunctionId(EMemMaxCurrentInMicroAmps,EMmcTestPsu),&aMaxCurrent));}

inline TInt RMMCSDTestControllerInterface::VoltCheckInterval(TUint& aVoltCheckInterval)
	{return(TestDoControl(TTCFunctionId(EMemVoltCheckInterval,EMmcTestPsu),&aVoltCheckInterval));}

//DMMCMediaChange
inline TInt RMMCSDTestControllerInterface::MediaChange(TAny* aMediaChange)
	{return(TestDoControl(TTCFunctionId(EMediaChange, EMmcTestMediaChange), aMediaChange));}

inline TInt RMMCSDTestControllerInterface::MediaStateInfo(TInt& aMediaState)
	{return(TestDoControl(TTCFunctionId(EMediaStateInfo, EMmcTestMediaChange), &aMediaState));}

inline TInt RMMCSDTestControllerInterface::ForceMediaChange()
	{return(TestDoControl(TTCFunctionId(EForceMediaChange, EMmcTestMediaChange), NULL));}

inline TInt RMMCSDTestControllerInterface::MediaChangeCounter(TInt& aCounter)
	{return(TestDoControl(TTCFunctionId(EMediaChangeCounter, EMmcTestMediaChange), &aCounter));}

// DMMCSocket
inline TInt RMMCSDTestControllerInterface::Socket(TAny* aSocket)
	{return(TestDoControl(TTCFunctionId(ESocket, EMmcTestSocket), aSocket));}

inline TInt RMMCSDTestControllerInterface::SocketAdjustPartialRead(TPartialReadData& aPartialReadData)
	{return(TestDoControl(TTCFunctionId(ESocketAdjustPartialRead, EMmcTestSocket), &aPartialReadData));}

inline TInt RMMCSDTestControllerInterface::SocketCardIsPresent(TInt& aCardPresent)
	{return(TestDoControl(TTCFunctionId(ESocketCardIsPresent, EMmcTestSocket), &aCardPresent));}

inline TInt RMMCSDTestControllerInterface::SocketGetBufferInfo(TBufferInfo& aBufferInfo)
	{return(TestDoControl(TTCFunctionId(ESocketGetBufferInfo, EMmcTestSocket), &aBufferInfo));}

inline TInt RMMCSDTestControllerInterface::SocketInit()
	{return(TestDoControl(TTCFunctionId(ESocketInit, EMmcTestSocket), NULL ));}

inline TInt RMMCSDTestControllerInterface::SocketInitiatePowerUpSequence()
	{return(TestDoControl(TTCFunctionId(ESocketInitiatePowerUpSequence, EMmcTestSocket), NULL));}

inline TInt RMMCSDTestControllerInterface::SocketMachineInfo(TMachineInfo& aMachineInfo)
	{return(TestDoControl(TTCFunctionId(ESocketMachineInfo, EMmcTestSocket), &aMachineInfo));}

inline TInt RMMCSDTestControllerInterface::SocketPrepareStore(TPasswordPrepareStoreData& aPwdData)
	{return(TestDoControl(TTCFunctionId(ESocketPrepareStore,  EMmcTestSocket),&aPwdData));}

inline TInt RMMCSDTestControllerInterface::SocketReset1()
	{return(TestDoControl(TTCFunctionId(ESocketReset1, EMmcTestSocket), NULL ));}

inline TInt RMMCSDTestControllerInterface::SocketReset2()
	{return(TestDoControl(TTCFunctionId(ESocketReset2, EMmcTestSocket), NULL));}

inline TInt RMMCSDTestControllerInterface::SocketResetInactivity(TInt aBusNumber)
	{return(TestDoControl(TTCFunctionId(ESocketResetInactivity, EMmcTestSocket), &aBusNumber));}

inline TInt RMMCSDTestControllerInterface::SocketStack(TStackPtr& aStackPtr)
	{return(TestDoControl(TTCFunctionId(ESocketStack, EMmcTestSocket), &aStackPtr));}

inline TInt RMMCSDTestControllerInterface::SocketiState_Get(TBusState& aState)
	{return(TestDoControl(TTCFunctionId(ESocketiState_Get, EMmcTestSocket), &aState));}

inline TInt RMMCSDTestControllerInterface::SocketiDoorOpened_Get(TBool& aDoorOpened)
	{return(TestDoControl(TTCFunctionId(ESocketiDoorOpened_Get, EMmcTestSocket), &aDoorOpened));}

inline TInt RMMCSDTestControllerInterface::SocketiStandby_Get(TBool& aStandby)
	{return(TestDoControl(TTCFunctionId(ESocketiStandby_Get, EMmcTestSocket), &aStandby));}

inline TInt RMMCSDTestControllerInterface::SocketiType_Get(TCardBusType& aType)
	{return(TestDoControl(TTCFunctionId(ESocketiType_Get, EMmcTestSocket), &aType));}

inline void RMMCSDTestControllerInterface::SocketPowerUp(TRequestStatus& aStatus)
	{ TestDoRequest(TTCFunctionId(ESocketPowerUp, EMmcTestSocket), aStatus); }

// DMMCStack
inline TInt RMMCSDTestControllerInterface::Stack(TAny* aStack)
	{ return(TestDoControl(TTCFunctionId(EStack, EMmcTestStack), aStack));}

inline TInt RMMCSDTestControllerInterface::StackAdjustPartialRead(TPartialReadData& aPartialReadData)
	{ return(TestDoControl(TTCFunctionId(EStackAdjustPartialRead, EMmcTestStack), &aPartialReadData));}

inline TInt RMMCSDTestControllerInterface::StackAllocSession()
	{ return(TestDoControl(TTCFunctionId(EStackAllocSession, EMmcTestStack), NULL));}

inline TInt RMMCSDTestControllerInterface::StackBufferInfo(TBufferInfo& aBufferInfo)
	{ return(TestDoControl(TTCFunctionId(EStackBufferInfo, EMmcTestStack), &aBufferInfo));}

inline TInt RMMCSDTestControllerInterface::StackGetBufferInfo(TBufferInfo& aBufferInfo)
	{ return(TestDoControl(TTCFunctionId(EStackGetBufferInfo, EMmcTestStack), &aBufferInfo));}

inline TInt RMMCSDTestControllerInterface::StackCardP(TCardPtr& aCardPtr)
	{ return(TestDoControl( TTCFunctionId(EStackCardP, EMmcTestStack), &aCardPtr));}

inline TInt RMMCSDTestControllerInterface::StackStop()
	{ return(TestDoControl(TTCFunctionId(EStackStop, EMmcTestStack), NULL));}

inline TInt RMMCSDTestControllerInterface::StackMaxCardsInStack(TUint& aNumCardsInStack)
	{ return(TestDoControl(TTCFunctionId(EStackMaxCardsInStack, EMmcTestStack), &aNumCardsInStack));}

inline TInt RMMCSDTestControllerInterface::StackMMCSocket(TAny* aAny)
	{ return(TestDoControl(TTCFunctionId(EStackMMCSocket, EMmcTestStack), aAny));}

inline TInt RMMCSDTestControllerInterface::StackReadPasswordStore(TStackPasswordStoreData& aPwdStoreData)
	{ return(TestDoControl(TTCFunctionId(EStackReadPasswordStore, EMmcTestStack), &aPwdStoreData));}

inline TInt RMMCSDTestControllerInterface::StackWritePasswordStore(TStackPasswordStoreData& aPwdStoreData)
	{ return(TestDoControl(TTCFunctionId(EStackWritePasswordStore, EMmcTestStack), &aPwdStoreData));}

inline TInt RMMCSDTestControllerInterface::StackProgramPeriodInMilliSeconds(TInt& aPeriodMs)
	{ return(TestDoControl(TTCFunctionId(EStackProgramPeriodInMs, EMmcTestStack), &aPeriodMs));}

inline TInt RMMCSDTestControllerInterface::StackInitStackInProgress(TBool& aInProgress)
	{ return(TestDoControl(TTCFunctionId(EStackInitStackInProgress, EMmcTestStack), &aInProgress));}

inline TInt RMMCSDTestControllerInterface::StackHasSessionsQueued(TBool& aSessionsQueued)
	{ return(TestDoControl(TTCFunctionId(EStackHasSessionsQueued, EMmcTestStack), &aSessionsQueued));}

inline TInt RMMCSDTestControllerInterface::StackHasCardsPresent(TBool& aCardsPresent)
	{ return(TestDoControl(TTCFunctionId(EStackHasCardsPresent, EMmcTestStack), &aCardsPresent));}

inline TInt RMMCSDTestControllerInterface::StackMachineInfo(TMachineInfo& aMachineInfo)
	{ return(TestDoControl(TTCFunctionId(EStackMachineInfo, EMmcTestStack), &aMachineInfo));}

inline TInt RMMCSDTestControllerInterface::StackDemandPagingInfo(TTCDemandPagingInfo& aInfo)
	{ return TestDoControl(TTCFunctionId(EStackDemandPagingInfo, EMmcTestStack), &aInfo); }

inline TInt RMMCSDTestControllerInterface::StackEffectiveModes(TUint32& aModes)
	{ return TestDoControl(TTCFunctionId(EStackEffectiveModes, EMmcTestStack), &aModes);}

inline TInt RMMCSDTestControllerInterface::StackInit()
	{ return TestDoControl(TTCFunctionId(EStackInit, EMmcTestStack));}

inline TInt RMMCSDTestControllerInterface::StackReportPowerUp()
	{ return TestDoControl(TTCFunctionId(EStackReportPowerUp, EMmcTestStack));}

inline TInt RMMCSDTestControllerInterface::StackReportPowerDown()
	{ return TestDoControl(TTCFunctionId(EStackReportPowerDown, EMmcTestStack));}

inline TInt RMMCSDTestControllerInterface::StackAlterEffectiveModes(TEffectiveModesFunctionId aFunctionId, TAny* aData)
	{
	return TestDoControl(TTCFunctionId(EStackConfigureEffectiveModes, EMmcTestStack), (TAny*)aFunctionId, aData); 
	}

#endif //__D_MMCSDIF_H__
