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

class Control: public QGraphicsScene
{
    Q_OBJECT
public:
	Control();
    Control(int boardWidth, int boardHeight, int enemyNumber,
        const string &myPlaneImageFile, int myLife,
                     const string &myBulletImageFile, int myBulletPower, int myBulletSpeed,
        const string &enemyPlaneImageFile, int enemyLife,
                     const string &enemyBulletImageFile, int enemyBulletSpeed, int enemyBulletPower);

protected:
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    string myPlaneImageFile;
	int myLife;

    string myBulletImageFile;
    int myBulletImageScaleHeight;
	int myBulletPower;
	int myBulletSpeed;

    string enemyPlaneImageFile;
	int enemyLife;

    string enemyBulletImageFile;
    int enemyBulletImageScaleHeight;
	int enemyBulletSpeed;
	int enemyBulletPower;

    int myBulletShootTimerId;
    int enemyBulletShootTimerId;
    int allBulletMoveTimerId;
    int enemyPlaneMoveTimerId;

    MyPlane *myplane;
	vector<EnemyPlane *> enemyplanes;

	bool generateEnemyPlane(); //����һ�ܵл�
	
	bool changePlanePosition(Plane *plane, int newX, int newY);
	bool updateEnemyPlanes(); //���ݵл�������µл�λ��
	
	bool changeBulletPosition(Bullet *bullet, int newX, int newY); //�ı��ӵ�λ�ã������ӵ��Ƿ���ս��
	void shootEnemyBullets(); //���ел������ӵ�
	void updateMyBullets(); //������������ӵ�λ��
	void updateEnemyBullets(); //���µл������ӵ�λ��
	void shootBullet(); //��ҷɻ������ӵ�
	
};

#endif // !CONTROL_H
