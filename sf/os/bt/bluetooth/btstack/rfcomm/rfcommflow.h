// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RFCOMMFLOW_H
#define RFCOMMFLOW_H

#include <es_prot.h>
#include <bttypes.h>
class CRfcommSAP;
class CRfcommMuxer;
class CRfcommDataFrame;
class TRfcommFlowStrategy;

/**
	CRfcommFlowStrategyFactory factory for flow strategies (ways to control flow 
	of packets in and out of Rfcomm).
**/
NONSHARABLE_CLASS(CRfcommFlowStrategyFactory) : public CBase
	{
friend class CRfcommProtocol;
public:
enum TFlowStrategies
	{
	EFlowInitial,
	EFlowCreditBased, //CBFC accepted
	EFlowNonCreditBased, //CBFC rejected
	//...
	EMaxFlowStrategy
	};
	TRfcommFlowStrategy& GetFlowStrategy(const TFlowStrategies aFlowStrategy) const;

	~CRfcommFlowStrategyFactory();
private:
	static CRfcommFlowStrategyFactory* NewL();
	CRfcommFlowStrategyFactory();

	// Horrible hacky workround for buggy VC6.
	enum {MaxFlowStrategies=3}; // Must be equal to number of flow strategies in TFlowStrategies above
	
	TFixedArray<TRfcommFlowStrategy*, MaxFlowStrategies> iFlowStrategies;
	};



/**
   Flow control strategy class
**/
NONSHARABLE_CLASS(TRfcommFlowStrategy) : public CBase
	{
public:
	// Create/destroy
	TRfcommFlowStrategy(CRfcommFlowStrategyFactory* aFactory=0); 
	~TRfcommFlowStrategy();

	virtual TInt DataBufferMultiple() = 0;
	virtual TUint16 UsableMTU(const CRfcommSAP& aSAP, TUint8 aCredit) const = 0;
	virtual TBool AllowWrite(CRfcommSAP& aSAP) = 0;
	virtual TBool AllowFCOnOff(TBool aCommand) = 0;
	virtual TInt PNConvergenceLayer(TBool aCommand) = 0;
	virtual TUint8 PNFinalOctet() = 0;
	virtual TUint8 WriteCredit(CRfcommSAP& aSAP) = 0;
	virtual void OutgoingMSCReviseSignals(TBool aCommand, TUint8& aSignals) = 0;
	virtual void ReviseDonatedCredits(CRfcommSAP& aSAP, TUint8 aCredit) = 0;
	virtual void ReviseTransmittedCredits(CRfcommSAP& aSAP) = 0;
	virtual CRfcommDataFrame* NewDataFrame(CRfcommMuxer& aMuxer, TUint8 aAddr, TInt aLen, 
										   TUint8 aCredit, CRfcommSAP* aSAP) = 0;
	virtual TBool CanProcessNewData(const TBool aAllowProcessNewData) const = 0;
	virtual void DecodeLength(TBool aCBFC, TDesC8& aPacket, 
							  TInt& aCreditBuffer, 
							  TInt& aHeaderLengthBuffer) = 0;
	virtual TBool ProcessDataFrameReviseCredits(CRfcommSAP& aSAP, TBool aPoll, 
											   TUint8 aCredit) = 0;
	virtual TBool MSC(CRfcommSAP& aSAP, TUint8 aSignals) = 0;
	virtual void UpdateRxFlowControlState(CRfcommSAP& aSAP) = 0;
	virtual TBool NewDataReviseCredits(CRfcommSAP& aSAP, const TDesC8& aData) = 0;
	virtual void NewData(CRfcommSAP& aSAP) = 0;
	virtual CRfcommFlowStrategyFactory::TFlowStrategies FlowType() = 0;

protected:
	CRfcommFlowStrategyFactory* iFactory;
	};


