#include "job.h"
#include <QStringList>

QString modesSingleP[] = {
    "Cool", "Good", "Bad", "Awful",
    "Cooler", "Less cool", "Better", "Worse"
};

QString examMode[] = {
    "EXAM_NONE", "EXAM_COOL", "EXAM_GOOD", "EXAM_BAD", "EXAM_AWFUL",
    "EXAM_HOOK", "EXAM_VS", "EXAM_CANCEL", "EXAM_BONUS", "EXAM_MAX"
};

QString examModeDo[] = {
    "EXAM_DO_NON", "EXAM_DO_END", "EXAM_DO_END_GO", "EXAM_DO_END_GO_RET"
};

QString job_setup_owners[] = {
    "Unknown", "None", "Teacher", "PaRappa", "BoxxyBoi", "Special"
};

QString job_setup_icon[] = {
    "Nothing", "PaRappa", "Boxxy", "Beard B.", "Master Onion", "Ant",
    "Sister Moosesha", "Takoyama", "Takoyama", "Colonel Noodle", "Mushi",
    "White fog", "Orange line", "Blue line"
};

QString subjob_displaylr[] = {
    "Lesson 1", "Lesson 2", "Lesson 3", "Lesson 4", "Lesson 5",
    "Round 1", "Round 2", "Round 3", "Round 4", "Round 5"
};

QString jobCommands[] = {
    "SCRRJ_PLY",               // 0x0
    "SCRRJ_SCR",               // 0x1
    "SCRRJ_EXAM",              // 0x2
    "SCRRJ_UP_LINE",           // 0x3
    "SCRRJ_UP_JOB",            // 0x4
    "SCRRJ_DOWN_LINE",         // 0x5
    "SCRRJ_DOWN_JOB",          // 0x6
    "SCRRJ_ENDJOB",            // 0x7
    "SCRRJ_ENDGAME",           // 0x8
    "SCRRJ_SUBJOB",            // 0x9
    "SCRRJ_MSG_DISP",          // 0xA
    "SCRRJ_ADD_JOB",           // 0xB
    "SCRRJ_SUB_JOB",           // 0xC
    "SCRRJ_LINE",              // 0xD
    "SCRRJ_JOB",               // 0xE
};

QString subjobCommands[] = {
    "SCRSUBJ_CDSND_ON",        // 0x0
    "SCRSUBJ_CDSND_OFF",       // 0x1
    "SCRSUBJ_DRAW_CHANGE",     // 0x2
    "SCRSUBJ_TAP_RESET",       // 0x3
    "SCRSUBJ_EFFECT",          // 0x4
    "SCRSUBJ_REVERS",          // 0x5
    "SCRSUBJ_SPU_ON",          // 0x6
    "SCRSUBJ_SPU_ON2",         // 0x7
    "SCRSUBJ_SPU_ON3",         // 0x8
    "SCRSUBJ_SPU_ON4",         // 0x9
    "SCRSUBJ_TITLE",           // 0xA
    "SCRSUBJ_LOOP",            // 0xB
    "SCRSUBJ_FADEOUT",         // 0xC
    "SCRSUBJ_ENDLOOP",         // 0xD
    "SCRSUBJ_SPUTRANS",        // 0xE
    "SCRSUBJ_STOP_MENDERER",   // 0xF
    "SCRSUBJ_BONUS_GAME",      // 0x10
    "SCRSUBJ_BONUS_GAME_END",  // 0x11
    "SCRSUBJ_LESSON",          // 0x12
    "SCRSUBJ_VS_RESET",        // 0x13
    "SCRSUBJ_CDSND_READY",     // 0x14
    "SCRSUBJ_CDSND_REQ",       // 0x15
    "SCRSUBJ_SPU_OFF",         // 0x16
    "SCRSUBJ_JIMAKU_OFF",      // 0x17
    "SCRSUBJ_MAX"              // 0x18
};

static int findStringIndex(const QString& target, const QString array[], int size) {
    for (int i = 0; i < size; ++i) {
        if (target == array[i]) return i;
    }
    return -1; // Not found
}

void intcommand::handleSceneCommand(GUICommand &gui, int rank, int time) {
    if (rank >= 0 && rank < std::size(modesSingleP)) {
        gui.arg1 = "RANK: " + modesSingleP[rank];
    } else {
        gui.arg1 = "RANK: " + QString::number(rank);
    }
    gui.arg4 = "TIME: " + QString::number(time);
}

void intcommand::reverseHandleSceneCommand(GUICommand &gui, uint16_t &rank, uint32_t &time) {
    const QString rankPrefix = "RANK: ";
    const QString timePrefix = "TIME: ";

    if (gui.arg1.startsWith(rankPrefix)) {
        QString rankTmp = gui.arg1.mid(rankPrefix.length());
        int index = findStringIndex(rankTmp, modesSingleP, std::size(modesSingleP));

        if (index != -1) {
            rank = index;
        } else {
            rank = rankTmp.toInt();
        }
    } else {
        rank = gui.arg1.toInt();
    }

    if (gui.arg4.startsWith(timePrefix)) {
        time = gui.arg4.mid(timePrefix.length()).toInt();
    } else {
        time = gui.arg4.toInt();
    }
}

