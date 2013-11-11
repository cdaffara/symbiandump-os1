// drvinfo.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <f32file.h>
#include <fshell/ioutils.h>

using namespace IoUtils;

class CCmdDrvinfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdDrvinfo();
private:
	CCmdDrvinfo();
	void ArgumentErrorL();
	void PrintDriveInfoL(TInt aDriveNum);
	void FormatVolInfoL(const TVolumeInfo& volInfo, IoUtils::CTextBuffer& aPrintBuf);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	HBufC* iDriveLetter;
	TBool iVerbose;
	TBool iHuman;
	};


CCommandBase* CCmdDrvinfo::NewLC()
	{
	CCmdDrvinfo* self = new(ELeave) CCmdDrvinfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDrvinfo::~CCmdDrvinfo()
	{
	delete iDriveLetter;
	}

CCmdDrvinfo::CCmdDrvinfo()
	{
	}

void CCmdDrvinfo::ArgumentErrorL()
	{
	Stderr().Write(_L("Invalid drive specification - use \'<drive_letter>:\', e.g. \'drvinfo c:\'\r\n"));
	User::Leave(KErrArgument);
	}

void FormatLineEndL(IoUtils::CTextBuffer& aPrintBuf)
	{
	_LIT(KSeparator, " | ");
	if (aPrintBuf.Descriptor().Right(KSeparator().Length()) == KSeparator)
		{
		aPrintBuf.Delete(aPrintBuf.Length() - KSeparator().Length(), KSeparator().Length());
		}
	if (aPrintBuf.Descriptor()[aPrintBuf.Length() - 1] == '(')
		{
		aPrintBuf.Delete(aPrintBuf.Length() - 2, 2);
		}
	else
		{
		aPrintBuf.AppendL(_L(")"));
		}
	aPrintBuf.AppendL(_L("\r\n"));
	}

void FormatDrvMediaTypeInfoL(const TDriveInfo& aDrvInfo, IoUtils::CTextBuffer& aPrintBuf)
	{
	aPrintBuf.AppendFormatL(_L("TMediaType:\t%d ("), aDrvInfo.iType);
	switch(aDrvInfo.iType)
		{
		case EMediaNotPresent:  aPrintBuf.AppendL(_L("EMediaNotPresent"));   break;
		case EMediaUnknown:	    aPrintBuf.AppendL(_L("EMediaUnknown"));      break;
		case EMediaFloppy:      aPrintBuf.AppendL(_L("EMediaFloppy"));       break;
		case EMediaHardDisk:    aPrintBuf.AppendL(_L("EMediaHardDisk"));     break;
		case EMediaCdRom:		aPrintBuf.AppendL(_L("EMediaCdRom"));        break;
		case EMediaRam:         aPrintBuf.AppendL(_L("EMediaRam"));          break;
		case EMediaFlash:       aPrintBuf.AppendL(_L("EMediaFlash"));        break;
		case EMediaRom:         aPrintBuf.AppendL(_L("EMediaRom"));          break;
		case EMediaRemote:      aPrintBuf.AppendL(_L("EMediaRemote"));       break;
		case EMediaNANDFlash:   aPrintBuf.AppendL(_L("EMediaNANDFlash"));    break;
		default:                aPrintBuf.AppendL(_L("??? Unknown Type"));   break;
		};
	FormatLineEndL(aPrintBuf);
	}

void FormatDriveAttInfoL(const TDriveInfo& aDrvInfo, IoUtils::CTextBuffer& aPrintBuf)
	{
	aPrintBuf.AppendFormatL(_L("DriveAtt:\t0x%x ("), aDrvInfo.iDriveAtt);
	if (aDrvInfo.iDriveAtt & KDriveAttLocal)         aPrintBuf.AppendL(_L("KDriveAttLocal | "));
	if (aDrvInfo.iDriveAtt & KDriveAttRom)           aPrintBuf.AppendL(_L("KDriveAttRom | "));
	if (aDrvInfo.iDriveAtt & KDriveAttRedirected)    aPrintBuf.AppendL(_L("KDriveAttRedirected | "));
	if (aDrvInfo.iDriveAtt & KDriveAttSubsted)       aPrintBuf.AppendL(_L("KDriveAttSubsted | "));
	if (aDrvInfo.iDriveAtt & KDriveAttInternal)      aPrintBuf.AppendL(_L("KDriveAttInternal | "));
	if (aDrvInfo.iDriveAtt & KDriveAttRemovable)     aPrintBuf.AppendL(_L("KDriveAttRemovable | "));
	if (aDrvInfo.iDriveAtt & KDriveAttRemote)        aPrintBuf.AppendL(_L("KDriveAttRemote | "));
	if (aDrvInfo.iDriveAtt & KDriveAttTransaction)   aPrintBuf.AppendL(_L("KDriveAttTransaction | "));
	FormatLineEndL(aPrintBuf);
	}

void FormatMediaAttInfoL(const TDriveInfo& aDrvInfo, IoUtils::CTextBuffer& aPrintBuf)
	{
	aPrintBuf.AppendFormatL(_L("MediaAtt:\t0x%x ("),aDrvInfo.iMediaAtt);
	if (aDrvInfo.iMediaAtt & KMediaAttVariableSize)      aPrintBuf.AppendL(_L("KMediaAttVariableSize | "));
	if (aDrvInfo.iMediaAtt & KMediaAttDualDensity)       aPrintBuf.AppendL(_L("KMediaAttDualDensity | "));
	if (aDrvInfo.iMediaAtt & KMediaAttFormattable)       aPrintBuf.AppendL(_L("KMediaAttFormattable | "));
	if (aDrvInfo.iMediaAtt & KMediaAttWriteProtected)    aPrintBuf.AppendL(_L("KMediaAttWriteProtected | "));
	if (aDrvInfo.iMediaAtt & KMediaAttLockable)          aPrintBuf.AppendL(_L("KMediaAttLockable | "));
	if (aDrvInfo.iMediaAtt & KMediaAttLocked)            aPrintBuf.AppendL(_L("KMediaAttLocked | "));
	if (aDrvInfo.iMediaAtt & KMediaAttHasPassword)       aPrintBuf.AppendL(_L("KMediaAttHasPassword | "));
	if (aDrvInfo.iMediaAtt & KMediaAttReadWhileWrite)    aPrintBuf.AppendL(_L("KMediaAttReadWhileWrite | "));
	if (aDrvInfo.iMediaAtt & KMediaAttDeleteNotify)      aPrintBuf.AppendL(_L("KMediaAttDeleteNotify | "));
	FormatLineEndL(aPrintBuf);
	}

void CCmdDrvinfo::FormatVolInfoL(const TVolumeInfo& volInfo, IoUtils::CTextBuffer& aPrintBuf)
	{
	if (iVerbose)
		{
		aPrintBuf.AppendFormatL(_L("VolId:\t0x%x\r\n"), volInfo.iUniqueID);
		}
	aPrintBuf.AppendL(_L("VolSize:\t"));
	if (iHuman)
		{
		aPrintBuf.AppendHumanReadableSizeL(volInfo.iSize);
		aPrintBuf.AppendL(_L("\r\n"));
		}
	else
		{
		aPrintBuf.AppendFormatL(_L("%ld\r\n"), volInfo.iSize);
		}
	aPrintBuf.AppendL(_L("Free:\t"));
	if (iHuman)
		{
		aPrintBuf.AppendHumanReadableSizeL(volInfo.iFree);
		aPrintBuf.AppendL(_L("\r\n"));
		}
	else
		{
		aPrintBuf.AppendFormatL(_L("%ld\r\n"), volInfo.iFree);
		}
	if (iVerbose)
		{
		aPrintBuf.AppendFormatL(_L("VolName:\t\'%S\'"), &volInfo.iName);
		}
	aPrintBuf.AppendL(_L("\r\n"));
	}

void CCmdDrvinfo::PrintDriveInfoL(TInt aDriveNum)
	{
	IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x100);

	TDriveInfo 	driveInfo;
	User::LeaveIfError(FsL().Drive(driveInfo, aDriveNum));

	TVolumeInfo volInfo;
	User::LeaveIfError(Fs().Volume(volInfo, aDriveNum));

	if (iVerbose || (iDriveLetter == NULL))
		{
		buf->AppendFormatL(_L("Drive:\t%c:\r\n"), 'A'+ aDriveNum);
		}

	if (iVerbose)
		{
		buf->AppendFormatL(_L("Number:\t%d\r\n"), aDriveNum);

		TBuf<256> tbuf;
		if (Fs().FileSystemName(tbuf, aDriveNum) == KErrNone)
			{
			buf->AppendFormatL(_L("FS name:\t\'%S\'\r\n"), &tbuf);
			}

		FormatDrvMediaTypeInfoL(driveInfo, *buf);
		buf->AppendFormatL(_L("BatteryState:\t%d\r\n"),driveInfo.iBattery);
		FormatDriveAttInfoL(driveInfo, *buf);
		FormatMediaAttInfoL(driveInfo, *buf);
		}

	FormatVolInfoL(volInfo, *buf);

	CTextFormatter* formatter = CTextFormatter::NewLC(Stdout());
	formatter->TabulateL(0, 2, buf->Descriptor());
	Write(formatter->Descriptor());

	CleanupStack::PopAndDestroy(2, buf);
	}

