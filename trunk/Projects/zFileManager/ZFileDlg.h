//
// C++ Interface: ZFileDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZFILEDLG_H
#define ZFILEDLG_H

#include "BaseDlg.h"
#include "ZFileBrowser.h"
#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include "resources.h"
#include <ZOptionsMenu.h>
#include <ZKbMainWidget.h>
#include <ZListBox.h>
#include <ZPressButton.h>

class FileBrowser;

class ZFileOpenDialog : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFileOpenDialog(bool onlyDirs); 
    ~ZFileOpenDialog();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);
    
    
  public slots:   
    void slot_cancel();
    void slot_paste();
    void selectItem(int);
    void slot_makedir();
    
  private:
    QString fileName;
    QString basePath;      
    FileBrowser* fb;
    ZOptionsMenu * menuPaste;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZFileSaveDialog : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFileSaveDialog(const QString &curPath = "/", const QString &originName = ""); 
    ~ZFileSaveDialog();
    //int exec();
    //int result() const ; 
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; }
	QString texto(const char*xString);

  private:
    ZMultiLineEdit *zmle;
    QString fileName;  
    QString basePath;   
    FileBrowser* fb;
           
  protected:
    virtual void keyPressEvent( QKeyEvent* e);    
    virtual void accept();  

};

class ZIconSelect : public MyBaseDlg {
  Q_OBJECT

  public:
    ZIconSelect(); 
    ~ZIconSelect();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
    ZOptionsMenu * menuPaste;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZOpenWith : public MyBaseDlg {
  Q_OBJECT

  public:
    ZOpenWith(); 
    ~ZOpenWith();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
    ZOptionsMenu * menuPaste;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZFolderSelect2 : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFolderSelect2(bool onlyDirs, QString path = "/"); 
    ~ZFolderSelect2();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
    void slot_paste();
    void selectItem(int);
    
  private:
    QString fileName;
    QString basePath;      
    FileBrowser* fb;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZFolderSelect : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFolderSelect(bool onlyDirs, QString path = "/"); 
    ~ZFolderSelect();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
    void slot_paste();
    void selectItem(int);
    void slot_makedir();
    
  private:
    QString fileName;
    QString basePath;      
    FileBrowser* fb;
    ZOptionsMenu * menuPaste;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};


class ZAdminFav : public MyBaseDlg {
  Q_OBJECT

  public:
    ZAdminFav(); 
    ~ZAdminFav();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void EditarFav();
	void BorrarFav();
	void CargarFavs();
	void brClicked();   
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
    ZOptionsMenu * menuMain;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};


class ZSettings : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSettings(); 
    ~ZSettings();
    QString getFileName(const QString &AFileName);
    QString getFilePath() { return basePath; } 
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void CargarSettings();
	void setClicked();   

	void MostrarOcultos();
	void Miniaturas();
	void VerLista();   
	void VerDetalle();   
	void VerGrandes();   
	void EditarHome();   
	void CambiarVista();   
	void FontSize();
	void TextEditor();
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};


class ZClipboard : public MyBaseDlg {
  Q_OBJECT

  public:
    ZClipboard(); 
    ~ZClipboard();
    //int exec();
    //int result() const ;  
    QString getFileName(const QString &AFileName);
    QString getFilePath(const QString &AFileName);
	QString texto(const char*xString);    
	bool PasteFiles(const QString &ASourse, const QString &ADest, bool remove = false);
	bool  CopyFile ( QString ASourceName, QString ADestName );
	int  GetFolderSize ( const QString &path );
	int  GetFilesCountInFolder ( const QString &path );
	QStringList GetFilesInFolder(const QString &path);
    ZAppInfoArea *pAIA;

  public slots:   
    void slot_cancel();
	void Borrar();
	void Vaciar();
	void brClicked();   
	void CargarFavs();
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
    ZOptionsMenu * menuMain;
    
  protected:
    virtual void accept(); 
};

class myButton : public ZPressButton {
  Q_OBJECT

  public:
    myButton(QWidget*, char const*, ZSkinService::WidgetClsID = ZSkinService::clsZPressButton);
    ~myButton();

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};

class ZSearch : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSearch(); 
    ~ZSearch();
	QString texto(const char*xString); 
    
  public slots:   
    void slot_cancel();
    void slot_search();
    
  private:
    ZListBox* browser;
	myButton* buttonDir; 
	ZLineEdit* lineEdit;  

  protected:
	//virtual void keyPressEvent(QKeyEvent* e);
   
};


class ZSearchRes : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSearchRes(); 
    ~ZSearchRes();
    //int exec();
    //int result() const ;  
    QString getFileName(const QString &AFileName);
    QString getFilePath(const QString &AFileName);
	QString texto(const char*xString);    
    
  public slots:   
    void slot_cancel();
	void brClicked();   
	void CargarFavs();
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
    
  protected:
    virtual void accept(); 
	virtual void keyPressEvent(QKeyEvent* e);
};



#endif
