// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLATFORMSURFACECAPTURE_H
#define QPLATFORMSURFACECAPTURE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qpointer.h>
#include <QtGui/qscreen.h>
#include <QtMultimedia/qcapturablewindow.h>
#include <QtMultimedia/private/qerrorinfo_p.h>
#include <QtMultimedia/private/qplatformvideosource_p.h>

#include <variant>

QT_BEGIN_NAMESPACE

class QVideoFrame;

class Q_MULTIMEDIA_EXPORT QPlatformSurfaceCapture : public QPlatformVideoSource
{
    Q_OBJECT

public:
    enum Error {
        NoError = 0,
        InternalError = 1,
        CapturingNotSupported = 2,
        CaptureFailed = 4,
        NotFound = 5,
    };

    using ScreenSource = QPointer<QScreen>;
    using WindowSource = QCapturableWindow;

    using Source = std::variant<ScreenSource, WindowSource>;

    explicit QPlatformSurfaceCapture(Source initialSource);

    void setActive(bool active) override;
    bool isActive() const override;

    void setSource(Source source);

    template<typename Type>
    Type source() const {
        return *q_check_ptr(std::get_if<Type>(&m_source));
    }

    Source source() const { return m_source; }

    Error error() const;
    QString errorString() const final;

protected:
    virtual bool setActiveInternal(bool) = 0;

    bool checkScreenWithError(ScreenSource &screen);

public Q_SLOTS:
    void updateError(Error error, const QString &errorString);

Q_SIGNALS:
    void sourceChanged(WindowSource);
    void sourceChanged(ScreenSource);
    void errorOccurred(Error error, QString errorString);

private:
    QErrorInfo<Error> m_error;
    Source m_source;
    bool m_active = false;
};

QT_END_NAMESPACE

#endif // QPLATFORMSURFACECAPTURE_H
