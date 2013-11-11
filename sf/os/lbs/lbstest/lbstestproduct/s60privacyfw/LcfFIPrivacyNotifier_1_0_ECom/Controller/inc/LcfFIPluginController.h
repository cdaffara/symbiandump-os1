/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* This is the header file defining the interface for LcfFIPrivacyNotifier
* usage.
*
*/



#ifndef _LCFFIPLUGINCONTROLLER_H_
#define _LCFFIPLUGINCONTROLLER_H_


#include <f32file.h>      // RFs

#include <EPos_Privacy.h>
#include <EPos_CPosRequestor.h>



// Some http addresses can be 160 characters long. In addition, some test
// drivers need 255 character format strings.
const TInt KMaxNameLength = 255;

enum TLcfPluginAnswer 
    { 
    ELcfAccept, 
    ELcfReject, 
    ELcfTimeOut, 
    ELcfAcceptAll, 
    ELcfRejectAll,
    ELcfTimeOutAll,
    ELcfComplete  // Cannot be set directly
    };


enum TLcfPluginExitReason
    { 
    ELcfExitAccepted, 
    ELcfExitRejected, 
    ELcfExitTimedOut, 
    ELcfExitAllAccepted, 
    ELcfExitAllRejected, 
    ELcfExitCancelledByEPos,
    ELcfExitAllCancelledByEpos,
    ELcfExitAllTimedOut,
    ELcfExitRequestCompleted
    };

enum TLcfPluginAnswerType
    {
    ELcfAnswerTypeNotification,
    ELcfAnswerTypeDefaultNotification,
    ELcfAnswerTypeVerification,
    ELcfAnswerTypeDefaultVerification,
    ELcfAnswerTypeNextNotification,
    ELcfAnswerTypeNextVerification,
    };

enum TLcfCompletionStyle
    {
    ELcfCurrentRequest,
    ELcfSpecificRequest,
    ELcfAllRequests
    };

// This must be a C class because otherwise it will use over
// 800 bytes of stack
class CLcfRequestor:public CBase
    {
  public:
    IMPORT_C static CLcfRequestor* NewL();
    IMPORT_C static CLcfRequestor* NewLC();

    IMPORT_C ~CLcfRequestor();
  private:
    CLcfRequestor();

  public:

    IMPORT_C CLcfRequestor(const CLcfRequestor& aRequestor);
    IMPORT_C CLcfRequestor& operator=(const CLcfRequestor& aRequestor);


    CPosRequestor::TRequestorType   iType;
    CPosRequestor::TRequestorIdFormat iFormat;
    TBuf<KMaxNameLength>         iIdString;
    TBool                        iHasDataBaseRef;
    TBool                        iHasMultipleMatches;
    TInt                         iDataBaseRef;
    TBuf<KMaxFileName>           iDataBase;
    TPosRequestSource			 iSource;
    };


class CLcfPluginDiagnostics: public CBase
{
  public:
    IMPORT_C static CLcfPluginDiagnostics* NewL();
    IMPORT_C static CLcfPluginDiagnostics* NewLC();

    IMPORT_C ~CLcfPluginDiagnostics();

  private:  
    CLcfPluginDiagnostics();
    void ConstructL();


  public:
    IMPORT_C TInt RequestId() const;
    IMPORT_C TBool VerificationRequest() const; // If EFalse then this was a notification
    IMPORT_C TLcfPluginExitReason ExitReason() const;
    IMPORT_C TInt NumberOfRequestors() const;
    IMPORT_C CLcfRequestor* RequestorL(TInt aIndex) const; 
    IMPORT_C CLcfRequestor* RequestorLC(TInt aIndex) const; 
    
  public: // Interface for plug-in and manager
    IMPORT_C void SetRequestId(TInt aRequestId);
    IMPORT_C void SetIsVerificationRequest(TBool aIsVerification);
    IMPORT_C void SetExitReason(TLcfPluginExitReason aReason);
    IMPORT_C void AddRequestorLD(CLcfRequestor* aRequestor);
  private:
    TInt                  iRequestId;
    TBool                 iThisWasVerification;
    TLcfPluginExitReason  iExitReason;
    CArrayPtrFlat<CLcfRequestor>* iRequestorArray;

};



