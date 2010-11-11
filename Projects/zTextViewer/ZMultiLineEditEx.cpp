//
// C++ Implementation: ZMultiLineEditEx
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ZMultiLineEditEx.h"

ZMultiLineEditEx::ZMultiLineEditEx(QWidget *parent,bool EZXStyle,int lines,const char *name)
  : ZMultiLineEdit(parent, EZXStyle, lines, name)
{
  
  setFixedHeight( DISPLAY_HEIGHT - 80 );
  setFixedWidth( DISPLAY_WIDTH );
  //setFontPercent( DEFAULT_EDIT_FONT_SIZE );
  setUnderline( false );
  setSizePolicy( QSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  setInsertionMethod(  ZMultiLineEdit::atPosNoSelected);  
  //setCellHeight( DEFAULT_EDIT_CELL_HEIGHT );
  setCellWidth( DEFAULT_EDIT_CELL_WIDTH );
  
  setVerScrollBar( true, false );
  //setWordWrap(ZMultiLineEdit::NoWrap);
  setMaxLineLength(-1);
  setAutoUpdate(true);
  undoAvailable(true);
  
  isMarked = false;
  m_iMarkStartX =  -1 ;
  m_iMarkStartY = -1 ;
  
}

ZMultiLineEditEx::~ZMultiLineEditEx()
{
  
}

void ZMultiLineEditEx::MarkBegin()
{
  getCursorPosition( &m_iMarkStartX, &m_iMarkStartY );
  isMarked = true;
}


void ZMultiLineEditEx::MarkEnd()
{
  if( -1 != m_iMarkStartX )  
  { 
    int iSelEndX = 0;
    int iSelEndY = 0;
    getCursorPosition( &iSelEndX, &iSelEndY );
    setSelection( m_iMarkStartX, m_iMarkStartY, iSelEndX, iSelEndY );
    m_iMarkStartX = m_iMarkStartY = -1;
    isMarked = false;
  }    
}

    void ZMultiLineEditEx::atBegin()
{
  setCursorPosition(0,0, false);
}


    void ZMultiLineEditEx::atEnd()
{
  int x = getLineCount() - 1;
  int y = textLine( x ).length();
  setCursorPosition( x, y, false );  
}


void ZMultiLineEditEx::keyPressEvent ( QKeyEvent* e )
{
  
 
  switch ( e->key() ) 
  {
    
    case Z6KEY_SIDE_UP:
    { pageUp(false);
    break; }
    
    case Z6KEY_SIDE_DOWN:
    { pageDown(false);
    break; }
        
    case Z6KEY_MUSIC:
    { undo();
      break; 
    }
        
    case Z6KEY_GREEN:
    { if ( ! isMarked) { MarkBegin();  }  else   { MarkEnd(); }
      break; 
    }
    
    case Z6KEY_LEFT:
    { 
      if ( e->isAutoRepeat())  
      {  cursorWordBackward(isMarked  ); }  
      else  { cursorLeft(isMarked, true); }
      break; 
    }
      
    case Z6KEY_UP:
    { cursorUp(isMarked);
      break; 
    }
        
    case Z6KEY_RIGHT:
    { 
      if ( e->isAutoRepeat())  
            {  cursorWordForward(isMarked  ); }  
      else  { cursorRight(isMarked, true); }
      break; 
    }
            
    case Z6KEY_DOWN: 
    { cursorDown(isMarked);
      break; 
    }
             
    case Z6KEY_C:
    { isMarked =false;
    m_iMarkStartX = m_iMarkStartY = -1;
    ZMultiLineEdit::keyPressEvent ( e );
    break; 
    }
    
    case Z6KEY_RED:
    { 
      QString ProgramDir = QString ( qApp->argv() [0] ) ;
	  int i = ProgramDir.findRev ( "/" );
	  ProgramDir.remove ( i+1, ProgramDir.length() - i );
	  ZConfig cfg(ProgramDir + "/" + APP_CFG);
	  QString Lang = cfg.readEntry(QString("General"), QString("LANGUAGE"), QString(""));
	  ZConfig cfg2(ProgramDir + "/languages/" + Lang);
	  QString WARNING = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_WARNING"), QString("Warning"));
	  QString CLOSE_APP = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_CLOSE_APP"), QString("Close app?"));
      
      ZMessageDlg *dlg = new ZMessageDlg(QString(WARNING), QString(CLOSE_APP), ZMessageDlg::yes_no, 0, this, "dlg", true, 0);
      if (dlg->exec())
      {
        qApp->quit();
      } 
      delete dlg;
    break; 
    }
    default:
      ZMultiLineEdit::keyPressEvent ( e );
  }  

  
}


void ZMultiLineEditEx::BlockCopy()
{
  isMarked =false;
  m_iMarkStartX = m_iMarkStartY = -1;
  copy();
}

void ZMultiLineEditEx::BlockCut()
{
  isMarked =false;
  m_iMarkStartX = m_iMarkStartY = -1;
  cut();
}

void ZMultiLineEditEx::BlockPaste()
{
  paste();
}
