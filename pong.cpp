#include <QGraphicsRectItem>
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <qmath.h>
#include <QMessageBox>
#include <QIcon>
#include <QMainWindow>

#include "pong.hpp"
#include "setportform.hpp"
#include "settingsform.hpp"

#define RAD(DEG) ((M_PI * DEG) / 180)
#define DEG(RAD) ((180 * RAD) / M_PI)
#define PLAYER_BRUSH_COLOR Qt::white

Pong::Pong(QRect rect, QObject * parent) :
    QGraphicsScene(rect, parent)
{
    setBackgroundBrush(QBrush(Qt::black));

    mSeed = (uint) QTime::currentTime().msec();

    mRect = rect;
    mMode = MODE_SINGLE;
    mThisPIsLeftP = true;
    mStoppable = false;
    mPort = NULL;

    mStirr = 15;
    mStirrAngle = 135;

    mSpeedX = 10;
    mSpeedY = 10;
    mMaxSpeedX = 20;
    mMaxSpeedY = 20;


    mMinSpeedX = 1;
    mMinSpeedY = 1;
    mFrequency = 35;
    mActionPeriod = 15;
    mNumberOfBalls = 5;
    mPoints = 10;
    mPaddleHeight = 96;
    mPaddleInc = mPaddleHeight / 1.5;

    mLeftP = new Player();
    mLeftP->r = new QGraphicsRectItem(0, 0, mRect.width() / 25, mRect.height() / 5);
    mLeftP->r->setPos(mLeftP->r->x(), mRect.height() / 2 - mLeftP->r->rect().height() / 2);
    mLeftP->r->setBrush(QBrush(PLAYER_BRUSH_COLOR));
    addItem(mLeftP->r);

    mRightP = new Player();
    mRightP->r = new QGraphicsRectItem(0, 0, mRect.width() / 25, mRect.height() / 5);
    mRightP->r->setPos(mRect.width() - mRightP->r->rect().width(), mRect.height() / 2 - mRightP->r->rect().height() / 2);
    mRightP->r->setBrush(QBrush(PLAYER_BRUSH_COLOR));
    addItem(mRightP->r);

    dsmax = (qreal) (mMaxSpeedX - mSpeedX) / (mActionPeriod * mFrequency);
    dsmin = (qreal) (mSpeedX - mMinSpeedX) / (mActionPeriod * mFrequency);

    mScoreTextGroup = new QGraphicsItemGroup();

    mSeparator = new QGraphicsSimpleTextItem();
    mSeparator->setText(":");
    mSeparator->setPen(QPen(Qt::black));
    mSeparator->setBrush(QBrush(Qt::white));
    mSeparator->setFont(QFont("Comic Sans", 25, QFont::Black));
    mSeparator->setPos(mRect.width() / 2 - mSeparator->boundingRect().width() / 2, 0);
    mScoreTextGroup->addToGroup(mSeparator);

    mLeftPScoreText = new QGraphicsSimpleTextItem();
    mLeftPScoreText->setText("0");
    mLeftPScoreText->setPen(QPen(Qt::black));
    mLeftPScoreText->setBrush(QBrush(Qt::white));
    mLeftPScoreText->setFont(QFont("Comic Sans", 25, QFont::Black));

    mLeftPScoreText->setPos(mRect.width() / 2 - mLeftPScoreText->boundingRect().width() - mSeparator->boundingRect().width() / 2, 0);
    mScoreTextGroup->addToGroup(mLeftPScoreText);

    mRightPScoreText = new QGraphicsSimpleTextItem();
    mRightPScoreText->setText("0");
    mRightPScoreText->setPen(QPen(Qt::black));
    mRightPScoreText->setBrush(QBrush(Qt::white));
    mRightPScoreText->setFont(QFont("Comic Sans", 25, QFont::Black));

    mRightPScoreText->setPos(mRect.width() / 2 + mSeparator->boundingRect().width() / 2, 0);
    mScoreTextGroup->addToGroup(mRightPScoreText);

    addItem(mScoreTextGroup);

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(slotUpdate()));

    mActionTimer = new QTimer(this);
    connect(mActionTimer, SIGNAL(timeout()), this, SLOT(slotAction()));

    slotNewGame();
}

void Pong::slotSingleMode()
{
    //TODO new random seed
    mMode = MODE_SINGLE;

    if (mPort) {
        mPort->close();
        delete mPort;
    }
}

