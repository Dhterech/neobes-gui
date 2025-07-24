#ifndef INTCOMMAND_H
#define INTCOMMAND_H

#include "data/ptr2struct.h"
#include "types.h"
#include "neodata.h"

struct GUICommand {
    QString commandType;
    QString arg1;
    QString arg2;
    QString arg3;
    QString arg4;
};

class intcommand
{
private:
    static void handleSceneCommand(GUICommand &gui, int rank, int time);
    static void reverseHandleSceneCommand(GUICommand &gui, uint16_t &rank, uint32_t &time);
    static void handleRecord(GUICommand &gui, int pointer);
    static uint32_t reverseHandleRecord(QString argument);
public:
    static GUICommand ConvertToGUI(commandbuffer_t command);
    static int ConvertToNormal(QString guiText, commandbuffer_t job, int row, int col);
};

#endif // INTCOMMAND_H
