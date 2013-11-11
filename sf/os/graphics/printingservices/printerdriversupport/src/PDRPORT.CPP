// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <pdrport.h>
#include "printerport.h"

_LIT(KPdrStoreECUART,"ECUART");

EXPORT_C CFilePrinterPort* CFilePrinterPort::NewL(const TDesC& aFileName)
	{
	CFilePrinterPort* fileprinterport = new(ELeave) CFilePrinterPort(aFileName);
	CleanupStack::PushL(fileprinterport);
	fileprinterport->ConstructL();
	CleanupStack::Pop();
	return fileprinterport;
	}

EXPORT_C CFilePrinterPort::~CFilePrinterPort()
	{
	iFile.Close();
	if (iCancelled)
		iFs.Delete(iFileName);  // ignore error
	iFs.Close();
	}
/** Implementation of the pure virtual function inherited from CPrinterPort.

Writes data asynchronously to the concrete file printer port.
     
@param aBuf 	      A descriptor containing the data to be written to the file printer port. 
@param aRequestStatus A reference to the request status object. 
	   				  On completion contains KErrNone if the data is successfully written to 
	   				  this file printer port, otherwise if the request is cancelled, this should 
	   				  be set to KErrCancel.
*/
void CFilePrinterPort::WriteRequest(const TDesC8& aBuf, TRequestStatus& aRequestStatus)
	{
//	iFile.Write(aBuf,aRequestStatus); //!! real code with fixed F32
//
	TRequestStatus* pStatus=&aRequestStatus;			// F32 bug workaround code
	User::RequestComplete(pStatus, iFile.Write(aBuf));	// F32 bug workaround code 
	}

void CFilePrinterPort::Cancel()
	{
	iCancelled = ETrue;
	}

void CFilePrinterPort::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Replace(iFs, iFileName,EFileStream|EFileWrite));
	}

CFilePrinterPort::CFilePrinterPort(const TDesC& aFileName)
:	iFileName(aFileName)
	{
	}

EXPORT_C TOutputHandshake::TOutputHandshake()
:	iXonXoff(EFalse),
	iCts(ETrue),
	iDsr(ETrue),
	iDcd(EFalse)
	{
	}

EXPORT_C void TOutputHandshake::InternalizeL(RReadStream& aStream)
	{
	iXonXoff = aStream.ReadInt8L();
	iCts = aStream.ReadInt8L();
	iDsr = aStream.ReadInt8L();
	iDcd = aStream.ReadInt8L();
	}

EXPORT_C void TOutputHandshake::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt8L(iXonXoff);
	aStream.WriteInt8L(iCts);
	aStream.WriteInt8L(iDsr);
	aStream.WriteInt8L(iDcd);
	}

EXPORT_C TSerialPrinterPortConfig::TSerialPrinterPortConfig()
:	iRate(EBps9600),
	iDataBits(EData8),
	iStopBits(EStop1),
	iParity(EParityNone),
	iIgnoreParity(EFalse),
	iHandshake()
	{
	}

EXPORT_C void TSerialPrinterPortConfig::InternalizeL(RReadStream& aStream)
	{
	iRate = (TBps) aStream.ReadInt8L();
	iDataBits = (TDataBits) aStream.ReadInt8L();
	iStopBits = (TStopBits) aStream.ReadInt8L();
	iParity = (TParity) aStream.ReadInt8L();
	iIgnoreParity = aStream.ReadInt8L();
	iHandshake.InternalizeL(aStream);
	}

EXPORT_C void TSerialPrinterPortConfig::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt8L(iRate);
	aStream.WriteInt8L(iDataBits);
	aStream.WriteInt8L(iStopBits);
	aStream.WriteInt8L(iParity);
	aStream.WriteInt8L(iIgnoreParity);
	iHandshake.ExternalizeL(aStream);
	}

EXPORT_C CCommPrinterPort* CCommPrinterPort::NewL(const TDesC& aCsyName, const TDesC& aPortName, const TSerialPrinterPortConfig& aConfig, const TFifo aFifo)
	{
	CCommPrinterPort* commprinterport = new(ELeave) CCommPrinterPort;
	CleanupStack::PushL(commprinterport);
	commprinterport->ConstructL(aCsyName, aPortName, aConfig, aFifo);
	CleanupStack::Pop();
	return commprinterport;
	}

EXPORT_C CCommPrinterPort::~CCommPrinterPort()
	{
	iComm.Close();
	iCommServ.Close();
	}

EXPORT_C void CCommPrinterPort::WriteRequest(const TDesC8& aBuf, TRequestStatus& aRequestStatus)
	{
	iComm.Write(aRequestStatus, 20000000, aBuf);  // Times out after 20 seconds
	}

EXPORT_C void CCommPrinterPort::Cancel()
	{
	iComm.WriteCancel();
	}

