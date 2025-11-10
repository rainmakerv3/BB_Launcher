// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QtWidgets/qwidget.h>

#include <QtMultimediaWidgets/qtmultimediawidgetsglobal.h>

QT_BEGIN_NAMESPACE

class QVideoSink;

class QVideoWidgetPrivate;
class Q_MULTIMEDIAWIDGETS_EXPORT QVideoWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
    Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode NOTIFY aspectRatioModeChanged)

public:
    explicit QVideoWidget(QWidget *parent = nullptr);
    ~QVideoWidget() override;

    Q_INVOKABLE QVideoSink *videoSink() const;

#ifdef Q_QDOC
    bool isFullScreen() const;
#endif

    Qt::AspectRatioMode aspectRatioMode() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setFullScreen(bool fullScreen);
    void setAspectRatioMode(Qt::AspectRatioMode mode);

Q_SIGNALS:
    void fullScreenChanged(bool fullScreen);
    void aspectRatioModeChanged(Qt::AspectRatioMode mode);

protected:
    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    QVideoWidgetPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QVideoWidget)
};

QT_END_NAMESPACE


#endif
