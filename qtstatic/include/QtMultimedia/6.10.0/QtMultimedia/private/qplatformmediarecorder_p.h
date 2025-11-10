// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QPLATFORMMEDIARECORDER_H
#define QPLATFORMMEDIARECORDER_H

#include <QtCore/qurl.h>
#include <QtCore/qsize.h>
#if QT_CONFIG(mimetype)
#include <QtCore/qmimetype.h>
#endif
#include <QtCore/qpointer.h>
#include <QtCore/qiodevice.h>

#include <QtMultimedia/qmediarecorder.h>
#include <QtMultimedia/qmediametadata.h>
#include <QtMultimedia/qmediaformat.h>
#include <QtMultimedia/private/qerrorinfo_p.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QUrl;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QMediaEncoderSettings
{
    QMediaRecorder::EncodingMode m_encodingMode = QMediaRecorder::ConstantQualityEncoding;
    QMediaRecorder::Quality m_quality = QMediaRecorder::NormalQuality;

    QMediaFormat m_format;
    int m_audioBitrate = -1;
    int m_audioSampleRate = -1;
    int m_audioChannels = -1;

    QSize m_videoResolution = QSize(-1, -1);
    int m_videoFrameRate = -1;
    int m_videoBitRate = -1;
public:

    QMediaFormat mediaFormat() const { return m_format; }
    void setMediaFormat(const QMediaFormat &format) { m_format = format; }
    void resolveFormat(QMediaFormat::ResolveFlags flags = QMediaFormat::NoFlags)
    { m_format.resolveForEncoding(flags); }

    QMediaFormat::FileFormat fileFormat() const { return mediaFormat().fileFormat(); }
    QMediaFormat::VideoCodec videoCodec() const { return mediaFormat().videoCodec(); }
    QMediaFormat::AudioCodec audioCodec() const { return mediaFormat().audioCodec(); }
#if QT_CONFIG(mimetype)
    QMimeType mimeType() const { return mediaFormat().mimeType(); }
#endif
    QString preferredSuffix() const
    {
#if QT_CONFIG(mimetype)
        return mimeType().preferredSuffix();
#else
        return QStringLiteral("");
#endif
    }

    QMediaRecorder::EncodingMode encodingMode() const { return m_encodingMode; }
    void setEncodingMode(QMediaRecorder::EncodingMode mode) { m_encodingMode = mode; }

    QMediaRecorder::Quality quality() const { return m_quality; }
    void setQuality(QMediaRecorder::Quality quality) { m_quality = quality; }

    QSize videoResolution() const { return m_videoResolution; }
    void setVideoResolution(const QSize &size) { m_videoResolution = size; }

    qreal videoFrameRate() const { return m_videoFrameRate; }
    void setVideoFrameRate(qreal rate) { m_videoFrameRate = rate; }

    int videoBitRate() const { return m_videoBitRate; }
    void setVideoBitRate(int bitrate) { m_videoBitRate = bitrate; }

    int audioBitRate() const { return m_audioBitrate; }
    void setAudioBitRate(int bitrate) { m_audioBitrate = bitrate; }

    int audioChannelCount() const { return m_audioChannels; }
    void setAudioChannelCount(int channels) { m_audioChannels = channels; }

    int audioSampleRate() const { return m_audioSampleRate; }
    void setAudioSampleRate(int rate) { m_audioSampleRate = rate; }

    bool operator==(const QMediaEncoderSettings &other) const
    {
        return m_format == other.m_format &&
               m_encodingMode == other.m_encodingMode &&
               m_quality == other.m_quality &&
               m_audioBitrate == other.m_audioBitrate &&
               m_audioSampleRate == other.m_audioSampleRate &&
               m_audioChannels == other.m_audioChannels &&
               m_videoResolution == other.m_videoResolution &&
               m_videoFrameRate == other.m_videoFrameRate &&
               m_videoBitRate == other.m_videoBitRate;
    }

    bool operator!=(const QMediaEncoderSettings &other) const
    { return !operator==(other); }
};

class Q_MULTIMEDIA_EXPORT QPlatformMediaRecorder
{
public:
    virtual ~QPlatformMediaRecorder() {}

    virtual bool isLocationWritable(const QUrl &location) const = 0;

    virtual QMediaRecorder::RecorderState state() const { return m_state; }
    virtual void record(QMediaEncoderSettings &settings) = 0;
    virtual void pause();
    virtual void resume();
    virtual void stop() = 0;

    virtual qint64 duration() const { return m_duration; }

    virtual void setMetaData(const QMediaMetaData &) {}
    virtual QMediaMetaData metaData() const { return {}; }

    QMediaRecorder::Error error() const { return m_error.code(); }
    QString errorString() const { return m_error.description(); }

    QUrl outputLocation() const { return m_outputLocation; }
    virtual void setOutputLocation(const QUrl &location) { m_outputLocation = location; }
    QUrl actualLocation() const { return m_actualLocation; }
    void clearActualLocation() { m_actualLocation.clear(); }
    void clearError() { updateError(QMediaRecorder::NoError, QString()); }

    QIODevice *outputDevice() const { return m_outputDevice; }
    void setOutputDevice(QIODevice *device) { m_outputDevice = device; }

    virtual void updateAutoStop() { }

protected:
    explicit QPlatformMediaRecorder(QMediaRecorder *parent);

    void stateChanged(QMediaRecorder::RecorderState state);
    void durationChanged(qint64 position);
    void actualLocationChanged(const QUrl &location);
    void updateError(QMediaRecorder::Error error, const QString &errorString);
    void metaDataChanged();

    QMediaRecorder *mediaRecorder() { return q; }

    QString findActualLocation(const QMediaEncoderSettings &settings) const;

private:
    QMediaRecorder *q = nullptr;
    QErrorInfo<QMediaRecorder::Error> m_error;
    QUrl m_actualLocation;
    QUrl m_outputLocation;
    QPointer<QIODevice> m_outputDevice;
    qint64 m_duration = 0;

    QMediaRecorder::RecorderState m_state = QMediaRecorder::StoppedState;
};

QT_END_NAMESPACE

#endif