EXPORT_C void CCommPrinterPort::ConstructL(const TDesC& aCsyName, const TDesC& aPortName, const TSerialPrinterPortConfig& aConfig, const TFifo aFifo)
	{
	User::LeaveIfError(iCommServ.Connect());
	User::LeaveIfError(iCommServ.LoadCommModule(aCsyName)); 
	User::LeaveIfError(iComm.Open(iCommServ, aPortName, ECommExclusive));

	TCommConfig config;
	iComm.Config(config);
	config().iRate = aConfig.iRate;
	config().iDataBits = aConfig.iDataBits;
	config().iStopBits = aConfig.iStopBits;
	config().iParityErrorChar = STATIC_CAST(TText8, aConfig.iParity);
	if (aConfig.iIgnoreParity)
		config().iParityErrorChar = KConfigParityErrorIgnore;
	TUint handshake = 0;
	if (aConfig.iHandshake.iXonXoff)
		handshake = handshake | KConfigObeyXoff;
	if (aConfig.iHandshake.iCts)
		handshake = handshake | KConfigObeyCTS;
	if (aConfig.iHandshake.iDsr)
		handshake = handshake | KConfigObeyDSR;
	if (aConfig.iHandshake.iDcd)
		handshake = handshake | KConfigObeyDCD;
	config().iHandshake = handshake;
	config().iFifo = aFifo;
	User::LeaveIfError(iComm.SetConfig(config));
	}

EXPORT_C CCommPrinterPort::CCommPrinterPort()
:	CPrinterPort(),
	iCommServ(),
	iComm()
	{
	}

EXPORT_C CSerialPrinterPort* CSerialPrinterPort::NewL(const TDesC& aPortName, const TSerialPrinterPortConfig& aConfig)
	{
	CSerialPrinterPort* serialprinterport=new(ELeave) CSerialPrinterPort(aConfig);
	CleanupStack::PushL(serialprinterport);
	serialprinterport->ConstructL(aPortName);
	CleanupStack::Pop();
	return serialprinterport;
	}

EXPORT_C CSerialPrinterPort::~CSerialPrinterPort()
	{
	}

EXPORT_C TSerialPrinterPortConfig CSerialPrinterPort::Config()
	{
	return iConfig;
	}

void CSerialPrinterPort::ConstructL(const TDesC& aPortName)
	{
	CCommPrinterPort::ConstructL(KPdrStoreECUART, aPortName, iConfig);
	}

CSerialPrinterPort::CSerialPrinterPort(const TSerialPrinterPortConfig& aConfig)
:	CCommPrinterPort(),
	iConfig(aConfig)
	{
	}

EXPORT_C CParallelPrinterPort* CParallelPrinterPort::NewL(const TDesC& aPortName)
	{
	CParallelPrinterPort* parallelprinterport = new(ELeave) CParallelPrinterPort;
	CleanupStack::PushL(parallelprinterport);
	parallelprinterport->ConstructL(aPortName);
	CleanupStack::Pop();
	return parallelprinterport;
	}

EXPORT_C CParallelPrinterPort::~CParallelPrinterPort()
	{
	}

void CParallelPrinterPort::ConstructL(const TDesC& aPortName)
	{
	TSerialPrinterPortConfig config;
	config.iRate = EBps19200;
	config.iHandshake.iXonXoff = ETrue;
	config.iHandshake.iCts = EFalse;
	config.iHandshake.iDsr = ETrue;
	config.iHandshake.iDcd = ETrue;
	CCommPrinterPort::ConstructL(KPdrStoreECUART, aPortName, config, EFifoDisable);

	TRequestStatus stat;
	iComm.Write(stat, 10, TPtrC8(NULL, 0));
	User::WaitForRequest(stat);

	TCommConfig buf;
	iComm.Config(buf);
	buf().iHandshake |= (KConfigFailDSR | KConfigFailDCD);
	User::LeaveIfError(iComm.SetConfig(buf));
	}

CParallelPrinterPort::CParallelPrinterPort()
:	CCommPrinterPort()
	{
	}

EXPORT_C CIrdaPrinterPort* CIrdaPrinterPort::NewL()
	{
	CIrdaPrinterPort* irdaprinterport = new(ELeave) CIrdaPrinterPort;
	CleanupStack::PushL(irdaprinterport);
	irdaprinterport->ConstructL();
	CleanupStack::Pop();
	return irdaprinterport;
	}

EXPORT_C CIrdaPrinterPort::~CIrdaPrinterPort()
	{
	}

void CIrdaPrinterPort::ConstructL()
	{
	TSerialPrinterPortConfig config;
	_LIT(KPdrStoreIRCOMM,"IRCOMM");
	_LIT(KPdrStoreIRCOMM0,"IrCOMM::0");
	CCommPrinterPort::ConstructL(KPdrStoreIRCOMM, KPdrStoreIRCOMM0, config, EFifoDisable);
	}

CIrdaPrinterPort::CIrdaPrinterPort()
:	CCommPrinterPort()
	{
	}

EXPORT_C CEpocConnectPort* CEpocConnectPort::NewL()
	{
	CEpocConnectPort* epocconnectport = new(ELeave) CEpocConnectPort;
	CleanupStack::PushL(epocconnectport);
	epocconnectport->ConstructL();
	CleanupStack::Pop();
	return epocconnectport;
	}

EXPORT_C CEpocConnectPort::~CEpocConnectPort()
	{
	}

void CEpocConnectPort::ConstructL()
	{
	TSerialPrinterPortConfig config;
	_LIT(KPdrStorePLPLPT,"PLPLPT");
	_LIT(KPdrStorePLPLPT0,"PLPLPT::0");
	CCommPrinterPort::ConstructL(KPdrStorePLPLPT, KPdrStorePLPLPT0, config, EFifoDisable);
	}

CEpocConnectPort::CEpocConnectPort()
:	CCommPrinterPort()
	{
	}


