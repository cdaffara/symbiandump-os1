// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "LcfFIPluginController.h"
#include <epos_privacy.h>


// Constants for Input file contents
_LIT(KAccept,    "Accept");
_LIT(KReject,    "Reject");
_LIT(KTimeout,   "Timeout");
_LIT(KAcceptAll, "AcceptAll");
_LIT(KRejectAll, "RejectAll");
_LIT(KTimeoutAll,"TimeoutAll");

_LIT(KCompleteThis,   "CompleteThisRequest");
_LIT(KCompleteWithId, "CompleteRequestId");
_LIT(KCompleteAll,    "CompleteAllRequests");

// Constants for exit reason
_LIT(KExitAccepted,           "Accepted");
_LIT(KExitRejected,           "Rejected");
_LIT(KExitTimedOut,           "TimedOut");
_LIT(KExitAllAccepted,        "AllAccepted");
_LIT(KExitAllRejected,        "AllRejected");
_LIT(KExitCancelledByEpos,    "CancelledByEpos");
_LIT(KExitAllCancelledByEpos, "AllCancelledByEpos");
_LIT(KExitAllTimedOut,        "AllTimedOut");
_LIT(KExitRequestCompleted,   "RequestCompleted");

// Constants for requestor type
_LIT(KRequestorContact, "Contact");
_LIT(KRequestorService, "Service");
_LIT(KRequestorUnknown, "Unknown");

// Constants for requestor format
_LIT(KFormatMail,        "E-Mail");
_LIT(KFormatUrl,         "URL");
_LIT(KFormatTelephone,   "PhoneNumber");
_LIT(KFormatGenericName, "Name");
_LIT(KFormatSIPURL,      "SIPURL");
_LIT(KFormatIMSPublicIdentity, "IMSPublicIdentity");
_LIT(KFormatMIN, "MIN");
_LIT(KFormatMDN, "MDN");
_LIT(KFormatUnkown,      "Unknown");

// Contants for data base matches
_LIT(KSingleMatch,    "Yes");
_LIT(KMultipleMathes, "Multiple");
_LIT(KNoMatch,        "No");

// Constants for file names
_LIT(KInputFileName,  "c:\\system\\test\\Lcf_UIPlugin_In.txt");
_LIT(KOutputFileName, "c:\\system\\test\\Lcf_UIPlugin_Out.txt");

_LIT(KNextVerification, "c:\\system\\test\\Lcf_UIPlugin_Next_V_In.txt");
_LIT(KNextNotification, "c:\\system\\test\\Lcf_UIPlugin_Next_N_In.txt");

_LIT(KResetFile,        "c:\\system\\test\\Lcf_UIPlugin_Reset.txt");

_LIT(KTestFilePath,     "c:\\system\\test\\");

_LIT(KLcfSeparator,          ":");
_LIT(KLcfRequestorSeparator, ",");

// Numeric constants
const TInt KMaxOutFileLineSize = 500;


