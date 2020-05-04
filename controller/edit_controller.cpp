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

#include "edit_controller.h"
#include "chess/pgn_printer.h"
#include <QClipboard>
#include <QApplication>
#include "dialogs/dialog_enterposition.h"
#include "dialogs/dialog_editheaders.h"
#include <QDebug>
#include <iostream>
#include "chess/pgn_reader.h"

EditController::EditController(GameModel *gameModel, QWidget *parent) :
    QObject(parent)
{
    this->gameModel = gameModel;
    this->parentWidget = parent;
}

void EditController::copyGameToClipBoard() {

    chess::PgnPrinter *pgnPrinter = new chess::PgnPrinter();
    QString pgn = pgnPrinter->printGame(this->gameModel->getGame()).join("\n");
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(pgn);

}

void EditController::copyPositionToClipBoard() {

    QString fen = this->gameModel->getGame()->getCurrentNode()->getBoard()->fen();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fen);

}

void EditController::paste() {
    /*
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    // first check whether text is fen string, try to create a board
    try {
        chess::Board b(text);
        this->gameModel->getGame()->resetWithNewRootBoard(std::move(b));
        this->gameModel->getGame()->setTreeWasChanged(true);
        this->gameModel->triggerStateChange();
    } catch(std::invalid_argument e) {
        std::cerr << e.what() << std::endl;
        // not a fen string. let's try pgn
        chess::Game *g = new chess::Game();
        try {
            chess::PgnReader *reader = new chess::PgnReader();            
            reader->readGameFromString(text, g);
            this->gameModel->setGame(g);
            this->gameModel->getGame()->setTreeWasChanged(true);
            this->gameModel->triggerStateChange();
        }
        catch(std::invalid_argument e) {
            std::cerr << e.what() << std::endl;
        }
    }*/
}

void EditController::enterPosition() {
    chess::Board currentBoard = this->gameModel->getGame()->getCurrentNode()->getBoard();
    DialogEnterPosition *dlg = new DialogEnterPosition(currentBoard,
                                                       this->gameModel->colorStyle,
                                                       this->parentWidget);
    if(dlg->exec() == QDialog::Accepted) {

        chess::Board new_board = dlg->getCurrentBoard();
        //std::cout << "GOT BOARD FROM DLG: " << std::endl;
        //std::cout << this->gameModel->getGame()->getCurrentNode()->getBoard() << "\n";
        this->gameModel->getGame()->resetWithNewRootBoard(new_board);
        //std::cout << "CURRENT NODE BOARD: " << std::endl;
        //std::cout << this->gameModel->getGame()->getCurrentNode()->getBoard() << "\n";
        this->gameModel->triggerStateChange();
    }
}

void EditController::editHeaders() {
    DialogEditHeaders *dlg = new DialogEditHeaders(*this->gameModel->getGame(), this->parentWidget);
    if(dlg->exec() == QDialog::Accepted) {

        this->gameModel->getGame()->setHeader("Event", dlg->leEvent->text());
        this->gameModel->getGame()->setHeader("Site", dlg->leSite->text());
        this->gameModel->getGame()->setHeader("Date", dlg->leDate->text());
        this->gameModel->getGame()->setHeader("Round", dlg->leRound->text());
        this->gameModel->getGame()->setHeader("White", dlg->leWhite->text());
        this->gameModel->getGame()->setHeader("Black", dlg->leBlack->text());
        this->gameModel->getGame()->setHeader("ECO", dlg->leECO->text());
        if(dlg->rbBlackWins->isChecked()) {
            this->gameModel->getGame()->setResult(chess::RES_BLACK_WINS);
            this->gameModel->getGame()->setHeader("Result", "0-1");
        } else if(dlg->rbWhiteWins->isChecked()) {
            this->gameModel->getGame()->setResult(chess::RES_WHITE_WINS);
            this->gameModel->getGame()->setHeader("Result", "1-0");
        } else if(dlg->rbDraw->isChecked()) {
            this->gameModel->getGame()->setResult(chess::RES_DRAW);
            this->gameModel->getGame()->setHeader("Result", "1/2-1/2");
        } else if(dlg->rbUndefined->isChecked()) {
            this->gameModel->getGame()->setResult(chess::RES_UNDEF);
            this->gameModel->getGame()->setHeader("Result", "*");
        }
        this->gameModel->getGame()->setTreeWasChanged(true);
    }
    delete dlg;
    this->gameModel->triggerStateChange();
}
