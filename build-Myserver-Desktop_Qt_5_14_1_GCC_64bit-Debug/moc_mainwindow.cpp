/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../MyserverEncrypt/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[32];
    char stringdata0[479];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 22), // "on_startserver_clicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 21), // "on_stopserver_clicked"
QT_MOC_LITERAL(4, 57, 15), // "onNewConnection"
QT_MOC_LITERAL(5, 73, 18), // "processTextMessage"
QT_MOC_LITERAL(6, 92, 7), // "message"
QT_MOC_LITERAL(7, 100, 20), // "processBinaryMessage"
QT_MOC_LITERAL(8, 121, 18), // "socketDisconnected"
QT_MOC_LITERAL(9, 140, 11), // "onSslErrors"
QT_MOC_LITERAL(10, 152, 16), // "QList<QSslError>"
QT_MOC_LITERAL(11, 169, 6), // "errors"
QT_MOC_LITERAL(12, 176, 24), // "on_startserver_2_clicked"
QT_MOC_LITERAL(13, 201, 14), // "prepareToStart"
QT_MOC_LITERAL(14, 216, 21), // "SimpleTextMessageTest"
QT_MOC_LITERAL(15, 238, 18), // "serverLoginRequest"
QT_MOC_LITERAL(16, 257, 11), // "QWebSocket*"
QT_MOC_LITERAL(17, 269, 12), // "clientSocket"
QT_MOC_LITERAL(18, 282, 8), // "username"
QT_MOC_LITERAL(19, 291, 17), // "serverLoginUnlock"
QT_MOC_LITERAL(20, 309, 5), // "token"
QT_MOC_LITERAL(21, 315, 19), // "serverAccountCreate"
QT_MOC_LITERAL(22, 335, 8), // "nickname"
QT_MOC_LITERAL(23, 344, 4), // "icon"
QT_MOC_LITERAL(24, 349, 8), // "password"
QT_MOC_LITERAL(25, 358, 21), // "serverErrorConnection"
QT_MOC_LITERAL(26, 380, 29), // "QWebSocketProtocol::CloseCode"
QT_MOC_LITERAL(27, 410, 9), // "closeCode"
QT_MOC_LITERAL(28, 420, 15), // "OpenDirOfClient"
QT_MOC_LITERAL(29, 436, 19), // "CreateFileForClient"
QT_MOC_LITERAL(30, 456, 4), // "file"
QT_MOC_LITERAL(31, 461, 17) // "OpenFileForClient"

    },
    "MainWindow\0on_startserver_clicked\0\0"
    "on_stopserver_clicked\0onNewConnection\0"
    "processTextMessage\0message\0"
    "processBinaryMessage\0socketDisconnected\0"
    "onSslErrors\0QList<QSslError>\0errors\0"
    "on_startserver_2_clicked\0prepareToStart\0"
    "SimpleTextMessageTest\0serverLoginRequest\0"
    "QWebSocket*\0clientSocket\0username\0"
    "serverLoginUnlock\0token\0serverAccountCreate\0"
    "nickname\0icon\0password\0serverErrorConnection\0"
    "QWebSocketProtocol::CloseCode\0closeCode\0"
    "OpenDirOfClient\0CreateFileForClient\0"
    "file\0OpenFileForClient"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x08 /* Private */,
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    1,  102,    2, 0x08 /* Private */,
       7,    1,  105,    2, 0x08 /* Private */,
       8,    0,  108,    2, 0x08 /* Private */,
       9,    1,  109,    2, 0x08 /* Private */,
      12,    0,  112,    2, 0x08 /* Private */,
      13,    0,  113,    2, 0x0a /* Public */,
      14,    0,  114,    2, 0x0a /* Public */,
      15,    2,  115,    2, 0x0a /* Public */,
      19,    2,  120,    2, 0x0a /* Public */,
      21,    5,  125,    2, 0x0a /* Public */,
      25,    1,  136,    2, 0x0a /* Public */,
      28,    1,  139,    2, 0x0a /* Public */,
      29,    2,  142,    2, 0x0a /* Public */,
      31,    2,  147,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QByteArray,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QString,   17,   18,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QString,   17,   20,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QString, QMetaType::QString, QMetaType::QImage, QMetaType::QString,   17,   18,   22,   23,   24,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QString,   17,   30,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QString,   17,   30,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_startserver_clicked(); break;
        case 1: _t->on_stopserver_clicked(); break;
        case 2: _t->onNewConnection(); break;
        case 3: _t->processTextMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->processBinaryMessage((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 5: _t->socketDisconnected(); break;
        case 6: _t->onSslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 7: _t->on_startserver_2_clicked(); break;
        case 8: _t->prepareToStart(); break;
        case 9: _t->SimpleTextMessageTest(); break;
        case 10: _t->serverLoginRequest((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->serverLoginUnlock((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->serverAccountCreate((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QImage(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        case 13: _t->serverErrorConnection((*reinterpret_cast< QWebSocketProtocol::CloseCode(*)>(_a[1]))); break;
        case 14: _t->OpenDirOfClient((*reinterpret_cast< QWebSocket*(*)>(_a[1]))); break;
        case 15: _t->CreateFileForClient((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 16: _t->OpenFileForClient((*reinterpret_cast< QWebSocket*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebSocket* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
