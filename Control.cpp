#include "Control.h"

const QPointF scoreTextPos = QPointF(650, 520);
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

Control::Control(int boardWidth, int boardHeight,
    const string &myPlaneImageFile, int myLife, int mySkill,
                 const string &myBulletImageFile, int myBulletPower,
    const string &enemyPlaneImageFile, int enemyLife,
                 const string &enemyBulletImageFile, int enemyBulletPower)
{
    this->myPlaneImageFile = myPlaneImageFile;
	this->myLife = myLife;
    this->mySkill = mySkill;

    this->myBulletImageFile = myBulletImageFile;
    this->myBulletImageScaleHeight = myBulletImageScaleHeight;
	this->myBulletPower = myBulletPower;

    this->enemyPlaneImageFile = enemyPlaneImageFile;
	this->enemyLife = enemyLife;

    this->enemyBulletImageFile = enemyBulletImageFile;
	this->enemyBulletPower = enemyBulletPower;

    /* ����ֵ */
    lifeFrameBar = new QGraphicsRectItem(LifeBarPos.x(), LifeBarPos.y(), myLife*2,5);
    lifeFrameBar->setPen(QPen(Qt::white));
    addItem(lifeFrameBar);
    lifeBar = new QGraphicsRectItem(LifeBarPos.x(), LifeBarPos.y(), myLife*2, 5);
    lifeBar->setBrush(QBrush(Qt::green));
    addItem(lifeBar);

    /* ����ֵ */
    skillFrameBar = new QGraphicsRectItem(SkillBarPos.x(),SkillBarPos.y(), myLife*2,5);
    skillFrameBar->setPen(QPen(Qt::white));
    addItem(skillFrameBar);
    skillBar = new QGraphicsRectItem(SkillBarPos.x(), SkillBarPos.y(), mySkill*2, 5);
    skillBar->setBrush(QBrush(Qt::blue));
    addItem(skillBar);

    /* ��ʼ�����������ű������� */
    setSceneRect(0,0,boardWidth,boardHeight);
    playList = new QMediaPlaylist;
    playList->addMedia(QUrl::fromLocalFile("music/starwars.mp3"));
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); //����ѭ��
    player = new QMediaPlayer(this);
    player->setPlaylist(playList);
    player->play();

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
    gamePausedText->setFont(QFont("Courier", 30, QFont::Bold));
    gamePausedText->setPos(250, 250);
    gamePausedText->setZValue(2);
    gamePausedText->hide();

    /* �÷���ʾ */
    myBulletType = 0; //�ӵ����ͳ�ʼ��
    score = 0; //�÷�����
    scoreText = new QGraphicsTextItem;
    scoreText->setPos(scoreTextPos);
    scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));
    scoreText->setFont(QFont("Courier"));
    addItem(scoreText);

    /* ��Ϸ��ֹ��ʾ */
    gameLostText = new QGraphicsTextItem;
    addItem(gameLostText);
    gameLostText->setHtml(tr("<font color=white>Game Over</font>"));
    gameLostText->setFont(QFont("Courier", 30, QFont::Bold));
    gameLostText->setPos(250, 250);
    gameLostText->setZValue(2);
    gameLostText->hide();

    /* ��Ϸ��ʼ��ʾ */
    hasStarted = false;
    gameHelpText = new QGraphicsTextItem;
    addItem(gameHelpText);
    QString helpText = tr("<font color=white size=4>Welcome to zs's WAR OF PLANES Game</font><br><br>");
    helpText += tr("<font color=white size=3>You are supposed to aviod being hit by enemy bullets,<br>");
    helpText += tr("and you will score when your bullet hits an enemy plane.<br>");
    helpText += tr("When you collides with an enemy plane,<br>");
    helpText += tr("your life will decline by 1 while the enemy plane also gets hurt.<br>");
    helpText += tr("<br>Here is some help:<br>");
    helpText += tr("<br>Process Bars:<br>");
    helpText += tr("Green Process Bar: You life value<br>");
    helpText += tr("Blue Process Bar: Your skill value<br>");
    helpText += tr("<br>General Operations:<br>");
    helpText += tr("W: Up;    S: Down;    A: Left;    D: Right<br>");
    helpText += tr("Space: Pause or continue the game<br>");
    helpText += tr("<br>Skill Operations:<br>");
    helpText += tr("Q: Shoot 3 bullets at a time, costing 5 skill points<br>");
    helpText += tr("E: Kill all enemy planes at a time, costing 3 skill points<br>");
    helpText += tr("R: Remove all enemy bullets at a time, costing 7 skill points<br>");
    helpText += tr("<br>Please push Enter to start the game");
    helpText += tr("</font>");
    gameHelpText->setHtml(helpText);
    gameHelpText->setFont(QFont("Courier"));
    gameHelpText->setPos(0,0);
    gameHelpText->setZValue(2);
    gameHelpText->hide();

    /* ���뻶ӭ���棬֮��Enter��ʼ��Ϸ */
    welcomeGame();
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
    else if(event->timerId()==skillQTimerId)
        myBulletType = 0;
}

