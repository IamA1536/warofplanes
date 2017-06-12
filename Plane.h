#ifndef PLANE_H
#define PLANE_H

#include "Bullet.h"
#include <cstring>
#include <vector>
using namespace std;

enum Type { MYPLANE, ENEMYPLANE };

class Plane
{
	friend class Control;
public:
	Plane();
	Plane(int x, int y);
	Plane(int x, int y, int width, int height, const char *shape);
	Plane(int x, int y, int width, int height, const char *shape, char **screen);
	Plane(int x, int y, int width, int height, const char *shape, int life);
	Plane(int x, int y, int width, int height, const char *shape, int life, Type type);
	Plane(int x, int y, int width, int height, const char * shape, char ** screen, int life);
	Plane(int x, int y, int width, int height, const char *shape, char **screen, int life, Type type);
	void delScreen(char **screen); //����Ļ�Ͻ��˷ɻ�Ĩȥ
	void synScreen(char **screen); //����Ļ��Ӧλ����Ӵ˷ɻ�
	bool crash(char **screen); //�ɻ�������ײ
	~Plane();
	
protected:
	int x; //��������
	int y; //�������
	int width; //���
	int height; //�߶�
	char *shape; //shape[0~width*height-1]��ʾ�ɻ���״���ж���Ҫ���ɻ���С����С��2*2���ҵ�2�е�2�б������ַ�
	int life; //����ֵ
	Type type;
	vector<Bullet*> bullets; //�ӵ�
};

#endif // !PLANE_H
