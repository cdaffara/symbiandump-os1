// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "EPos_CPosResourceReader.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosResourceReader::CPosResourceReader()
    :iFileSession(), 
    iResourceFile(), 
    iResourceBuffer(NULL)
    {
    }

// Two-phased constructor.
EXPORT_C CPosResourceReader* CPosResourceReader::NewLC(const TDesC& aPath)
    {
    CPosResourceReader* self = new (ELeave) CPosResourceReader;
    CleanupStack::PushL( self );
    self->ConstructL(aPath);
    return self;
    }

// Destructor
EXPORT_C CPosResourceReader::~CPosResourceReader()
    {
    delete iResourceBuffer;
    iResourceFile.Close();
    iFileSession.Close();
    }

// EPOC default constructor can leave.
void CPosResourceReader::ConstructL(const TDesC& aPath)
    {
    User::LeaveIfError(iFileSession.Connect());

    TFindFile* filefinder = new (ELeave) TFindFile(iFileSession);
    CleanupStack::PushL(filefinder);

    TParse* fileparser = new (ELeave) TParse;
    CleanupStack::PushL(fileparser);
    fileparser->Set(aPath, NULL, NULL);

    User::LeaveIfError(filefinder->FindByDir(fileparser->NameAndExt(),
        fileparser->DriveAndPath()));
    
    iResourceFile.OpenL(iFileSession, filefinder->File());
    iResourceFile.ConfirmSignatureL(0);
    
    CleanupStack::Pop(2, filefinder); // fileparser
    delete filefinder;
    delete fileparser;
    }

// ---------------------------------------------------------
// CPosResourceReader::ReadHBufCL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C HBufC* CPosResourceReader::ReadHBufCL(TInt aResourceId)
    {
    LoadResourceL(aResourceId);
    return iResourceReader.ReadHBufCL();
    }

// ---------------------------------------------------------
// CPosResourceReader::ReadInt8L
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CPosResourceReader::ReadInt8L(TInt aResourceId)
    {
    LoadResourceL(aResourceId);
    return iResourceReader.ReadInt8();
    }

// ---------------------------------------------------------
// CPosResourceReader::ReadInt16L
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CPosResourceReader::ReadInt16L(TInt aResourceId)
    {
    LoadResourceL(aResourceId);
    return iResourceReader.ReadInt16();
    }

// ---------------------------------------------------------
// CPosResourceReader::ReadInt32L
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CPosResourceReader::ReadInt32L(TInt aResourceId)
    {
    LoadResourceL(aResourceId);
    return iResourceReader.ReadInt32();
    }

// ---------------------------------------------------------
// CPosResourceReader::LoadResourceL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPosResourceReader::LoadResourceL(TInt aResourceId)
   {    
   if (iResourceBuffer)
       {
       delete iResourceBuffer;
       iResourceBuffer = NULL;
       }
   iResourceBuffer = iResourceFile.AllocReadL(aResourceId);
   iResourceReader.SetBuffer(iResourceBuffer);
   }

//  End of File