//
// CLcfRequestor class
//
EXPORT_C CLcfRequestor* CLcfRequestor::NewL()
    {
    CLcfRequestor* self = NewLC();
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CLcfRequestor* CLcfRequestor::NewLC()
    {
    CLcfRequestor* self = new (ELeave) CLcfRequestor();
    CleanupStack::PushL(self);
    return self;
    }


// C++ destructor.
//
EXPORT_C CLcfRequestor::~CLcfRequestor()
    {
    }


// C++ constructor
//
CLcfRequestor::CLcfRequestor()
    {
    iIdString.Zero();
    iDataBase.Zero();
    }


// C++ copy constructor
//
EXPORT_C CLcfRequestor::CLcfRequestor(const CLcfRequestor& aRequestor)
    {
    iType               = aRequestor.iType;
    iFormat             = aRequestor.iFormat;
    iIdString.Copy(aRequestor.iIdString);
    iHasDataBaseRef     = aRequestor.iHasDataBaseRef;
    iHasMultipleMatches = aRequestor.iHasMultipleMatches;
    iDataBaseRef        = aRequestor.iDataBaseRef;
    iDataBase.Copy(aRequestor.iDataBase);
    }


// C++ assignment operator
//
EXPORT_C CLcfRequestor& CLcfRequestor::operator=(const CLcfRequestor& aRequestor)
    {
    iType               = aRequestor.iType;
    iFormat             = aRequestor.iFormat;
    iIdString.Copy(aRequestor.iIdString);
    iHasDataBaseRef     = aRequestor.iHasDataBaseRef;
    iHasMultipleMatches = aRequestor.iHasMultipleMatches;
    iDataBaseRef        = aRequestor.iDataBaseRef;
    iDataBase.Copy(aRequestor.iDataBase);
	iSource				= aRequestor.iSource;
	
    return *this;
    }


//
// class CLcfPluginDiagnostics
//

// Constructors and destructors


// NewL constructor.
// 
EXPORT_C CLcfPluginDiagnostics* CLcfPluginDiagnostics::NewL()
    {
    CLcfPluginDiagnostics* self = CLcfPluginDiagnostics::NewLC();
    CleanupStack::Pop();
    return self;

    }


// NewLC constructor.
//
EXPORT_C CLcfPluginDiagnostics* CLcfPluginDiagnostics::NewLC()
    {
    CLcfPluginDiagnostics* self = new (ELeave) CLcfPluginDiagnostics;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;

    }


// C++ destructor.
EXPORT_C CLcfPluginDiagnostics::~CLcfPluginDiagnostics()
    {
    iRequestorArray->ResetAndDestroy();
    delete iRequestorArray;
    }


// C++ constructor.
// 
CLcfPluginDiagnostics::CLcfPluginDiagnostics()
    {
    }


// Second phase contruction.
// 
void CLcfPluginDiagnostics::ConstructL()
    {
    iRequestorArray = new (ELeave) CArrayPtrFlat<CLcfRequestor>(1);
    }


//
// Exported functions
//


// This function return the request id.
// 
EXPORT_C TInt CLcfPluginDiagnostics::RequestId() const
    {
    return iRequestId;
    }


// This function returns ETrue if this was a verification. In
// case this was notification, EFalse is returned.
// 
EXPORT_C TBool CLcfPluginDiagnostics::VerificationRequest() const
    {
    return iThisWasVerification;
    }


// This function returns the reason why this request was dismissed.
// 
EXPORT_C TLcfPluginExitReason CLcfPluginDiagnostics::ExitReason() const
    {
    return iExitReason;
    }


// This function returns the amount of requestors in this request.
// 
EXPORT_C TInt CLcfPluginDiagnostics::NumberOfRequestors() const
    {
    if (iRequestorArray)
        {
        return iRequestorArray->Count();
        }
    else 
        {
        return 0;
        }
    }


// This function returns the requestor of given index. if index
// is out of range, function leaves with KErrArgument.
// Client must delete returned requestor afer use.
// 
EXPORT_C CLcfRequestor* CLcfPluginDiagnostics::RequestorL(TInt aIndex) const
    {
    if (aIndex < 0 || aIndex >= iRequestorArray->Count())
        User::Leave(KErrArgument);

    CLcfRequestor* wantedRequestor = CLcfRequestor::NewL();
    *wantedRequestor = *(*iRequestorArray)[aIndex];

    return wantedRequestor;
    }  

  
// This function returns the requestor of given index. if index
// is out of range, function leaves with KErrArgument.
// Client must delete returned requestor afer use.
// 
EXPORT_C CLcfRequestor* CLcfPluginDiagnostics::RequestorLC(TInt aIndex) const
    {
    if (aIndex < 0 || aIndex >= iRequestorArray->Count())
        User::Leave(KErrArgument);

    CLcfRequestor* wantedRequestor = CLcfRequestor::NewLC();
    *wantedRequestor = *(*iRequestorArray)[aIndex];

    return wantedRequestor;
    }    


// This function sets the request id.
// 
EXPORT_C void CLcfPluginDiagnostics::SetRequestId(TInt aRequestId)
    {
    iRequestId = aRequestId;
    }


// This function sets the boolean value that indicates if this
// was a verification or notification.
// 
EXPORT_C void CLcfPluginDiagnostics::SetIsVerificationRequest(TBool aIsVerification)
    {
    iThisWasVerification = aIsVerification;
    }


// This function sets the exit reason.
// 
EXPORT_C void CLcfPluginDiagnostics::SetExitReason(TLcfPluginExitReason aReason)
    {
    iExitReason = aReason;
    }


// This function adds a requestor for this request.
// 
EXPORT_C void CLcfPluginDiagnostics::AddRequestorLD(CLcfRequestor* aRequestor)
    {
    iRequestorArray->AppendL(aRequestor);
    }



//
// class CLcfNotifierManager
//


// NewL constructor.
// 
// Parameters:
//   aDeleteFiles - If this is ETrue, then all files are deleted
//                  and reset file is created.
// 
EXPORT_C CLcfNotifierManager* CLcfNotifierManager::NewL(TBool aDeleteFiles)
    {
    CLcfNotifierManager* self = CLcfNotifierManager::NewLC(aDeleteFiles);
    CleanupStack::Pop(); // self
    return self;
    }


// NewLC constructor.
// 
// Parameters:
//   aDeleteFiles - If this is ETrue, then all files are deleted
//                  and reset file is created.
// 
EXPORT_C CLcfNotifierManager* CLcfNotifierManager::NewLC(TBool aDeleteFiles)
    {
    CLcfNotifierManager* self = new (ELeave) CLcfNotifierManager;
    CleanupStack::PushL(self);
    self->ConstructL(aDeleteFiles);
    return self;
    }


// Second phase construction.
// 
void CLcfNotifierManager::ConstructL(TBool aDeleteFiles)
    {
    // Anycase create path here, because otherwise there can
    // be multiple errors when trying to open files
    User::LeaveIfError(iFs.Connect());

    TEntry entry;
    TInt err =  iFs.Entry(KTestFilePath, entry);

    if (err != KErrNone)
        {
        err = iFs.MkDir(KTestFilePath);

        if (err!= KErrNone)
            {
            iFs.Close();
            User::Leave(err);
            }
        }

    iFs.Close();

    if (aDeleteFiles)
        {
        ClearAllInputAndOutputL();
        }
    }


// C++ destructor.
// 
EXPORT_C CLcfNotifierManager::~CLcfNotifierManager()
    {
    }


// Sets the answer for specific notification.
//
// Parameters:
//   aNb     - Ordinal number of the notification (zero is first notification)
//   aAnswer - Answer to the notification.
//   aDelay  - Time to wait before nofification is answered (in seconds).
//  
EXPORT_C void CLcfNotifierManager::SetNotificationAnswerL(TInt aNb, TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    TBuf<10> start;

    start.Copy(_L("N"));

    start.AppendNum(aNb);

    AppendAnswer(start, aAnswer, aDelay);

    }


// Sets default answer for  notifications. This answer is used for those
// notifications that don't have specific answer.
//
// Parameters:
//   aAnswer - Answer to the notification.
//   aDelay  - Time to wait before nofification is answered (in seconds).
//  
EXPORT_C void CLcfNotifierManager::SetDefaultNotificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    TBuf<10> start;

    start.Copy(_L("ND"));

    AppendAnswer(start, aAnswer, aDelay);

    }


// Sets the answer for specific verification.
//
// Parameters:
//   aNb     - Ordinal number of the verification (zero is first verification)
//   aAnswer - Answer to the verification.
//   aDelay  - Time to wait before verification is answered (in seconds).
//  
EXPORT_C void CLcfNotifierManager::SetVerificationAnswerL(TInt aNb, TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    TBuf<10> start;

    start.Copy(_L("V"));

    start.AppendNum(aNb);

    AppendAnswer(start, aAnswer, aDelay);
    }


// Sets default answer for  verifications. This answer is used for those
// verifications that don't have specific answer.
//
// Parameters:
//   aAnswer - Answer to the verification.
//   aDelay  - Time to wait before verification is answered (in seconds).
//  
EXPORT_C void CLcfNotifierManager::SetDefaultVerificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    TBuf<10> start;

    start.Copy(_L("VD"));

    AppendAnswer(start, aAnswer, aDelay);

    }


// Sets answer for next verification. This overrides any default or specific
// answers for next verification.
//
//   aAnswer - Answer to the verification.
//   aDelay  - Time to wait before verification is answered (in seconds).
// 
EXPORT_C void CLcfNotifierManager::SetNextVerificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    SetNextAnswerL(KNextVerification,_L("V"), aAnswer, aDelay);
    }


