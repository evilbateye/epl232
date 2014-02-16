#ifndef SETPORTFORM_HPP
#define SETPORTFORM_HPP

#include <QDialog>
#include "qextserialport/qextserialport.h"
#include "qextserialport/qextserialenumerator.h"

namespace Ui {
class SetPortForm;
}

class SetPortForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetPortForm(QWidget *parent = 0);
    QString getPortName();
    BaudRateType getBaudRate();
    StopBitsType getStopBits();
    ParityType getParity();
    FlowType getFlow();
    ~SetPortForm();
    
private:
    Ui::SetPortForm *ui;
};

#endif // SETPORTFORM_HPP
