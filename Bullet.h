#ifndef BULLET_H
#define BULLET_H

#include <QtWidgets>

#include <utility>
#include <fstream>
#include <iostream>
using namespace std;

enum WarPart { ME, ENEMY };
enum BulletState { READY, RUNNING }; //�ӵ�״̬��׼������or�Ѿ�����
enum BulletDirection { UP, UPLEFT, UPRIGHT, DOWN, DOWNLEFT, DOWNRIGHT }; //�ӵ�����

class Bullet: public QGraphicsPixmapItem
{
	friend class Control;
public:
	Bullet();
    Bullet(WarPart part, int x, int y, const string &imageFile, BulletDirection dir, int power, int speed);
    void synScreen(QGraphicsScene *scene);
    void delScreen(QGraphicsScene *scene);
    pair<qreal, qreal> updatePosition();
    bool hit(QGraphicsScene *scene);

private: //TODO�������ӵ���˵��(x,y)ӦΪ�ӵ���
    WarPart part;
	BulletDirection dir;
	int power; //�ӵ�ɱ����
	BulletState state;
	int speed; //�ӵ��ٶ�
	//���ڿ�����Ӳ�ͬ��״����ͬɱ�������ӵ�
};

#endif // !BULLET_H

