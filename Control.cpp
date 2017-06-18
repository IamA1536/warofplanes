#include "Control.h"

const QPointF LifeBarPos = QPointF(650,550);
const QPointF SkillBarPos = QPointF(650, 570);

const int myBulletShootTimerItv = 300;
const int enemyBulletShootTimerItv = 1000;
const int allBulletMoveTimerItv = 10;
const int enemyPlaneMoveTimerItv = 50;
const int enemyPlaneGenerateTimerItv = 3000;

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
	this->enemyBulletPower = enemyBulletPower;
	this->enemyBulletSpeed = enemyBulletSpeed;

    /* ����ֵ */
    lifeFrameBar = new QGraphicsRectItem(LifeBarPos.x(), LifeBarPos.y(),100,5);
    lifeFrameBar->setPen(QPen(Qt::white));
    addItem(lifeFrameBar);
    lifeBar = new QGraphicsRectItem(LifeBarPos.x(), LifeBarPos.y(), 100, 5);
    lifeBar->setBrush(QBrush(Qt::green));
    addItem(lifeBar);

    /* ����ֵ */
    skillFrameBar = new QGraphicsRectItem(SkillBarPos.x(),SkillBarPos.y(),100,5);
    skillFrameBar->setPen(QPen(Qt::white));
    addItem(skillFrameBar);
    skillBar = new QGraphicsRectItem(SkillBarPos.x(), SkillBarPos.y(), 0, 5);
    skillBar->setBrush(QBrush(Qt::blue));
    addItem(skillBar);

    /* ���ø���������ʱ�� */
    myBulletShootTimerId = startTimer(myBulletShootTimerItv);
    enemyBulletShootTimerId = startTimer(enemyBulletShootTimerItv);
    allBulletMoveTimerId = startTimer(allBulletMoveTimerItv);
    enemyPlaneMoveTimerId = startTimer(enemyPlaneMoveTimerItv);
    enemyPlaneGenerateTimerId = startTimer(enemyPlaneGenerateTimerItv);

    /* ��ʼ�����������ű������� */
    setSceneRect(0,0,boardWidth,boardHeight);
    playList = new QMediaPlaylist;
    playList->addMedia(QUrl::fromLocalFile("music/starwars.mp3"));
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); //����ѭ��
    player = new QMediaPlayer(this);
    player->setPlaylist(playList);
    player->play();
	
    /* �����ҷɻ� */
    myplane = new MyPlane(boardWidth / 2, boardHeight / 2, myPlaneImageFile, this, myLife);
    myplane->synScreen(this);

	/* ��ӵл� */
	for (int i = 0; i < enemyNumber; i++)
		generateEnemyPlane();

    /* ������� */
    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 80)));
    mask->resize(width(),height());
    maskWidget = addWidget(mask);
    maskWidget->setPos(0,0);
    maskWidget->setZValue(1); //���ô���zֵΪ0��ͼ�����Ϸ�
    maskWidget->hide();

    /* ��Ϸ��ͣ��ʾ */
    isPause = false;
    gamePausedText = new QGraphicsTextItem;
    addItem(gamePausedText);
    gamePausedText->setHtml(tr("<font color=white>GAME PAUSED</font>"));
    gamePausedText->setFont(QFont("Times", 30, QFont::Bold));
    gamePausedText->setPos(250, 250);
    gamePausedText->setZValue(2);
    gamePausedText->hide();
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
    else if(event->timerId()==enemyPlaneGenerateTimerId)
    {
        for(int i=0;i<2;i++)
            generateEnemyPlane();
    }
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
    else if(event->key()==Qt::Key_Q)
    {
        //��Q�ļ��ܿ�������ɨ���������ӵ������ǻ����ļ���ֵ
    }
    else if(event->key()==Qt::Key_E)
    {
        //��E�ļ��ܿ��Դ�����зɻ������ļ���ֵ
    }
    else if(event->key()==Qt::Key_Space)
        pauseGame();
    myplane->update();
}

bool Control::generateEnemyPlane()
{
	/* ����ڵ�һ�����ɵл� */
    srand(time(NULL));//��ʼ��ʱ�����ӣ�֮ǰ�������ѭ���ڻᵼ�¿���
    int x = rand() % (int)width(); //�л������λ��
    QPixmap pixmap(QPixmap(QString::fromStdString(enemyPlaneImageFile)));
    while(!items(x, 0, pixmap.width(), pixmap.height(), Qt::IntersectsItemBoundingRect, Qt::DescendingOrder).empty()) //�����ཻģʽ
        x = rand() % (int)width();

    /* �����л� */
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
	}

	return plane->life > 0;
}

