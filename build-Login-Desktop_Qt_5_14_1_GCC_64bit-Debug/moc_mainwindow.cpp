/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../Login/mainwindow.h"
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
    QByteArrayData data[18];
    char stringdata0[242];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 15), // "on_sign_clicked"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 12), // "fieldForSign"
QT_MOC_LITERAL(4, 41, 5), // "state"
QT_MOC_LITERAL(5, 47, 15), // "on_back_clicked"
QT_MOC_LITERAL(6, 63, 16), // "on_login_clicked"
QT_MOC_LITERAL(7, 80, 11), // "onConnected"
QT_MOC_LITERAL(8, 92, 11), // "onSslErrors"
QT_MOC_LITERAL(9, 104, 16), // "QList<QSslError>"
QT_MOC_LITERAL(10, 121, 6), // "errors"
QT_MOC_LITERAL(11, 128, 23), // "on_sign_account_clicked"
QT_MOC_LITERAL(12, 152, 25), // "MessageReceivedFromServer"
QT_MOC_LITERAL(13, 178, 7), // "message"
QT_MOC_LITERAL(14, 186, 12), // "Registeruser"
QT_MOC_LITERAL(15, 199, 15), // "StartNewWindows"
QT_MOC_LITERAL(16, 215, 15), // "StartEditorText"
QT_MOC_LITERAL(17, 231, 10) // "fileeditor"

    },
    "MainWindow\0on_sign_clicked\0\0fieldForSign\0"
    "state\0on_back_clicked\0on_login_clicked\0"
    "onConnected\0onSslErrors\0QList<QSslError>\0"
    "errors\0on_sign_account_clicked\0"
    "MessageReceivedFromServer\0message\0"
    "Registeruser\0StartNewWindows\0"
    "StartEditorText\0fileeditor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    1,   70,    2, 0x08 /* Private */,
       5,    0,   73,    2, 0x08 /* Private */,
       6,    0,   74,    2, 0x08 /* Private */,
       7,    0,   75,    2, 0x08 /* Private */,
       8,    1,   76,    2, 0x08 /* Private */,
      11,    0,   79,    2, 0x08 /* Private */,
      12,    1,   80,    2, 0x08 /* Private */,
      14,    0,   83,    2, 0x08 /* Private */,
      15,    0,   84,    2, 0x08 /* Private */,
      16,    1,   85,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_sign_clicked(); break;
        case 1: _t->fieldForSign((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->on_back_clicked(); break;
        case 3: _t->on_login_clicked(); break;
        case 4: _t->onConnected(); break;
        case 5: _t->onSslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 6: _t->on_sign_account_clicked(); break;
        case 7: _t->MessageReceivedFromServer((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 8: _t->Registeruser(); break;
        case 9: _t->StartNewWindows(); break;
        case 10: _t->StartEditorText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
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
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