const TDesC& CCmdDrvinfo::Name() const
	{
	_LIT(KName, "drvinfo");	
	return KName;
	}

void CCmdDrvinfo::DoRunL()
	{
	TInt driveNum = -1;

	if (iDriveLetter)
		{
		const TInt len = iDriveLetter->Length();

		if ((len < 2) || (len > 3) || ((*iDriveLetter)[1] != ':'))
			{
			ArgumentErrorL();
			}

		if ((len == 3) && ((*iDriveLetter)[2] != '\\'))
			{
			ArgumentErrorL();
			}

		driveNum = TChar((*iDriveLetter)[0]).GetUpperCase() - 'A';

		if (driveNum > EDriveZ)
			{
			ArgumentErrorL();
			}
		}


	TDriveList driveList;
	User::LeaveIfError(FsL().DriveList(driveList));

	if (driveNum >= 0)
		{
		if (!driveList[driveNum])
			{
			ArgumentErrorL();
			}

		PrintDriveInfoL(driveNum);
		}
	else
		{
		for (TInt i = 0; i < KMaxDrives; ++i)
			{
			if (driveList[i])
				{
				TRAPD(err, PrintDriveInfoL(i));
				if (err)
					{
					PrintWarning(_L("Couldn't read drive info for %c"), 'A'+ i);
					}
				Printf(_L("\r\n"));
				}
			}
		}
	}

void CCmdDrvinfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg, "drive_letter");
	aArguments.AppendStringL(iDriveLetter, KArg);
	}

void CCmdDrvinfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	_LIT(KOptHuman, "human");

	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	aOptions.AppendBoolL(iHuman, KOptHuman);
	}


EXE_BOILER_PLATE(CCmdDrvinfo)

