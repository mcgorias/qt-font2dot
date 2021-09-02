#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QPainter>
#include <QFontDatabase>
#include <QImage>
#include <QColor>
#include <QChar>
#include <QCheckBox>
#include <QMessageBox>
#include <sstream>
#include <QDebug>
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //assert(_H<=8);

    ui->setupUi(this);
    connect(ui->_Save,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->_SaveAs,SIGNAL(clicked(bool)),this,SLOT(saveAs()));
    connect(ui->_Store,SIGNAL(clicked(bool)),this,SLOT(store()));
    connect(ui->_SelectCustomFont,SIGNAL(clicked(bool)),this,SLOT(selectCustomFontFile()));

    connect(ui->_SelectFont,SIGNAL(clicked()),this,SLOT(selectFont()));
    connect(ui->_TopLeftX,SIGNAL(valueChanged(int)),this,SLOT(pixelizeFont()));
    connect(ui->_TopLeftY,SIGNAL(valueChanged(int)),this,SLOT(pixelizeFont()));
    connect(ui->_PixelSize,SIGNAL(valueChanged(int)),this,SLOT(pixelizeFont()));
    connect(ui->_PixThreshold,SIGNAL(valueChanged(int)),this,SLOT(pixelizeFont()));
    connect(ui->_UseCustom,SIGNAL(clicked(bool)),this,SLOT(pixelizeFont()));
    connect(ui->_UsePixelized,SIGNAL(clicked(bool)),this,SLOT(pixelizeFont()));

    connect(ui->_NextChar,SIGNAL(clicked(bool)),this,SLOT(charInc()));
    connect(ui->_PreviousChar,SIGNAL(clicked(bool)),this,SLOT(charDec()));
    connect(ui->_Char,SIGNAL(valueChanged(int)),this,SLOT(pixelizeFont()));

    connect(ui->_PrePixelizedChar,SIGNAL(moveX(int)),this,SLOT(moveX(int)));
    connect(ui->_PrePixelizedChar,SIGNAL(moveY(int)),this,SLOT(moveY(int)));
    connect(ui->_PrePixelizedChar,SIGNAL(zoom(int)),this,SLOT(zoom(int)));

    connect(ui->_AutoRun,SIGNAL(clicked(bool)),this,SLOT(autoRun()));
    connect(&_IteratorTimer,SIGNAL(timeout()),this,SLOT(autoIterate()));
    connect(ui->_AutoCharFrom,SIGNAL(valueChanged(int)),this,SLOT(autoCharFromUpdated()));
    connect(ui->_AutoCharTo,SIGNAL(valueChanged(int)),this,SLOT(autoCharToUpdated()));

    connect(ui->_CharEdit,SIGNAL(textChanged(QString)),this,SLOT(search()));

    connect(ui->_MoveUp   ,SIGNAL(clicked(bool)),this,SLOT(moveUp()));
    connect(ui->_MoveDown ,SIGNAL(clicked(bool)),this,SLOT(moveDown()));
    connect(ui->_MoveLeft ,SIGNAL(clicked(bool)),this,SLOT(moveLeft()));
    connect(ui->_MoveRight,SIGNAL(clicked(bool)),this,SLOT(moveRight()));

    for(auto i=0;i<_W;i++)
        for(auto j=0;j<_H;j++)
        {
            auto cb = new QCheckBox(this);
            connect(cb,SIGNAL(clicked(bool)),this,SLOT(updatePreview()));
            ui->_CBLayout->addWidget(cb, _H-1-j,i);
        }

    ui->_UsePixelized->setChecked(true);
    pixelizeFont();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readCheckBoxes(char *data)
{
    for(auto i=0;i<_W;i++)
    {
        data[i]=0;
        for(auto j=0;j<_H;j++)
        {
            auto cb = ((QCheckBox*) ui->_CBLayout->itemAtPosition(j,i)->widget());
            auto on = cb->isChecked();
            if(on)
                data[i] |= (1<<((_H-1)-j)) ;
        }
    }
}

void MainWindow::writeCheckBoxes(char *data)
{
    for(auto i=0;i<_W;i++)
    {
        for(auto j=0;j<_H;j++)
        {
            auto cb = ((QCheckBox*) ui->_CBLayout->itemAtPosition(j,i)->widget());
            auto on = (data[i] & (1<<(_H-1-j)));
            cb->setChecked(on);
        }
    }
}