QString intcommand::handleRecord(int pointer) {
    int count = 0;
    for (e_suggestrecord_t &record : Records) {
        if (pointer == record.address) {
            return QString::number(count);
        }
        count++;
    }
    return "Not Loaded";
}

uint32_t intcommand::reverseHandleRecord(QString argument) {
    if (argument.contains(": ")) {
        argument = argument.split(": ").last();
    }

    int recordNum = argument.toInt();

    if (recordNum >= 0 && recordNum < Records.size()) {
        return Records[recordNum].address;
    }
    return 0xFFFFFFF;
}

QString intcommand::handleOwners(uint32_t owner) {
    if (owner == 0) return job_setup_owners[0];

    QStringList owners;
    if (owner & 0x01) owners << job_setup_owners[1];
    if (owner & 0x02) owners << job_setup_owners[2];
    if (owner & 0x04) owners << job_setup_owners[3];
    if (owner & 0x08) owners << job_setup_owners[4];
    if (owner & 0x10) owners << job_setup_owners[5];

    return owners.join(" ");
}

GUICommand intcommand::ConvertToGUI(commandbuffer_t command) {
    GUICommand gui;

    if (command.cmd_id >= 0 && command.cmd_id < std::size(jobCommands)) {
        gui.commandType = jobCommands[command.cmd_id];
    } else {
        gui.commandType = "UNKNOWN JOB";
    }

    switch (command.cmd_id) {
        case 0x00: // SCRRJ_PLY (SETUP)
            gui.arg1 = "Owner: " + handleOwners(command.arg1);
            if (command.arg2 >= 0 && command.arg2 < std::size(job_setup_icon)) {
                gui.arg2 = "Icon: " + job_setup_icon[command.arg2];
            } else {
                gui.arg2 = "Icon: " + QString::number(command.arg2);
            }
            gui.arg4 = "Time: " + QString::number(command.arg4);
            break;

        case 0x01: // SCRRJ_SCR (LOAD_PLAY_RECORD)
        {
            QString follows[] = {"None", "Save", "Load"};
            if (command.arg1 >= 0 && command.arg1 < 3) {
                gui.arg1 = "Follow: " + follows[command.arg1];
            } else {
                gui.arg1 = "Follow: " + QString::number(command.arg1);
            }
            gui.arg4 = "Record: " + handleRecord(command.arg4);
            break;
        }

        case 0x02: // SCRRJ_EXAM (SCORE)
            if (command.arg1 >= 0 && command.arg1 < std::size(examMode)) {
                gui.arg1 = "Mode: " + examMode[command.arg1];
            } else {
                gui.arg1 = "Mode: " + QString::number(command.arg1);
            }

            if (VSMode) {
                gui.arg4 = "Player: " + handleOwners(command.arg4);
            } else {
                gui.arg4 = "PlayerCode: " + QString::number(command.arg4);
            }
            break;

        case 0x03: // SCRRJ_UP_LINE
        case 0x04: // SCRRJ_UP_JOB
        case 0x05: // SCRRJ_DOWN_LINE
        case 0x06: // SCRRJ_DOWN_JOB
            handleSceneCommand(gui, command.arg1, command.arg4);
            break;

        case 0x07: // SCRRJ_ENDJOB
        case 0x08: // SCRRJ_ENDGAME
            break;

        case 0x09: // SCRRJ_SUBJOB (RUN_SUBJOB)
            if (command.arg1 >= 0 && command.arg1 < std::size(subjobCommands)) {
                gui.arg1 = subjobCommands[command.arg1];
            } else {
                gui.arg1 = "UNKNOWN SUBJOB";
            }

            // Sub-job specific handling
            switch (command.arg1) {
                case 0x04: // SCRSUBJ_EFFECT
                    gui.arg2 = "Mode: " + QString::number(command.arg2);
                    gui.arg3 = "Depth: " + QString::number(command.arg3);
                    break;
                case 0x05: // SCRSUBJ_REVERS
                    gui.arg2 = "Duration: " + QString::number(command.arg2);
                    break;
                case 0x06: // SCRSUBJ_SPU_ON
                case 0x07: // SCRSUBJ_SPU_ON2
                case 0x08: // SCRSUBJ_SPU_ON3
                case 0x09: // SCRSUBJ_SPU_ON4
                    gui.arg2 = "SoundBoard: " + QString::number(command.arg2);
                    gui.arg3 = "Number: " + QString::number(command.arg3);
                    break;
                case 0x0A: // SCRSUBJ_TITLE
                    gui.arg2 = "Dera: " + QString::number(command.arg2);
                    gui.arg3 = "Menderer: " + QString::number(command.arg3);
                    break;
                case 0x0B: // SCRSUBJ_LOOP
                    gui.arg2 = "Target Time: " + QString::number(command.arg2);
                    break;
                case 0x0E: // SCRSUBJ_SPUTRANS
                    gui.arg2 = "Record: " + handleRecord(command.arg2);
                    break;
                case 0x12: // SCRSUBJ_LESSON
                    if (command.arg2 >= 0 && command.arg2 < std::size(subjob_displaylr)) {
                        gui.arg2 = subjob_displaylr[command.arg2];
                    }
                    gui.arg3 = "Time: " + QString::number(command.arg3);
                    break;
                case 0x14: // SCRSUBJ_CDSND_READY
                    gui.arg2 = "File Index: " + QString::number(command.arg2);
                    break;
                case 0x15: // SCRSUBJ_CDSND_REQ
                    gui.arg2 = "Volume: " + QString::number(command.arg2);
                    break;
                case 0x16: // SCRSUBJ_SPU_OFF
                    gui.arg2 = "SoundBoard: " + QString::number(command.arg2);
                    break;
            }
            break;

                case 0x0A: // SCRRJ_MSG_DISP
                    break;

                case 0x0B: // SCRRJ_ADD_JOB
                case 0x0C: // SCRRJ_SUB_JOB
                    if (command.arg1 >= 0 && command.arg1 < std::size(modesSingleP)) {
                        gui.arg1 = "SrcRank: " + modesSingleP[command.arg1];
                    } else {
                        gui.arg1 = "SrcLine: " + QString::number(command.arg1);
                    }
                    gui.arg2 = "TgtLine: " + QString::number(command.arg2);
                    gui.arg4 = "Time: " + QString::number(command.arg4);
                    break;

                case 0x0D: // SCRRJ_LINE
                case 0x0E: // SCRRJ_JOB
                    if (command.arg1 >= 0 && command.arg1 < std::size(modesSingleP)) {
                        gui.arg1 = "TgtRank: " + modesSingleP[command.arg1];
                    } else {
                        gui.arg1 = "TgtLine: " + QString::number(command.arg1);
                    }
                    gui.arg3 = "ExamJob: " + QString::number(command.arg3);
                    gui.arg4 = "Time: " + QString::number(command.arg4);
                    break;
    }

    return gui;
}

