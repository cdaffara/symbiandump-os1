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


#if !defined(__D_COMM_H__)
#define __D_COMM_H__
#if !defined(__K32STD_H__)
#include <k32std.h>
#endif
#if !defined(__D32COMM_H__)
#include <d32comm.h>
#endif
#if !defined(__E32VER_H__)
#include <e32ver.h>
#endif
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
	virtual void SetTimer(const TUint aTimeOut) =0;
	virtual void CancelTimer() =0;
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
// 32 bit sequence number space
//
class TCommSeqNum
	{
public:
	inline TCommSeqNum& operator++();
	inline TCommSeqNum& operator++(TInt);
	inline TCommSeqNum& operator--();
	inline TCommSeqNum& operator--(TInt);
	inline TCommSeqNum& operator=(TUint aVal);
	inline TCommSeqNum& operator=(const TCommSeqNum& aVal);
	inline TCommSeqNum& operator+=(TInt aVal);
	inline TCommSeqNum& operator-=(TInt aVal);
	inline TBool operator==(const TCommSeqNum& aVal) const;
	inline TBool operator!=(const TCommSeqNum& aVal) const;
	inline TBool operator<(const TCommSeqNum& aVal) const;
	inline TBool operator<=(const TCommSeqNum& aVal) const;
	inline TBool operator>(const TCommSeqNum& aVal) const;
	inline TBool operator>=(const TCommSeqNum& aVal) const;
	inline TCommSeqNum operator+(TInt aVal);
	inline TCommSeqNum operator-(TInt aVal);
	inline TInt operator-(const TCommSeqNum& aVal);
	inline TUint Int() const;
private:
	TUint iNum;
	};
//
class CCommRxBuf : public CBase
	{
public:
	CCommRxBuf();
	~CCommRxBuf();
	void SetLengthL(TInt aLength);
	void Reset();
	inline TInt Count() { return iCount; }
	inline TInt Length()  { return iLength; }
	TInt PutChar(TUint aChar);
	TInt ClientWrite(TUint& aStatus, DComm* aDriver, DThread* aThread, const TAny* aPtr, TInt& aRxLength, TInt& aRxOffset, TInt aMode);
	TInt TestTerm(TInt aCount, TInt aTermCount, TUint8* aTermList);
	TBool RescanTerminators(DComm* aDriver, TInt aTermCount, TText8* aTermChars);
public:
	TInt iLength;
	TInt iCount;
	TInt iInsP;
	TInt iRemP;
	TUint8* iCharPtr;
	TUint8* iInfoPtr;
	TCommSeqNum iInsSeqNum;
	TCommSeqNum iRemSeqNum;
	TCommSeqNum iLastSeqNum;
	};