// socat -d -d pty,raw,echo=0 pty,raw,echo=0
void Pong::slotSerialMode()
{
    mMode = MODE_SERIAL;

    //Zobrazenie formulára pre používateľa.
    SetPortForm pf((QWidget *)(this->parent()));
    if (!(pf.exec() == QDialog::Accepted)) return;

    //Inicializácia objektu reprezentujúceho port pre sériovú komunikáciu.
    mPort = new QextSerialPort(pf.getPortName().trimmed());

    if (!mPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Cannot open serial port";
        return;
    }

    //Zavedenie slotu - Ak sú dáta pripravené na čítanie z portu,
    //zavolaj funkciu slotDataAvailable
    connect(mPort, SIGNAL(readyRead()), this, SLOT(slotDataAvailable()));

    //Nastav port na základe hodnôt získaných od používateľa.
    mPort->setBaudRate(pf.getBaudRate());
    mPort->setStopBits(pf.getStopBits());
    mPort->setParity(pf.getParity());
    mPort->setFlowControl(pf.getFlow());

    //Pošli zariadeniu na druhej strane portu informáciu o tom že
    //začala nová hra a seed ktorý vygenerovala táto aplikácia
    mPort->write(QByteArray(1, InitGame).append(QString::number(mSeed)));
    slotNewGame(false);
}

void Pong::slotDataAvailable()
{
    QByteArray array = mPort->readAll();

    switch(array[0]) {
        case InitGame:
        {
            mSeed = array.right(array.size() - 1).toUInt();

            qsrand(mSeed);

            mThisPIsLeftP = false;
            slotNewGame(false);
            break;
        }
        case StartGame:
        {
            slotStartGame(false);
            break;
        }
        case NewGame:
        {
            slotNewGame(false);
            break;
        }
        case StopGame:
        {
            slotStopGame(false);
            break;
        }
        case MoveSecondPlayerUp:
        {
            if (mThisPIsLeftP) {
                mRightP->r->moveBy(0, -mPaddleInc);
            } else {
                mLeftP->r->moveBy(0, -mPaddleInc);
            }
            checkIfPlayersGoOutOfMap();
            break;
        }
        case MoveSecondPlayerDown:
        {
            if (mThisPIsLeftP) {
                mRightP->r->moveBy(0, mPaddleInc);
            } else {
                mLeftP->r->moveBy(0, mPaddleInc);
            }
            checkIfPlayersGoOutOfMap();
            break;
        }
    }
}

void Pong::slotNewGame(bool send)
{
    qsrand(mSeed);

    mTimer->stop();
    mActionTimer->stop();

    mLeftPScore = 0;
    mRightPScore = 0;
    mLeftPScoreText->setText("0");
    mLeftPScoreText->setPos(mRect.width() / 2 - mSeparator->boundingRect().width() / 2 - mLeftPScoreText->boundingRect().width() , mLeftPScoreText->y());
    mRightPScoreText->setText("0");

    foreach (Ball * ball , mBalls) {
        this->deinitEffect(ball);
        this->removeItem(ball->ellipse);
        delete ball->ellipse;
    }
    mBalls.clear();

    mBalls.push_back(initBall(QVector2D(0, 0)));

    mLeftP->r->setPos(mLeftP->r->x(), mRect.height() / 2 - mLeftP->r->rect().height() / 2);
    mRightP->r->setPos(mRect.width() - mRightP->r->rect().width(), mRect.height() / 2 - mRightP->r->rect().height() / 2);

    if (!send) return;

    switch(mMode) {
        case MODE_SERIAL:
        {
            mPort->write(QByteArray(1, NewGame));
            break;
        }
        case MODE_SINGLE:
        {
            break;
        }
    }
}

void Pong::slotStopGame(bool send)
{
    if (!mStoppable) return;

    mTimer->stop();
    mActionTimer->stop();
    mStoppable = false;

    for (int i = 0; i < mBalls.size(); i++) {
        Ball * ball = mBalls.at(i);
        ball->oldSpeed.setX(ball->speed.x());
        ball->oldSpeed.setY(ball->speed.y());
        ball->speed.setX(0);
        ball->speed.setY(0);
    }

    if (!send) return;

    switch(mMode) {
        case MODE_SERIAL:
        {
            mPort->write(QByteArray(1, StopGame));
            break;
        }
        case MODE_SINGLE:
        {
            break;
        }
    }
}

