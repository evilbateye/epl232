#ifndef PONG_HPP
#define PONG_HPP

#include <QGraphicsScene>
#include <QVector2D>
#include <QTime>
#include <QResizeEvent>

#include "qextserialport/qextserialport.h"
#include "qextserialport/qextserialenumerator.h"

class Pong : public QGraphicsScene
{
    Q_OBJECT

public:
    enum SerialCommands {
      StartGame = 0x01,
      StopGame = 0x02,
      NewGame = 0x03,
      MoveSecondPlayerUp = 0x04,
      MoveSecondPlayerDown = 0x05,
      InitGame = 0x06
    };
    enum Mode {
        MODE_SINGLE, MODE_SERIAL
    };
    enum BallEffect {
        EFFECT_NONE, EFFECT_STIRR, EFFECT_SPEED, EFFECT_SLOW, EFFECT_PORT, EFFECT_AVOID
    };
    enum PlayerEffect {
        EFFECT_PLAYER_NONE, EFFECT_PLAYER_SWITCH
    };
    struct Player {
        QGraphicsRectItem * r;
        PlayerEffect effect;
    };
    struct Ball {
        QGraphicsEllipseItem * ellipse;
        qreal angle;
        QVector2D speed;
        QVector2D oldSpeed;
        BallEffect effect;
    };

    Pong(QRect rect, QObject * parent);
    void speedUp(Ball *ball);
    void stirrBall(Ball *ball);
    void resolveCollisionWithPlayers();
    void resolveCollisionsWithWall();
    void checkIfPlayersGoOutOfMap();
    void moveAll();
    Ball * initBall(QVector2D speed);
    void applyEffects();
    void deinitEffect(Ball * ball);
    void createNewEffect(Ball * ball);
    void speedDown(Ball * ball);
    void resize(int w, int h);
    ~Pong();
public slots:
    void slotUpdate();
    void slotBGChange();
    void slotBallChange();
    void slotAction();
    void slotDataAvailable();
    void slotNewGame(bool send = true);
    void slotStartGame(bool send = true);
    void slotStopGame(bool send = true);
    void slotSingleMode();
    void slotSerialMode();
    void slotSettings();
protected:
    void keyPressEvent(QKeyEvent * e);
private:
    Player * mLeftP;
    Player * mRightP;
    bool mThisPIsLeftP;
    QRect mRect;
    QVector2D mVelocity;
    QVector2D mAcceleration;
    QVector<Ball *> mBalls;
    qreal dsmax;
    qreal dsmin;
    QGraphicsSimpleTextItem  * mLeftPScoreText;
    QGraphicsSimpleTextItem  * mRightPScoreText;
    QGraphicsSimpleTextItem  * mSeparator;
    QGraphicsItemGroup * mScoreTextGroup;
    int mLeftPScore;
    int mRightPScore;
    QextSerialPort * mPort;
    QTimer * mTimer;
    QTimer * mActionTimer;
    Mode mMode;
    quint16 mSeed;
    bool mStoppable;

    int mStirr;
    int mStirrAngle;
    int mSpeedX;
    int mSpeedY;
    int mMaxSpeedX;
    int mMaxSpeedY;
    int mMinSpeedX;
    int mMinSpeedY;
    int mFrequency;
    int mActionPeriod;
    int mNumberOfBalls;
    int mPoints;

    int mPaddleHeight;
    int mPaddleInc;
};

#endif // PONG_HPP