// Sets answer for next notification. This overrides any default or specific
// answers for next notification.
//
//   aAnswer - Answer to the notification.
//   aDelay  - Time to wait before notification is answered (in seconds).
// 
EXPORT_C void CLcfNotifierManager::SetNextNotificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    SetNextAnswerL(KNextNotification,_L("N"), aAnswer, aDelay);
    }


// This function returns diagnostics for a specific request (verification 
// or notification).
//
// Parameters:
//   aNb          - Number of request (first is number zero).
//   aDiagnostics - This pointer will point to new diagnostics instance.
//                  User must delete this pointer after use.
// 
EXPORT_C void CLcfNotifierManager::GetDiagnosticsL(TInt aNb, CLcfPluginDiagnostics*& aDiagnostics)
    {
    TInt err = OpenFileForRead(KOutputFileName);

    if (err != KErrNone)
        {
        iFile.Close();
        iFs.Close();

        User::Leave(err);
        }

    HBufC* lineBuf = HBufC::NewLC(KMaxOutFileLineSize);
    TPtr linePtr = lineBuf->Des();

    // Get to correct line and read it
    for (TInt i=0; i <= aNb;i++)
        {
        User::LeaveIfError(GetNextLine(linePtr));
        }

    iFile.Close();
    iFs.Close();

    CreateDiagnosticsFromDescriptorL(linePtr, aDiagnostics);

    CleanupStack::PopAndDestroy(); // lineBuf
    }


// Returns number of diagnostics available (i.e. total number of
// verifications and notifications). In case of an error
// may return negative number (error code).
// 
EXPORT_C TInt CLcfNotifierManager::NbOfDiagnostics()
    {
    TInt err = OpenFileForRead(KOutputFileName);

    // If file does not exist, return 0 as there are no request
    if (err != KErrNone)
        {
        iFile.Close();
        iFs.Close();

        if (err == KErrNotFound)
            {
            return 0;
            }
        else 
            {
            return err;
            }
        }

    // Otherwise count the lines in out -file
    HBufC* lineBuf = HBufC::NewLC(KMaxOutFileLineSize);
    TPtr linePtr = lineBuf->Des();

    TInt count = 0;
    while (GetNextLine(linePtr) == KErrNone)
        {
        count++;
        }

    CleanupStack::PopAndDestroy(); // lineBuf

    iFile.Close();
    iFs.Close();

    return count;
    }


// This function returns latest diagnostics.
//
// Parameters:
//   aDiagnostics - This pointer will point to new diagnostics instance.
//                  User must delete this pointer after use.
// 
EXPORT_C void CLcfNotifierManager::GetLatestDiagnosticsL(CLcfPluginDiagnostics*& aDiagnostics)
    {
    TInt err = OpenFileForRead(KOutputFileName);

    if (err != KErrNone)
        {
        iFile.Close();
        iFs.Close();

        User::Leave(err);
        }

    HBufC* lineBuf = HBufC::NewLC(KMaxOutFileLineSize);
    HBufC* tmpBuf  = HBufC::NewLC(KMaxOutFileLineSize);

    TPtr linePtr = lineBuf->Des();
    TPtr tmpPtr  = tmpBuf->Des();

    // Read lines as long as possible, only copy after succesfull read operation
    // (when read fails, result is an empty descriptor)
    while (GetNextLine(tmpPtr) == KErrNone)
        {
        linePtr.Copy(tmpPtr);
        }

    iFile.Close();
    iFs.Close();

    CreateDiagnosticsFromDescriptorL(linePtr, aDiagnostics);

    CleanupStack::PopAndDestroy(2); // lineBuf, tmpBuf

    }


// This file clears all input and creates the Reset file.
//
EXPORT_C void CLcfNotifierManager::ClearAllInputL()
    {
    DeleteFileL(KInputFileName);
    DeleteFileL(KNextVerification);
    DeleteFileL(KNextNotification);
    ResetPluginL();
    }


// This functions deletes thi Output file.
// 
EXPORT_C void CLcfNotifierManager::ClearAllOutputL()
    {
    DeleteFileL(KOutputFileName);
    iNbOfDiagnostics = 0;
    }


// This function is combination of ClearAllInputL and 
// and ClearAllOutputL.
// 
EXPORT_C void CLcfNotifierManager::ClearAllInputAndOutputL() 
    {
    ClearAllInputL();
    ClearAllOutputL();
    }


// This function creates the Reset file.
// 
EXPORT_C void CLcfNotifierManager::ResetPluginL()
    {
    User::LeaveIfError(iFs.Connect());

    TInt err = iFile.Create(iFs,KResetFile,EFileWrite);

    // c:\system\test migh not exist
    if (err == KErrPathNotFound)
        {
        // Create path, MkDir ignores file name
        // at end.
        err = iFs.MkDir(KResetFile);

        if (err!= KErrNone)
            {
            iFs.Close();
            User::Leave(err);
            }
        else 
            {
            err = iFile.Create(iFs,KResetFile,EFileWrite);
            }
        }


    if (err != KErrNone && err != KErrAlreadyExists)
        {
        iFs.Close();
        User::Leave(err);
        }

    iFile.Close();
    iFs.Close();
    }