void Pong::slotStartGame(bool send)
{
    mTimer->start(1000 / mFrequency);
    mActionTimer->start(mActionPeriod * 1000);
    mStoppable = true;

    for (int i = 0; i < mBalls.size(); i++) {
        Ball * ball = mBalls.at(i);
        ball->speed.setX(ball->oldSpeed.x());
        ball->speed.setY(ball->oldSpeed.y());
    }

    if (!send) return;

    switch(mMode) {
        case MODE_SERIAL:
        {
            mPort->write(QByteArray(1, StartGame));
            break;
        }
        case MODE_SINGLE:
        {
            break;
        }
    }
}

Pong::Ball * Pong::initBall(QVector2D speed)
{
    Ball * ball = new Ball();
    ball->ellipse = new QGraphicsEllipseItem(0, 0, mLeftP->r->rect().width(), mLeftP->r->rect().width());
    ball->ellipse->setPos(mRect.width() / 2 - mLeftP->r->rect().width() / 2, mRect.height() / 2 - mLeftP->r->rect().width() / 2);
    ball->ellipse->setBrush(QBrush(Qt::white));

    int random = -1;
    if (qrand() % 2) {
        if (qrand() % 2) {
            random = qrand() % 46;
        } else {
            random = 360 - qrand() % 46;
        }
    } else {
        random = qrand() % 91 + 135;
    }

    ball->angle = RAD(random);

    ball->speed.setX(speed.x());
    ball->speed.setY(speed.y());

    ball->oldSpeed.setX(mSpeedX);
    ball->oldSpeed.setY(mSpeedY);

    ball->effect = EFFECT_NONE;

    addItem(ball->ellipse);

    return ball;
}

void Pong::deinitEffect(Pong::Ball * ball)
{
    switch (ball->effect) {
        case EFFECT_SPEED:
        {
            ball->speed.setX(mSpeedX);
            break;
        }
        case EFFECT_STIRR:
        {
            break;
        }
        case EFFECT_NONE:
        {
            break;
        }
        case EFFECT_SLOW:
        {
            ball->speed.setX(mSpeedX);
            break;
        }
        case EFFECT_PORT:
        {
            break;
        }
        case EFFECT_AVOID:
        {
            mLeftP->effect = EFFECT_PLAYER_NONE;
            mLeftP->r->setBrush(PLAYER_BRUSH_COLOR);

            mRightP->effect = EFFECT_PLAYER_NONE;
            mRightP->r->setBrush(PLAYER_BRUSH_COLOR);
            break;
        }
    }
}

void Pong::createNewEffect(Pong::Ball * ball)
{
    ball->effect = (Pong::BallEffect) (qrand() % 5 + 1);

    switch (ball->effect) {
        case EFFECT_SPEED:
        {
            ball->ellipse->setBrush(QBrush(Qt::cyan));
            break;
        }
        case EFFECT_STIRR:
        {
            ball->ellipse->setBrush(QBrush(Qt::magenta));
            break;
        }
        case EFFECT_NONE:
        {
            break;
        }
        case EFFECT_SLOW:
        {
            ball->ellipse->setBrush(QBrush(Qt::yellow));
            break;
        }
        case EFFECT_PORT:
        {
            ball->ellipse->setBrush(QBrush(QColor("#FF8C00")));
            break;
        }
        case EFFECT_AVOID:
        {
            ball->ellipse->setBrush(QBrush(Qt::red));
            break;
        }
    }
}

void Pong::slotAction()
{
    for (int i = 0; i < mBalls.size(); i++) {
        Ball * ball = mBalls.at(i);
        deinitEffect(ball);
        createNewEffect(ball);
    }

    if (mBalls.size() < mNumberOfBalls) {
        mBalls.push_back(initBall(QVector2D(mSpeedX, mSpeedY)));
    }
}

void Pong::speedUp(Pong::Ball * ball)
{
    if (ball->speed.x() < mMaxSpeedX) {
        ball->speed.setX(ball->speed.x() + dsmax);
    }
}

void Pong::speedDown(Pong::Ball * ball)
{
    if (ball->speed.x() > mMinSpeedX) {
        ball->speed.setX(ball->speed.x() - dsmin);
    }
}

