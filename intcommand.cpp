#include "intcommand.h"

QString modesSingleP[] = {"Cool", "Good", "Bad", "Awful", "Cooler", "Less cool", "Better", "Worse"};

void intcommand::handleSceneCommand(GUICommand &gui, int rank, int time) {
    gui.arg1 = "RANK: " + modesSingleP[rank];
    gui.arg4 = "TIME: " + QString::number(time);
}

void intcommand::handlePointer(GUICommand &gui, int pointer) {
    // TODO
}

GUICommand intcommand::ConvertToGUI(commandbuffer_t command) {
    GUICommand gui;
    QString run_display_lr[] = {"Lesson 1", "Lesson 2", "Lesson 3", "Lesson 4", "Lesson 5", "Round 1", "Round 2", "Round 3", "Round 4", "Round 5"};
    QString arg1[] = {"Teacher", "PaRappa", "BoxxyBoi", "Special"};
    QString arg2[] = {"Nothing", "PaRappa", "Boxxy", "Beard B.", "Master Onion", "Ant", "Sister Moosesha", "Takoyama", "Takoyama", "Colonel Noodle", "Mushi"
                      , "White fog", "Orange line", "Blue line"};
    switch(command.cmd_id) {
    case 00:
        gui.commandType = "SETUP";
        gui.arg1 = "OWNER: ";
        switch(command.arg1) {
        case 0x02:
            gui.arg1 += "Teacher"; break;
        case 0x04:
            gui.arg1 += "PaRappa"; break;
        case 0x08:
            gui.arg1 += "BoxxyBoi"; break;
        case 0x10:
            gui.arg1 += "Special"; break;
        }
        gui.arg2 = "ICON: " + arg2[command.arg2];
        gui.arg4 = "TIME: " + QString::number(command.arg4);

        break;
    case 1:
        gui.commandType = "LOAD_PLAY_RECORD";
        gui.arg4 = "TODO: Pointer";
        break;
    case 2:
        gui.commandType = "SCORE";
        handleSceneCommand(gui, command.arg1, command.arg4);
        break;
    case 3:
        gui.commandType = "SCENE_BETTER";
        handleSceneCommand(gui, command.arg1, command.arg4);
        break;
    case 4:
        gui.commandType = "SCENE_COOLER";
        handleSceneCommand(gui, command.arg1, command.arg4);
        break;
    case 5:
        gui.commandType = "SCENE_OOPS";
        handleSceneCommand(gui, command.arg1, command.arg4);
        break;
    case 6:
        gui.commandType = "SCENE_WORSE";
        handleSceneCommand(gui, command.arg1, command.arg4);
        break;
    case 7:
        gui.commandType = "RETURN";
        gui.arg1 = "Unknown";
        gui.arg4 = "Unknown";
        break;
    case 8:
        gui.commandType = "END_STAGE";
        gui.arg1 = "Unknown";
        gui.arg4 = "Unknown";
        break;
    case 9:
        gui.commandType = "RUN";
        if(command.arg1 == 0x2) {
            gui.arg1 = "SET_BG";
            gui.arg4 = "TODO: Unknown";
        }
        if(command.arg1 == 0xE) {
            gui.arg1 = "PRELOAD_RECORD";
            gui.arg4 = "TODO: Pointer";
        }
        if(command.arg1 == 0x12) {
            gui.arg1 = "DISPLAY_LR";
            gui.arg2 = run_display_lr[command.arg2];
        }
        break;
    }
    return gui;
}

commandbuffer_t intcommand::ConvertToNormal() {
    commandbuffer_t normal;
    return normal;
}
