// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(D_COMM_H)
#define D_COMM_H

#include "DParams.h"

//
const TInt KCommsMajorVersionNumber=1;
const TInt KCommsMinorVersionNumber=0;
const TInt KCommsBuildVersionNumber=KE32BuildVersionNumber;
//
const TInt KDefaultRxBufferSize=0x400;
const TInt KTxBufferSize=0x400;
const TInt KMaxHighWaterMark=0x080;
//
const TUint KReceiveIsrParityError=0x10000000;
const TUint KReceiveIsrFrameError=0x20000000;
const TUint KReceiveIsrOverrunError=0x40000000;
const TUint KReceiveIsrMaskError=0x70000000;
//
const TInt KTransmitIrqEmpty=KErrGeneral;
//
const TUint KReceiveIsrTermChar=0x80000000;
const TUint KReceiveIsrMaskComplete=0xf0000000;
const TUint KReceiveIsrShift=24;
const TUint KReceiveIsrShiftedMask=0x0f;
//
//
enum TStopMode {EStopNormal,EStopPwrDown,EStopEmergency};
//
//

#ifdef __MARM__
const TInt KDummyTimerGranularity = 31000;
#else
const TInt KDummyTimerGranularity = 80000;
#endif

const TInt KDummyTimeOut=200000;	// 0.2 seconds


class DChannelComm;
class DComm : public CBase
	{
public:
	virtual TInt Start() =0;
	virtual void Stop(TStopMode aMode) =0;
	virtual void Break(TBool aState) =0;
	virtual void EnableTransmit() =0;
	virtual TUint Signals() const =0;
	virtual void SetSignals(TUint aSetMask,TUint aClearMask) =0;
	virtual TInt Validate(const TCommConfigV01 &aConfig) const =0;
	virtual void Configure(TCommConfigV01 &aConfig) =0;
	virtual void Caps(TDes8 &aCaps) const =0;
	virtual void CheckConfig(TCommConfigV01& aConfig)=0;
	virtual void EnableInterrupts() =0;
	virtual void DisableInterrupts() =0;
	virtual TInt CompleteSlowOpen(DThread *aThread,TRequestStatus *aReqStat) =0;
	inline DChannelComm &Comm() {return(*iComm);}
	inline void SetComm(DChannelComm *aComm) {iComm=aComm;}
private:
	DChannelComm *iComm;
#ifdef _DEBUG_DEVCOMM
public:
	TInt iRxIntCount;
	TInt iTxIntCount;
	TInt iRxErrCount;
	TInt iTxErrCount;
#endif
	};

//
class DDeviceComm : public DLogicalDevice
	{
public:
	DDeviceComm();
	virtual TInt Install();
	virtual void GetCaps(TDes8 &aDes) const;
	virtual DLogicalChannel *CreateL();
	};
//

class DChannelComm : public DLogicalChannel
	{
public:
	DChannelComm(DLogicalDevice *aDevice);
	~DChannelComm();
	void CompleteReq(TInt aReqNo);
	void SetSignals();
	void ChangeConfig();
protected:
	virtual void DoCreateL(TInt aUnit,CBase *aDriver,const TDesC *anInfo,const TVersion &aVer);
	virtual void DoCancel(TInt aReqNo);
	virtual void DoRequest(TInt aReqNo,TAny *a1,TAny *a2);
	virtual TInt DoControl(TInt aFunction,TAny *a1,TAny *a2);
private:
	static void SignalChangeCompleted(TAny*,TInt);
	static void ConfigChangeCompleted(TAny*,TInt);
	static void RxDataAvailableCompleted(TAny*,TInt);
	static void FlowControlChangeCompleted(TAny*,TInt);
	static void WriteCompleted(TAny*,TInt);
public:
	TCommNotificationV01 iConfig;
private:
	DComm *iDriver;

	TTickLink iQueuedRxDataAvailable;
	TTickLink iQueuedSignalNotify;
	TTickLink iQueuedConfigChange;
	TTickLink iQueuedFlowControl;
	TTickLink iQueuedWrite;

	TUint* iSignals;	
	TUint iSignalMask;
	TFlowControl iFlowControl;
	TCommNotificationPckg* iConfigPointer;

	TBool iRxDataAvailablePending;
	TBool iSignalNotifyPending;
	TBool iConfigChangePending;
	TBool iFlowControlChangePending;
	TBool iWritePending;

	TInt iSignalCount;
	
#ifdef _DEBUG_DEVCOMM
	TInt iDoDrainSeq;
	TInt iTxChars, iRxChars;
	TInt iTxXon, iTxXoff, iRxXon, iRxXoff;
#endif
	};

#endif