// This function can be used to set "illegal" behaviour of ui-plugin.
// This is extension to original interface, and thus new function is
// used to avoid changes in test drivers that don't use this new
// function.
//
// Parameters:
//   aType - Defines if this behaviour is applied to some specific verification
//           or notification, default verification or notification or next
//           verification or notification.
//   aStyle - Defines if CompleteRequest or CompleteRequest is called, and
//            whether specific request id is used.
//   aNb    - Number of verification or notification (if applicable). For
//            default and next -cases is ignored.
//   aRequestId - Request id, if applicable.
//   aCompletionCode - Code which is used to complete request.
// 
EXPORT_C void CLcfNotifierManager::SetCompletionCodeL(TLcfPluginAnswerType aType, 
                                                      TLcfCompletionStyle aStyle, 
                                                      TInt aNb, 
                                                      TInt aRequestId, 
                                                      TInt aCompletionCode,
                                                      TReal aDelay)
    {
    TBuf<80> line;

    switch(aType)
        {
    case ELcfAnswerTypeNotification:
        OpenFileForAppend(KInputFileName);
        line.Copy(_L("N"));
        line.AppendNum(aNb);
        break;
    case ELcfAnswerTypeDefaultNotification:
        OpenFileForAppend(KInputFileName);
        line.Copy(_L("ND"));
        break;
    case ELcfAnswerTypeVerification:
        OpenFileForAppend(KInputFileName);
        line.Copy(_L("V"));
        line.AppendNum(aNb);
        break;
    case ELcfAnswerTypeDefaultVerification:
        OpenFileForAppend(KInputFileName);
        line.Copy(_L("VD"));
        break;
    case ELcfAnswerTypeNextNotification:
        User::LeaveIfError(iFs.Connect());
        User::LeaveIfError(iFile.Replace(iFs,KNextNotification,EFileStreamText|EFileWrite));
        line.Copy(_L("N"));
        break;
    case ELcfAnswerTypeNextVerification:
        User::LeaveIfError(iFs.Connect());
        User::LeaveIfError(iFile.Replace(iFs,KNextVerification,EFileStreamText|EFileWrite));
        line.Copy(_L("V"));
        break;
    default:
        iFs.Close();
        User::Leave(KErrNotFound);
        break;
        }

    line.Append(KLcfSeparator);

    iFileText.Set(iFile);


    switch(aStyle)
        {
    case ELcfCurrentRequest:
        line.Append(KCompleteThis);
        line.Append(_L("("));
        line.AppendNum(aCompletionCode);
        line.Append(_L(")"));
        break;
    case ELcfSpecificRequest:
        line.Append(KCompleteWithId);
        line.Append(_L("("));
        line.AppendNum(aRequestId);
        line.Append(_L(","));
        line.AppendNum(aCompletionCode);
        line.Append(_L(")"));
        break;
    case ELcfAllRequests:
        line.Append(KCompleteAll);
        line.Append(_L("("));
        line.AppendNum(aCompletionCode);
        line.Append(_L(")"));
        break;
    default:
        iFs.Close();
        iFile.Close();
        User::Leave(KErrNotFound);
        break;
        }
    

    line.Append(KLcfSeparator);

    TRealFormat realFormat;

    line.AppendNum(aDelay, realFormat);

    line.Append(_L("s\r"));

    iFileText.Write(line);

    iFile.Close();
    iFs.Close();    
    }



//
// Exported functions for Notifier
//


// This function appends new diagnostics at the end of output file.
//
// Parameters:
//   aDiagnostics - The diagnostics to append.
// 
EXPORT_C void CLcfNotifierManager::AppendDiagnosticsL(const CLcfPluginDiagnostics* aDiagnostics)
    {
    // Open "out" -file for appending
    OpenFileForAppend(KOutputFileName);

    // Create buffer for new line in out -file
    HBufC* line = HBufC::NewLC(KMaxOutFileLineSize);
    
    TPtr linePtr = line->Des();
    
    // This is a hack to get output file in such form, that windows
    // regognizes it as unicode. If this is first line of output file,
    // then insert unicode byte order mark at beginning of descriptor.
#ifdef __WINS__
    if (iFileCreated)
        {
        TChar tmpChar;

        tmpChar = 0xFEFF;
        linePtr.Append(tmpChar);
        }
#endif

    // Create contents of line as in design specification of LcfFIPrivacyNotifier
    // First append ordinal number
    linePtr.AppendNum(iNbOfDiagnostics);

    linePtr.Append(KLcfSeparator);

    // Append 'V' or 'N' (verification or notification)
    if (aDiagnostics->VerificationRequest())
        {
        linePtr.Append(_L("V"));
        }
    else
        {
        linePtr.Append(_L("N"));
        }

    linePtr.Append(KLcfSeparator);

    // Append request id
    linePtr.AppendNum(aDiagnostics->RequestId());

    linePtr.Append(KLcfSeparator);

    // Append number of requestors
    linePtr.AppendNum(aDiagnostics->NumberOfRequestors());

    // Append each requestor

    for (TInt i=0; i < aDiagnostics->NumberOfRequestors();i++)
        {
        linePtr.Append(KLcfSeparator);

        linePtr.Append(_L("("));

        CLcfRequestor* requestor = aDiagnostics->RequestorLC(i);

	    // Append source of request
    	switch (requestor->iSource)
        	{
    	case EPosRequestSourceNetwork:
        	linePtr.Append( _L("SourceNetwork") );
    		break;
    	default:
        	linePtr.Append( _L("SourceUnknown") );
    		break;
        }

        linePtr.Append(KLcfRequestorSeparator);

        // Append requetor type
        if (requestor->iType == CPosRequestor::ERequestorContact)
            linePtr.Append(KRequestorContact);
        else if (requestor->iType == CPosRequestor::ERequestorService)
            linePtr.Append(KRequestorService);
        else
            linePtr.Append(KRequestorUnknown);

        linePtr.Append(KLcfRequestorSeparator);

        // Append requestor format
        if (requestor->iFormat == CPosRequestor::EIdFormatEmail)
            linePtr.Append(KFormatMail);
        else if(requestor->iFormat == CPosRequestor::EIdFormatUrl)
            linePtr.Append(KFormatUrl);
        else if(requestor->iFormat == CPosRequestor::EIdFormatPhoneNumber)
            linePtr.Append(KFormatTelephone);
        else if(requestor->iFormat == CPosRequestor::EIdFormatGenericName)
            linePtr.Append(KFormatGenericName);
        else if(requestor->iFormat == CPosRequestor::EIdFormatSIPUrl)
            linePtr.Append(KFormatSIPURL);
        else if(requestor->iFormat == CPosRequestor::EIdFormatIMSPublicIdentity)
            linePtr.Append(KFormatIMSPublicIdentity);
        else if(requestor->iFormat == CPosRequestor::EIdFormatMIN)
            linePtr.Append(KFormatMIN); 
        else if(requestor->iFormat == CPosRequestor::EIdFormatMDN)
            linePtr.Append(KFormatMDN);        
        else if(requestor->iFormat == CPosRequestor::EIdFormatUnknown)
            linePtr.Append(KFormatUnkown);
        else
            User::Leave(KErrGeneral);

        linePtr.Append(KLcfRequestorSeparator);

        // Append id-string
        linePtr.Append(_L("\""));
        linePtr.Append(requestor->iIdString);
        linePtr.Append(_L("\""));

        linePtr.Append(KLcfRequestorSeparator);

        // Append db-ref if it exists
        if (requestor->iHasDataBaseRef)
            {
            linePtr.Append(KSingleMatch);

            linePtr.Append(KLcfRequestorSeparator);

            // append db-reference
            linePtr.AppendNum(requestor->iDataBaseRef);

            linePtr.Append(KLcfRequestorSeparator);

            // append db-name
            linePtr.Append(requestor->iDataBase);
            }
        else if (requestor->iHasMultipleMatches)
            {
            linePtr.Append(KMultipleMathes);
            }
        else
            {
            linePtr.Append(KNoMatch);
            }
        
        CleanupStack::PopAndDestroy(); // requestor

        linePtr.Append(_L(")"));
        }

    linePtr.Append(KLcfSeparator);

    // Append exit reason
    switch (aDiagnostics->ExitReason())
        {
    case ELcfExitAccepted:
        linePtr.Append(KExitAccepted);
        break;
    case ELcfExitRejected:
        linePtr.Append(KExitRejected);
        break;
    case ELcfExitTimedOut:
        linePtr.Append(KExitTimedOut);
        break;
    case ELcfExitAllAccepted:
        linePtr.Append(KExitAllAccepted);
        break;
    case ELcfExitAllRejected:
        linePtr.Append(KExitAllRejected);
        break;
    case ELcfExitCancelledByEPos:
        linePtr.Append(KExitCancelledByEpos);
        break;
    case ELcfExitAllCancelledByEpos:
        linePtr.Append(KExitAllCancelledByEpos);
        break;
    case ELcfExitAllTimedOut:
        linePtr.Append(KExitAllTimedOut);
        break;
    case ELcfExitRequestCompleted:
        linePtr.Append(KExitRequestCompleted);
        break;
    default:
        User::Leave(KErrUnknown);
        }

	
    iNbOfDiagnostics++;

    // Append endline and write to file
    linePtr.Append(_L("\r"));

    iFileText.Write(linePtr);

    iFile.Close();
    iFs.Close();


    CleanupStack::PopAndDestroy(); // line
    }


