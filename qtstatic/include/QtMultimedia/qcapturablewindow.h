// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCAPTURABLEWINDOW_H
#define QCAPTURABLEWINDOW_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QCapturableWindowPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QCapturableWindowPrivate, Q_MULTIMEDIA_EXPORT)

class QMediaCaptureSession;
class QWindow;
class QWindowCapturePrivate;

class QCapturableWindow
{
    Q_GADGET_EXPORT(Q_MULTIMEDIA_EXPORT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
public:
    Q_MULTIMEDIA_EXPORT QCapturableWindow();
    Q_MULTIMEDIA_EXPORT Q_INVOKABLE Q_REVISION(6, 10) explicit QCapturableWindow(QWindow *window);

    Q_MULTIMEDIA_EXPORT ~QCapturableWindow();

    Q_MULTIMEDIA_EXPORT QCapturableWindow(const QCapturableWindow &other);

    QCapturableWindow(QCapturableWindow &&other) noexcept = default;

    Q_MULTIMEDIA_EXPORT QCapturableWindow& operator=(const QCapturableWindow &other);

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QCapturableWindow);

    void swap(QCapturableWindow &other) noexcept
    { d.swap(other.d); }

    Q_MULTIMEDIA_EXPORT friend bool operator==(const QCapturableWindow &lhs, const QCapturableWindow &rhs) noexcept;

    friend bool operator!=(const QCapturableWindow &lhs, const QCapturableWindow &rhs) noexcept
    { return !(lhs == rhs); }

    Q_MULTIMEDIA_EXPORT bool isValid() const;

    Q_MULTIMEDIA_EXPORT QString description() const;

#ifndef QT_NO_DEBUG_STREAM
    Q_MULTIMEDIA_EXPORT friend QDebug operator<<(QDebug, const QCapturableWindow &);
#endif

private:
    Q_MULTIMEDIA_EXPORT QCapturableWindow(QCapturableWindowPrivate *capturablePrivate);
    friend class QCapturableWindowPrivate;

    QExplicitlySharedDataPointer<QCapturableWindowPrivate> d;
};

Q_DECLARE_SHARED(QCapturableWindow)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QCapturableWindow)

#endif // QCAPTURABLEWINDOW_H
