/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "audiostuborbit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiostuborbit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_audiostuborbit[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_audiostuborbit[] = {
    "audiostuborbit\0"
};

const QMetaObject audiostuborbit::staticMetaObject = {
    { &HbMainWindow::staticMetaObject, qt_meta_stringdata_audiostuborbit,
      qt_meta_data_audiostuborbit, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &audiostuborbit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *audiostuborbit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *audiostuborbit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_audiostuborbit))
        return static_cast<void*>(const_cast< audiostuborbit*>(this));
    return HbMainWindow::qt_metacast(_clname);
}

int audiostuborbit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HbMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
