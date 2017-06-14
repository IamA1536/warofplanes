#include "Control.h"

Control::Control()
{
}

Control::Control(int boardWidth, int boardHeight, int enemyNumber,
    const string &myPlaneImageFile, int myLife,
                 const string &myBulletImageFile, int myBulletPower, int myBulletSpeed,
    const string &enemyPlaneImageFile, int enemyLife,
                 const string &enemyBulletImageFile, int enemyBulletSpeed, int enemyBulletPower)
{
    this->myPlaneImageFile = myPlaneImageFile;
	this->myLife = myLife;

    this->myBulletImageFile = myBulletImageFile;
    this->myBulletImageScaleHeight = myBulletImageScaleHeight;
	this->myBulletPower = myBulletPower;
	this->myBulletSpeed = myBulletSpeed;

    this->enemyPlaneImageFile = enemyPlaneImageFile;
	this->enemyLife = enemyLife;

    this->enemyBulletImageFile = enemyBulletImageFile;
    this->enemyBulletImageScaleHeight = enemyBulletImageScaleHeight;
	this->enemyBulletPower = enemyBulletPower;
	this->enemyBulletSpeed = enemyBulletSpeed;

    /* ���ø���������ʱ�� */
    myBulletShootTimerId = startTimer(500); //300
    enemyBulletShootTimerId = startTimer(1000);
    allBulletMoveTimerId = startTimer(10);
    enemyPlaneMoveTimerId = startTimer(1000);

    /* ��ʼ������ */
    setSceneRect(0,0,boardWidth,boardHeight);
	
    /* �����ҷɻ� */
    myplane = new MyPlane(boardWidth / 2, boardHeight / 2, myPlaneImageFile, this, myLife);
    myplane->synScreen(this);

	/* ��ӵл� */
	for (int i = 0; i < enemyNumber; i++)
		generateEnemyPlane();
}

void Control::timerEvent(QTimerEvent *event)
{
    //TODO: ����ҷɻ�׹�٣���֪����ν�����Ϸ
    if(event->timerId()==enemyBulletShootTimerId)
        shootEnemyBullets();
    else if(event->timerId()==myBulletShootTimerId)
        shootBullet();
    else if(event->timerId()==allBulletMoveTimerId)
    {
        updateMyBullets();
        updateEnemyBullets();
    }
    else if(event->timerId()==enemyPlaneMoveTimerId)
        updateEnemyPlanes();
}

void Control::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_W)
        myplane->moveBy(0, -10);
    else if(event->key()==Qt::Key_S)
        myplane->moveBy(0, 10);
    else if(event->key()==Qt::Key_A)
        myplane->moveBy(-10, 0);
    else if(event->key()==Qt::Key_D)
        myplane->moveBy(10, 0);
    myplane->update();
}

bool Control::generateEnemyPlane()
{
	/* ����ڵ�һ�����ɵл� */
    int x = rand() % (int)width(); //�л������λ��
    while(!items(QPointF(x,0), Qt::IntersectsItemBoundingRect).empty()) //�����ཻģʽ
    {
        srand(time(NULL));//��ʼ��ʱ������
        x = rand() % (int)width();
    }

    /* �����л� */
    qDebug() << "gen: " << x;
    EnemyPlane *enemy = new EnemyPlane(x, 0, enemyPlaneImageFile, this, enemyLife);
    enemyplanes.push_back(enemy);
    return true;
}

bool Control::changePlanePosition(Plane *plane, int newX, int newY)
{
    /* ���λ���Ƿ��б仯���ޱ仯��ֱ�ӷ��� */
    if (plane->x() == newX && plane->y() == newY)
		return true;

    /* �����λ���Ƿ�Ϸ������Ϸ���ֱ�ӷ��� */
    if (newX<0 || newX>width() || newY<0 || newY>height())
	{
        if (plane->part == ME) //��ҷɻ����������
			return true;
        else if (plane->part == ENEMY) //�л���������Ͻ硢��硢�ҽ磬�����½�֮�󽫱�ɾ��
		{
            if (newY>height())
			{
                plane->delScreen(this);
				return false;
			}
			else
				return true;
		}
	}

	/* �����λ���Ƿ���ĳһ�ɻ�λ�ó�ͻ */
	/* ���ȼ���Ƿ���л���ײ */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); ) //�����л�
	{
		if (plane == *it) //�����Լ�
		{
			it++;
			continue;
		}

		bool alive = true; //it��ָ��ķɻ��Ƿ�������ֵ
        if (plane->collidesWithItem(*it))
		{
			/* ��Ϊ��ҷɻ������ܷɻ���crash������ֵ����1 */
            if (plane->part == ME)
			{
                plane->crash(this);
                alive = (*it)->crash(this);
			}
            if (plane->part == ENEMY) //��ͬΪ�л���������ı�λ�ã�NOCHANGE
				return true;
		}

		if (alive)
			it++;
		else //���ɻ�׹�٣��򽫴˷ɻ�ȥ��
		{
			delete *it;
			it = enemyplanes.erase(it);
		}
	}

    /* ���˷ɻ�Ϊ�л���������ҷɻ���������ײ */
    if (plane->part == ENEMY && plane->collidesWithItem(myplane))
    {
        myplane->crash(this);
        plane->crash(this);
    }

	/* ��plane����������겢ͬ����Ļ */
	if (plane->life > 0)
	{
        plane->moveBy(newX-plane->x(), newY-plane->y());
        plane->update();

		/* �˷ɻ��ڹ��е��ӵ�λ����ɻ����� */
		for(vector<Bullet*>::iterator it=plane->bullets.begin();it!=plane->bullets.end();it++)
			if ((*it)->state == READY)
                (*it)->moveBy(newX-plane->x(), newY-plane->y());
	}

	return plane->life > 0;
}

