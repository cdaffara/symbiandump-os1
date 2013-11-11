// vtc_busdevcons.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <d32comm.h>
#include <fshell/vtc_base.h>
#include <fshell/common.mmh>

class TPortConfig
	{
public:
	TPortConfig();

	TPtrC iPdd;
	TPtrC iLdd;
	TInt iPort;
	TBps iRate;
	TBool iDebug;
	};

#define ErrOrDebug(err) (((err) < 0  && (err) != KErrAlreadyExists) ? EError : EDebug)

NONSHARABLE_CLASS(CVtcBusDevCommConsole) : public CVtcConsoleBase
	{
public:
	CVtcBusDevCommConsole();
	~CVtcBusDevCommConsole();

protected: // From CVtcConsoleBase.
	virtual void ConstructL(const TDesC& aTitle);
private: // From MConsoleOutput.
	virtual TInt Output(const TDesC8& aDes);
private: // From MConsoleInput.
	virtual void Input(TDes8& aDes, TRequestStatus& aStatus);
	virtual void CancelInput(TRequestStatus& aStatus);
private:
	TInt ReadConfig(const TDesC& aConfigDes, TPortConfig& aConfig);
private:
	RBusDevComm iComm;
	};

extern "C" EXPORT_C TAny* NewConsole()
	{
	return new CVtcBusDevCommConsole;
	}

CVtcBusDevCommConsole::CVtcBusDevCommConsole()
	{
	}

CVtcBusDevCommConsole::~CVtcBusDevCommConsole()
	{
	iComm.Close();
	}

#ifdef __WINS__
_LIT(KPdd, "ecdrv");
#else
_LIT(KPdd, "euart");
#endif
_LIT(KLdd, "ecomm");

TPortConfig::TPortConfig()
	:iPdd(KNullDesC), iLdd(KNullDesC), iPort(-1), iRate(EBpsAutobaud), iDebug(EFalse)
	{
	}

TInt CVtcBusDevCommConsole::ReadConfig(const TDesC& aConfigDes, TPortConfig& aConfig)
	{
	_LIT(KKeywordPdd, "pdd");
	_LIT(KKeywordLdd, "ldd");
	_LIT(KKeywordPort, "port");
	_LIT(KKeywordRate, "rate");
	_LIT(KKeywordDebug, "debug");

	TBool keywordFound(EFalse);
	TLex lex(aConfigDes);
	while (!lex.Eos())
		{
		TPtrC keyword;
		TPtrC value;
		TInt err = ReadKeywordValuePair(lex, keyword, value);
		if (err != KErrNone)
			{
			break;
			}

		if (keyword == KKeywordPdd)
			{
			aConfig.iPdd.Set(value);
			keywordFound = ETrue;
			}
		else if (keyword == KKeywordLdd)
			{
			aConfig.iLdd.Set(value);
			keywordFound = ETrue;
			}
		else if (keyword == KKeywordPort)
			{
			TLex lex(value);
			TInt err = lex.Val(aConfig.iPort);
			if (err) return err;
			keywordFound = ETrue;
			}
		else if (keyword == KKeywordRate)
			{
			TLex lex(value);
			TUint rate;
			if (lex.Val(rate) == KErrNone)
				{
				switch (rate)
					{
				case 115200:
					aConfig.iRate = EBps115200; break;
				case 9600:
					aConfig.iRate = EBps9600; break;
				case 19200:
					aConfig.iRate = EBps19200; break;
				case 57600:
					aConfig.iRate = EBps57600; break;
				default:
					break;
					}
				}
			keywordFound = ETrue;
			}
		else if (keyword == KKeywordDebug)
			{
			aConfig.iDebug = ETrue;
			keywordFound = ETrue;
			}
		}

	if (!keywordFound)
		{
		// Treat unrecognised string as a port number (to preserve backwards compatibility with earlier releases).
		TLex lex(aConfigDes);
		lex.Val(aConfig.iPort);
		}

	return KErrNone;
	}

void CVtcBusDevCommConsole::ConstructL(const TDesC& aTitle)
	{
	TPortConfig portConfig;
	User::LeaveIfError(ReadConfig(aTitle, portConfig));
	if (portConfig.iPort < 0) User::Leave(KErrArgument);

	if (portConfig.iDebug) SetDebug(ETrue);

	TPtrC pdd(portConfig.iPdd);
	if (pdd.Length() == 0) pdd.Set(KPdd());
	TPtrC ldd(portConfig.iLdd);
	if (ldd.Length() == 0) ldd.Set(KLdd());

	TInt err = User::LoadPhysicalDevice(pdd);
	Message(ErrOrDebug(err), _L("Loading PDD %S returned %d"), &pdd, err);
	if (err != KErrAlreadyExists && portConfig.iPdd.Length()) User::LeaveIfError(err); // Don't error if we failed to load the default PDD

	err = User::LoadLogicalDevice(ldd);
	Message(ErrOrDebug(err), _L("Loading LDD %S returned %d"), &ldd, err);
	if (err != KErrAlreadyExists && portConfig.iLdd.Length()) User::LeaveIfError(err); // Don't error if we failed to load the default LDD
	err = iComm.Open(portConfig.iPort);
	Message(ErrOrDebug(err), _L("Opening port %d returned %d"), portConfig.iPort, err);
	User::LeaveIfError(err);

	if (portConfig.iRate != EBpsAutobaud)
		{
		TCommConfig cfg;
		cfg().iRate = portConfig.iRate;
		cfg().iDataBits = EData8;
		cfg().iStopBits = EStop1;
		cfg().iParity = EParityNone;
		cfg().iHandshake = 0;
		cfg().iParityError = KConfigParityErrorFail;
		cfg().iFifo = EFifoEnable;
		cfg().iSpecialRate = 0;
		cfg().iTerminatorCount = 0;
		cfg().iSIREnable = ESIRDisable;
		err = iComm.SetConfig(cfg);
		Message(ErrOrDebug(err), _L("RBusDevComm::SetConfig returned %d"), err);
		User::LeaveIfError(err);
		}
	iComm.ResetBuffers();
	CVtcConsoleBase::ConstructL(aTitle);
	}
	
TInt CVtcBusDevCommConsole::Output(const TDesC8& aDes)
	{
	TRequestStatus stat;
#ifdef FSHELL_PLATFORM_OPP
retry:
#endif
	iComm.Write(stat, aDes);
	User::WaitForRequest(stat);
#ifdef FSHELL_PLATFORM_OPP
	// Temporary OPP specific change - the drivers for the mid-sized prototype currently complete requests with KErrAbort just before power management sends the device to sleep.
	if (stat.Int() == KErrAbort)
		{
		goto retry;
		}
#endif
	return stat.Int();
	}

void CVtcBusDevCommConsole::Input(TDes8& aDes, TRequestStatus& aStatus)
	{
	iComm.ReadOneOrMore(aStatus, aDes);
	}

void CVtcBusDevCommConsole::CancelInput(TRequestStatus&)
	{
	iComm.ReadCancel();
	}
