// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIAFORMAT_H
#define QMEDIAFORMAT_H

#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qtmetamacros.h>
#include <QtMultimedia/qtmultimediaglobal.h>

QT_BEGIN_NAMESPACE

class QMimeType;
class QMediaFormat;
class QMediaFormatPrivate;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QMediaFormatPrivate, Q_MULTIMEDIA_EXPORT)

class Q_MULTIMEDIA_EXPORT QMediaFormat
{
    Q_GADGET
    Q_PROPERTY(FileFormat fileFormat READ fileFormat WRITE setFileFormat)
    Q_PROPERTY(AudioCodec audioCodec READ audioCodec WRITE setAudioCodec)
    Q_PROPERTY(VideoCodec videoCodec READ videoCodec WRITE setVideoCodec)
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    enum FileFormat {
        UnspecifiedFormat = -1,
        // Video Formats
        WMV,
        AVI,
        Matroska,
        MPEG4,
        Ogg,
        QuickTime,
        WebM,
        // Audio Only Formats
        Mpeg4Audio,
        AAC,
        WMA,
        MP3,
        FLAC,
        Wave,
        LastFileFormat = Wave
    };
    Q_ENUM(FileFormat)

    enum class AudioCodec {
        Unspecified = -1,
        MP3,
        AAC,
        AC3,
        EAC3,
        FLAC,
        DolbyTrueHD,
        Opus,
        Vorbis,
        Wave,
        WMA,
        ALAC,
        LastAudioCodec = ALAC
    };
    Q_ENUM(AudioCodec)

    enum class VideoCodec {
        Unspecified = -1,
        MPEG1,
        MPEG2,
        MPEG4,
        H264,
        H265,
        VP8,
        VP9,
        AV1,
        Theora,
        WMV,
        MotionJPEG,
        LastVideoCodec = MotionJPEG
    };
    Q_ENUM(VideoCodec)

    enum ConversionMode {
        Encode,
        Decode
    };
    Q_ENUM(ConversionMode)

    enum ResolveFlags
    {
        NoFlags,
        RequiresVideo
    };

    QMediaFormat(FileFormat format = UnspecifiedFormat);
    ~QMediaFormat();
    QMediaFormat(const QMediaFormat &other) noexcept;
    QMediaFormat &operator=(const QMediaFormat &other) noexcept;

    QMediaFormat(QMediaFormat &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QMediaFormat)
    void swap(QMediaFormat &other) noexcept
    {
        std::swap(fmt, other.fmt);
        std::swap(audio, other.audio);
        std::swap(video, other.video);
        d.swap(other.d);
    }

    FileFormat fileFormat() const { return fmt; }
    void setFileFormat(FileFormat f) { fmt = f; }

    void setVideoCodec(VideoCodec codec) { video = codec; }
    VideoCodec videoCodec() const { return video; }

    void setAudioCodec(AudioCodec codec) { audio = codec; }
    AudioCodec audioCodec() const { return audio; }

    Q_INVOKABLE bool isSupported(ConversionMode mode) const;

#if QT_CONFIG(mimetype)
    QMimeType mimeType() const;
#endif

    Q_INVOKABLE QList<FileFormat> supportedFileFormats(ConversionMode m);
    Q_INVOKABLE QList<VideoCodec> supportedVideoCodecs(ConversionMode m);
    Q_INVOKABLE QList<AudioCodec> supportedAudioCodecs(ConversionMode m);

    Q_INVOKABLE static QString fileFormatName(FileFormat fileFormat);
    Q_INVOKABLE static QString audioCodecName(AudioCodec codec);
    Q_INVOKABLE static QString videoCodecName(VideoCodec codec);

    Q_INVOKABLE static QString fileFormatDescription(QMediaFormat::FileFormat fileFormat);
    Q_INVOKABLE static QString audioCodecDescription(QMediaFormat::AudioCodec codec);
    Q_INVOKABLE static QString videoCodecDescription(QMediaFormat::VideoCodec codec);

    bool operator==(const QMediaFormat &other) const;
    bool operator!=(const QMediaFormat &other) const
    { return !operator==(other); }

    void resolveForEncoding(ResolveFlags flags);

protected:
    friend class QMediaFormatPrivate;
    FileFormat fmt;
    AudioCodec audio = AudioCodec::Unspecified;
    VideoCodec video = VideoCodec::Unspecified;
    QExplicitlySharedDataPointer<QMediaFormatPrivate> d;
};

QT_END_NAMESPACE

#endif
