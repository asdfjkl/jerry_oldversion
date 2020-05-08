/* Jerry - A Chess Graphical User Interface
 * Copyright (C) 2014-2016 Dominik Klein
 * Copyright (C) 2015-2016 Karl Josef Klein
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "game_model.h"
#include "chess/game.h"
#include "chess/board.h"
#include <QDebug>
#include <QPixmap>
#include <assert.h>
#include <QSettings>
#include <iostream>
#include "chess/pgn_printer.h"
#include "chess/pgn_reader.h"
#include <QtGui>
#include "chess/node_pool.h"
#include "internalengine.h"
#include "various/resource_finder.h"

GameModel::GameModel(QObject *parent) :
    QObject(parent)
{
    this->modelVersion = 4;
    this->wasSaved = false;
    this->lastSaveFilename = "";
    this->lastOpenDir = "";
    this->lastSaveDir = "";
    this->currentPgnFilename = "";
    this->game = new chess::Game();
    //this->colorStyle = new ColorStyle(ResourceFinder::getPath());
    //this->fontStyle = new FontStyle();
    this->mode = MODE_ENTER_MOVES;
    InternalEngine default_engine = InternalEngine();
    //qDebug() << "on model startup:" << default_engine.getPath();

    this->engines.append(default_engine);
    this->activeEngineIdx = this->engines.size() - 1;
    this->lastAddedEnginePath = QString("");
    this->humanPlayerColor = chess::WHITE;
    // stockfish specific: 0 (min), 20 (max)
    this->engineStrength = 20;
    this->engineThinkTimeMs = 3000;
    this->flipBoard = false;

    this->currentBestPv = QString("");
    this->currentMateInMoves = -1;
    this->currentEval = -10000;

    this->prevBestPv = QString("");
    this->prevMateInMoves = -1;
    this->prevEval = -10000;

    this->analysisThreshold = 0.5;

    this->showEval = true;

    this->gameAnalysisStarted = false;

    this->gameAnalysisForPlayer = ANALYSE_BOTH_PLAYERS;

    this->company = QString("dkl");
    this->appId = QString("jerry_").append(JERRY_VERSION);

    this->loadOpeningBook();

    this->database = PgnDatabase();

    this->nrPvLines = 1;
}

GameModel::~GameModel() {

}


void GameModel::loadOpeningBook() {
    QString path = ResourceFinder::getPath().append("/books/");
    path = path.append(QString("varied.bin"));
    path = QDir::toNativeSeparators(QDir::cleanPath(path));
    this->book = new chess::Polyglot(path);
}

bool GameModel::canAndMayUseBook(chess::GameNode *node) {

    if(node->getDepth() > 20) {
        return false;
    } else {
        if(this->engineStrength <= 5 && node->getDepth() <= 8) {
            return true;
        }
        if(this->engineStrength <= 10 && node->getDepth() <= 12) {
            return true;
        }
        if(this->engineStrength > 10) {
            return true;
        }
        return false;
    }
}

QVector<chess::Move> GameModel::getBookMoves(chess::GameNode *node) {
    chess::Board b = node->getBoard();
    return this->book->findMoves(b);
}

bool GameModel::isInBook(chess::GameNode *node) {
    chess::Board b = node->getBoard();
    return this->book->inBook(b);
}

QString GameModel::getLastAddedEnginePath() {
    return this->lastAddedEnginePath;
}

void GameModel::setLastAddedEnginePath(QString &path) {
    this->lastAddedEnginePath = path;
}

void GameModel::setEngines(QVector<Engine> engines) {
    this->engines = engines;
}

void GameModel::setActiveEngine(int activeEngineIdx) {
    assert(activeEngineIdx >= 0 && activeEngineIdx < this->engines.size());
    this->activeEngineIdx = activeEngineIdx;
}

chess::Game* GameModel::getGame() {
    return this->game;
}

void GameModel::setGame(chess::Game *g) {
    assert(g != nullptr);
    chess::NodePool::deleteNode(this->game->getRootNode());
    delete this->game;
    this->game = g;
}

void GameModel::triggerStateChange() {
    emit(stateChange());
}

int GameModel::getMode() {
    return this->mode;
}

void GameModel::setMode(int mode) {
    this->mode = mode;
}

QVector<Engine> GameModel::getEngines() {
    return this->engines;
}

Engine GameModel::getActiveEngine() {
    return this->engines.at(this->activeEngineIdx);
}

int GameModel::getActiveEngineIdx() {
    return this->activeEngineIdx;
}

Engine GameModel::getInternalEngine() {
    assert(this->engines.size() > 0);
    return this->engines.at(0);
}

int GameModel::getEngineThinkTime() {
    return 5;
}

int GameModel::getEngineStrength() {
    return this->engineStrength;
}

void GameModel::setInternalEngine(Engine e) {
    this->engines[0] = e;
}

void GameModel::saveGameState() {

    QSettings settings(this->company, this->appId);

    settings.clear();

    settings.setValue("modelVersion", this->modelVersion);
    settings.setValue("lastSaveFilename", this->lastSaveFilename);
    settings.setValue("lastSaveDir", this->lastSaveDir);
    settings.setValue("lastOpenDir", this->lastOpenDir);

    settings.setValue("colorTheme", colorStyle.styleType);
    settings.setValue("pieceType", colorStyle.pieceType);
    settings.setValue("lastAddedEnginePath", this->lastAddedEnginePath);

    settings.setValue("engineViewFontSize", fontStyle.engineOutFontSize);
    settings.setValue("moveViewFontSize", fontStyle.moveWindowFontSize);

    // stockfish specific settings
    settings.setValue("engineStrength", this->engineStrength);
    settings.setValue("engineThinkTimeMs", this->engineThinkTimeMs);
    settings.setValue("analysisThreshold", this->analysisThreshold);
    settings.setValue("showEval", this->showEval);

    settings.beginWriteArray("engines");
    for(int i=0;i<this->engines.size();i++) {
        settings.setArrayIndex(i);
        Engine e = this->engines.at(i);
        settings.setValue("engineName", e.getName());
        settings.setValue("enginePath", e.getPath());
        // internal engine must always be at position 0
        if(i == 0) {
            settings.setValue("internalEngine", true);
        } else {
            settings.setValue("internalEngine", false);
        }
        if(i == this->activeEngineIdx) {
            settings.setValue("activeEngine", true);
        } else {
            settings.setValue("activeEngine", false);
        }
        settings.beginWriteArray("engineOptions");
        for(int j=0;j<e.getUciOptions().size();j++) {
            EngineOption o = e.getUciOptions().at(j);
            QString uciOptStr = o.toUciOptionString();

                settings.setArrayIndex(j);
                QString optNr = QString::number(j);
                QString option = QString(optNr).append("option");
                QString val = QString(optNr).append("value");
                QString type = QString(optNr).append("type");
                settings.setValue(type, o.type);
                settings.setValue(option, uciOptStr);
                if(o.type == EN_OPT_TYPE_CHECK) {
                    settings.setValue(val, o.check_val);
                } else if(o.type == EN_OPT_TYPE_COMBO) {
                    settings.setValue(val, o.combo_val);
                } else if(o.type == EN_OPT_TYPE_SPIN) {
                    settings.setValue(val, o.spin_val);
                } else if(o.type == EN_OPT_TYPE_STRING) {
                    settings.setValue(val, o.string_val);
                }


        }
        settings.endArray();
    }
    settings.endArray();

    // write current game as pgn to settings
    // first set dummy value
    settings.setValue("currentGame", "");
    chess::PgnPrinter printer;
    try {
        QStringList pgnStringList = printer.printGame(this->getGame());
        QString pgn = pgnStringList.join("\n");
        settings.setValue("currentGame", pgn);
    } catch(std::exception e) {
        std::cerr << e.what() << std::endl;
    }
    settings.sync();
}

void GameModel::restoreGameState() {

    QSettings settings(this->company, this->appId);

    if(settings.contains("currentGame")) {
        QString pgnString = settings.value("currentGame").toString();
        chess::PgnReader *reader = new chess::PgnReader();
        try {
            chess::Game *g = this->getGame();
            reader->readGameFromString(pgnString, g);
            this->game->findEco();
        } catch(std::exception e) {
            if(this->game != nullptr) {
                delete this->game;
            }
            this->game = new chess::Game();
            std::cerr << e.what() << std::endl;
        }
        delete reader;
    }
    if(settings.contains("modelVersion")) {
        this->modelVersion = settings.value("modelVersion").toInt();
    }
    if(settings.contains("lastSaveFilename")) {
        this->lastSaveFilename = settings.value("lastSaveFilename").toString();
    }
    if(settings.contains("lastSaveDir")) {
        this->lastSaveDir = settings.value("lastSaveDir").toString();
    }
    if(settings.contains("lastOpenDir")) {
        this->lastOpenDir = settings.value("lastOpenDir").toString();
    }
    if(settings.contains("colorTheme")) {
        int styleType = settings.value("colorTheme").toInt();
        this->colorStyle.setStyle(styleType);
    }
    if(settings.contains("engineViewFontSize")) {
        this->fontStyle.engineOutFontSize = settings.value("engineViewFontSize").toString();
    }
    if(settings.contains("moveViewFontSize")) {
        this->fontStyle.moveWindowFontSize = settings.value("moveViewFontSize").toString();
    }
    if(settings.contains("pieceType")) {
        this->colorStyle.pieceType = settings.value("pieceType").toInt();
    }
    if(settings.contains("lastAddedEnginePath")) {
        this->lastAddedEnginePath = settings.value("lastAddedEnginePath").toString();
    }
    if(settings.contains("engineStrength")) {
        this->engineStrength = settings.value("engineStrength").toInt();
    }
    if(settings.contains("engineThinkTimeMs")) {
        this->engineThinkTimeMs = settings.value("engineThinkTimeMs").toInt();
    }
    if(settings.contains("analysisThreshold")) {
        this->analysisThreshold = settings.value("analysisThreshold").toFloat();
    }
    /* omit show_eval: on starting up, eval should always
     * be displayed, no matter what user did before
    if(settings.contains("showEval")) {
        this->showEval = settings.value("showEval").toBool();
    }*/
    int size = settings.beginReadArray("engines");
    for(int i=0;i<size;i++) {
        settings.setArrayIndex(i);
        Engine e; // = new Engine();
        if(settings.contains("engineName")) {
            QString eName = settings.value("engineName").toString();
            e.setName(eName);
        }
        if(settings.contains("enginePath")) {
            QString ePath = settings.value("enginePath").toString();
            //qDebug() << "load path: " << ePath;
            e.setPath(ePath);
        }
        bool isInternal = false;
        if(settings.contains("internalEngine")) {
            isInternal = settings.value("internalEngine").toBool();
        }
        bool isActive = false;
        if(settings.contains("activeEngine")) {
            isActive = settings.value("activeEngine").toBool();
            //if(isActive && !isInternal) {
            //
            //    //this->active_engine = e; //TODO: proper serialization of engines & options
            //}
        }
        int sizeOpts = settings.beginReadArray("engineOptions");
        for(int j=0;j<sizeOpts;j++) {
            settings.setArrayIndex(j);
            EngineOption o; // = new EngineOption();
            QString optNr = QString::number(j);
            QString option = QString(optNr).append("option");
            QString val = QString(optNr).append("value");
            QString type = QString(optNr).append("type");
            bool restored = false;
            if(settings.contains(option) && settings.contains(val) && settings.contains(type)) {
                int typeCode = settings.value(type).toInt();
                QString enopt = settings.value(option).toString();
                if(o.restoreFromString(enopt)) {
                    if(typeCode == EN_OPT_TYPE_CHECK) {
                        o.check_val = settings.value(val).toBool();
                        restored = true;
                    } else if(typeCode == EN_OPT_TYPE_COMBO) {
                        o.combo_val = settings.value(val).toString();
                        restored = true;
                    } else if(typeCode == EN_OPT_TYPE_SPIN) {
                        o.spin_val = settings.value(val).toInt();
                        restored = true;
                    } else if(typeCode == EN_OPT_TYPE_STRING) {
                        o.string_val = settings.value(val).toString();
                        restored = true;
                    }
                }
            }
            if(restored) {
                e.addEngineOption(o);
            }
        }
        settings.endArray();
        // there should _always_ be at least one engine
        // prior to recovering state, namely internal engine
        if(isInternal) {
            assert(this->engines.size() > 0);
            this->setInternalEngine(e);
        } else {
            this->engines.append(e);
        }
        if(isActive) {
            this->setActiveEngine(engines.size() - 1);
        }
    }
    settings.endArray();
}
