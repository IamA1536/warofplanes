#ifndef BULLET_H
#define BULLET_H

#include <utility>
#include <fstream>
#include <iostream>
using namespace std;

enum BulletState { READY, RUNNING }; //�ӵ�״̬��׼������or�Ѿ�����
enum BulletDirection { UP, UPLEFT, UPRIGHT, DOWN, DOWNLEFT, DOWNRIGHT }; //�ӵ�����

class Bullet
{
	friend class Control;
public:
	Bullet();
	Bullet(int x, int y);
	Bullet(int x, int y, BulletDirection dir, int power, int speed);
	void delScreen(char **screen);
	void synScreen(char **screen);
	pair<int, int> updatePosition();
	bool hit(char **screen);

private: //TODO�������ӵ���˵��(x,y)ӦΪ�ӵ���
	int x;
	int y;
	BulletDirection dir;
	int power; //�ӵ�ɱ����
	BulletState state;
	int speed; //�ӵ��ٶ�
	//���ڿ�����Ӳ�ͬ��״����ͬɱ�������ӵ�
};

#endif // !BULLET_H

