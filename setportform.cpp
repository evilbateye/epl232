#include "setportform.hpp"
#include "ui_setportform.h"

SetPortForm::SetPortForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetPortForm)
{
    ui->setupUi(this);

#if defined(Q_OS_UNIX)
    ui->portName->setText("/dev/pts/3");
#endif

#if defined(Q_OS_WIN)
    ui->portName->setText("COM1");
#endif

    ui->baudRateComboBox->addItem("110", BAUD110);
    ui->baudRateComboBox->addItem("300", BAUD300);
    ui->baudRateComboBox->addItem("600", BAUD600);
    ui->baudRateComboBox->addItem("1200", BAUD1200);
    ui->baudRateComboBox->addItem("2400", BAUD2400);
    ui->baudRateComboBox->addItem("4800", BAUD4800);
    ui->baudRateComboBox->addItem("9600", BAUD9600);
    ui->baudRateComboBox->addItem("19200", BAUD19200);
    ui->baudRateComboBox->addItem("38400", BAUD38400);
    ui->baudRateComboBox->addItem("57600", BAUD57600);
    ui->baudRateComboBox->addItem("115200", BAUD115200);
#if defined(Q_OS_UNIX) || defined(qdoc)
    ui->baudRateComboBox->addItem("50", BAUD50);
    ui->baudRateComboBox->addItem("75", BAUD75);
    ui->baudRateComboBox->addItem("134", BAUD134);
    ui->baudRateComboBox->addItem("150", BAUD150);
    ui->baudRateComboBox->addItem("200", BAUD200);
    ui->baudRateComboBox->addItem("1800", BAUD1800);
#if defined(B76800) || defined(qdoc)
    ui->baudRateComboBox->addItem("76800", BAUD76800);
#endif
#if (defined(B230400) && defined(B4000000)) || defined(qdoc)
    ui->baudRateComboBox->addItem("230400", BAUD230400);
    ui->baudRateComboBox->addItem("460800", BAUD460800);
    ui->baudRateComboBox->addItem("500000", BAUD500000);
    ui->baudRateComboBox->addItem("576000", BAUD576000);
    ui->baudRateComboBox->addItem("921600", BAUD921600);
    ui->baudRateComboBox->addItem("1000000", BAUD1000000);
    ui->baudRateComboBox->addItem("1152000", BAUD1152000);
    ui->baudRateComboBox->addItem("1500000", BAUD1500000);
    ui->baudRateComboBox->addItem("2000000", BAUD2000000);
    ui->baudRateComboBox->addItem("2500000", BAUD2500000);
    ui->baudRateComboBox->addItem("3000000", BAUD3000000);
    ui->baudRateComboBox->addItem("3500000", BAUD3500000);
    ui->baudRateComboBox->addItem("4000000", BAUD4000000);
#endif
#endif //Q_OS_UNIX
#if defined(Q_OS_WIN) || defined(qdoc)
    ui->baudRateComboBox->addItem("14400", BAUD14400);
    ui->baudRateComboBox->addItem("56000", BAUD56000);
    ui->baudRateComboBox->addItem("128000", BAUD128000);
    ui->baudRateComboBox->addItem("256000", BAUD256000);
#endif  //Q_OS_WIN

    ui->baudRateComboBox->setCurrentIndex(6); //9600

    ui->stopBitsComboBox->addItem("stop 1", STOP_1);
    ui->stopBitsComboBox->addItem("stop 2", STOP_2);

    ui->parityComboBox->addItem("none", PAR_NONE);
    ui->parityComboBox->addItem("odd", PAR_ODD);
    ui->parityComboBox->addItem("even", PAR_EVEN);

    ui->flowControlComboBox->addItem("off", FLOW_OFF);
    ui->flowControlComboBox->addItem("hardware", FLOW_HARDWARE);
    ui->flowControlComboBox->addItem("xonxoff", FLOW_XONXOFF);
}

QString SetPortForm::getPortName()
{
    return ui->portName->text();
}

BaudRateType SetPortForm::getBaudRate()
{
    return (BaudRateType) ui->baudRateComboBox->itemData(ui->baudRateComboBox->currentIndex()).toInt();;
}

StopBitsType SetPortForm::getStopBits()
{
    return (StopBitsType) ui->stopBitsComboBox->itemData(ui->stopBitsComboBox->currentIndex()).toInt();
}

ParityType SetPortForm::getParity()
{
    return (ParityType) ui->parityComboBox->itemData(ui->parityComboBox->currentIndex()).toInt();
}

FlowType SetPortForm::getFlow()
{
    return (FlowType) ui->flowControlComboBox->itemData(ui->flowControlComboBox->currentIndex()).toInt();
}

SetPortForm::~SetPortForm()
{

    delete ui;
}