class CCommTxBuf : public CCirBuffer
	{
public:
	CCommTxBuf();
	TInt ClientRead(DComm* aDriver, DThread* aThread, const TAny* aPtr, TInt& aTxLength, TInt& aTxOffset, TInt aMode);
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

class DCommPowerHandler : public DPowerHandler
	{
public:
	DCommPowerHandler(DChannelComm *aLdd);
	void RequestComplete();
	void RequestPending();
	static void Timeout(TAny *, TInt);

	virtual TInt DoPowerOn();
	virtual void DoPowerStandby();
	virtual void DoPowerEmergencyStandby();
public:
	DChannelComm *iLdd;
	TTickLink iTimer;
	};

class DChannelComm : public DLogicalChannel
	{
public:
	enum TState {ENotActive,EActive,EClosed};
private:
	enum TDfcRequestFlags
		{
		EFillTxBufferRequired=0x00000001,
		EDrainRxBufferRequired=0x00000002,
		ECompleteRead=0x00000008,
		ECompleteWrite=0x00000010,
		ECompleteBreak=0x00000020,
		EEarlyCompleteWrite=0x00000040,
		ECompleteEmergency=0x00000080,
		ECompleteSignalChange=0x00000100,
		ECompleteDataAvailable=0x00000200,
		ETurnaroundTimerDfc=0x00000400
		};
public:
	DChannelComm(DLogicalDevice *aDevice);
	~DChannelComm();
	virtual void ReceiveIsr(TUint aChar);
	virtual TInt TransmitIsr();
	virtual void StateIsr(TUint aStatus);
	inline TBool IsPowerGood();
	inline void SetStatus(TState aStatus);
	inline void CompleteAllReqs(TInt aReason);
	inline TBool IsReading() const;
	inline TBool IsWriting() const;
protected:
	virtual void DoCreateL(TInt aUnit,CBase *aDriver,const TDesC *anInfo,const TVersion &aVer);
	virtual void DoCancel(TInt aReqNo);
	virtual void DoRequest(TInt aReqNo,TAny *a1,TAny *a2);
	virtual TInt DoControl(TInt aFunction,TAny *a1,TAny *a2);
	inline TBool AreAnyPending() const;
	void Start();
	void Stop(TStopMode aMode);
	void BreakOn();
	void BreakOff();
	void EnableTransmit();
	TInt SetRxBufferSize(TInt aSize);
	void ResetBuffers();
	TBool IsLineFail(TUint aHandshake);
	void DoDrainRxBuffer();
	void DoFillTxBuffer();
	inline void DrainRxBuffer() {QueueDfc(EDrainRxBufferRequired);};
	inline void FillTxBuffer() {QueueDfc(EFillTxBufferRequired);};
	inline void CompleteWrite() {QueueDfc(ECompleteWrite);};
	inline void EarlyCompleteWrite() {QueueDfc(EEarlyCompleteWrite);};
	inline void CompleteSignalChange() {QueueDfc(ECompleteSignalChange);};
	inline void CompleteDataAvailable() {QueueDfc(ECompleteDataAvailable);};
//	void CompleteWrite(TInt anError=KErrNone);
//	void CompleteRead(TInt anError=KErrNone);
//	void DoCompleteWrite();
//	void DoCompleteRead();
public:
	virtual void DoPowerDown();
	virtual void DoEmergencyPowerDown();

	TInt TurnaroundSet(TUint aNewTurnaroundMicroSeconds);
	TInt TurnaroundClear();
	void RestartTurnaroundTimer();
	virtual void TurnaroundStartDfcImplementation();
	void TurnaroundTimeoutImplementation();

private:
	static TInt CallDfc(TAny* aDChannelComm);
	void DfcHandler();
	void QueueDfc(TUint aRequestMask);
	void ResetDevice();
	void RestoreDtrRrs();
public:
	TCommConfigV02 iConfig;
private:
	DComm *iDriver;
	DCommPowerHandler *iPowerHandler;
	TState iStatus;
	TUint iDfcRequest;
	TDfc iDfc;
	TInt iInputHeld;			// Receive suspended by signalling peer
	TInt iOutputHeld;			// Transmit suspended by peer
	TInt iReceiveError;
	TInt iWriteError;
	TInt iReceiveOneOrMore;
	TInt iWriteZero;			// Awaiting ability to write
	TInt iReceiveErrorLength;
	TInt iReceiveLength;
	TInt iReceiveOffset;
	TInt iTransmitLength;
	TInt iTransmitOffset;
	TUint iFlags;
	TUint iSignals;
	TInt iLowWaterMark;
	TInt iHighWaterMark;
	TInt iHalfFullMark;
	TUint8 *iReceivePtr;		// Client space pointer
	TUint8 *iTransmitPtr;		// Client space pointer
	CCommRxBuf *iRxBuf;
	CCommTxBuf *iTxBuf;
	TBool iIsReading;
	TBool iIsWriting;
	TBool iDrainingRxBuf;
	TBool iFillingTxBuf;
	TBool iRunningDfc;
	TBool iIsClientWriting;
	TBool iSignalChangeNotification;
	TUint* iSignalChangePtr;
	TUint iSignalMask;
    TUint iSignalChangeInfo;
	TBool iDataAvailableNotification;

	// Min Turnaround time between Rx and Tx
	TUint		iTurnaroundMinMicroSeconds;	// delay after a receive before transmission
	TBool		iTurnaroundTimerRunning;	// a receive has started the timer
	TBool		iTurnaroundTransmitDelayed;	// a transmission is held until time elapses after a receive
	TAny*		iTurnaroundTxDesPtr;		// client descriptor awaiting transmission
	TInt		iTurnaroundTxDesLength;		// initial size of tx descriptor

#ifdef _DEBUG_DEVCOMM
	TInt iDfcCount;
	TInt iDfcReqSeq;
	TInt iDfcHandlerSeq;
	TInt iDoDrainSeq;
	TInt iTxChars, iRxChars;
	TInt iTxXon, iTxXoff, iRxXon, iRxXoff;
#endif
	friend class DCommPowerHandler;
	};

#include "d_comm.inl"

#endif