int intcommand::ConvertToNormal(QString guiText, commandbuffer_t job, int row, int col) {
    int numberInput = 0;
    bool isNumber = false;

    QString textToConvert = guiText;
    if (textToConvert.contains(": ")) {
        textToConvert = textToConvert.split(": ").last();
    }

    if (textToConvert.startsWith("0x")) {
        numberInput = textToConvert.toInt(&isNumber, 16);
    } else {
        numberInput = textToConvert.toInt(&isNumber, 10);
    }

    if (col == 0) {
        if (!isNumber) {
            int idx = findStringIndex(guiText, jobCommands, std::size(jobCommands));
            if (idx != -1) return idx;
        }
    }

    if (!isNumber) {
        switch (job.cmd_id) {
            case 0x00: // SCRRJ_PLY (SETUP)
                if (col == 2) {
                    int idx = findStringIndex(textToConvert, job_setup_icon, std::size(job_setup_icon));
                    if (idx != -1) return idx;
                }
                break;

            case 0x01: // SCRRJ_SCR (LOAD_PLAY_RECORD)
                if (col == 1) {
                    if (textToConvert == "None") return 0;
                    if (textToConvert == "Save") return 1;
                    if (textToConvert == "Load") return 2;
                }
                break;

            case 0x02: // SCRRJ_EXAM (SCORE)
                if (col == 1) { // Exam Mode
                    int idx = findStringIndex(textToConvert, examMode, std::size(examMode));
                    if (idx != -1) return idx;
                }
                break;

            case 0x09: // SCRRJ_SUBJOB
                if (col == 1) {
                    int idx = findStringIndex(guiText, subjobCommands, std::size(subjobCommands));
                    if (idx != -1) return idx;
                }
                if (col == 2 && job.arg1 == 0x12) {
                    int idx = findStringIndex(textToConvert, subjob_displaylr, std::size(subjob_displaylr));
                    if (idx != -1) return idx;
                }
                break;
        }

        if (col == 1 && ((job.cmd_id >= 3 && job.cmd_id <= 6) ||
            (job.cmd_id >= 0xB && job.cmd_id <= 0xE))) {
            int idx = findStringIndex(textToConvert, modesSingleP, std::size(modesSingleP));
        if (idx != -1) return idx;
            }
    }

    // SCRRJ_SCR (Cmd 1 Col 4)
    if (col == 4 && job.cmd_id == 0x01) {
        return reverseHandleRecord(guiText);
    }

    // SCRRJ_SUBJOB (Cmd 9)
    if (job.cmd_id == 0x09) {
        // SCRSUBJ_SPUTRANS (Subjob 0xE, Col 2)
        if (col == 2 && job.arg1 == 0x0E) {
            return reverseHandleRecord(guiText);
        }
    }

    return numberInput;
}
