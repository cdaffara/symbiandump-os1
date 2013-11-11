/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __HWRMLightTestF_DOCUMENT_H__
#define __HWRMLightTestF_DOCUMENT_H__


#include <eikdoc.h>

class CHWRMLightTestFAppUi;
class CEikApplication;


/** 
  @class CHWRMLightTestFDocument
  
  @returnAn instance of class CHWRMLightTestFDocument is the Document part of the AVKON
  application framework for the HWRMLightTestF example application
  * * Nokia Core OS *
  */
class CHWRMLightTestFDocument : public CEikDocument
    {
public:

/**
  @returnNewL
  
  @returnConstruct a CHWRMLightTestFDocument for the AVKON application aApp 
  using two phase construction, and return a pointer to the created object
  @param aApp The application creating this document
  @return A pointer to the created instance of CHWRMLightTestFDocument
  */
    static CHWRMLightTestFDocument* NewL(CEikApplication& aApp);

/**
  @returnNewLC
  
  @returnConstruct a CHWRMLightTestFDocument for the AVKON application aApp 
  using two phase construction, and return a pointer to the created object
  @param aApp The application creating this document
  @return A pointer to the created instance of CHWRMLightTestFDocument
  */
    static CHWRMLightTestFDocument* NewLC(CEikApplication& aApp);

/**
  @return~CHWRMLightTestFDocument
  
  @returnDestroy the object
  */
    ~CHWRMLightTestFDocument();

public: // from CAknDocument
/**
  @returnCreateAppUiL 
  
  @returnCreate a CHWRMLightTestFAppUi object and return a pointer to it
  @return A pointer to the created instance of the AppUi created
  */
    CEikAppUi* CreateAppUiL();

private:

/**
  @returnConstructL
  
  @returnPerform the second phase construction of a CHWRMLightTestFDocument object
  */
    void ConstructL();

/**
  @return CHWRMLightTestFDocument
  
  @return Perform the first phase of two phase construction 
  @param aApp The application creating this document
  */
    CHWRMLightTestFDocument(CEikApplication& aApp);

    };


#endif // __HWRMLightTestF_DOCUMENT_H__
