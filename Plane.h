#ifndef PLANE_H
#define PLANE_H

#include "Bullet.h"
#include <QtWidgets>
#include <cstring>
#include <vector>
using namespace std;

class Plane: public QGraphicsPixmapItem
{
	friend class Control;
public:
	Plane();
    Plane(int x, int y, const string &imageFile, QGraphicsScene *scene, int life, enum WarPart part);
    void delScreen(QGraphicsScene *scene); //����Ļ�Ͻ��˷ɻ�Ĩȥ
    void synScreen(QGraphicsScene *scene); //����Ļ��Ӧλ����Ӵ˷ɻ�
    bool crash(QGraphicsScene *scene); //�ɻ�������ײ
	
protected:
	int life; //����ֵ
    enum WarPart part;
};

#endif // !PLANE_H
