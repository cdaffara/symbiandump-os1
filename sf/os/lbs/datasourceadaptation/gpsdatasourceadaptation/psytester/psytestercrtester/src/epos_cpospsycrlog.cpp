// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <centralrepository.h> 
#include <ecom/ecom.h>
#include <f32file.h>
#include <bautils.h>

#include "epos_cpospsycrlog.h"

// Constant Definitions
const TUid KPosPsyInterfaceUid = {0x101f7a7c};
const TInt KMaximumNameLength = 25;
_LIT(KPsyTesterDir, ":\\logs\\psytester\\");


// Global Functions
// ---------------------------------------------------------
// ResetAndDestroy Resets and destroys the members in a RImplInfoPtrArray. This
// function can be used as a TCleanupItem.
// ---------------------------------------------------------
//
void ResetAndDestroy(
    TAny* aArray)
    {
    ((RImplInfoPtrArray*)aArray)->ResetAndDestroy();
    }
    
    
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCRLog::CPosPSYCRLog()
    {
    }

// EPOC default constructor can leave.
void CPosPSYCRLog::ConstructL()
    {
   		User::LeaveIfError(iFileSession.Connect());
    }

// Two-phased constructor.
CPosPSYCRLog* CPosPSYCRLog::NewL()
    {
    CPosPSYCRLog* self = new (ELeave) CPosPSYCRLog;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYCRLog::~CPosPSYCRLog()
    {
    REComSession::FinalClose();
    iFileSession.Close();
    }

// ---------------------------------------------------------
// CPosPSYLog::OpenLogL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//   
void CPosPSYCRLog::OpenLogL(TUid aUid )
    {
        // Get the name of the PSY
        RImplInfoPtrArray implInfoArray;
        CleanupStack::PushL(TCleanupItem(*ResetAndDestroy, &implInfoArray));
    	REComSession::ListImplementationsL(KPosPsyInterfaceUid, implInfoArray);
    	
    	TInt found = 0, index;
    	
    	for (index = 0; index < implInfoArray.Count() && !found; index++)
        {
        	found = (aUid == implInfoArray[index]->ImplementationUid());
        }
    	if (!found)
        {
        	User::Leave(KErrNotFound);
        }
        
        if(index > 0) 
        	index --;
        
        TBuf<KMaximumNameLength> psyDisplayName;
        const TDesC& displayName = implInfoArray[index]->DisplayName();
        TInt copyLength = Min(displayName.Length(), KMaximumNameLength);
        psyDisplayName.Copy(displayName.Left(copyLength));
        
        TLex psyName(psyDisplayName);
    	
    	TBuf<KMaxFileName> fileName;
    	
        TDriveNumber cDrive;
        User::LeaveIfError(BaflUtils::GetSystemDrive(cDrive));
        TChar cDriveChar;
        User::LeaveIfError(RFs::DriveToChar(cDrive, cDriveChar));
        TFileName crTesterExe;
        fileName.Append(cDriveChar);
    	fileName.Append(KPsyTesterDir);
    	
    	while (!psyName.Eos())
		{
		if (psyName.Peek().IsDigit() || psyName.Peek().IsAlpha())
			{
			fileName.Append(psyName.Get());
			}
		else
			{
			psyName.Inc();
			}
		}
		
		fileName.Append('_');
    	
    	TLex psyUidLex(aUid.Name());
    	
		while (!psyUidLex.Eos())
		{
		if (psyUidLex.Peek() != '[' && psyUidLex.Peek() != ']')
			{
			fileName.Append(psyUidLex.Get());
			}
		else
			{
			psyUidLex.Inc();
			}
		}
		
		_LIT(KLogExtension,".txt");
		
		fileName.Append(KLogExtension);
		
		TInt error = iCurrentLogFile.Open(iFileSession, fileName, EFileWrite | EFileShareExclusive);
		if(KErrNone != error)
		{
			User::LeaveIfError(error);
		}
		
		iCurrentLogFile.Seek(ESeekEnd, iCurrentPosition);
		
		CleanupStack::PopAndDestroy(&implInfoArray);
		
		return;
    }

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------

void CPosPSYCRLog::CloseLog()
{
	iCurrentLogFile.Flush();
	iCurrentLogFile.Close();
}
// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYCRLog::AppendLineL(
    const TDesC& aLine)
    {
    HBufC8* buf;
    buf = HBufC8::NewLC(aLine.Length());
    TPtr8 ptr = buf->Des();
    ptr.Zero();
    ptr.Append(aLine);
    iCurrentLogFile.Write(ptr);
    CleanupStack::PopAndDestroy(buf);
    }

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYCRLog::AppendLineL(
    const TInt aNumber)
    {
    TBuf<KMaxNumberSize> number;
    number.AppendNum(aNumber);
    AppendLineL(number);
    }

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CPosPSYCRLog::AppendInfoMsgL(TDesC &aMsg)
{
	AppendLineL(KInfoMessage);
	AppendLineL(aMsg);
	AppendLineL(KNewLine);
}

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CPosPSYCRLog::AppendWarningMsgL(TDesC &aMsg)
{
	AppendLineL(KWarningMessage);
	AppendLineL(aMsg);
	AppendLineL(KNewLine);
}

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CPosPSYCRLog::AppendErrorMsgL(TDesC &aMsg)
{
	AppendLineL(KErrorMessage);
	AppendLineL(aMsg);
	AppendLineL(KNewLine);
}
//  End of File
