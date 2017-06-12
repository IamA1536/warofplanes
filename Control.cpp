#include "Control.h"
#include <fstream>

static int myPlaneWidth = 5;
static int myPlaneHeight = 3;
static char myPlaneShape[] = "  |  --*-- -+- ";

static int enemyPlaneWidth = 3;
static int enemyPlaneHeight = 3;
static char enemyPlaneShape[] = " - -+- | ";

Control::Control()
{
}

Control::Control(int boardWidth, int boardHeight, int boardEntension, int enemyNumber, 
	int myLife, int myBulletPower, int myBulletSpeed,
	int enemyLife, int enemyBulletSpeed, int enemyBulletPower):
	myplane(boardHeight - myPlaneHeight, boardWidth / 2, myPlaneWidth, myPlaneHeight, myPlaneShape, myLife)
{
	this->myLife = myLife;
	this->myBulletPower = myBulletPower;
	this->myBulletSpeed = myBulletSpeed;
	this->enemyLife = enemyLife;
	this->enemyBulletPower = enemyBulletPower;
	this->enemyBulletSpeed = enemyBulletSpeed;

	/* ����־ */
/*	ofstream fout("log.txt");
	fout << "boardHeight: " << boardHeight << "  boardWidth: " << boardWidth << endl;
	fout << "myplane: (" << myplane.x << "," << myplane.y << ")" << endl;
	fout << "generate " << enemyNumber << " enemyplanes" << endl;
	fout.close();*/

	/* ��ʼ����Ļ */
	this->boardWidth = boardWidth;
	this->boardHeight = boardHeight;
	this->screenWidth = boardWidth + boardEntension + 3;
	this->screenHeight = boardHeight + 3;
	screen = new char *[screenHeight];
	for (int i = 0; i < screenHeight; i++)
	{
		screen[i] = new char[screenWidth];
		for (int j = 0; j < screenWidth; j++)
			screen[i][j] = ' ';
	}
	
	/* ��ӱ߿򣬱߿�����ֻ��1���ַ�����д����Plane��ˢ����Ļ�Ĵ��룩 */
	for (int j = 0; j < screenWidth; j++)
		screen[0][j] = '-';
	for (int j = 0; j < screenWidth; j++)
		screen[1+boardHeight][j] = '-';
	for (int i = 0; i < screenHeight; i++)
		screen[i][0] = '|';
	for (int i = 0; i < screenHeight; i++)
		screen[i][boardWidth + 1] = '|';
	for (int i = 0; i < screenHeight; i++)
		screen[i][screenWidth - 1] = '|';
	
	/* ������̱�� */
/*	for (int i = 0; i < boardHeight; i++)
		screen[1 + i][1 + boardWidth + 1] = i % 10 + '0';
	for (int i = 0; i < boardWidth; i++)
		screen[1 + boardHeight + 1][1 + i] = i % 10 + '0';
*/
	/* ����ҷɻ�ͬ������Ļ */
	myplane.synScreen(screen);

	/* ��ӵл� */
	for (int i = 0; i < enemyNumber; i++)
		generateEnemyPlane();

	refreshScreen();
	run();
}

void Control::refreshScreen()
{
	char **myInfo = new char *[boardHeight / 2];
	myInfo[0] = new char[screenWidth - boardWidth - 3];
	strcpy(myInfo[0], "Yours: ");
	myInfo[1] = new char[screenWidth - boardWidth - 3];
	sprintf(myInfo[1], "  Life: %d", myplane.life);
	myInfo[2] = new char[screenWidth - boardWidth - 3];
	sprintf(myInfo[2], "  Bullet Power: %d", myBulletPower);
	myInfo[3] = new char[screenWidth - boardWidth - 3];
	sprintf(myInfo[3], "  Bullet Speed: %d", myBulletSpeed);
	for (int i = 0; i < 4; i++)
		strcpy(screen[1 + i] + boardWidth + 3, myInfo[i]);

	char **enemyInfo = new char *[boardHeight / 2];
	enemyInfo[0] = new char[screenWidth - boardWidth - 3];
	strcpy(enemyInfo[0], "Enemy: ");
	enemyInfo[1] = new char[screenWidth - boardWidth - 3];
	sprintf(enemyInfo[1], "  Left: %d", enemyplanes.size());
	enemyInfo[2] = new char[screenWidth - boardWidth - 3];
	sprintf(enemyInfo[2], "  Bullet Power: %d", enemyBulletPower);
	enemyInfo[3] = new char[screenWidth - boardWidth - 3];
	sprintf(enemyInfo[3], "  Bullet Speed: %d", enemyBulletSpeed);
	for (int i = 0; i < 4; i++)
		strcpy(screen[1 + boardHeight / 2 + i] + boardWidth + 3, enemyInfo[i]);

	/* �����λ������Ϊ(0,0) */
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { 0,0 };
	SetConsoleCursorPosition(hOut, pos);

	/* ���ع�� */
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(hOut, &CursorInfo);//��ȡ����̨�����Ϣ  
	CursorInfo.bVisible = false; //���ؿ���̨���  
	SetConsoleCursorInfo(hOut, &CursorInfo);//���ÿ���̨���״̬  

	for (int i = 0; i < screenHeight; i++)
	{
		for (int j = 0; j < screenWidth; j++)
			cout << screen[i][j];
		cout << endl;
	}
}

