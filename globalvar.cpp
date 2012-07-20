#include "globalvar.h"

QString fileName;
QString targetID;
MyButton * globalButton;
bool group_singleFinish = false;
QList<MyButton *> globalMyButtons;
QPushButton * groupSave;
QString scene_ID;
bool confirmTarget = false;

int up = 0;
int fore = 0;
int thigh = 0;
int calf = 0;
int head = 0;
int body = 0;

int orignalImageWidth;
int orignalImageHeight;
QString limbs[2] = {"L", "R"};

