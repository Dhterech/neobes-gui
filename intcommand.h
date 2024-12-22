#ifndef INTCOMMAND_H
#define INTCOMMAND_H

#include "data/ptr2struct.h"
#include "types.h"

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
    static void handlePointer(GUICommand &gui, int pointer);
public:
    static GUICommand ConvertToGUI(commandbuffer_t command);
    static commandbuffer_t ConvertToNormal(GUICommand gui);
};

#endif // INTCOMMAND_H