// This function gets answer for specific verification request.
// The order is as follows:
//   1. Next verification file
//   2. Input file, specific answer for this verification
//   3. Input file, default answer
// If no answer if found, then function leaves with KErrNotFound.
//
// Parameters:
//   aNb     - Number of verification.
//   aAnswer - Proper answer is returned here.
//   aDelay  - Proper delay is returned here.
// 
EXPORT_C void CLcfNotifierManager::GetVerificationAnswerL(TInt aNb, TLcfPluginAnswer& aAnswer, TReal& aDelay)
    {
    GetAnswerL('V', aNb, aAnswer, aDelay);
    }


// This function gets answer for specific notification.
// The order is as follows:
//   1. Next notification file
//   2. Input file, specific answer for this notification
//   3. Input file, default answer
// If no answer if found, then function leaves with KErrNotFound.
//
// Parameters:
//   aNb     - Number of notification.
//   aAnswer - Proper answer is returned here.
//   aDelay  - Proper delay is returned here.
// 
EXPORT_C void CLcfNotifierManager::GetNotificationAnswerL(TInt aNb, TLcfPluginAnswer& aAnswer, TReal& aDelay)
    {
    GetAnswerL('N', aNb, aAnswer, aDelay);
    }


// Returns ETrue if some input files exists where control data MAY be
// found. Note that this does not quarantee that answer for specific
// request really exists.
// 
EXPORT_C TBool CLcfNotifierManager::ControlDataExists()
    {
    TBool inputFile = EFalse;
    TBool nextVFile = EFalse;
    TBool nextNFile = EFalse;

    TInt err = iFs.Connect();
    
    if (err == KErrNone)
        {
        TEntry  dirEntry;

        inputFile = (iFs.Entry(KInputFileName, dirEntry) == KErrNone);
        nextVFile = (iFs.Entry(KNextVerification, dirEntry) == KErrNone);
        nextNFile = (iFs.Entry(KNextNotification, dirEntry) == KErrNone);

        iFs.Close();
        }

    return (inputFile || nextVFile || nextNFile);
    }


// Returns ETrue if Reset file exists. Also destroyes the reset
// file in this case.
// 
EXPORT_C TBool CLcfNotifierManager::ResetRequested()
    {
    TInt err = iFs.Connect();

    if (err != KErrNone)
        return EFalse;


    TEntry dummyEntry;

    if (iFs.Entry(KResetFile, dummyEntry) == KErrNone)
        {
        iFs.Delete(KResetFile);
        iFs.Close();

        iNbOfDiagnostics = 0;
        
        return ETrue;
        }
    else
        {
        iFs.Close();
        return EFalse;
        }
    }


EXPORT_C void CLcfNotifierManager::GetCompletionAnswer(TLcfCompletionStyle&  aStyle, 
                                                       TInt&                 aRequestId, 
                                                       TInt&                 aCompletionCode)
    {
    aStyle          = iStyle;
    aRequestId      = iRequestId;
    aCompletionCode = iCompletionCode;
    }



// 
// Private functions
//


// Opens given file for appending. After this iFileText member
// variable can be used for writing. iFs and iFile members must
// be closed when file is not needed for writint anymore.
//
// In case of error, an error code is returned.
//
// Parameters:
//   aFileName - The name of file to open.
// 
TInt CLcfNotifierManager::OpenFileForAppend(const TDesC& aFileName)
    {
    TInt err;

    iFileCreated = EFalse;

    err = iFs.Connect();

    if (err != KErrNone)
        {
        return err;
        }

    err = iFile.Open(iFs,aFileName,EFileStreamText|EFileWrite);


    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err=iFile.Create(iFs,aFileName,EFileStreamText|EFileWrite);
            }

        if (err != KErrNone)
            {
            return err;
            }

        iFileCreated = ETrue;
        }

    iFileText.Set(iFile);

    return KErrNone;
    }


// Opens given file for reading. After this iFileText member
// variable can be used for reading. iFs and iFile members must
// be closed when file is not needed for reading anymore.
//
// In case of error, an error code is returned.
//
// Parameters:
//   aFileName - The name of file to open.
// 
TInt CLcfNotifierManager::OpenFileForRead(const TDesC& aFileName)
    {
    TInt err;

    err = iFs.Connect();

    if (err != KErrNone)
        {
        return err;
        }

    err = iFile.Open(iFs,aFileName,EFileStreamText|EFileRead);


    if (err != KErrNone)
        {
        return err;
        }

    iFileText.Set(iFile);

    return KErrNone;
    }