void MainWindow::selectFont()
{
    _FontPath = QFileDialog::getOpenFileName(this,
                                             tr("Select font"), "", tr("Font file (*.ttf)"));

    if(_FontPath.isEmpty())
        return;
    int id = QFontDatabase::addApplicationFont(_FontPath);
    _FontFamily = QFontDatabase::applicationFontFamilies(id).at(0);

    pixelizeFont();
}

void MainWindow::selectCustomFontFile()
{
    auto f = QFileDialog::getOpenFileName(this,
                                             tr("Select font"), "", tr("Custom font (*.hex)"));

    if(f.isEmpty())
       return;

    loadCustomFont(f);
}

void MainWindow::loadCustomFont(QString f)
{
    ui->_CustomFontFile->setText(f);
    _CustomFontPath=f;
    _CustomFont.setFileName(_CustomFontPath.toUtf8().data());
    auto loaded = _CustomFont.load();

    if(!loaded)
        QMessageBox::warning(this,tr("Error"),tr("Failed to load custom font file"));
    else
    {
        refreshDump();
        pixelizeFont();
    }
}

void MainWindow::save()
{
    if(_CustomFontPath.isEmpty())
    {
        saveAs();
        return;
    }
    _CustomFont.setFileName(_CustomFontPath.toUtf8().data());
    auto saved = _CustomFont.save();
    if(!saved)
        QMessageBox::warning(this,tr("Error"),tr("Failed to save custom font file"));
    loadCustomFont(_CustomFontPath);

}

void MainWindow::saveAs()
{
    _CustomFontPath = QFileDialog::getSaveFileName(this,"Save custom font","", tr("Custom font (*.hex)"));
    if(_CustomFontPath.isEmpty())
        return;
    save();

}

void MainWindow::store()
{
    auto c = ui->_Char->value();
    char data[_W]={0};

    readCheckBoxes(data);

    _CustomFont.set(c,data);
    refreshDump();
}

void MainWindow::refreshDump()
{
    std::ostringstream s;
    _CustomFont.dump(s);
    ui->_HexOutput->setText(QString::fromUtf8(s.str().c_str()));
}

void MainWindow::pixelizeFont()
{
    ComputeUTF8Char();

    auto current_char = ui->_Char->value();
    bool custom_available = _CustomFont.isSet(current_char);
    bool use_custom = false;
    ui->_UseCustom->setEnabled(custom_available);
    if(!custom_available)
        ui->_UsePixelized->setChecked(true);
    else
        use_custom = ui->_UseCustom->isChecked();

    auto step_x_size=100;
    auto step_y_size=100;
    QPixmap pix(_W*step_x_size,_H*step_y_size);
    pix.fill(Qt::white);
    QPainter painter( &pix );
    auto font = QFont(_FontFamily ) ;
    font.setPixelSize(ui->_PixelSize->value());
    font.setFixedPitch(true);
    painter.setFont( font );
    painter.setPen(Qt::black);
    painter.drawText( QPoint(ui->_TopLeftX->value(),ui->_TopLeftY->value()), ui->_CharEdit->text() );

    for(auto i=0;i<=_W;i++)
        painter.drawLine(i*step_x_size,0, i*step_x_size, _H*step_y_size  );
    for(auto j=0;j<=_H;j++)
        painter.drawLine(0,j*step_y_size, _W*step_x_size, j*step_y_size  );

    QImage im(pix.toImage());

    QString raw_hex;

    char data[_H]={0};

    if(use_custom)
    {
        _CustomFont.get(current_char,data);

    }
    else
    {
        for(auto i=0;i<_W;i++)
            for(auto j=0;j<_H;j++)
            {
                auto sum=0;
                for(auto x=0;x<step_x_size;x++)
                    for(auto y=0;y<step_y_size;y++)
                    {
                        auto p = QColor( im.pixel(i*step_x_size + x, j*step_y_size +y));
                        sum+=(p.red()+p.green()+p.blue())/3;
                    }
                sum /= step_x_size*step_y_size;

              if( sum<ui->_PixThreshold->value())
                    data[i] |= 1<<(_H-1-j) ;

            }
    }

    writeCheckBoxes(data);
    ui->_PrePixelizedChar->setPixmap(pix);

    updatePreview();
}

