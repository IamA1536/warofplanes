#include "Plane.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

Plane::Plane()
{
}

Plane::Plane(int x, int y, const string &imageFile, QGraphicsScene *scene, int life, enum WarPart part):
    QGraphicsPixmapItem(QPixmap(QString::fromStdString(imageFile)))
{
    setPos(x, y);
    this->life = life;
    this->part = part;
    scene->addItem(this);
    update();
}



void Plane::delScreen(QGraphicsScene *scene)
{
    qDebug() << "del plane begin";
    scene->removeItem(this);
    update();
    qDebug() << "del plane end";
}

void Plane::synScreen(QGraphicsScene *scene)
{
    if(!scene->items().contains(this))
    {
        scene->addItem(this);
        update();
    }
}

bool Plane::crash(QGraphicsScene *scene)
{
	if (life <= 0) //���Ѿ��ľ�����ֵ
		return false;
	else if (--life <= 0) //������ֵ�˴�crash��ľ�
	{
        delScreen(scene);
		return false;
	}
	else
		return true;
}