/**
   Flow control strategy class before decision has been made (1st PN transaction)
**/
NONSHARABLE_CLASS(TRfcommFlowStrategyInitial) : public TRfcommFlowStrategy
	{
public:
	// Create/destroy
	TRfcommFlowStrategyInitial(CRfcommFlowStrategyFactory* aFactory); 
	~TRfcommFlowStrategyInitial();

	TInt DataBufferMultiple();
	TUint16 UsableMTU(const CRfcommSAP& aSAP, TUint8 aCredit) const;
	TBool AllowWrite(CRfcommSAP& aSAP);
	TBool AllowFCOnOff(TBool aCommand);
	TInt PNConvergenceLayer(TBool aCommand);
	TUint8 PNFinalOctet();
	TUint8 WriteCredit(CRfcommSAP& aSAP);
	void OutgoingMSCReviseSignals(TBool aCommand, TUint8& aSignals);
	void ReviseDonatedCredits(CRfcommSAP& aSAP, TUint8 aCredit);
	void ReviseTransmittedCredits(CRfcommSAP& aSAP);
	CRfcommDataFrame* NewDataFrame(CRfcommMuxer& aMuxer, TUint8 aAddr, TInt aLen, 
								   TUint8 aCredit, CRfcommSAP* aSAP);
	TBool CanProcessNewData(const TBool AllowProcessNewData) const;
	void DecodeLength(TBool aCBFC, TDesC8& aPacket, 
					  TInt& aCreditBuffer, TInt& aHeaderLengthBuffer);
	TBool ProcessDataFrameReviseCredits(CRfcommSAP& aSAP, TBool aPoll, TUint8 aCredit);
	TBool MSC(CRfcommSAP& aSAP, TUint8 aSignals);
	void UpdateRxFlowControlState(CRfcommSAP& aSAP);
	TBool NewDataReviseCredits(CRfcommSAP& aSAP, const TDesC8& aData);
	void NewData(CRfcommSAP& aSAP);
	CRfcommFlowStrategyFactory::TFlowStrategies FlowType();
	};


/**
   Flow control strategy class for "Credit Based Flow Control"
**/
NONSHARABLE_CLASS(TRfcommFlowStrategyCreditBased) : public TRfcommFlowStrategyInitial
	{
public:
	// Create/destroy

	TRfcommFlowStrategyCreditBased(CRfcommFlowStrategyFactory* aFactory); 
	~TRfcommFlowStrategyCreditBased();

	TInt DataBufferMultiple();
	TUint16 UsableMTU(const CRfcommSAP& aSAP, TUint8 aCredit) const;
	TBool AllowWrite(CRfcommSAP& aSAP);
	TBool AllowFCOnOff(TBool aCommand);
	TUint8 WriteCredit(CRfcommSAP& aSAP);
	void OutgoingMSCReviseSignals(TBool aCommand, TUint8& aSignals);
	void ReviseDonatedCredits(CRfcommSAP& aSAP, TUint8 aCredit);
	void ReviseTransmittedCredits(CRfcommSAP& aSAP);
	CRfcommDataFrame* NewDataFrame(CRfcommMuxer& aMuxer, TUint8 aAddr, TInt aLen, 
								   TUint8 aCredit, CRfcommSAP* aSAP);
	TBool CanProcessNewData(const TBool aAllowProcessNewData) const;
	void DecodeLength(TBool aCBFC, TDesC8& aPacket, 
					  TInt& aCreditBuffer, TInt& aHeaderLengthBuffer);
	TBool ProcessDataFrameReviseCredits(CRfcommSAP& aSAP, TBool aPoll, TUint8 aCredit);
	TBool MSC(CRfcommSAP& aSAP, TUint8 aSignals);
	void UpdateRxFlowControlState(CRfcommSAP& aSAP);
	TBool NewDataReviseCredits(CRfcommSAP& aSAP, const TDesC8& aData);
	void NewData(CRfcommSAP& aSAP);
	CRfcommFlowStrategyFactory::TFlowStrategies FlowType();
	};

/**
   Flow control strategy class for FOn/FOff ... which is per muxer
**/
NONSHARABLE_CLASS(TRfcommFlowStrategyNonCreditBased) : public TRfcommFlowStrategyInitial
	{
public:
	// Create/destroy

	TRfcommFlowStrategyNonCreditBased(CRfcommFlowStrategyFactory* aFactory); 
	~TRfcommFlowStrategyNonCreditBased();


	TInt PNConvergenceLayer(TBool aCommand);
	TUint8 PNFinalOctet();
	CRfcommFlowStrategyFactory::TFlowStrategies FlowType();
	};



#endif
