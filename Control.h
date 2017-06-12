#ifndef CONTROL_H
#define CONTROL_H

#include <ctime>
#include <vector>
#include <cassert>
#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include "Plane.h"
#include "MyPlane.h"
#include "EnemyPlane.h"
using namespace std;

/* changePosition�����ķ���ֵ��δִ�У�������������ײδ�ľ�����ֵ����ײ�Һľ�����ֵ */
enum ChangePosFlag { NOCHANGE, CHANGED, CRASH };

class Control
{
	
public:
	Control();
	Control(int boardWidth, int boardHeight, int boardEntension, int enemyNumber,
		int myLife, int myBulletPower, int myBulletSpeed, 
		int enemyLife, int enemyBulletSpeed, int enemyBulletPower);

private:
	int boardWidth;
	int boardHeight;
	int screenWidth;
	int screenHeight;
	int myLife;
	int myBulletPower;
	int myBulletSpeed;
	int enemyLife;
	int enemyBulletSpeed;
	int enemyBulletPower;
	char **screen;
	MyPlane myplane;
	vector<EnemyPlane *> enemyplanes;

	void refreshScreen(); //ˢ����Ļ
	bool generateEnemyPlane(); //����һ�ܵл�
	
	int judgeCrash(int aX, int aY, int aHeight, int aWidth, char *aShape,
		int bX, int bY, int bHeight, int bWidth, char *bShape); //�ж����ɻ��Ƿ���ײ��������ײ��ĸ���
	bool changePlanePosition(Plane *plane, int newX, int newY);
	bool updateEnemyPlanes(); //���ݵл�������µл�λ��
	
	/* �ж��ӵ��Ƿ���зɻ� */
	bool judgeHit(Bullet * bullet, int bulletNewX, int bulletNewY,
		int planeX, int planeY, int planeHeight, int planeWidth, char *planeShape); 
	bool changeBulletPosition(Bullet *bullet, int newX, int newY); //�ı��ӵ�λ�ã������ӵ��Ƿ���ս��
	bool updateBullets(); //���������ӵ�λ��
	void shootEnemyBullets(); //���ел������ӵ�
	void updateMyBullets(); //������������ӵ�λ��
	void updateEnemyBullets(); //���µл������ӵ�λ��
	void shootBullet(); //��ҷɻ������ӵ�
	bool keyboardHandle(char cmd);
	void run();
	
};

#endif // !CONTROL_H