// Appends given answer to file pointed by iFileText member variable.
// Closes handles to file and file server before returning.
//
// Parameters:
//   aStartOfLine - String to write at beginning of file.
//   aAnswer      - Answer to append.
//   aDelay       - Delay to append.
// 
void CLcfNotifierManager::AppendAnswer(const TDesC& aStartOfLine, TLcfPluginAnswer aAnswer, TReal aDelay)
    {
    OpenFileForAppend(KInputFileName);

    TBuf<50> line;

    line.Copy(aStartOfLine);

    line.Append(KLcfSeparator);
    
    switch (aAnswer)
        {
    case ELcfAccept:
        line.Append(KAccept);
        break;
    case ELcfReject:
        line.Append(KReject);
        break;
    case ELcfTimeOut:
        line.Append(KTimeout);
        break;
    case ELcfAcceptAll:
        line.Append(KAcceptAll);
        break;
    case ELcfRejectAll:
        line.Append(KRejectAll);
        break;
    case ELcfTimeOutAll:
        line.Append(KTimeoutAll);
        break;
    default:
        User::Leave(KErrUnknown);
        }

    line.Append(KLcfSeparator);

    TRealFormat realFormat;

    line.AppendNum(aDelay, realFormat);

    line.Append(_L("s\r"));

    iFileText.Write(line);

    iFile.Close();
    iFs.Close();
    }


// Gets next line from file pointed by iFileText member.
// In case next line cannot be gotten, returns an error code.
//
// Parameters:
//   aLine - Descriptor where the line is returned.
// 
TInt CLcfNotifierManager::GetNextLine(TDes& aLine)
    {
    return iFileText.Read(aLine);
    }


// Creates a new diagnostics instance from given descriptor. Descriptor must
// follow the format specified in LcfFIPrivacyNotifier Design Specification.
//
// Parameters:
//   aLine        - The line where to create the diagnostics instance.
//   aDiagnostics - Pointer to new diagnostics instance.
// 
void CLcfNotifierManager::CreateDiagnosticsFromDescriptorL(const TDesC& aLine, CLcfPluginDiagnostics*& aDiagnostics)
    {
    aDiagnostics = CLcfPluginDiagnostics::NewL();

    CleanupStack::PushL(aDiagnostics);

    // Verification/Notification
    TPtrC parsePtr = FindItemL(1,aLine);

    if (parsePtr == _L("V"))
        aDiagnostics->SetIsVerificationRequest(ETrue);
    else if (parsePtr == _L("N"))
        aDiagnostics->SetIsVerificationRequest(EFalse);
    else
        User::Leave(KErrGeneral);

    // RequestId
    parsePtr.Set(FindItemL(2,aLine));

    TLex input(parsePtr);
    TInt tmp;

    User::LeaveIfError(input.Val(tmp));

    aDiagnostics->SetRequestId(tmp);

    // Nb of requestors 
    parsePtr.Set(FindItemL(3,aLine));

    input.Assign(parsePtr);
    TInt nbOfRequestors;

    User::LeaveIfError(input.Val(nbOfRequestors));

    // Requestors
    for (TInt i=0; i < nbOfRequestors;i++)
        {
        CLcfRequestor* newRequestor = CLcfRequestor::NewLC();

        // Take whole requestor info
        parsePtr.Set(FindItemL(4+i,aLine));

        // remove braces (opening and closing
        TPtrC tmpPtr = parsePtr.Mid(1,parsePtr.Length()-2);

        // Requestor source
        TPtrC parse2 = FindItemL(0,tmpPtr,',');

		_LIT(KSourceNetwork, "SourceNetwork");
		_LIT(KSourceUnknown, "SourceUnknown");
		
        if (parse2 == KSourceNetwork)
            {
            newRequestor->iSource = EPosRequestSourceNetwork;
            }
        else if (parse2 == KSourceUnknown)
            {
            newRequestor->iSource = EPosRequestSourceNotAvailable;
            }
        else
            {
            User::Leave(KErrGeneral);
            }

        // Requestor Type
        parse2.Set(FindItemL(1,tmpPtr,','));

        if (parse2 == KRequestorContact)
            {
            newRequestor->iType = CPosRequestor::ERequestorContact;
            }
        else if (parse2 == KRequestorService)
            {
            newRequestor->iType = CPosRequestor::ERequestorService;
            }
            else
            {
            User::Leave(KErrGeneral);
            }

        // Requestor format
        parse2.Set(FindItemL(2,tmpPtr,','));

        if (parse2 == KFormatMail)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatEmail;
            }
        else if (parse2 == KFormatUrl)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatUrl;
            }
        else if (parse2 == KFormatTelephone)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatPhoneNumber;
            }
        else if (parse2 == KFormatSIPURL)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatSIPUrl;
            }
        else if (parse2 == KFormatIMSPublicIdentity)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatIMSPublicIdentity;
            }
        else if (parse2 == KFormatMIN)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatMIN;
            }
        else if (parse2 == KFormatGenericName)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatGenericName;
            }
        else if (parse2 == KFormatMDN)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatMDN;
            }              
               
        else if (parse2 == KFormatUnkown)
            {
            newRequestor->iFormat = CPosRequestor::EIdFormatUnknown;
            }
        else
            {
            User::Leave(KErrGeneral);
            }

        // Requestor id-string
        parse2.Set(FindItemL(3,tmpPtr,','));
        
        newRequestor->iIdString.Copy(parse2.Mid(1,parse2.Length()-2)); // remove quotes

        // Has database reference
        parse2.Set(FindItemL(4,tmpPtr,','));


        if (parse2 == KSingleMatch)
            {
            newRequestor->iHasDataBaseRef     = ETrue;
            newRequestor->iHasMultipleMatches = EFalse;

            // database ref
            //parse2.Set(FindItemL(4,tmpPtr,','));
            parse2.Set(FindItemL(5,tmpPtr,','));

            TLex tmpLex(parse2);
            TInt  tmpRef;

            // If there is an error in conversion, set reference to -1

            if (tmpLex.Val(tmpRef) != KErrNone)
                {
                newRequestor->iDataBaseRef = -1;
                }
            else
                {
                newRequestor->iDataBaseRef = tmpRef;
                }

            // database name
            //parse2.Set(FindItemL(5,tmpPtr,','));
            parse2.Set(FindItemL(6,tmpPtr,','));
            
            newRequestor->iDataBase.Copy(parse2);
            }
        else if(parse2 == KMultipleMathes)
            {
            newRequestor->iHasDataBaseRef     = EFalse;
            newRequestor->iHasMultipleMatches = ETrue;
            }
        else if(parse2 == KNoMatch)
            {
            newRequestor->iHasDataBaseRef     = EFalse;
            newRequestor->iHasMultipleMatches = EFalse;
            }
        else
            {
            User::Leave(KErrGeneral);
            }

        aDiagnostics->AddRequestorLD(newRequestor);


        CleanupStack::Pop(); // newRequestor
        }


    // Exit reason
    parsePtr.Set(FindItemL(4+nbOfRequestors,aLine));

    if (parsePtr == KExitAccepted)
        {
        aDiagnostics->SetExitReason(ELcfExitAccepted);
        }
    else if (parsePtr == KExitRejected)
        {
        aDiagnostics->SetExitReason(ELcfExitRejected);
        }
    else if (parsePtr == KExitTimedOut)
        {
        aDiagnostics->SetExitReason(ELcfExitTimedOut);
        }
    else if (parsePtr == KExitAllAccepted)
        {
        aDiagnostics->SetExitReason(ELcfExitAllAccepted);
        }
    else if (parsePtr == KExitAllRejected)
        {
        aDiagnostics->SetExitReason(ELcfExitAllRejected);
        }
    else if (parsePtr == KExitCancelledByEpos)
        {
        aDiagnostics->SetExitReason(ELcfExitCancelledByEPos);
        }
    else if (parsePtr == KExitAllCancelledByEpos)
        {
        aDiagnostics->SetExitReason(ELcfExitAllCancelledByEpos);
        }
    else if (parsePtr == KExitAllTimedOut)
        {
        aDiagnostics->SetExitReason(ELcfExitAllTimedOut);
        }
    else if (parsePtr == KExitRequestCompleted)
        {
        aDiagnostics->SetExitReason(ELcfExitRequestCompleted);
        }
    else
        {
        User::Leave(KErrGeneral);
        }


    CleanupStack::Pop();
    }


