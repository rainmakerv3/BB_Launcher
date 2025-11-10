// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMEDIAMETADATA_H
#define QMEDIAMETADATA_H

#if 0
#pragma qt_class(QMediaMetaData)
#endif

#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>
#include <QtCore/qhash.h>
#include <QtMultimedia/qtmultimediaglobal.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QMediaMetaData
{
    Q_GADGET
public:
    enum Key {
        Title,
        Author,
        Comment,
        Description,
        Genre,
        Date,

        Language,
        Publisher,
        Copyright,
        Url,

        Duration,
        MediaType,
        FileFormat,

        AudioBitRate,
        AudioCodec,
        VideoBitRate,
        VideoCodec,
        VideoFrameRate,

        AlbumTitle,
        AlbumArtist,
        ContributingArtist,
        TrackNumber,
        Composer,
        LeadPerformer,

        ThumbnailImage,
        CoverArtImage,

        Orientation,
        Resolution,

        HasHdrContent,
    };
    Q_ENUM(Key)

    static constexpr int NumMetaData = HasHdrContent + 1;

//    QMetaType typeForKey(Key k);
    Q_INVOKABLE QVariant value(Key k) const { return data.value(k); }
    Q_INVOKABLE void insert(Key k, const QVariant &value) { data.insert(k, value); }
    Q_INVOKABLE void remove(Key k) { data.remove(k); }
    Q_INVOKABLE QList<Key> keys() const { return data.keys(); }

    QVariant &operator[](Key k) { return data[k]; }
    Q_INVOKABLE void clear() { data.clear(); }

    Q_INVOKABLE bool isEmpty() const { return data.isEmpty(); }
    Q_INVOKABLE QString stringValue(Key k) const;

    Q_INVOKABLE static QString metaDataKeyToString(Key k);

    QT_POST_CXX17_API_IN_EXPORTED_CLASS // don't export QHash's key-value-range
    auto asKeyValueRange() const { return data.asKeyValueRange(); }

protected:
    Q_MULTIMEDIA_EXPORT friend QDebug operator<<(QDebug, const QMediaMetaData &);

    friend bool operator==(const QMediaMetaData &a, const QMediaMetaData &b)
    { return a.data == b.data; }
    friend bool operator!=(const QMediaMetaData &a, const QMediaMetaData &b)
    { return a.data != b.data; }

    static QMetaType keyType(Key key);

    QHash<Key, QVariant> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QMediaMetaData)

#endif // QMEDIAMETADATA_H
