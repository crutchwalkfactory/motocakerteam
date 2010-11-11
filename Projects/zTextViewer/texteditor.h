#ifndef __Z6_TEXTEDITOR_H__
#define __Z6_TEXTEDITOR_H__

#include "ZFileBrowser.h"
#include "ZFileDlg.h"
#include "ZMultiLineEditEx.h"
#include "ZAboutDlg.h"

#include <ZKbMainWidget.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZMessageDlg.h>
//#include <ZMultiLineEdit.h>
#include <ZLineEdit.h>
#include <ZScrollPanel.h>
#include <ZScrollView.h>
#include <ZAppInfoArea.h>
#include <ZNumPickerDlg.h>
#include <ZLabel.h>

#include <ZConfig.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

class TextEditor: public ZKbMainWidget
{
	Q_OBJECT
public:    
	TextEditor(QWidget *parent, char const *name);
	~TextEditor();
	        
  void CreateWindow(QWidget* parent);
  void LoadSettings();
	
  ZNumPickerDlg* textDialog;
  
  QString Idioma;
  QString getProgramDir();
  QString getFilePath(const QString &AFileName);
  QString getFileName(const QString &AFileName);
  
	//ZMultiLineEdit * m_pTextEdit;
  ZMultiLineEditEx * m_pTextEdit;
	ZScrollPanel* m_pScrollPanel;

	//ZLineEdit
	//ZMultiLineEdit * m_pLineEdit;
  ZMultiLineEditEx * m_pLineEdit;
         
	QString m_sFileName;
	QString m_sFilePath;
	        
  int DEFAULT_EDIT_FONT_SIZE;
  QString FUENTE;
  QString DEFAULT_EDIT_CELL_HEIGHT;
      
public slots:
	void load(const QString &fileName);
	void slot_about();
	void slot_language();
	void slot_textSize();
	void slot_saveFile();
	void slot_saveFileAs();
	void slot_newFile();
	void slot_openFile();

	void slot_PageUp();
	void slot_PageDown();
	void slot_Home();
	void slot_End();

	void slot_search();

	void slot_Copy();
	void slot_Paste();
	void slot_Cut();
	void slot_selectAll();
	void slot_MarkBegin();
	void slot_MarkEnd();

	void saveAs( const QString& sFileName);    

	void CreateMenu( QWidget * pParent );
	void CreateSoftKeys(QWidget * pParent);

private:    

	ZSoftKey * m_pSoftKey;    
	ZOptionsMenu * m_pMainMenu;
	ZOptionsMenu * m_pFileMenu;
	ZOptionsMenu * m_pEditMenu;
	ZOptionsMenu * m_pNavigateMenu;
	ZOptionsMenu * m_pOptionsMenu;
	ZAppInfoArea *pAIA;

	QString m_sBuffer;

  QString m_sProgramDir;
  
	bool m_bSaveAs;
};


#endif // __Z6_TEXTEDITOR_H__