// This function is used when parsing the input and output files.
// Takes a string, separator and index as parameters, and returns a
// pointer to the "indexth" string between separator marks.
// 
// Example: If aLine is "a:bc:cdef", and aIndex 1 and aSeparator ':',
//          then "bc" is returned.
//
// If not enough separators are found, function leaves with 
// KErrNotFound.
//
// Parameters:
//   aIndex     - The ordinal number.
//   aLine      - The string where separators are searched.
//   aSeparator - The separator character.
// 
TPtrC CLcfNotifierManager::FindItemL(TInt aIndex, const TDesC& aLine, const TChar aSeparator)
    {
    TInt separatorsFound = 0;

    TInt startIndex = 0; // First char to include
    TInt endIndex   = aLine.Length(); // first char not to include anymore
   
    // A flag for allwing double colons inside service name
    // (very necessary for http -addresses for example) 
    TBool serviceName = EFalse;

    for (TInt i=0;i < aLine.Length();i++)
        {
        if(aLine[i] == '"')
            {
            if (serviceName)
                {
                serviceName = EFalse;
                }
            else
                {
                serviceName = ETrue;
                }
            }

        // Ignore anything inside service name
        if (serviceName)
            continue;

        if (aLine[i]==aSeparator)
            {
            // Hack to avoid problem with database file path: if
            // next character is '/', then ignore this separator
            // (and if separator is ':').
            if ((i+1) < aLine.Length() && aSeparator == ':' 
                && aLine[i+1] == '\\')
                {
                continue;
                }


            if (separatorsFound == (aIndex-1)) // left separator found
                {
                startIndex = i+1;               
                }
            else if (separatorsFound == aIndex) // right separator found
                {
                endIndex = i;

                return aLine.Mid(startIndex, (endIndex-startIndex));
                }

            separatorsFound++;
            }
        }
    
    
    // Right separator might be missing, this is normal
    if (separatorsFound >= (aIndex - 1))
        {
        return aLine.Mid(startIndex, (endIndex-startIndex));
        }
    else
        {
        User::Leave(KErrNotFound);
        }


    return TPtrC(); // To avoid compiler warning
    }


// Creates a new answer  from given descriptor. Descriptor must
// follow the format specified in LcfFIPrivacyNotifier Design Specification.
//
// Parameters:
//   aLine   - The line where to create the diagnostics instance.
//   aAnswer - Reference to enumeration where answer is returned.
//   aDelay  - Reference to TReal where delay is returned.   
// 
void CLcfNotifierManager::ConstructAnswerFromLineL(const TDesC& aLine, TLcfPluginAnswer& aAnswer, TReal& aDelay)
    {

    TPtrC parsePtr = FindItemL(1,aLine);
    TInt index;

    if (parsePtr == KAccept)
        {
        aAnswer = ELcfAccept;
        }
    else if (parsePtr == KReject)
        {
        aAnswer = ELcfReject;
        }
    else if (parsePtr == KTimeout)
        {
        aAnswer = ELcfTimeOut;
        }
    else if (parsePtr == KAcceptAll)
        {
        aAnswer = ELcfAcceptAll;
        }
    else if (parsePtr == KRejectAll)
        {
        aAnswer = ELcfRejectAll;
        }
    else if (parsePtr == KTimeoutAll)
        {
        aAnswer = ELcfTimeOutAll;
        } // New for completion of any request with any code
    else if (parsePtr.Left(KCompleteThis().Length()) == KCompleteThis)
        {
        aAnswer = ELcfComplete;
        
        index = parsePtr.Find(_L("(")) + 1; // Index to first number
        TPtrC tmp(parsePtr.Mid(index));
        TLex lex(tmp);
        User::LeaveIfError(lex.Val(iCompletionCode));
        
        iStyle = ELcfCurrentRequest;
        }
    else if (parsePtr.Left(KCompleteWithId().Length()) == KCompleteWithId)
        {
        aAnswer = ELcfComplete;
        
        index = parsePtr.Find(_L("(")) + 1; // Index to first number
        TPtrC tmp(parsePtr.Mid(index));
        TLex lex(tmp);
        User::LeaveIfError(lex.Val(iRequestId)); // set request id

        index = parsePtr.Find(_L(",")) + 1;
        tmp.Set(parsePtr.Mid(index));
        lex.Assign(tmp);
        User::LeaveIfError(lex.Val(iCompletionCode)); // set completion code
        
        iStyle = ELcfSpecificRequest;
        }
    else if (parsePtr.Left(KCompleteAll().Length()) == KCompleteAll)
        {
        aAnswer = ELcfComplete;
        
        index = parsePtr.Find(_L("(")) + 1; // Index to first number
        TPtrC tmp(parsePtr.Mid(index));
        TLex lex(tmp);
        User::LeaveIfError(lex.Val(iCompletionCode));
        
        iStyle = ELcfAllRequests;
        }
    else
        {
        User::Leave(KErrGeneral);
        }

    parsePtr.Set(FindItemL(2,aLine));

    TLex lex(parsePtr);

    User::LeaveIfError(lex.Val(aDelay));


    }