class CLcfNotifierManager: public CBase
{
  public:
      // Interface for test driver
    IMPORT_C static CLcfNotifierManager* NewL(TBool aDeleteFiles = ETrue);
    IMPORT_C static CLcfNotifierManager* NewLC(TBool aDeleteFiles = ETrue);

    IMPORT_C ~CLcfNotifierManager();

    IMPORT_C void SetNotificationAnswerL(TInt aNb, TLcfPluginAnswer aAnswer, TReal aDelay);
    IMPORT_C void SetDefaultNotificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay);

    IMPORT_C void SetVerificationAnswerL(TInt aNb, TLcfPluginAnswer aAnswer, TReal aDelay);
    IMPORT_C void SetDefaultVerificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay);

    IMPORT_C void SetNextVerificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay);
    IMPORT_C void SetNextNotificationAnswerL(TLcfPluginAnswer aAnswer, TReal aDelay);

    IMPORT_C void GetDiagnosticsL(TInt aNb, CLcfPluginDiagnostics*& aDiagnostics);
    IMPORT_C TInt NbOfDiagnostics();
    IMPORT_C void GetLatestDiagnosticsL(CLcfPluginDiagnostics*& aDiagnostics);

    IMPORT_C void ClearAllInputL();
    IMPORT_C void ClearAllOutputL();
    IMPORT_C void ClearAllInputAndOutputL();

    IMPORT_C void ResetPluginL();

    // Extensions to original interface
    IMPORT_C void SetCompletionCodeL(TLcfPluginAnswerType aType, 
                                     TLcfCompletionStyle aStyle, 
                                     TInt  aNb, 
                                     TInt  aRequestId, 
                                     TInt  aCompletionCode,
                                     TReal aDelay);

  public:
      // Interface for UI-plugin
    IMPORT_C void  AppendDiagnosticsL(const CLcfPluginDiagnostics* aDiagnostics);
    IMPORT_C void  GetVerificationAnswerL(TInt aNb, TLcfPluginAnswer& aAnswer, TReal& aDelay);
    IMPORT_C void  GetNotificationAnswerL(TInt aNb, TLcfPluginAnswer& aAnswer, TReal& aDelay);
    IMPORT_C TBool ControlDataExists();
    IMPORT_C TBool ResetRequested();

    IMPORT_C void GetCompletionAnswer(TLcfCompletionStyle& aStyle, 
                                      TInt&  aRequestId, 
                                      TInt&  aCompletionCode);

  private:
    void ConstructL(TBool aDeleteFiles);

  private: // Test driver interface helpers
    TInt OpenFileForAppend(const TDesC& aFileName);
    TInt OpenFileForRead(const TDesC& aFileName);

    void AppendAnswer(const TDesC& aStartOfLine, TLcfPluginAnswer aAnswer, TReal aDelay);
    TInt GetNextLine(TDes& aLine);
    void CreateDiagnosticsFromDescriptorL(const TDesC& aLine, CLcfPluginDiagnostics*& aDiagnostics);
    TPtrC FindItemL(TInt aIndex, const TDesC& aLine, const TChar aSeparator = ':');

    void ConstructAnswerFromLineL(const TDesC& aLine, TLcfPluginAnswer& aAnswer, TReal& aDelay);
    void GetAnswerL(const TChar aStart, 
                    TInt aNb, 
                    TLcfPluginAnswer& aAnswer, 
                    TReal& aDelay);

    void SetNextAnswerL(const TDesC& aFileName, const TDesC& aPrefix, TLcfPluginAnswer aAnswer, TReal aDelay);
    
    TBool FileExists(const TDesC& aFileName);


  private: // Other private functions
    void DeleteFileL(const TDesC& aFile);


  private:
    RFs       iFs;

    RFile     iFile;
    TFileText iFileText;

    TInt      iNbOfDiagnostics;

    TBool     iFileCreated; // hack to get output file in unicode

  private: // Members for interface extension

    TLcfCompletionStyle  iStyle;
    TInt                 iRequestId;
    TInt                 iCompletionCode;
};



#endif

// End of file
