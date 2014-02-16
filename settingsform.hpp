#ifndef SETTINGSFORM_HPP
#define SETTINGSFORM_HPP

#include <QDialog>

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsForm(int stirr, int stirrA,
                          int sX, int sY,
                          int maxX, int maxY,
                          int minX, int minY,
                          int f, int ap,
                          int b, int p,
                          int ph, int pi, QWidget *parent = 0);
    int getSpeedX();
    int getSpeedY();
    int getMaxSpeedX();
    int getMaxSpeedY();
    int getMinSpeedX();
    int getMinSpeedY();
    int getStirrInc();
    int getStirrAngle();
    int getFrequency();
    int getActionPeriod();
    int getNumbOfBalls();
    int getNumbOfPoints();
    int getPaddleH();
    int getPaddleInc();
    ~SettingsForm();
    
private slots:
    void on_verticalSliderMaxX_valueChanged(int value);

    void on_verticalSliderMaxY_valueChanged(int value);

    void on_verticalSliderMinX_valueChanged(int value);

    void on_verticalSliderMinY_valueChanged(int value);

    void on_verticalSliderSpeedX_valueChanged(int value);

    void on_verticalSliderSpeedY_valueChanged(int value);

private:
    Ui::SettingsForm *ui;
};

#endif // SETTINGSFORM_HPP