bool Control::generateEnemyPlane()
{
	/* ����ڵ�һ�����ɵл� */
	srand(time(NULL));//��ʼ��ʱ������
	int y = rand() % (boardWidth - enemyPlaneWidth); //�л������λ��
	int cnt = 100; //��¼ѭ������������ǰ�޷������µл�����ֱ�ӷ��أ�����ѯ100��
	while (cnt > 0 && (screen[1 + 1][1 + y] != ' ' 
		|| screen[1 + 1][1 + y + 1] != ' ' 
		|| screen[1 + 1][1 + y + 2] != ' ') ) //����2����һ����Ϊ�գ�����Ҫ��������
	{
		y = rand() % (boardWidth - enemyPlaneWidth);
		cnt--;
	}
	
	if (cnt < 0)
		return false;
	else
	{
		/* �����л� */
		EnemyPlane *enemy = new EnemyPlane(0, y, enemyPlaneWidth, enemyPlaneHeight, enemyPlaneShape, screen, enemyLife);
		enemyplanes.push_back(enemy);
		return true;
	}
}

int Control::judgeCrash(int aX, int aY, int aHeight, int aWidth, char * aShape, int bX, int bY, int bHeight, int bWidth, char * bShape)
{
	/* ����������β��غϵ����:aBottom<bTop, aTop>bBottom, aRight<bLeft, aLeft>bRight */
	if (aX + aHeight<bX || aX>bX + bHeight || aY + aWidth < bY || aY > bY + bWidth)
		return 0;
//	ofstream fout("log.txt", ios::app);
	int crashPointCnt = 0;
	for (int i = 0; i < aHeight; i++)
		for (int j = 0; j < aWidth; j++)
			if (aShape[i*aWidth + j] != ' ')
				for (int m = 0; m < bHeight; m++)
					for (int n = 0; n < bWidth; n++)
						if (bShape[m*bWidth + n] != ' ' && aX + i == bX + m && aY + j == bY + n)
						{
//							fout << "<" << aX + i << "," << aY + j << ">";
							crashPointCnt++;
						}
//	fout.close();
	return crashPointCnt;
}

