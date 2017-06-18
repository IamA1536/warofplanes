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

#include <QUrl>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "Plane.h"
#include "MyPlane.h"
#include "EnemyPlane.h"
using namespace std;

class Control: public QGraphicsScene
{
    Q_OBJECT
public:
	Control();
    Control(int boardWidth, int boardHeight,
        const string &myPlaneImageFile, int myLife, int mySkill,
                     const string &myBulletImageFile, int myBulletPower,
        const string &enemyPlaneImageFile, int enemyLife,
                     const string &enemyBulletImageFile, int enemyBulletPower);

protected:
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    string myPlaneImageFile;
	int myLife;
    int mySkill;

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
    int enemyPlaneGenerateTimerId;

    MyPlane *myplane;
    vector<Bullet*> mybullets;
    vector<EnemyPlane *> enemyplanes;
    vector<Bullet*> enemybullets;

    QGraphicsRectItem *lifeFrameBar;
    QGraphicsRectItem *lifeBar;
    QGraphicsRectItem *skillFrameBar;
    QGraphicsRectItem *skillBar;

    QMediaPlayer *player;
    QMediaPlaylist *playList;

	bool generateEnemyPlane(); //����һ�ܵл�
	
	bool changePlanePosition(Plane *plane, int newX, int newY);
	bool updateEnemyPlanes(); //���ݵл�������µл�λ��
	
	bool changeBulletPosition(Bullet *bullet, int newX, int newY); //�ı��ӵ�λ�ã������ӵ��Ƿ���ս��
	void shootEnemyBullets(); //���ел������ӵ�
	void updateMyBullets(); //������������ӵ�λ��
	void updateEnemyBullets(); //���µл������ӵ�λ��
	void shootBullet(); //��ҷɻ������ӵ�

    void updateBar(QGraphicsRectItem *bar, QGraphicsRectItem *frameBar, const QPointF &pos, qreal var, const QBrush &brush); //����Ѫ��������

    bool isPause; //��Ϸ�Ƿ���ͣ
    QGraphicsWidget *maskWidget; //�������
    QGraphicsTextItem *gamePausedText; //��Ϸ��ͣ��ʾ�ı�
    void pauseGame(); //��ͣ��Ϸ

    int myBulletType; //�Ƿ�ʹ�ü���
    int skillQTimerId; //����Qʹ��ʱ��
    int score; //����ķɻ���
    QGraphicsTextItem *scoreText; //��ʾ��ǰ����ķɻ���

    QGraphicsTextItem *gameLostText; //���������ʾ�ı�
    void loseGame(); //�������ֵ�þ�

    QGraphicsTextItem *gameHelpText; //��Ϸ��ʼ����
    void welcomeGame(); //��ʼ����

    bool hasStarted; //�Ƿ��Ѿ���ʼ��Ϸ
    void startGame(); //��ʼ��Ϸ
	
};

#endif // !CONTROL_H
