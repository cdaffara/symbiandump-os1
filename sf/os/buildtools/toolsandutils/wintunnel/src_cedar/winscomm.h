// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins/specific/winscomm.h
// 
//

#ifndef __M32COMM_H__
#define __M32COMM_H__
#include <platform.h>
//#include <kpower.h>
#include <d32comm.h>
#include <e32ver.h>
//
const TInt KCommsMajorVersionNumber=1;
const TInt KCommsMinorVersionNumber=1;
const TInt KCommsBuildVersionNumber=KE32BuildVersionNumber;

//
enum TStopMode {EStopNormal,EStopPwrDown,EStopEmergency};
//
//
class DChannelComm;
class DComm : public DBase
	{
public:
	virtual TInt Start() =0;
	virtual void Stop(TStopMode aMode) =0;
	virtual void Break(TBool aState) =0;
	virtual void Write(DThread*,TAny*,TInt) = 0;
	virtual void Read(DThread*,TAny*,TInt) = 0;
	virtual void NotifySignals(DThread*, TInt) = 0;
	virtual void NotifyDataAvailable() = 0;
	virtual TUint Signals() const =0;
	virtual void SetSignals(TUint aSetMask,TUint aClearMask) =0;
	virtual TInt ValidateConfig(const TCommConfigV01 &aConfig) const =0;
	virtual void Configure(TCommConfigV01 &aConfig) =0;
	virtual void Caps(TDes8 &aCaps) const =0;
	virtual void CheckConfig(TCommConfigV01& aConfig)=0;
	virtual TDfcQue* DfcQ(TInt aUnit)=0;
	inline void SetCurrent(TInt aCurrent);
	inline TBool Transmitting();
	virtual TInt RxCount() =0;
	virtual void ResetBuffers(TBool) = 0;
	virtual TInt SetRxBufferSize(TInt aSize) = 0;
	virtual TInt RxBufferSize() =0;
	virtual TDes8* RxBuffer() =0;
	virtual TBool AreAnyPending() = 0;
	virtual void ReadCancel() = 0;
	virtual void WriteCancel() = 0;
	virtual void SignalChangeCancel() = 0;

public:
	DChannelComm *iLdd;
	TBool iTransmitting;
	TUint8 * iInBufPtr;
	TUint8 * iOutBufPtr;

	};


