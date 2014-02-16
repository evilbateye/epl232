#include "settingsform.hpp"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(int stirr, int stirrA,
                           int sX, int sY,
                           int maxX, int maxY,
                           int minX, int minY,
                           int f, int ap,
                           int b, int p,
                           int ph, int pi, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    ui->spinBoxActionPeriod->setValue(ap);
    ui->spinBoxMaxBalls->setValue(b);
    ui->spinBoxPoints->setValue(p);

    ui->dialFrequency->setValue(f);
    ui->dialStirr->setValue(stirr);
    ui->dialStirrAngle->setValue(stirrA);

    ui->verticalSliderMinX->setValue(minX);
    ui->verticalSliderMinY->setValue(minY);

    ui->verticalSliderMaxX->setValue(maxX);
    ui->verticalSliderMaxY->setValue(maxY);

    ui->verticalSliderSpeedX->setValue(sX);
    ui->verticalSliderSpeedY->setValue(sY);

    ui->horizontalSliderPaddleHeight->setValue(ph);
    ui->horizontalSliderPaddleInc->setValue(pi);
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

int SettingsForm::getPaddleH()
{
    return ui->horizontalSliderPaddleHeight->value();
}

int SettingsForm::getPaddleInc()
{
    return ui->horizontalSliderPaddleInc->value();
}

int SettingsForm::getActionPeriod()
{
    return ui->spinBoxActionPeriod->value();
}

int SettingsForm::getFrequency()
{
    return ui->dialFrequency->value();
}

int SettingsForm::getMaxSpeedX()
{

    return ui->verticalSliderMaxX->value();
}

int SettingsForm::getMaxSpeedY()
{

    return ui->verticalSliderMaxY->value();
}

int SettingsForm::getMinSpeedX()
{
    return ui->verticalSliderMinX->value();
}

int SettingsForm::getMinSpeedY()
{
    return ui->verticalSliderMinY->value();
}

int SettingsForm::getNumbOfBalls()
{
    return ui->spinBoxMaxBalls->value();
}

int SettingsForm::getNumbOfPoints()
{
    return ui->spinBoxPoints->value();
}

int SettingsForm::getSpeedX()
{
    return ui->verticalSliderSpeedX->value();
}

int SettingsForm::getSpeedY()
{
    return ui->verticalSliderSpeedY->value();
}

int SettingsForm::getStirrAngle()
{
    return ui->dialStirrAngle->value();
}

int SettingsForm::getStirrInc()
{
    return ui->dialStirr->value();
}

void SettingsForm::on_verticalSliderMaxX_valueChanged(int value)
{
    int startVal = ui->verticalSliderSpeedX->value();
    if (value < startVal) {
        ui->verticalSliderMaxX->setValue(startVal);
    }
}

void SettingsForm::on_verticalSliderMaxY_valueChanged(int value)
{
    int startVal = ui->verticalSliderSpeedY->value();
    if (value < startVal) {
        ui->verticalSliderMaxY->setValue(startVal);
    }
}

void SettingsForm::on_verticalSliderMinX_valueChanged(int value)
{
    int startVal = ui->verticalSliderSpeedX->value();

    if (startVal == 0) return;

    if (value > startVal) {
        ui->verticalSliderMinX->setValue(startVal);
    }
}

void SettingsForm::on_verticalSliderMinY_valueChanged(int value)
{
    int startVal = ui->verticalSliderSpeedY->value();

    if (startVal == 0) return;

    if (value > startVal) {
        ui->verticalSliderMinY->setValue(startVal);
    }
}

void SettingsForm::on_verticalSliderSpeedX_valueChanged(int value)
{
    if (value > ui->verticalSliderMaxX->value()) {
        ui->verticalSliderSpeedX->setValue(ui->verticalSliderMaxX->value());
    } else if (value < ui->verticalSliderMinX->value()) {
        ui->verticalSliderSpeedX->setValue(ui->verticalSliderMinX->value());
    }
}

void SettingsForm::on_verticalSliderSpeedY_valueChanged(int value)
{
    if (value > ui->verticalSliderMaxY->value()) {
        ui->verticalSliderSpeedY->setValue(ui->verticalSliderMaxY->value());
    } else if (value < ui->verticalSliderMinY->value()) {
        ui->verticalSliderSpeedY->setValue(ui->verticalSliderMinY->value());
    }
}