bool Control::updateEnemyPlanes()
{
	/* ����ǰ�л�����3�����Զ����ɵл�����Ŀ�����С��8 */
/*	if (enemyplanes.size() < 3)
	{
		int genNum = rand() % 8;
		for (int i = 0; i < genNum; i++)
			generateEnemyPlane();
    }*/

	shootEnemyBullets(); //���ел������ӵ�

	/* ���ел��ƶ�λ�� */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
	{
        qDebug() << it-enemyplanes.begin() << "  (" << (*it)->x() << "," << (*it)->y() << ")";
		pair<int, int> loc = (*it)->updatePosition();
		if (changePlanePosition(*it, loc.first, loc.second))
			it++;
		else
			it = enemyplanes.erase(it);
	}

    return myplane->life > 0;
}

bool Control::changeBulletPosition(Bullet * bullet, int newX, int newY)
{
	/* ���λ���Ƿ��б仯���ޱ仯�򷵻�true */
    if (bullet->x() == newX && bullet->y() == newY)
		return true;

	/* ����ӵ��Ƿ����ĳһ�ɻ� */
	/* ���ȼ����ҷɻ� */
    if (bullet->part==ENEMY && bullet->collidesWithItem(myplane))
	{
        qDebug() << "bullet hit player begin " << bullet->part;
        bullet->hit(this);
        myplane->crash(this);
        qDebug() << "bullet hit player end " << bullet->part;
	}
    else if(bullet->part==ME)
    {
        /* Ȼ����л�����ʹ�л��Ѿ�û������ֵ��Ҳ����enemyplanes��ɾȥ����Ϊ�˷ɻ����ܻ����ӵ����˶� */
        for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); it++)
            if (bullet->collidesWithItem(*it))
            {
                qDebug() << "collid del bullet begin" << bullet->part;
                bullet->hit(this);
                (*it)->crash(this);
                qDebug() << "collid del bullet end" << bullet->part;
            }
    }

	/* ���ӵ�������ɱ���������λ�ò�ͬ����Ļ */
	if (bullet->power > 0)
	{
		/* ����ʱ�ӵ���ͼ����߽磬�������ӵ� */
        if (newX <= 0 || newX >= width() || newY <= 0 || newY >= height())
		{
            qDebug() << "illegal del bullet begin" << bullet->part;
            bullet->delScreen(this);
            qDebug() << "illegal del bullet end" << bullet->part;
            return false;
		}
		if (bullet->state == READY) //���ӵ������ڹ��У������״̬����ɾ��ԭ��λ��
			bullet->state = RUNNING;
        bullet->synScreen(this);
        bullet->moveBy(newX-bullet->x(), newY-bullet->y());
        bullet->update();
	}

	return bullet->power>0;
}

void Control::updateEnemyBullets()
{
	for (vector<EnemyPlane*>::iterator iter = enemyplanes.begin(); iter != enemyplanes.end(); )
	{
		for (vector<Bullet*>::iterator it = (*iter)->bullets.begin(); it != (*iter)->bullets.end(); )
		{
			pair<int, int> loc = (*it)->updatePosition();
			if (changeBulletPosition(*it, loc.first, loc.second)) //���ӵ��ѻ٣������ٲ��Ƴ�
				it++;
			else
			{
				delete *it;
				it = (*iter)->bullets.erase(it);
			}
		}

		if ((*iter)->life <= 0 && (*iter)->bullets.empty()) //���л��ѻ���û���ӵ�����ɾȥ�˵л�
		{
			delete *iter;
			iter = enemyplanes.erase(iter);
		}
		else
			iter++;
	}
}

void Control::shootEnemyBullets()
{
	/* �Դ��ĵл��������ӵ������ӵ��ڵл��ڹܼ�˵�λ�� */
	for (vector<EnemyPlane*>::iterator iter = enemyplanes.begin(); iter != enemyplanes.end(); iter++)
		if ((*iter)->life > 0)
			(*iter)->bullets.push_back(
                new Bullet(ENEMY, (*iter)->x()+(*iter)->pixmap().width()/2, (*iter)->y()+(*iter)->pixmap().height()-1,
                           enemyBulletImageFile, DOWN, enemyBulletPower, enemyBulletSpeed));
	updateEnemyBullets();
}

void Control::updateMyBullets()
{
    for (vector<Bullet*>::iterator it = myplane->bullets.begin(); it != myplane->bullets.end(); )
	{
		pair<int, int> loc = (*it)->updatePosition();
		if (changeBulletPosition(*it, loc.first, loc.second))
			it++;
		else
		{
			delete *it;
            it = myplane->bullets.erase(it);
		}
	}
}

//��ǣ����ﻹ��Ҫ�������ӵ��Զ�������߼�
void Control::shootBullet()
{
	/* ��ҷɻ��������ӵ������ӵ�����ҷɻ��ڹܼ�˵�λ�� */
    myplane->bullets.push_back(new Bullet(ME, myplane->x()+40, myplane->y() + 2,
                                          myBulletImageFile, UP, myBulletPower, myBulletSpeed));

	/* ������ҷɻ��ӵ�λ�� */
	updateMyBullets();
}
