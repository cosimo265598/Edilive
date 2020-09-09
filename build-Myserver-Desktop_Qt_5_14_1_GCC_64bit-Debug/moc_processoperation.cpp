/****************************************************************************
** Meta object code from reading C++ file 'processoperation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../MyserverEncrypt/processoperation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'processoperation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProcessOperation_t {
    QByteArrayData data[18];
    char stringdata0[202];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProcessOperation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProcessOperation_t qt_meta_stringdata_ProcessOperation = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ProcessOperation"
QT_MOC_LITERAL(1, 17, 12), // "loginRequest"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 11), // "QWebSocket*"
QT_MOC_LITERAL(4, 43, 12), // "clientSocket"
QT_MOC_LITERAL(5, 56, 8), // "username"
QT_MOC_LITERAL(6, 65, 11), // "loginUnlock"
QT_MOC_LITERAL(7, 77, 5), // "token"
QT_MOC_LITERAL(8, 83, 13), // "accountCreate"
QT_MOC_LITERAL(9, 97, 8), // "nickname"
QT_MOC_LITERAL(10, 106, 4), // "icon"
QT_MOC_LITERAL(11, 111, 8), // "password"
QT_MOC_LITERAL(12, 120, 13), // "SimpleMessage"
QT_MOC_LITERAL(13, 134, 4), // "mess"
QT_MOC_LITERAL(14, 139, 15), // "OpenDirOfClient"
QT_MOC_LITERAL(15, 155, 19), // "CreateFileForClient"
QT_MOC_LITERAL(16, 175, 8), // "filename"
QT_MOC_LITERAL(17, 184, 17) // "OpenFileForClient"

    },
    "ProcessOperation\0loginRequest\0\0"
    "QWebSocket*\0clientSocket\0username\0"
    "loginUnlock\0token\0accountCreate\0"
    "nickname\0icon\0password\0SimpleMessage\0"
    "mess\0OpenDirOfClient\0CreateFileForClient\0"
    "filename\0OpenFileForClient"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProcessOperation[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06 /* Public */,
       6,    2,   54,    2, 0x06 /* Public */,
       8,    5,   59,    2, 0x06 /* Public */,
      12,    2,   70,    2, 0x06 /* Public */,
      14,    1,   75,    2, 0x06 /* Public */,
      15,    2,   78,    2, 0x06 /* Public */,
      17,    2,   83,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    7,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::QString, QMetaType::QImage, QMetaType::QString,    4,    5,    9,   10,   11,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,   13,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,   16,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,   16,

       0        // eod
};

void ProcessOperation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProcessOperation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loginRequest((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->loginUnlock((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->accountCreate((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QImage(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        case 3: _t->SimpleMessage((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->OpenDirOfClient((*reinterpret_cast< QWebSocket*(*)>(_a[1]))); break;
        case 5: _t->CreateFileForClient((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->OpenFileForClient((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProcessOperation::*)(QWebSocket * , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::loginRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProcessOperation::*)(QWebSocket * , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::loginUnlock)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProcessOperation::*)(QWebSocket * , QString , QString , QImage , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::accountCreate)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProcessOperation::*)(QWebSocket * , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::SimpleMessage)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProcessOperation::*)(QWebSocket * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::OpenDirOfClient)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ProcessOperation::*)(QWebSocket * , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::CreateFileForClient)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ProcessOperation::*)(QWebSocket * , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProcessOperation::OpenFileForClient)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProcessOperation::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ProcessOperation.data,
    qt_meta_data_ProcessOperation,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProcessOperation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProcessOperation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProcessOperation.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProcessOperation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ProcessOperation::loginRequest(QWebSocket * _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProcessOperation::loginUnlock(QWebSocket * _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProcessOperation::accountCreate(QWebSocket * _t1, QString _t2, QString _t3, QImage _t4, QString _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProcessOperation::SimpleMessage(QWebSocket * _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ProcessOperation::OpenDirOfClient(QWebSocket * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ProcessOperation::CreateFileForClient(QWebSocket * _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ProcessOperation::OpenFileForClient(QWebSocket * _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
