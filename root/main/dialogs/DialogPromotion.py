class DialogPromotion(QDialog):
 
    def __init__(self, whitePromotes, parent=None):
        super(DialogPromotion, self).__init__(parent) 

        self.border = 10
        h = self.size().height() - (2*self.border)
        w = (self.size().width()-(2*self.border))/4
        # piece size
        self.ps = min(h,w)
        self.resize(300, 90)
        self.whitePromotes = whitePromotes
        self.final_piece = "Q"
        self.sel_idx = 0
        self.pieceImages = PieceImages()

    def paintEvent(self, event):
        qp = QtGui.QPainter()
        qp.begin(self)
        # draw images
        h = self.size().height() - (2*self.border)
        w = (self.size().width()-(2*self.border))/4
        self.ps = min(h,w)
        s = self.ps
        print("size is: "+str(s))
        
        lightBlue2 = QtGui.QColor(166,188,231)
        qp.setBrush(lightBlue2)
        for i in range(0,4):
            if self.sel_idx == i:
                qp.drawRect(self.border+i*s,self.border,s,s)
            qp.drawImage(self.border+i*s,self.border,self.pieceImages.getWp(self.piece_by_idx(i),s))
        
        #lightBlue = QtGui.QColor(90,106,173) 
        #lightBlue2 = QtGui.QColor(166,188,231)
        #darkWhite = QtGui.QColor(239,239,239)
        #boardOffsetX = self.borderWidth;
        #boardOffsetY = self.borderWidth;
        # qp.setBrush(darkBlue)
        # qp.drawImage(0,0,self.pieceImages.getWp("Q", 40))
        # (boardSize,squareSize) = self.calculateBoardSize()
        
        # qp.drawRect(1,1,boardSize,boardSize)
        
        qp.end()
    
    def piece_by_idx(self,idx):
        if(self.whitePromotes):
            if(idx == 0):
                return "Q"
            elif(idx == 1):
                return "R"
            elif(idx == 2):
                return "B"
            elif(idx == 3):
                return "N"
        if(not self.whitePromotes):
            if(idx == 0):
                return "q"
            elif(idx == 1):
                return "r"
            elif(idx == 2):
                return "b"
            elif(idx == 3):
                return "n"    
        
    def mousePressEvent(self, mouseEvent):
        print("mouse x:"+str(mouseEvent.x())+"y:"+str(mouseEvent.y()))
        sel_idx = mouseEvent.x() // self.ps
        print("sel idx: "+str(sel_idx))
        self.sel_idx = sel_idx
        self.update()
    
    def mouseReleaseEvent(self, mouseEvent):
        print("mouse release event")
        self.final_piece = self.piece_by_idx(self.sel_idx);
        self.done(True)
                
    def keyPressEvent(self, event):
        key = event.key()
        if key == QtCore.Qt.Key_Left:
            self.sel_idx = max(self.sel_idx-1,0)
            self.update()
        elif key == QtCore.Qt.Key_Right:
            self.sel_idx = min(self.sel_idx+1,3)
            self.update()
        elif key == QtCore.Qt.Key_Return :
            self.final_piece = self.piece_by_idx(self.sel_idx);
            self.done(True)