void Control::keyPressEvent(QKeyEvent *event)
{
    if(hasStarted==false && event->key()!=Qt::Key_Enter) //����Ϸδ��ʼ�����Enter��İ�������Ч
        return;

    if(event->key()==Qt::Key_Enter) //ֻ����û�п�ʼ��Ϸ��ʱ�򰴴˼�����Ч
    {
        hasStarted = true;
        gameHelpText->hide();
        maskWidget->hide();
        startGame();
    }
    else if(event->key()==Qt::Key_W)
    {
        myplane->moveBy(0, -10);
        myplane->update();
    }
    else if(event->key()==Qt::Key_S)
    {
        myplane->moveBy(0, 10);
        myplane->update();
    }
    else if(event->key()==Qt::Key_A)
    {
        myplane->moveBy(-10, 0);
        myplane->update();
    }
    else if(event->key()==Qt::Key_D)
    {
        myplane->moveBy(10, 0);
        myplane->update();
    }
    else if(event->key()==Qt::Key_Q && myplane->skill>=5)
    {
        //��Q�ļ��ܿ���һ�η���3���ӵ������ǻ�����5�㼼��
        myBulletType = 1;
        myplane->skill -= 5;
        updateBar(skillBar, skillFrameBar, SkillBarPos, -10, QBrush(Qt::blue));
        skillQTimerId = startTimer(5000); //5��ʹ��ʱ��
    }
    else if(event->key()==Qt::Key_E && myplane->skill>=3)
    {
        //��E�ļ��ܿ��Դ�����зɻ�������3�㼼��ֵ
        for(vector<EnemyPlane*>::iterator iter=enemyplanes.begin(); iter!=enemyplanes.end(); iter++)
        {
            score++;
            scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));
            removeItem(*iter);
            delete *iter;
        }
        enemyplanes.clear();

        myplane->skill -= 3;
        updateBar(skillBar, skillFrameBar, SkillBarPos, -6, QBrush(Qt::blue));
    }
    else if(event->key()==Qt::Key_R && myplane->skill>=7)
    {
        //��R�����������ел��ӵ�������7�㼼��ֵ
        for(vector<Bullet*>::iterator it = enemybullets.begin(); it!= enemybullets.end(); it++)
        {
            removeItem(*it);
            delete *it;
        }
        enemybullets.clear();

        myplane->skill -= 7;
        updateBar(skillBar, skillFrameBar, SkillBarPos, -14, QBrush(Qt::blue));
    }
    else if(event->key()==Qt::Key_Space)
        pauseGame();
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
                updateBar(lifeBar, lifeFrameBar, LifeBarPos, -2, QBrush(Qt::green));
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
        updateBar(lifeBar, lifeFrameBar, LifeBarPos, -2, QBrush(Qt::green));
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
        updateBar(lifeBar, lifeFrameBar, LifeBarPos, -2, QBrush(Qt::green));
        if(myplane->life<=0)
            loseGame();
	}
    else if(bullet->part==ME)
    {
        /* Ȼ����л������л��Ѿ�û������ֵ���ʹ�enemyplanes��ɾȥ */
        for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
        {
            bool alive = true;
            if (bullet->collidesWithItem(*it))
            {
                bullet->hit(this);
                alive = (*it)->crash(this)==false;
                myplane->skill++;
                updateBar(skillBar, skillFrameBar, SkillBarPos, +2, QBrush(Qt::blue));
                score++;
                scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));
            }

            if(alive)
                it++;
            else
            {
                delete *it;
                it = enemyplanes.erase(it);
            }
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
                                        enemyBulletImageFile, QPointF(0,1), enemyBulletPower);
            enemybullets.push_back(bullet);
            addItem(bullet);
        }
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
    if(myBulletType==0)
    {
        Bullet *bullet = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(0,-3), myBulletPower);
        mybullets.push_back(bullet);
        addItem(bullet);
    }
    else if(myBulletType==1)
    {
        Bullet *bullet1 = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(-3,-3), myBulletPower);
        mybullets.push_back(bullet1);
        bullet1->setRotation(-45);
        addItem(bullet1);

        Bullet *bullet2 = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(0,-3), myBulletPower);
        mybullets.push_back(bullet2);
        addItem(bullet2);

        Bullet *bullet3 = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(3,-3), myBulletPower);
        mybullets.push_back(bullet3);
        bullet3->setRotation(45);
        addItem(bullet3);
    }
}

void Control::updateBar(QGraphicsRectItem *bar, QGraphicsRectItem *frameBar, const QPointF &pos, qreal var, const QBrush &brush)
{
    qreal wid = bar->rect().width();
    if(var<0)
        wid = max((qreal)0, wid+var);
    else
        wid = min(frameBar->rect().width(), wid+var);
    bar->setRect(pos.x(), pos.y(), wid, bar->rect().height());
    bar->setBrush(brush);
    bar->update();
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

void Control::loseGame()
{
    killTimer(myBulletShootTimerId);
    killTimer(enemyBulletShootTimerId);
    killTimer(allBulletMoveTimerId);
    killTimer(enemyPlaneMoveTimerId);
    killTimer(enemyPlaneGenerateTimerId);
    maskWidget->show();
    gameLostText->setHtml(tr("<font color=white>Game Over<br>Score: %1</font>").arg(score));
    gameLostText->show();
}

void Control::welcomeGame()
{
    hasStarted = false;
    maskWidget->show();
    gameHelpText->show();
}

void Control::startGame()
{
    /* ���ø���������ʱ�� */
    myBulletShootTimerId = startTimer(myBulletShootTimerItv);
    enemyBulletShootTimerId = startTimer(enemyBulletShootTimerItv);
    allBulletMoveTimerId = startTimer(allBulletMoveTimerItv);
    enemyPlaneMoveTimerId = startTimer(enemyPlaneMoveTimerItv);
    enemyPlaneGenerateTimerId = startTimer(enemyPlaneGenerateTimerItv);

    /* �����ҷɻ� */
    myplane = new MyPlane(width() / 2, height() / 2, myPlaneImageFile, this, myLife, mySkill);
    myplane->synScreen(this);

    /* ��ӵл� */
    for (int i = 0; i < 3; i++)
        generateEnemyPlane();
}