void MainWindow::ComputeUTF8Char()
{
    UTF8Utl::UTF8CHAR cnt=ui->_Char->value();

    _Str = QString::fromUtf8( UTF8Utl::Index2UTF8Char(cnt).c_str() );

    ui->_CharEdit->setText(_Str);
}

void MainWindow::charInc()
{
    ui->_Char->setValue(ui->_Char->value()+1);
}

void MainWindow::charDec()
{
    ui->_Char->setValue(ui->_Char->value()-1);
}

void MainWindow::moveX(int x)
{
    ui->_TopLeftX->setValue(ui->_TopLeftX->value()+x);
}

void MainWindow::moveY(int y)
{
    ui->_TopLeftY->setValue(ui->_TopLeftY->value()+y);
}

void MainWindow::zoom(int n)
{
    ui->_PixelSize->setValue( ui->_PixelSize->value() + n);
}

void MainWindow::moveUp()
{
    char data[_H] ={0};
    readCheckBoxes(data);
    for(size_t i=0; i<_H;i++)
    {
        data[i] = data[i]<<1;
    }
    ui->_UseCustom->setChecked(true);
    writeCheckBoxes(data);
}

void MainWindow::moveDown()
{
    char data[_H] ={0};
    readCheckBoxes(data);
    for(size_t i=0; i<_H;i++)
    {
        data[i] = data[i]>>1;
    }
    ui->_UseCustom->setChecked(true);
    writeCheckBoxes(data);
}

void MainWindow::moveLeft()
{
    char data[_H] ={0};
    readCheckBoxes(data);
    for(size_t i=0; i<_H-1;i++)
    {
        data[i] = data[i+1];
    }
    data[_H-1]=0;
    ui->_UseCustom->setChecked(true);
    writeCheckBoxes(data);
}

void MainWindow::moveRight()
{
    char data[_H] ={0};
    readCheckBoxes(data);
    for(size_t i=_H-1; i>0;i--)
    {
        data[i] = data[i-1];
    }
    data[0]=0;
    ui->_UseCustom->setChecked(true);
    writeCheckBoxes(data);
}

void MainWindow::updatePreview()
{
    auto step_x_size=100;
    auto step_y_size=100;
    QPixmap pix(_W*step_x_size,_H*step_y_size);

    pix.fill(Qt::black);
    QPainter painter(&pix);
    painter.setPen(Qt::red);

    for(auto i=0;i<_W;i++)
        for(auto j=0;j<_H;j++)
        {
            auto cb = ((QCheckBox*) ui->_CBLayout->itemAtPosition(j,i)->widget());
            if(cb)
            {
                auto on = cb->isChecked();

                if(on)
                {
                    painter.drawEllipse(    step_x_size*(i+0.5) , step_y_size*(j+0.5), step_x_size/2, step_x_size/2    );
                }
            }
        }
    ui->_PixelizedChar->setPixmap(pix);
}

void MainWindow::search()
{
    auto q = ui->_CharEdit->text();
    if(q.isEmpty())
        return;

    if(_Str != q)
    {
        auto c = q.toUtf8().data();
        ui->_Char->setValue(UTF8Utl::UTF8Char2Index(c));
        pixelizeFont();
    }
}

void MainWindow::autoCharFromUpdated()
{
    auto from = ui->_AutoCharFrom->value();
    auto to   = ui->_AutoCharTo->value();
    if(from>to)
        ui->_AutoCharFrom->setValue(to);

}

void MainWindow::autoCharToUpdated()

{
    auto from = ui->_AutoCharFrom->value();
    auto to   = ui->_AutoCharTo->value();
    if(to<from)
        ui->_AutoCharTo->setValue(from);

}

void MainWindow::autoRun()
{
    auto from = ui->_AutoCharFrom->value();
    if(_IteratorTimer.isActive())
    {
        _IteratorTimer.stop();
        ui->_AutoRun->setText("Start");
    }
    else
    {
        ui->_AutoRun->setText("Stop");
        ui->_Char->setValue(from);
        _IteratorTimer.start(5);
    }

    _AutoIt=0;
}

void MainWindow::autoIterate()
{
    auto from = ui->_AutoCharFrom->value();
    auto to   = ui->_AutoCharTo->value();

    if(ui->_Char->value()<to)
    {
        ui->_Char->setValue( from + _AutoIt);
        store();
        pixelizeFont();
        _AutoIt++;
    }
    else
       autoRun(); // to stop Timer
}