class DDeviceComm : public DLogicalDevice
	{
public:
	DDeviceComm();
	virtual TInt Install();
	virtual void GetCaps(TDes8 &aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};

//class DCommPowerHandler;
class DChannelComm : public DLogicalChannel
	{
public:
	enum TState {EOpen,EActive,EClosed};
	enum TRequest {ERx=1, ETx=2, ESigChg=4, ERxDA=8, EAll=0xff};

	DChannelComm();
	~DChannelComm();
	inline void SetStatus(TState aStatus);
	TInt TurnaroundSet(TUint aNewTurnaroundMilliSeconds);
	TBool TurnaroundStopTimer();
	TInt TurnaroundClear();
	static void TurnaroundStartDfc(TAny* aSelf);
	void TurnaroundStartDfcImplementation(TBool inIsr);
	static void TurnaroundTimeout(TAny* aSelf);
	void TurnaroundTimeoutImplementation(void);

protected:
	virtual TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	virtual void HandleMsg(TMessageBase* aMsg);
	void DoCancel(TInt aMask);
	TInt DoControl(TInt aId, TAny* a1, TAny* a2);
	TInt DoRequest(TInt aId, TRequestStatus* aStatus, TAny* a1, TAny* a2);
	void Start();
	void Shutdown();
	void BreakOn();
	void BreakOff();
	void ResetBuffers(TBool aResetTx);
	void DoCompleteRx();
	void DoCompleteTx();
	void DoSignalNotify();
	void DoRxDataAvailable();
	void Complete(TInt aMask, TInt aReason);
	inline void CompleteRead()	{ iRxCompleteDfc.Add(); }
	inline void CompleteWrite() { iTxCompleteDfc.Add(); }
protected:
	inline void Write(DThread* aT, TAny* aP, TInt aL) {((DComm*)iPdd)->Write(aT, aP,aL);}
	inline void Read(DThread* aT, TAny* aP, TInt aL) {((DComm*)iPdd)->Read(aT, aP,aL);}
	inline void NotifySignals(DThread* aThread, TInt aMask) {((DComm*)iPdd)->NotifySignals(aThread, aMask);}
	inline void NotifyDataAvailable() {((DComm*)iPdd)->NotifyDataAvailable(); }
	inline TInt PddStart();
	inline void Stop(TStopMode aMode);
	inline void PddBreak(TBool aState);
	inline TUint Signals() const;
	inline void SetSignals(TUint aSetMask,TUint aClearMask);
	inline TInt ValidateConfig(const TCommConfigV01 &aConfig) const;
	inline void PddConfigure(TCommConfigV01 &aConfig);
	inline void PddCaps(TDes8 &aCaps) const;
	inline void PddCheckConfig(TCommConfigV01& aConfig);
	inline TBool Transmitting();
	inline TInt BufferSize();
	inline void ReadCancel();
	inline void WriteCancel();
	inline void SignalChangeCancel();
private:
//	static void PowerUpDfc(TAny* aPtr);
//	static void PowerDownDfc(TAny* aPtr);
	static void CompleteRxDfc(TAny* aPtr);
	static void CompleteTxDfc(TAny* aPtr);
	static void SignalNotifyDfc(TAny* aPtr);
	static void RxDataAvailableDfc(TAny* aPtr);
	inline TInt RxCount();
	inline TInt RxBufferSize();
	inline TDes8* RxBuffer();
	inline TInt SetRxBufferSize(TInt aSize);
	inline TBool AreAnyPending() const;
//	void UpdateSignals();
	void UpdateAndProcessSignals();
	void InitiateRead(TAny* aRxDes, TInt aLength);
	void InitiateWrite(TAny* aTxDes, TInt aLength);
	void InitiateNotifySignals(TAny* aResult, TInt aMask);
	void NotifyReadDataAvailable();
//	TUint FailSignals(TUint aHandshake);
//	TUint HoldSignals(TUint aHandshake);
//	TUint FlowControlSignals(TUint aHandshake);
//	TUint AutoSignals(TUint aHandshake);
	TInt SetConfig(TCommConfigV01& aConfig);
//	void CheckOutputHeld();
//	void ReleaseBuffers();

	// Min Turnaround time between Rx and Tx
	TUint		iTurnaroundMicroSeconds;		// delay after a receive before transmission in us
	TUint		iTurnaroundMinMilliSeconds;		// delay after a receive before transmission in ms
	TBool		iTurnaroundTimerRunning;		// a receive has started the timer
	TBool		iTurnaroundTransmitDelayed;		// a transmission is held until time elapses after a receive
	NTimer		iTurnaroundTimer;				// used to delay transmission after a receive
	TDfc		iTurnaroundDfc;					// used in interrupt space, to trigger a call in user space

	TAny*		iTurnaroundTxDesPtr;			// client descriptor awaiting transmission
	TInt		iTurnaroundTxDesLength;			// initial size of tx descriptor

public:
	// Port configuration
	TCommConfigV01 iConfig;

	// General items
	DThread* iClient;
	TAny* iClientDestPtr;
//	DCommPowerHandler* iPowerHandler;
//	TDfc iPowerUpDfc;
//	TDfc iPowerDownDfc;
//	TUint32 iPowerDownMask;
	TState iStatus;
	TDfc iRxCompleteDfc;
	TDfc iTxCompleteDfc;
	TDfc iRxDataAvailableDfc;
	TDfc iSigNotifyDfc;
	TUint iFlags;				//
	TUint iSignals;				// State of handshake lines
//	TUint iFailSignals;			// 1 bit means line low causes line fail error
//	TUint iHoldSignals;			// 1 bit means line low halts TX
//	TUint iFlowControlSignals;	// 1 bit means signal is used for RX flow control
//	TUint iAutoSignals;			// 1 bit means signal is high when channel is open
	TBool iShutdown;			// ETrue means device is being closed
	TBool iMsgHeld;				// ETrue means a message has been held up waiting power on

	// RX client related items
	TInt iRxError;
	TRequestStatus* iRxStatus;


	// TX client related items
	TInt iTxError;
	TRequestStatus* iTxStatus;

	// DataAvailable client related items
	TInt iRxDAError;
	TRequestStatus* iRxDAStatus;

	// Signal change notification
	TInt iSignalError;
	TRequestStatus* iSignalStatus;	
	TUint* iClientSignalResultPtr;	//ptr to uint in client address space
	TUint iSignalResult;			//holds result of the signal (in Symbian OS values)
	};

//class DCommPowerHandler : public DPowerHandler
//	{
//public:
//	DCommPowerHandler(DChannelComm* aChannel);
//	virtual TInt DoPowerUp();
//	virtual void DoPowerDown(TUint32 aPowerDownMask);
//	virtual void DoEmergencyPowerDown();
//public:
//	DChannelComm* iChannel;
//	};

#include "winscomm.inl"

#endif