bool Control::updateEnemyPlanes()
{
    /* ����ǰ�л�����1�����Զ����ɵл�����Ŀ�����С��5 */
    if (enemyplanes.size() < 1)
	{
        int genNum = rand() % 5;
		for (int i = 0; i < genNum; i++)
			generateEnemyPlane();
    }

	/* ���ел��ƶ�λ�� */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
	{
        //qDebug() << it-enemyplanes.begin() << "  (" << (*it)->x() << "," << (*it)->y() << ")";
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
        //qDebug() << "myplane: " << myplane->life;
        bullet->hit(this);
        myplane->crash(this);
        lifeBar->setRect(LifeBarPos.x(), LifeBarPos.y(), lifeBar->rect().width()-2, lifeBar->rect().height());
        lifeBar->setBrush(Qt::green);
        lifeBar->update();
	}
    else if(bullet->part==ME)
    {
        /* Ȼ����л������л��Ѿ�û������ֵ���ʹ�enemyplanes��ɾȥ */
        for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
        {
            if (bullet->collidesWithItem(*it))
            {
                bullet->hit(this);
                (*it)->crash(this);
                delete *it;
                it = enemyplanes.erase(it);
                skillBar->setRect(SkillBarPos.x(), SkillBarPos.y(),
                                  min(skillBar->rect().width()+2, skillFrameBar->rect().width()), lifeBar->rect().height());
                skillBar->setBrush(Qt::blue);
                skillBar->update();
            }
            else
                it++;
        }
    }

	/* ���ӵ�������ɱ���������λ�ò�ͬ����Ļ */
	if (bullet->power > 0)
	{
		/* ����ʱ�ӵ���ͼ����߽磬�������ӵ� */
        if (newX <= 0 || newX >= width() || newY <= 0 || newY >= height())
		{
            bullet->delScreen(this);
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
    for(vector<Bullet*>::iterator it = enemybullets.begin(); it!=enemybullets.end(); )
    {
        pair<int ,int> loc = (*it)->updatePosition();
        if(changeBulletPosition(*it, loc.first, loc.second))
            it++;
        else
        {
            delete *it;
            it = enemybullets.erase(it);
        }
    }
}

void Control::shootEnemyBullets()
{
    /* �Դ��ĵл��������ӵ������ӵ��ڵл��ڹ����λ�� */
    for (vector<EnemyPlane*>::iterator iter = enemyplanes.begin(); iter != enemyplanes.end(); iter++)
		if ((*iter)->life > 0)
        {
            Bullet *bullet = new Bullet(ENEMY, (*iter)->x()+(*iter)->pixmap().width()/2, (*iter)->y()+(*iter)->pixmap().height()-15,
                                        enemyBulletImageFile, DOWN, enemyBulletPower, enemyBulletSpeed);
            enemybullets.push_back(bullet);
            addItem(bullet);
        }
    //updateEnemyBullets();
}

void Control::updateMyBullets()
{
    for (vector<Bullet*>::iterator it = mybullets.begin(); it != mybullets.end(); )
	{
		pair<int, int> loc = (*it)->updatePosition();
		if (changeBulletPosition(*it, loc.first, loc.second))
			it++;
		else
		{
			delete *it;
            it = mybullets.erase(it);
		}
	}
}

//��ǣ����ﻹ��Ҫ�������ӵ��Զ�������߼�
void Control::shootBullet()
{
    /* ��ҷɻ��������ӵ������ӵ�����ҷɻ��ڹ����λ�� */
    Bullet *bullet = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                myBulletImageFile, UP, myBulletPower, myBulletSpeed);
    mybullets.push_back(bullet);
    addItem(bullet);

	/* ������ҷɻ��ӵ�λ�� */
    //updateMyBullets();
}

void Control::pauseGame()
{
    if(!isPause)
    {
        isPause = true;
        killTimer(myBulletShootTimerId);
        killTimer(enemyBulletShootTimerId);
        killTimer(allBulletMoveTimerId);
        killTimer(enemyPlaneMoveTimerId);
        killTimer(enemyPlaneGenerateTimerId);
        maskWidget->show();
        gamePausedText->show();
    }
    else
    {
        isPause = false;
        myBulletShootTimerId = startTimer(myBulletShootTimerItv);
        enemyBulletShootTimerId = startTimer(enemyBulletShootTimerItv);
        allBulletMoveTimerId = startTimer(allBulletMoveTimerItv);
        enemyPlaneMoveTimerId = startTimer(enemyPlaneMoveTimerItv);
        enemyPlaneGenerateTimerId = startTimer(enemyPlaneGenerateTimerItv);
        maskWidget->hide();
        gamePausedText->hide();
    }
}