void Pong::stirrBall(Pong::Ball * ball)
{
    int random = qrand() % mStirr - (mStirr / 2);

    if ((ball->angle > RAD(90) - RAD(mStirrAngle / 2)) && ball->angle < RAD(90) + RAD(mStirrAngle / 2)) {
        if (ball->angle > RAD(90)) {
            ball->angle += RAD(mStirr + 2);
        } else {
            ball->angle -= RAD(mStirr + 2);
        }
    } else if (ball->angle > RAD(270) - RAD(mStirrAngle / 2) && ball->angle < RAD(270) + RAD(mStirrAngle / 2)) {
        if (ball->angle > RAD(270)) {
            ball->angle += RAD(mStirr + 2);
        } else {
            ball->angle -= RAD(mStirr + 2);
        }
    }

    ball->angle += RAD(random);
}

void Pong::resolveCollisionWithPlayers()
{
    for (int i = 0; i < mBalls.size(); i++) {

        Ball * ball = mBalls.at(i);

        //One frame ahead
        QGraphicsEllipseItem tmp(0, 0, ball->ellipse->rect().width(), ball->ellipse->rect().height());
        tmp.setPos(ball->ellipse->x(), ball->ellipse->y());
        tmp.moveBy(qCos(ball->angle) * ball->speed.x(), qSin(ball->angle) * ball->speed.y());

        if (tmp.collidesWithItem(mLeftP->r)) {

            ball->angle = RAD(540) - ball->angle; //180 + 360 = 540
            ball->ellipse->setPos(mLeftP->r->rect().width(), ball->ellipse->y());

            if (ball->effect == EFFECT_AVOID) {
                    mRightPScoreText->setText(QString::number(++mRightPScore));
                    mRightP->effect = EFFECT_PLAYER_NONE;
                    mRightP->r->setBrush(PLAYER_BRUSH_COLOR);

                    mLeftP->r->setBrush(ball->ellipse->brush());
                    mLeftP->effect = EFFECT_PLAYER_SWITCH;
            }
        } else if (tmp.collidesWithItem(mRightP->r)) {

            ball->angle = RAD(540) - ball->angle; //180 + 360 = 540
            ball->ellipse->setPos(mRect.width() - mRightP->r->rect().width() - ball->ellipse->rect().width(), ball->ellipse->y());

            if (ball->effect == EFFECT_AVOID) {
                mLeftPScoreText->setText(QString::number(++mLeftPScore));
                mLeftPScoreText->setPos(mRect.width() / 2 - mSeparator->boundingRect().width() / 2 - mLeftPScoreText->boundingRect().width() , mLeftPScoreText->y());
                mLeftP->effect = EFFECT_PLAYER_NONE;
                mLeftP->r->setBrush(PLAYER_BRUSH_COLOR);

                mRightP->r->setBrush(ball->ellipse->brush());
                mRightP->effect = EFFECT_PLAYER_SWITCH;
            }
        } else if (ball->ellipse->x() + ball->ellipse->rect().width() > mRect.width()) {

            ball->ellipse->setPos(mLeftP->r->rect().width(), ball->ellipse->y());

            if (ball->effect != EFFECT_AVOID) {
                mLeftPScoreText->setText(QString::number(++mLeftPScore));
                mLeftPScoreText->setPos(mRect.width() / 2 - mSeparator->boundingRect().width() / 2 - mLeftPScoreText->boundingRect().width() , mLeftPScoreText->y());
            }

        } else if (ball->ellipse->x() < mRect.x()) {

            ball->ellipse->setPos(mRect.width() - mRightP->r->rect().width() - ball->ellipse->rect().width(), ball->ellipse->y());

            if (ball->effect != EFFECT_AVOID) {
                mRightPScoreText->setText(QString::number(++mRightPScore));
            }
        }

        //TODO
        if (mRightPScore == mPoints || mLeftPScore == mPoints) {

            QMessageBox m(((QMainWindow *)(parent())));
            m.setWindowTitle("Game Over");

            QString winner = "Left";
            QString looser = "Right";

            if (mRightPScore == mPoints) {
                winner = looser;
                looser = "Left";
            }

            m.setText("<p align='left'><big><strong>" + winner + "</strong></big> is the winner!<br><br>\
                      <small><strong>" + looser + "</strong></small> is a putrid looser!<br><br>\
                      Want a rematch?</p>");

            m.addButton(QMessageBox::Ok);
            m.addButton(QMessageBox::Cancel);
            switch (m.exec()) {
                case QMessageBox::Ok:
                {
                    slotNewGame();
                    break;
                }
                case QMessageBox::Cancel:
                {

                    ((QMainWindow *)(parent()))->close();
                    break;
                }
            }
        }
    }
}

