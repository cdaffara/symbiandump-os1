 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testlistcontainer.h"
#include "testlistengine.h"
#include "testlauncher.hrh"

#include <testlauncher.rsg>
#include <EIKAPP.H>
#include <eikbtgpc.h>


void CFileListContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    iListBox = new (ELeave) CAknDoubleNumberStyleListBox;
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructL(this, EAknListBoxMarkableList);

    // Create the scroll indicator
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()
        ->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->ActivateL();

    // Create the FileListEngine
    iAppEngine = new (ELeave) CFileListEngine;

#ifdef __SERIES60_3X__
    iAppEngine->ConstructL();
#else
    iAppEngine->ConstructL((CEikProcess*)(((CEikAppUi*)iCoeEnv->AppUi())->Application()->Process()));
#endif

    SetFileListL(EFileListPictures, EFileListDate);
    
    SetRect(aRect);
    ActivateL();
    }

CFileListContainer::~CFileListContainer()
    {
    delete iAppEngine;
    delete iListBox;
    }



// This will set up filelist.
// Directory and Size can be changed. See Filelist.hrh for possible values
// This function is located in the container and not in the engine, because it
// activates the listbox.
void CFileListContainer::SetFileListL(TInt aDirectory, TInt aSizeDate)
    {
    // Set the listbox to use the file list model
    CDesCArray* items = static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray());

    // If there are items, they will be removed here
    if (iAppEngine->RemoveItems(items))
        {
        // This makes changes to the actual listbox
        iListBox->HandleItemRemovalL();
        }

    // Let's show directory
    iAppEngine->SetDirectory(aDirectory);
    // Let's decide whether to show file size or modification date
    iAppEngine->SetSizeDate(aSizeDate);
    // Do preparations for the FileList
    if(iAppEngine->StartFileList() == KErrNone)
        {
        // Create FileList Items in the ListBox
        iAppEngine->GetFileListItemsL(items);
        }
    // Close FileList session
    iAppEngine->EndFileList();

    // Refresh the listbox due to model change
    iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex(0);

    // Set correct middle softkey
    CEikButtonGroupContainer * cbaGroup = iEikonEnv->AppUiFactory()->Cba();
    if (iAppEngine->IsDirListEmpty())
        {
        // Don't use middle softkey at all
        cbaGroup->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
        }

    iListBox->DrawNow();
    }

// Called by framework when the view size is changed
void CFileListContainer::SizeChanged()
    {
    // control resizing
    TRect rect = Rect();
    iListBox->SetExtent(TPoint(0,0),rect.Size());
    }

// Called by framework when a key is pressed
TKeyResponse CFileListContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TKeyResponse ret;

    // See if we have a selection
    TInt code = aKeyEvent.iCode;
    switch(code)
        {
        // is navigator button pressed
        case EKeyOK:
            iAppEngine->LaunchCurrentL(iListBox->CurrentItemIndex());
            ret = EKeyWasConsumed;
            break;

        default:
            // Let Listbox take care of its key handling
            ret = iListBox->OfferKeyEventL(aKeyEvent, aType);
            break;
        }
    return ret;
    }

void CFileListContainer::LaunchCurrentL()
    {
    iAppEngine->LaunchCurrentL(iListBox->CurrentItemIndex());
    }

TInt CFileListContainer::CountComponentControls() const
    {
    return 1;
    }
CCoeControl* CFileListContainer::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

void CFileListContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    // drawing code
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

void CFileListContainer::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
    {
    // empty
    }