bool Control::changePlanePosition(Plane *plane, int newX, int newY)
{
	/* ���λ���Ƿ��б仯���ޱ仯�򷵻�NOCHANGE */
	if (plane->x == newX && plane->y == newY)
		return true;

	/* �����λ���Ƿ�Ϸ������Ϸ��򷵻�NOCHANGE */
	if (newX<0 || newX>boardHeight - plane->height || newY<0 || newY>boardWidth - plane->width)
	{
		if (plane->type == MYPLANE) //��ҷɻ����������
			return true;
		else if (plane->type == ENEMYPLANE) //�л���������Ͻ硢��硢�ҽ磬�����½�֮�󽫱�ɾ�� 
		{
			if (newX>boardHeight - plane->height)
			{
				plane->delScreen(screen);
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
		if (judgeCrash(newX, newY, plane->height, plane->width, plane->shape,
			(*it)->x, (*it)->y, (*it)->height, (*it)->width, (*it)->shape) > 0)
		{
			/* ��Ϊ��ҷɻ������ܷɻ���crash������ֵ����1 */
			if (plane->type == MYPLANE)
			{
				plane->crash(screen);
				alive = (*it)->crash(screen);
			}
			if (plane->type == ENEMYPLANE) //��ͬΪ�л���������ı�λ�ã�NOCHANGE
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

	/* Ȼ�����Ƿ�����ҷɻ���ײ */
	if (plane->type == ENEMYPLANE) //���˷ɻ�Ϊ�л�
	{
		if(judgeCrash(newX, newY, plane->height, plane->width, plane->shape,
			myplane.x, myplane.y, myplane.height, myplane.width, myplane.shape) >0)
		{
			myplane.crash(screen);
			plane->crash(screen);
		}
	}

	/* ��plane����������겢ͬ����Ļ */
	if (plane->life > 0)
	{
		plane->delScreen(screen);
		plane->x = newX;
		plane->y = newY;
		plane->synScreen(screen);

		/* �˷ɻ��ڹ��е��ӵ�λ����ɻ����� */
		for(vector<Bullet*>::iterator it=plane->bullets.begin();it!=plane->bullets.end();it++)
			if ((*it)->state == READY)
			{
				(*it)->x += (newX - plane->x);
				(*it)->y += (newY - plane->y);
			}
	}

	return plane->life > 0;
}

bool Control::updateEnemyPlanes()
{
	/* ����ǰ�л�����3�����Զ����ɵл�����Ŀ�����С��8 */
	if (enemyplanes.size() < 3)
	{
		int genNum = rand() % 8;
		for (int i = 0; i < genNum; i++)
			generateEnemyPlane();
	}

	shootEnemyBullets(); //���ел������ӵ�

	/* ���ел��ƶ�λ�� */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
	{
		pair<int, int> loc = (*it)->updatePosition();
		if (changePlanePosition(*it, loc.first, loc.second))
			it++;
		else
			it = enemyplanes.erase(it);
	}

	refreshScreen();

	return myplane.life > 0;
}

bool Control::judgeHit(Bullet * bullet, int bulletNewX, int bulletNewY, 
	int planeX, int planeY, int planeHeight, int planeWidth, char * planeShape)
{
	/* �����ӵ����ڷɻ������е���� */
//	if (bulletX<planeX || bulletX>planeX + planeHeight || bulletY<planeY || bulletY>planeY + planeWidth)
	//	return false;

	/* �����ɻ����㣬�������ӵ��غϵĵ��򷵻�true */
	for (int i = 0; i < planeHeight; i++)
		for (int j = 0; j < planeWidth; j++)
			if (planeShape[i*planeWidth + j] != ' ' &&
				((bullet->y == planeY + j && bullet->dir == DOWN && bullet->x < planeX + i && planeX + i <= bulletNewX)
					|| (bullet->y == planeY + j && bullet->dir == UP && bulletNewX <= planeX + i && planeX + i < bullet->x)))
				return true;

	return false;
}

bool Control::changeBulletPosition(Bullet * bullet, int newX, int newY)
{
//	ofstream fout("log.txt", ios::app);
	/* ���λ���Ƿ��б仯���ޱ仯�򷵻�true */
	if (bullet->x == newX && bullet->y == newY)
		return true;

	/* ����ӵ��Ƿ����ĳһ�ɻ� */
	/* ���ȼ����ҷɻ� */
	bool flag = judgeHit(bullet, newX, newY, myplane.x, myplane.y, myplane.height, myplane.width, myplane.shape);
/*	fout << "[ <" << bullet->x << "," << bullet->y << "> -> <"
		<< newX << "," << newY << "> & (" 
		<< myplane.x << "," << myplane.y <<  ") ]=" << flag << endl;*/
	if (flag)
	{
		bullet->hit(screen);
		myplane.crash(screen);
	}

	/* Ȼ����л�����ʹ�л��Ѿ�û������ֵ��Ҳ����enemyplanes��ɾȥ����Ϊ�˷ɻ����ܻ����ӵ����˶� */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); it++)
	{
		flag = judgeHit(bullet, newX, newY, (*it)->x, (*it)->y, (*it)->height, (*it)->width, (*it)->shape);
/*		fout << "[ <" << bullet->x << "," << bullet->y << "> -> <"
			<< newX << "," << newY << "> & ("
			<< (*it)->x << "," << (*it)->y << ") ]=" << flag << endl;*/
		if (flag)
		{
			bullet->hit(screen);
			(*it)->crash(screen);
		}
	}

	/* ����ӵ��Ƿ����ӵ���ײ����ײ��ͬʱ���� */
//	fout << "try: <" << bullet->x << "," << bullet->y << "> -> <" << newX << "," << newY << ">  ";
	for(vector<EnemyPlane*>::iterator iter=enemyplanes.begin();iter!=enemyplanes.end();iter++)
		for (vector<Bullet*>::iterator it = (*iter)->bullets.begin(); it != (*iter)->bullets.end();)
		{
			if (*it == bullet) //�����Լ�
				it++;
			else
			{
				bool alive = true;
				if ((*it)->x == newX && (*it)->y == newY)
				{
					bullet->hit(screen);
					alive = (*it)->hit(screen);
				}

				if (alive)
				{
					assert(it != (*iter)->bullets.end());
					it++;
				}
				else
				{
	/*				fout << "collide @(" << newX << "," << newY << ") [" 
						<< "(" << bullet->x << "," << bullet->y << ")and"
						<< "(" << (*it)->x << "," << (*it)->y << ")]" << endl;*/
					delete *it;
					it = (*iter)->bullets.erase(it);
				}
			}
		}

	/* ���ӵ�������ɱ���������λ�ò�ͬ����Ļ */
	if (bullet->power > 0)
	{
		/* ����ʱ�ӵ���ͼ����߽磬�������ӵ� */
		if (newX <= 0 || newX >= boardHeight || newY <= 0 || newY >= boardWidth)
		{
			bullet->delScreen(screen);
			return false;
		}
//		fout << "<" << bullet->x << "," << bullet->y << "> -> <" << newX << "," << newY << ">" << endl;
		if (bullet->state == READY) //���ӵ������ڹ��У������״̬����ɾ��ԭ��λ��
			bullet->state = RUNNING;
		else if(bullet->state==RUNNING) //���ӵ�����ս������Ҫɾ��ԭ��λ��
			bullet->delScreen(screen);
		bullet->x = newX;
		bullet->y = newY;
		bullet->synScreen(screen);
	}
	refreshScreen();

//	fout.close();

	return bullet->power>0;
}

bool Control::updateBullets()
{
	updateMyBullets();
	updateEnemyBullets();
	return myplane.life > 0;
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
				new Bullet((*iter)->x + (*iter)->height - 1, (*iter)->y + 1, DOWN, enemyBulletPower, enemyBulletSpeed));
	updateEnemyBullets();
}

void Control::updateMyBullets()
{
	for (vector<Bullet*>::iterator it = myplane.bullets.begin(); it != myplane.bullets.end(); )
	{
		pair<int, int> loc = (*it)->updatePosition();
		if (changeBulletPosition(*it, loc.first, loc.second))
			it++;
		else
		{
			delete *it;
			it = myplane.bullets.erase(it);
		}
	}
}

void Control::shootBullet()
{
	/* ��ҷɻ��������ӵ������ӵ�����ҷɻ��ڹܼ�˵�λ�� */
	myplane.bullets.push_back(new Bullet(myplane.x, myplane.y + 2, UP, myBulletPower, myBulletSpeed));

	/* ������ҷɻ��ӵ�λ�� */
	updateMyBullets();
}

bool Control::keyboardHandle(char cmd)
{
	/* ���ݰ����ı���ҷɻ�λ�� */
	int newX = myplane.x;
	int newY = myplane.y;
	switch (cmd)
	{
	case 'w': newX = myplane.x - 1; break; //��
	case 's': newX = myplane.x + 1; break; //��
	case 'a': newY = myplane.y - 1;	break; //��
	case 'd': newY = myplane.y + 1; break; //��
	case 'm': shootBullet(); return true; //�����ӵ�
	case 'p': generateEnemyPlane(); return true; //�����µл�
	case 'o': myplane.life++; return true; //������ֵ
	case 'u': myBulletSpeed = max(myBulletSpeed, 0); return true; //�ӵ��ٶ�+
	case 'i': myBulletSpeed++; return true; //�ӵ��ٶ�-
	case 'h': enemyBulletPower = max(enemyBulletPower, 0); return true; //��С�л��ӵ�����
	case 'j': enemyBulletPower++; return true; //���ӵл��ӵ�����
	case 'k': enemyBulletSpeed = max(enemyBulletSpeed, 0);; return true; //��С�л��ӵ��ٶ�
	case 'l': enemyBulletSpeed++; return true; //���ӵл��ӵ��ٶ�
	default: return true;
	}

	changePlanePosition(&myplane, newX, newY);
	refreshScreen();
	return myplane.life>0;
}

void Control::run()
{	
	clock_t bulletPositionClock = clock();
	clock_t newBulletClock = clock();
	clock_t planePositionClock = clock();
	while (1)
	{
		if (clock() - bulletPositionClock >= 300) //ÿ500����ˢ��һ���ӵ�λ��
		{
			if (!updateBullets())
				break;
			bulletPositionClock = clock();
		}
		
		if (clock() - newBulletClock >= 500) //ÿ1000���뷢��һ�����ӵ�
		{
			shootEnemyBullets();
			newBulletClock = clock();
		}

		if (clock() - planePositionClock >= 1000) //ÿ2000�������һ�εл���λ��
		{
			if (!updateEnemyPlanes())
				break;
			planePositionClock = clock();
		}

		if (kbhit() && !keyboardHandle(getch()))
			break;

	}
}