// Gets answer for a verification or notification (specified by aStart).
// 
// Parameters:
//   aStart  - Start of line. Must be either 'V' or 'N'.
//   aNb     - Number of verification or notificaiton.
//   aAnswer - Answer is returned here.
//   aDelay  - Delay is returned here.
// 
void CLcfNotifierManager::GetAnswerL(const TChar aStart, 
                                     TInt aNb, 
                                     TLcfPluginAnswer& aAnswer, 
                                     TReal& aDelay)
    {      
    TBool matchFound = EFalse;
    TBool needToDeleteFile = EFalse;

    TInt err;
    
    // Open either next file or input file
    if (aStart == 'V' && FileExists(KNextVerification))
        {
        err = OpenFileForRead(KNextVerification);
        needToDeleteFile = ETrue;
        }
    else if (aStart == 'N' && FileExists(KNextNotification))
        {
        err = OpenFileForRead(KNextNotification);
        needToDeleteFile = ETrue;
        }
    else
        {
        err = OpenFileForRead(KInputFileName);
        }

    

    User::LeaveIfError(err);

    HBufC* lineBuf = HBufC::NewLC(KMaxOutFileLineSize);
    TPtr linePtr = lineBuf->Des();

    HBufC* defaultLine = NULL;

    // NOTE: It seems to cause a memory leak later if a null pointer is
    // pushed into cleanup stack (memory does not get freed if it is 
    // allocated for this pointer after pushing it into cleanup stack).
    // Thus defaultline is pushed into cleanupstack only if really needed.

    // Get to correct line and read it
    while (GetNextLine(linePtr) == KErrNone)
        {
        if (linePtr[0] != aStart) // 
            continue;

        if (linePtr[1] == 'D' || linePtr[1] == ':')
            {
            // Change 9.6.2003: Allow multiple default lines, and always
            // use latest one.
            if (defaultLine) 
                {
                CleanupStack::PopAndDestroy(); // defaultLine
                defaultLine = NULL;
                }

            defaultLine = lineBuf->AllocL();

            CleanupStack::PushL(defaultLine);
            
            continue;
            }

        TPtrC tmp = FindItemL(0,linePtr);
        TPtrC tmp2 = tmp.Mid(1); // Get rid of trailing 'V' or 'N'

        TInt nb;
        TLex tmpLex(tmp2);

        User::LeaveIfError(tmpLex.Val(nb));

        if (nb == aNb)
            {
            ConstructAnswerFromLineL(linePtr, aAnswer, aDelay);
            matchFound = ETrue;
            break;
            }
        
        }

    iFile.Close();
    iFs.Close();

    if (!matchFound)
        {
        if (defaultLine)
            {
            linePtr.Set(defaultLine->Des());
            ConstructAnswerFromLineL(linePtr, aAnswer, aDelay);
            }        
        else
            {
            User::Leave(KErrNotFound);
            }
        }

    // Cheack if two items or only one item needs to be destroyed
    // from cleanup stack.
    if (defaultLine)
        {
        CleanupStack::PopAndDestroy(2); // lineBuf, defaultLine
        }
    else
        {
        CleanupStack::PopAndDestroy(); // lineBuf
        }

    if (needToDeleteFile)
        {
        if (aStart == 'V')
            DeleteFileL(KNextVerification);
        else
            DeleteFileL(KNextNotification);
        }


    }


// Sets answer for next verification or notification by replacing
// correspoinding file.
//
// Parameters:
//   aFileName - A file to replace.
//   aPrefix   - Prefix to write at beginning of file.
//   aAnswer   - Answer to append.
//   aDelay    - Delay to append.
// 
void CLcfNotifierManager::SetNextAnswerL(const TDesC& aFileName, 
                                         const TDesC& aPrefix, 
                                         TLcfPluginAnswer aAnswer, 
                                         TReal aDelay)
    {
    User::LeaveIfError(iFs.Connect());


    User::LeaveIfError(iFile.Replace(iFs,aFileName,EFileStreamText|EFileWrite));

    iFileText.Set(iFile);


    TBuf<50> line;

    line.Copy(aPrefix);

    line.Append(KLcfSeparator);
    
    switch (aAnswer)
        {
    case ELcfAccept:
        line.Append(KAccept);
        break;
    case ELcfReject:
        line.Append(KReject);
        break;
    case ELcfTimeOut:
        line.Append(KTimeout);
        break;
    case ELcfAcceptAll:
        line.Append(KAcceptAll);
        break;
    case ELcfRejectAll:
        line.Append(KRejectAll);
        break;
    case ELcfTimeOutAll:
        line.Append(KTimeoutAll);
        break;
    default:
        User::Leave(KErrUnknown);
        }

    line.Append(KLcfSeparator);

    TRealFormat realFormat;

    line.AppendNum(aDelay, realFormat);

    line.Append(_L("s\r"));

    iFileText.Write(line);

    iFile.Close();
    iFs.Close();
    }


// Returns ETrue if the file given as parameter exists..
//
// Parameters:
//   aFileName - The name of file which is searched.
// 
TBool CLcfNotifierManager::FileExists(const TDesC& aFileName)
    {
    TInt err = iFs.Connect();

    if (err != KErrNone)
        return EFalse;

    TEntry entry;

    TBool retValue = (iFs.Entry(aFileName, entry) == KErrNone);

    iFs.Close();

    return retValue;
    }


// Deletes the file given as parameter.
//
// Parameters:
//   aFile - Name of file which to delete.
// 
void CLcfNotifierManager::DeleteFileL(const TDesC& aFile)
    {
    iFs.Connect();

    TInt err = iFs.Delete(aFile);

    iFs.Close();

    if (err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
        {
        User::Leave(err);
        }
    }


// End of file