void Pong::resolveCollisionsWithWall()
{
    for (int i = 0; i < mBalls.size(); i++) {

        Ball * ball = mBalls.at(i);

        if (ball->ellipse->y() < 0 || ball->ellipse->y() + ball->ellipse->rect().height() > mRect.height()) {
            if (ball->effect == Pong::EFFECT_PORT) {
                if (ball->ellipse->y() < 0) {
                    ball->ellipse->setPos(ball->ellipse->x(), mRect.height() - ball->ellipse->rect().height());
                } else if(ball->ellipse->y() + ball->ellipse->rect().height() > mRect.height()) {
                    ball->ellipse->setPos(ball->ellipse->x(), 0);
                }
            } else {
                ball->angle = RAD(360) - ball->angle;
            }
        }
    }
}

void Pong::moveAll()
{
    for (int i = 0; i < mBalls.size(); i++) {
        Ball * ball = mBalls.at(i);
        ball->ellipse->moveBy(qCos(ball->angle) * ball->speed.x(), qSin(ball->angle) * ball->speed.y());
    }
}

void Pong::applyEffects()
{
    for (int i = 0; i < mBalls.size(); i++) {

        Ball * ball = mBalls.at(i);

        switch(ball->effect) {
            case Pong::EFFECT_STIRR:
            {
                stirrBall(ball);
                break;
            }

            case Pong::EFFECT_SPEED:
            {
                speedUp(ball);
                break;
            }

            case Pong::EFFECT_NONE:
            {
                break;
            }

            case Pong::EFFECT_SLOW:
            {
                speedDown(ball);
                break;
            }
            case EFFECT_PORT:
            {
                break;
            }
            case EFFECT_AVOID:
            {
                break;
            }
        }
    }
}

void Pong::slotUpdate()
{
    applyEffects();
    resolveCollisionWithPlayers();
    resolveCollisionsWithWall();
    moveAll();
}

void Pong::keyPressEvent(QKeyEvent * e) {

    switch(mMode) {
        case MODE_SERIAL:
        {
switch(e->key()) {
    //Tento hráč stlačil šípku hore.
    case (Qt::Key_Up):
    {
        //Tento hráč je v hracom poli vľavo.
        if (mThisPIsLeftP) {
            if (mLeftP->effect == EFFECT_PLAYER_SWITCH) {
                mLeftP->r->moveBy(0, mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerDown));
            } else {
                mLeftP->r->moveBy(0, -mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerUp));
            }
        } else {
            if (mRightP->effect == EFFECT_PLAYER_SWITCH) {
                mRightP->r->moveBy(0, mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerDown));
            } else {
                mRightP->r->moveBy(0, -mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerUp));
            }
        }

        break;
    }
    case (Qt::Key_Down):
    {
        if (mThisPIsLeftP) {
            if (mLeftP->effect == EFFECT_PLAYER_SWITCH) {
                mLeftP->r->moveBy(0, -mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerUp));
            } else {
                mLeftP->r->moveBy(0, mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerDown));
            }
        } else {
            if (mRightP->effect == EFFECT_PLAYER_SWITCH) {
                mRightP->r->moveBy(0, -mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerUp));
            } else {
                mRightP->r->moveBy(0, mPaddleInc);
                mPort->write(QByteArray(1, MoveSecondPlayerDown));
            }
        }
        break;
    }
}

            break;
        }
        case MODE_SINGLE:
        {
            switch(e->key()) {
                case (Qt::Key_W):
                {
                    if (mLeftP->effect == EFFECT_PLAYER_SWITCH) {
                        mLeftP->r->moveBy(0, mPaddleInc);
                    } else {
                        mLeftP->r->moveBy(0, -mPaddleInc);
                    }
                    break;
                }
                case (Qt::Key_S):
                {
                    if (mLeftP->effect == EFFECT_PLAYER_SWITCH) {
                        mLeftP->r->moveBy(0, -mPaddleInc);
                    } else {
                        mLeftP->r->moveBy(0, mPaddleInc);
                    }
                    break;
                }
                case (Qt::Key_Up):
                {
                    if (mRightP->effect == EFFECT_PLAYER_SWITCH) {
                        mRightP->r->moveBy(0, mPaddleInc);
                    } else {
                        mRightP->r->moveBy(0, -mPaddleInc);
                    }
                    break;
                }
                case (Qt::Key_Down):
                {
                    if (mRightP->effect == EFFECT_PLAYER_SWITCH) {
                        mRightP->r->moveBy(0, -mPaddleInc);
                    } else {
                        mRightP->r->moveBy(0, mPaddleInc);
                    }
                    break;
                }
                case (Qt::Key_P):
                {
                    if (mStoppable) {
                        slotStopGame();
                    } else {
                        slotStartGame();
                    }
                    break;
                }
            }

            break;
        }
    }

    checkIfPlayersGoOutOfMap();
}

