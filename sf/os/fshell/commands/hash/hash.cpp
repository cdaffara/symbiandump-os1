// hash.cpp
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

#include <fshell/ioutils.h>
#include <fshell/ltkutils.h>
#include <fshell/common.mmh>
#include <hash.h>

const TInt KBlockSize = 512;

using namespace IoUtils;

class CCmdHash : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdHash();
private:
	CCmdHash();
	CMessageDigest::THashId HashIdL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iVerbose;
	TFileName2 iInputFileName;
	TFileName2 iOutputFileName;
	enum TAlgorithm
		{
		ESha1,
		ESha224,
		ESha256,
		ESha384,
		ESha512,
		EMd2,
		EMd4,
		EMd5
		} iAlgorithm;
	CMessageDigest* iMessageDigest;
	};


CCommandBase* CCmdHash::NewLC()
	{
	CCmdHash* self = new(ELeave) CCmdHash();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdHash::~CCmdHash()
	{
	delete iMessageDigest;
	}

CCmdHash::CCmdHash()
	{
	}

CMessageDigest::THashId CCmdHash::HashIdL()
	{
	switch (iAlgorithm)
		{
		case ESha1:
			return CMessageDigest::ESHA1;
		case EMd2:
			return CMessageDigest::EMD2;
#ifdef FSHELL_MD4_SUPPORT
		case EMd4:
			return CMessageDigest::EMD4;
#endif // FSHELL_MD4_SUPPORT
		case EMd5:
			return CMessageDigest::EMD5;
#ifdef FSHELL_SHA2_SUPPORT
		case ESha224:
			return CMessageDigest::ESHA224;
		case ESha256:
			return CMessageDigest::ESHA256;
		case ESha384:
			return CMessageDigest::ESHA384;
		case ESha512:
			return CMessageDigest::ESHA512;
#endif // FSHELL_SHA2_SUPPORT
		default:
			LeaveIfErr(KErrNotSupported, _L("Requested hashing algorithm is not available in this Symbian OS platform"));
			return CMessageDigest::ESHA1;
		}
	}

const TDesC& CCmdHash::Name() const
	{
	_LIT(KName, "hash");	
	return KName;
	}

void CCmdHash::DoRunL()
	{
	iMessageDigest = CMessageDigestFactory::NewDigestL(HashIdL());

	RIoReadHandle readHandle;
	readHandle.Create(IoSession());
	CleanupClosePushL(readHandle);

	RIoFile ioFile;
	if (iOptions.IsPresent(&iInputFileName))
		{
		LeaveIfErr(ioFile.Create(IoSession(), iInputFileName, RIoFile::ERead), _L("Couldn't open handle to file '%S'"), &iInputFileName);
		CleanupClosePushL(ioFile);
		LeaveIfErr(ioFile.Attach(readHandle, RIoEndPoint::EForeground), _L("Couldn't attach read handle to file object"));
		}
	else
		{
		LeaveIfErr(readHandle.Duplicate(Stdin()), _L("Couldn't duplicate STDIN"));
		}

	LeaveIfErr(readHandle.SetReadMode(RIoReadHandle::EOneOrMore), _L("Couldn't set read handle into read one or more mode"));

	TBuf<KBlockSize> buf;
	TInt err = KErrNone;
	do
		{
		err = readHandle.Read(buf);
		if (err == KErrNone)
			{
			iMessageDigest->Update(buf.Collapse());
			}
		}
		while (err == KErrNone);

	if (err != KErrEof)
		{
		LeaveIfErr(err, _L("Unable to read input"));
		}

	TPtrC8 hash(iMessageDigest->Final());

	if (iOptions.IsPresent(&iOutputFileName))
		{
		RFile outputFile;
		LeaveIfErr(outputFile.Create(FsL(), iOutputFileName, EFileWrite), _L("Couldn't create '%S'"), &iOutputFileName);
		CleanupClosePushL(outputFile);
		outputFile.Write(hash);
		CleanupStack::PopAndDestroy(&outputFile);
		}
	else
		{
		LtkUtils::HexDumpToOutput(hash, Stdout());
		}

	if (iOptions.IsPresent(&iInputFileName))
		{
		CleanupStack::PopAndDestroy(&ioFile);
		}

	CleanupStack::PopAndDestroy(&readHandle);
	}

void CCmdHash::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	aOptions.AppendFileNameL(iInputFileName, _L("input"));
	aOptions.AppendFileNameL(iOutputFileName, _L("output"));
	aOptions.AppendEnumL((TInt&)iAlgorithm, _L("algorithm"));
	}


EXE_BOILER_PLATE(CCmdHash)

