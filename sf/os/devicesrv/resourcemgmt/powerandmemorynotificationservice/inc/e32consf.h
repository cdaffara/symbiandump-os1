// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __E32CONSF_H__
#define __E32CONSF_H__

#include <e32std.h> 
#include <e32cons.h>
#include <e32test.h>
#include <f32file.h>

#include <utf.h>

class CConsoleFile: public CConsoleBase 
    {
public:
    static CConsoleFile* New(const TDesC& aLogFileName)
        {
        CConsoleFile* self = new CConsoleFile();
        if (self)
            {
            if (self->Construct(aLogFileName))
                {
                delete self;
                self = NULL;
                }
            }
        return self;
        }
    
    virtual ~CConsoleFile()
        {
        iFile.Close();
        iFs.Close();
        }

private:
    CConsoleFile()
    :CConsoleBase()
        {
        }
    
    TInt Construct(const TDesC& aLogFileName)
        {
        TInt err=iFs.Connect();
        if (!err)
            {
            (void)iFs.MkDirAll(aLogFileName);
            if (iFile.Replace(iFs, aLogFileName, EFileShareExclusive | EFileWrite))
                {
                err=iFile.Create(iFs, aLogFileName, EFileShareExclusive | EFileWrite);            
                }
            }
        return err;
        }

    void DoWriteL(const TDesC &aDes)
        {
        HBufC8* outBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aDes);
        (void)iFile.Write(*outBuf);
        delete outBuf;
        }
    
private: // CConsoleBase
    virtual TInt Create(const TDesC& /*aTitle*/,TSize /*aSize*/)
        {
        return KErrNone;
        }
    
    virtual void Read(TRequestStatus& aStatus)
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, (TInt)EKeyNull);
        }
    
    virtual void ReadCancel()
        {
        }
    
    virtual void Write(const TDesC &aDes)
        {
        // so that RTest::Title can be called before the TrapHandler has been created
        CTrapCleanup* tc = NULL;
        if (!User::TrapHandler())
            {
            tc = CTrapCleanup::New();
            }
        TRAP_IGNORE(DoWriteL(aDes));
        delete tc;
        }
    
    virtual TPoint CursorPos() const
        {
        return TPoint(0,0);
        }
    
    virtual void SetCursorPosAbs(const TPoint& /*aPoint*/)
        {
        }
    
    virtual void SetCursorPosRel(const TPoint& /*aPoint*/)
        {
        }
    
    virtual void SetCursorHeight(TInt /*aPercentage*/)
        {
        }
    
    virtual void SetTitle(const TDesC& /*aTitle*/)
        {
        }
    
    virtual void ClearScreen()
        {
        }
    
    virtual void ClearToEndOfLine()
        {
        }
    
    virtual TSize ScreenSize() const
        {
        return TSize(0,0);
        }
    
    virtual TKeyCode KeyCode() const
        {
        return EKeyNull;
        }
    
    virtual TUint KeyModifiers() const
        {
        return 0;
        }

private:
    RFs iFs;
    RFile iFile;
    };

void LogRTestToFile(RTest& aTest)
    {
    RProcess myProcess;
    TParsePtrC parsePtr(myProcess.FileName());
	TFileName logFileName;
	TPtrC fileNameWithoutExtension = parsePtr.Name();
    logFileName.Format(_L("c:\\logs\\testexecute\\%S.htm"), &fileNameWithoutExtension);
    aTest.SetConsole(CConsoleFile::New(logFileName)); // SMH - Setting NULL console is OK
    }

#endif // __E32CONSF_H__