void Pong::checkIfPlayersGoOutOfMap()
{
    if (mLeftP->r->y() < 0) mLeftP->r->setPos(mLeftP->r->x(), mRect.height() - mLeftP->r->rect().height());
    if (mLeftP->r->y() >= mRect.height()) mLeftP->r->setPos(mLeftP->r->x(), 0);
    if (mRightP->r->y() < 0) mRightP->r->setPos(mRightP->r->x(), mRect.height() - mRightP->r->rect().height());
    if (mRightP->r->y() >= mRect.height()) mRightP->r->setPos(mRightP->r->x(), 0);
}

void Pong::resize(int w, int h)
{
    mRect.setWidth(w);
    mRect.setHeight(h);

    mLeftP->r->setPos(mLeftP->r->x(), mRect.height() / 2 - mLeftP->r->rect().height() / 2);
    mRightP->r->setPos(mRect.width() - mRightP->r->rect().width(), mRect.height() / 2 - mRightP->r->rect().height() / 2);

//    ball->ellipse->setPos(mRect.width() / 2 - mLeftP->rect->rect().width() / 2, mRect.height() / 2 - mLeftP->rect->rect().width() / 2);

    mSeparator->setPos(mRect.width() / 2 - mSeparator->boundingRect().width() / 2, 0);
    mLeftPScoreText->setPos(mRect.width() / 2 - mLeftPScoreText->boundingRect().width() - mSeparator->boundingRect().width() / 2, 0);
    mRightPScoreText->setPos(mRect.width() / 2 + mSeparator->boundingRect().width() / 2, 0);
}

void Pong::slotSettings()
{
    slotStopGame();

    SettingsForm form(mStirr, 180 - mStirrAngle,
                      mSpeedX, mSpeedY,
                      mMaxSpeedX, mMaxSpeedY,
                      mMinSpeedX, mMinSpeedY,
                      mFrequency, mActionPeriod,
                      mNumberOfBalls, mPoints,
                      mPaddleHeight, mPaddleInc, (QMainWindow *)this->parent());

    if (!(form.exec() == QDialog::Accepted)) {
        return;
    }

    mStirr = form.getStirrInc();
    mStirrAngle = 180 - form.getStirrAngle();
    mSpeedX = form.getSpeedX();
    mSpeedY = form.getSpeedY();

    mMaxSpeedX = form.getMaxSpeedX();
    mMaxSpeedY = form.getMaxSpeedY();
    mMinSpeedX = form.getMinSpeedX();
    mMinSpeedY = form.getMinSpeedY();
    mFrequency = form.getFrequency();
    mActionPeriod = form.getActionPeriod();
    mNumberOfBalls = form.getNumbOfBalls();
    mPoints = form.getNumbOfPoints();

    mPaddleHeight = form.getPaddleH();
    mLeftP->r->setRect(0, 0, mLeftP->r->rect().width(), mPaddleHeight);
    mRightP->r->setRect(0, 0, mRightP->r->rect().width(), mPaddleHeight);

    mPaddleInc = form.getPaddleInc();

    dsmax = (qreal) (mMaxSpeedX - mSpeedX) / (mActionPeriod * mFrequency);
    dsmin = (qreal) (mSpeedX - mMinSpeedX) / (mActionPeriod * mFrequency);

    //TODO
    slotNewGame(false);
}

//TODO
void Pong::slotBGChange()
{
    qDebug()<<"implement";
}

//TODO
void Pong::slotBallChange()
{
    qDebug()<<"implement";
}

Pong::~Pong()
{
    if (mLeftP) delete mLeftP;
    if (mRightP) delete mRightP;

    foreach (Ball * ball, mBalls) {
        if (ball) delete ball;
    }

    if (mPort) {
        mPort->close();
        delete mPort;
    }
}
