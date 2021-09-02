#ifndef FONTEDITLABEL_H
#define FONTEDITLABEL_H

#include <QLabel>
#include <QPoint>

class FontEditLabel : public QLabel
{
    Q_OBJECT
    using QLabel::QLabel;
public:

signals:

    void moveX(int);
    void moveY(int);

    void zoom (int);

protected:
    void mouseMoveEvent(QMouseEvent * event) override ;
    void mousePressEvent(QMouseEvent * event) override ;
    void mouseReleaseEvent(QMouseEvent * event) override ;
    void wheelEvent(QWheelEvent * event) override;


    bool _Released=true;

    QPoint _InitialClick;
public slots:
};

#endif // FONTEDITLABEL_H
