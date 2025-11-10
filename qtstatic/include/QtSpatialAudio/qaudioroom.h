// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-3.0-only

#ifndef QAUDIOROOM_H
#define QAUDIOROOM_H

#include <QtSpatialAudio/qtspatialaudioglobal.h>
#include <QtCore/qobject.h>

#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

class QAudioEngine;
class QAudioRoomPrivate;

class Q_SPATIALAUDIO_EXPORT QAudioRoom : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D dimensions READ dimensions WRITE setDimensions NOTIFY dimensionsChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(float reflectionGain READ reflectionGain WRITE setReflectionGain NOTIFY reflectionGainChanged)
    Q_PROPERTY(float reverbGain READ reverbGain WRITE setReverbGain NOTIFY reverbGainChanged)
    Q_PROPERTY(float reverbTime READ reverbTime WRITE setReverbTime NOTIFY reverbTimeChanged)
    Q_PROPERTY(float reverbBrightness READ reverbBrightness WRITE setReverbBrightness NOTIFY reverbBrightnessChanged)
public:
    explicit QAudioRoom(QAudioEngine *engine);
    ~QAudioRoom() override;

    enum Material {
      Transparent,
      AcousticCeilingTiles,
      BrickBare,
      BrickPainted,
      ConcreteBlockCoarse,
      ConcreteBlockPainted,
      CurtainHeavy,
      FiberGlassInsulation,
      GlassThin,
      GlassThick,
      Grass,
      LinoleumOnConcrete,
      Marble,
      Metal,
      ParquetOnConcrete,
      PlasterRough,
      PlasterSmooth,
      PlywoodPanel,
      PolishedConcreteOrTile,
      Sheetrock,
      WaterOrIceSurface,
      WoodCeiling,
      WoodPanel,
      UniformMaterial,
    };

    enum Wall {
        LeftWall,
        RightWall,
        Floor,
        Ceiling,
        FrontWall,
        BackWall
    };

    void setPosition(QVector3D pos);
    QVector3D position() const;

    void setDimensions(QVector3D dim);
    QVector3D dimensions() const;

    void setRotation(const QQuaternion &q);
    QQuaternion rotation() const;

    void setWallMaterial(Wall wall, Material material);
    Material wallMaterial(Wall wall) const;

    void setReflectionGain(float factor);
    float reflectionGain() const;

    void setReverbGain(float factor);
    float reverbGain() const;

    void setReverbTime(float factor);
    float reverbTime() const;

    void setReverbBrightness(float factor);
    float reverbBrightness() const;

Q_SIGNALS:
    void positionChanged();
    void dimensionsChanged();
    void rotationChanged();
    void wallsChanged();
    void reflectionGainChanged();
    void reverbGainChanged();
    void reverbTimeChanged();
    void reverbBrightnessChanged();

private:
    Q_DECLARE_PRIVATE(QAudioRoom)

    // ### Qt7: remove unused member
    QT6_ONLY(Q_DECL_UNUSED_MEMBER void *unused = nullptr;) // for ABI compatibility
};

QT_END_NAMESPACE

#endif
