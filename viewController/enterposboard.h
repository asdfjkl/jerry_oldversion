#ifndef ENTERPOSBOARD_H
#define ENTERPOSBOARD_H

#include "viewController/chessboard.h"

class EnterPosBoard : public QWidget
{
    Q_OBJECT

public:
    explicit EnterPosBoard(const ColorStyle &style, const chess::Board &board,
                           QWidget *parent = 0, bool incl_joker_piece = false);
    void setToInitialPosition();
    void setToCurrentBoard();
    void clearBoard();
    chess::Board getCurrentBoard();
    void setCastlingRights(bool wking, bool wqueen, bool bking, bool bqueen);
    void setTurn(bool turn);
    void setFlipBoard(bool onOff);
private:
    void calculateBoardSize(int *boardSize, int *squareSize);
    ColorStyle style;
    int borderWidth;
    PieceImages *pieceImages;
    chess::Board board { true };
    chess::Board currentGameBoard;
    double dpr = 1.0;

    bool flipBoard;

    bool clickedOnBoard(int x, int y);
    bool clickedOnPiceceSelector(int x, int y);
    bool incl_joker_piece;
    // get selected piece for mouse coordinates
    // mouse coordinates must be on piece selector
    uint8_t getSelectedPiece(int x, int y);
    // get board position for mouse coordinates
    // board position is tuple (i,j) where i is x-axis from 0 to 7
    // and y is y-axis
    QPoint getBoardPosition(int x, int y);

    void drawBoard(QPaintEvent *e, QPainter *q);
    //void calculateBoardSize(int *boardSize, int *squareSize);
    void resizeTo(float ratio);

    uint8_t pickupPieces[6][2] = {{chess::WHITE_PAWN, chess::BLACK_PAWN},
                                  {chess::WHITE_KNIGHT, chess::BLACK_KNIGHT},
                                  {chess::WHITE_BISHOP, chess::BLACK_BISHOP},
                                  {chess::WHITE_ROOK, chess::BLACK_ROOK},
                                  {chess::WHITE_QUEEN, chess::BLACK_QUEEN},
                                  {chess::WHITE_KING, chess::BLACK_KING}};
                                  //{chess::WHITE_ANY_PIECE, chess::BLACK_ANY_PIECE}};
    uint8_t selectedPiece;

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *m);

signals:
    void squareChanged();

public slots:

};

#endif // ENTERPOSBOARD_H
