// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWINDOWCAPTURE_H
#define QWINDOWCAPTURE_H

#include <QtMultimedia/qtmultimediaexports.h>
#include <QtMultimedia/qcapturablewindow.h>
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QPlatformSurfaceCapture;

class Q_MULTIMEDIA_EXPORT QWindowCapture : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QCapturableWindow window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
public:
    enum Error {
        NoError = 0,
        InternalError = 1,
        CapturingNotSupported = 2,
        CaptureFailed = 4,
        NotFound = 5,
    };
    Q_ENUM(Error)

    explicit QWindowCapture(QObject *parent = nullptr);
    ~QWindowCapture() override;

    Q_INVOKABLE static QList<QCapturableWindow> capturableWindows();

    QMediaCaptureSession *captureSession() const;

    void setWindow(QCapturableWindow window);

    QCapturableWindow window() const;

    bool isActive() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void setActive(bool active);
    void start() { setActive(true); }
    void stop() { setActive(false); }

Q_SIGNALS:
    void activeChanged(bool);
    void windowChanged(QCapturableWindow window);
    void errorChanged();
    void errorOccurred(QWindowCapture::Error error, const QString &errorString);

private:
    void setCaptureSession(QMediaCaptureSession *captureSession);
    QPlatformSurfaceCapture *platformWindowCapture() const;

    friend class QMediaCaptureSession;
    Q_DISABLE_COPY(QWindowCapture)
    Q_DECLARE_PRIVATE(QWindowCapture)
};

QT_END_NAMESPACE

#endif // QWINDOWCAPTURE_H
