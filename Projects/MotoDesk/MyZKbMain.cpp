#include "MyZKbMain.h"
#include <ZLabel.h>
#include <ZMessageDlg.h>
#include <ZComboBox.h>
#include <ZLabel.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qpixmap.h>

#define Z6KEY_CENTER            0x1004
#define Z6KEY_LEFT              0x1012
#define Z6KEY_UP                0x1013
#define Z6KEY_RIGHT             0x1014
#define Z6KEY_DOWN              0x1015
#define Z6KEY_GREEN             0x1030
#define Z6KEY_RED               0x1031
#define Z6KEY_LEFT_SOFTKEY      0x1038
#define Z6KEY_SIDE_SELECT       0x1005
#define Z6KEY_RIGHT_SOFTKEY     0x103a
#define Z6KEY_MUSIC             0x1048

#define line_color 15823405
#define other_color 75757575
#define start_x 0 //Posicion inicial
#define start_y 0 //Posicion inicial

int current = 1;
int jugadas;
int valortemp;
int calcular=1;
QStringList lista;

MyZKbMain::~MyZKbMain()
{

}

void MyZKbMain::keyPressEvent(QKeyEvent* k)
{

	QPixmap pm; pm.load("ruleta_usr.png");
	ZConfig Num(QString("numeros.cfg"), true);
	ZConfig Hist(QString("historial.cfg"), true);

	switch (k->key())
	{
	case 0x1046: //Boton Verde
	break;

	case 0x1030: //Boton Verde
	calcular=0;
	reboot = new ZMessageDlg("zRoulette", 
		QString("Sistema Ubeda 0.5\nBasado en el Sistema de Cuadros\n\nwww.developermoto.com/es\nwww.modmymoto.com\n\n(C)2009 RacingLocura07, CepiPerez"), 
		(ZMessageDlg::MessageDlgType)2, 0, this,"about", true, 0);
	reboot->setTitleIcon(pm); reboot->show();
	break;

	case 0x1031: //Boton Rojo
	calcular=0;
	reboot = new ZMessageDlg("zRoulette", "Desea salir?", 
			(ZMessageDlg::MessageDlgType)1, 0, this,"about", true, 0);
	reboot->setTitleIcon(pm); reboot->show();
	if(reboot->exec()==1) { exit(); }
	else { update(); break; }
	
	case 0x1012: //Tecla de Navegacion De Izquierda
	calcular=0;
	if (current > 1 ) { --current; update(); break; } else { break; }

	case 0x1013: //Tecla de Navegacion De Arriba
	calcular=0;
	if ( current > 3 ) { ------current; update(); break; } else { break; }

	case 0x1014: //Tecla de Navegacion De Derecha
	calcular=0;
	if (current < 36 ) { ++current; update(); break; } else { break; }

	case 0x1015: //Tecla de Navegacion De Abajo
	calcular=0;
	if (current < 34 ) { ++++++current; update(); break; } else { break; }

	case 0x1038: //Softkey Izquierda
	calcular=0;
	reboot = new ZMessageDlg("zRoulette", "Desea reinciar la Ruleta?", (ZMessageDlg::MessageDlgType)1, 0, this,"about", true, 0);
	reboot->setTitleIcon(pm); reboot->show();
	if(reboot->exec()==1) { system("rm numeros.cfg"); system("rm historial.cfg"); current = 1; jugadas = 0; lista.clear(); update(); break; }
	else { update(); break; }

	case 0x103a: //Softkey Derecha
	int temp = Hist.readNumEntry ( QString("HISTORIAL"), QString("18"), 0 );
	if ( temp != 0 ) {
		reboot = new ZMessageDlg("zRoulette", "Desea limpiar las jugadas dejando las ultimas 18 solamente?", 
				(ZMessageDlg::MessageDlgType)1, 0, this,"about", true, 0); reboot->setTitleIcon(pm); reboot->show();
		if(reboot->exec()==1) { 
		  for ( int l=1; l<37; ++l) { Num.writeEntry ( QString("NUMEROS"), QString("%1").arg(l), QString("0") ); }
		  for ( int j=1; j<19; ++j) {
			int temp = Hist.readNumEntry ( QString("HISTORIAL"), QString("%1").arg(j), 0 );
			if ( temp > 0 ) { Num.writeEntry ( QString("NUMEROS"), QString("%1").arg(temp), QString("1") ); }
		  }	calcular=1; update(); break; 
		} else { break; }
	} else {
		reboot = new ZMessageDlg("zRoulette", "Deben haber mas de 18 bolas jugadas para el reinicio parcial.", 
			(ZMessageDlg::MessageDlgType)2, 0, this,"about", true, 0); reboot->setTitleIcon(pm); reboot->show(); break;
	}

	case 0x1004:  //Boton Central
	valortemp = Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(current), 0 );
	if ( valortemp < 1 ) { valortemp=0; } ++valortemp;
	Num.writeEntry(QString("NUMEROS"), QString("%1").arg(current), valortemp);

	int lugar=0; for ( int j=18; j>0; --j)
	{ int temp = Hist.readNumEntry ( QString("HISTORIAL"), QString("%1").arg(j), 0 ); if ( temp==0 ) { lugar=j; } }
	
	if ( lugar > 0 ) {
	  Hist.writeEntry ( QString("HISTORIAL"), QString("%1").arg(lugar), QString("%1").arg(current) );
	} else {
	  for ( int j=1; j<18; ++j) {
		int temp = Hist.readNumEntry ( QString("HISTORIAL"), QString("%1").arg(j+1), 0 ); 
		Hist.writeEntry ( QString("HISTORIAL"), QString("%1").arg(j), QString("%1").arg(temp) ); 
	  }
	  Hist.writeEntry ( QString("HISTORIAL"), QString("18"), QString("%1").arg(current) ); 
	}
	calcular=1;
	update();

	}

}

void MyZKbMain::Probabilidad(int valor)
{

	int valx1=valor; int valx2=valor+1; int valx3=valor+3; int valx4=valor+4;
	ZConfig Num(QString("numeros.cfg"), true);
	int val1= Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(valx1), 0);
	int val2= Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(valx2), 0);
	int val3= Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(valx3), 0);
	int val4= Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(valx4), 0);

	QPixmap pm; pm.load("ruleta_usr.png");
	QString agregar="SI";
	if (( val1 == 0 ) && ( val2 > 0 ) && (val3 > 0) && (val4 > 0))
	{ for ( int j=0 ; j < lista.count(); ++j ) { if ( QString("%1").arg(valx1) == QString("%1").arg(lista[j]) ) { agregar="NO"; } }
	  if ( agregar == "SI" ) { system(QString("echo \"Probable: %1\"").arg(valx1)); lista += QString("%1").arg(valx1); }
	} else { }

	if (( val1 > 0 ) && ( val2 == 0 ) && (val3 > 0) && (val4 > 0))
	{ for ( int j=0 ; j < lista.count(); ++j ) { if ( QString("%1").arg(valx2) == QString("%1").arg(lista[j]) ) { agregar="NO"; } }
	  if ( agregar == "SI" ) { system(QString("echo \"Probable: %1\"").arg(valx2)); lista += QString("%1").arg(valx2); }
	} else { }

	if (( val1 > 0 ) && ( val2 > 0 ) && (val3 == 0) && (val4 > 0))
	{ for ( int j=0 ; j < lista.count(); ++j ) { if ( QString("%1").arg(valx3) == QString("%1").arg(lista[j]) ) { agregar="NO"; } }
	  if ( agregar == "SI" ) { system(QString("echo \"Probable: %1\"").arg(valx3)); lista += QString("%1").arg(valx3); }
	} else { }

	if (( val1 > 0 ) && ( val2 > 0 ) && (val3 > 0) && (val4 == 0))
	{ for ( int j=0 ; j < lista.count(); ++j ) { if ( QString("%1").arg(valx4) == QString("%1").arg(lista[j]) ) { agregar="NO"; } }
	  if ( agregar == "SI" ) { system(QString("echo \"Probable: %1\"").arg(valx4)); lista += QString("%1").arg(valx4); }
	} else { }


}





void MyZKbMain::paintEvent(QPaintEvent * pe)
{	

  
  ZConfig Num(QString("numeros.cfg"), true);
  ZConfig ini(QString("skin.ini"), false);
  /*************/

  int X1 = ini.readNumEntry(QString("UBICACION"), QString("1_X"), 2);
  int X2 = ini.readNumEntry(QString("UBICACION"), QString("2_X"), 2);
  int X3 = ini.readNumEntry(QString("UBICACION"), QString("3_X"), 2);
  int X4 = ini.readNumEntry(QString("UBICACION"), QString("4_X"), 2);
  int X5 = ini.readNumEntry(QString("UBICACION"), QString("5_X"), 2);
  int X6 = ini.readNumEntry(QString("UBICACION"), QString("6_X"), 2);
  int X7 = ini.readNumEntry(QString("UBICACION"), QString("7_X"), 2);
  int X8 = ini.readNumEntry(QString("UBICACION"), QString("8_X"), 2);
  int X9 = ini.readNumEntry(QString("UBICACION"), QString("9_X"), 2);
  int X10 = ini.readNumEntry(QString("UBICACION"), QString("10_X"), 2);
  int X11 = ini.readNumEntry(QString("UBICACION"), QString("11_X"), 2);
  int X12 = ini.readNumEntry(QString("UBICACION"), QString("12_X"), 2);
  int X13 = ini.readNumEntry(QString("UBICACION"), QString("13_X"), 2);
  int X14 = ini.readNumEntry(QString("UBICACION"), QString("14_X"), 2);
  int X15 = ini.readNumEntry(QString("UBICACION"), QString("15_X"), 2);
  int X16 = ini.readNumEntry(QString("UBICACION"), QString("16_X"), 2);
  int X17 = ini.readNumEntry(QString("UBICACION"), QString("17_X"), 2);
  int X18 = ini.readNumEntry(QString("UBICACION"), QString("18_X"), 2);
  int X19 = ini.readNumEntry(QString("UBICACION"), QString("19_X"), 2);
  int X20 = ini.readNumEntry(QString("UBICACION"), QString("20_X"), 2);
  int X21 = ini.readNumEntry(QString("UBICACION"), QString("21_X"), 2);
  int X22 = ini.readNumEntry(QString("UBICACION"), QString("22_X"), 2);
  int X23 = ini.readNumEntry(QString("UBICACION"), QString("23_X"), 2);
  int X24 = ini.readNumEntry(QString("UBICACION"), QString("24_X"), 2);
  int X25 = ini.readNumEntry(QString("UBICACION"), QString("25_X"), 2);
  int X26 = ini.readNumEntry(QString("UBICACION"), QString("26_X"), 2);
  int X27 = ini.readNumEntry(QString("UBICACION"), QString("27_X"), 2);
  int X28 = ini.readNumEntry(QString("UBICACION"), QString("28_X"), 2);
  int X29 = ini.readNumEntry(QString("UBICACION"), QString("29_X"), 2);
  int X30 = ini.readNumEntry(QString("UBICACION"), QString("30_X"), 2);
  int X31 = ini.readNumEntry(QString("UBICACION"), QString("31_X"), 2);
  int X32 = ini.readNumEntry(QString("UBICACION"), QString("32_X"), 2);
  int X33 = ini.readNumEntry(QString("UBICACION"), QString("33_X"), 2);
  int X34 = ini.readNumEntry(QString("UBICACION"), QString("34_X"), 2);
  int X35 = ini.readNumEntry(QString("UBICACION"), QString("35_X"), 2);
  int X36 = ini.readNumEntry(QString("UBICACION"), QString("36_X"), 2);

  /*************/

  int Y1 = ini.readNumEntry(QString("UBICACION"), QString("1_Y"), 2);
  int Y2 = ini.readNumEntry(QString("UBICACION"), QString("2_Y"), 2);
  int Y3 = ini.readNumEntry(QString("UBICACION"), QString("3_Y"), 2);
  int Y4 = ini.readNumEntry(QString("UBICACION"), QString("4_Y"), 2);
  int Y5 = ini.readNumEntry(QString("UBICACION"), QString("5_Y"), 2);
  int Y6 = ini.readNumEntry(QString("UBICACION"), QString("6_Y"), 2);
  int Y7 = ini.readNumEntry(QString("UBICACION"), QString("7_Y"), 2);
  int Y8 = ini.readNumEntry(QString("UBICACION"), QString("8_Y"), 2);
  int Y9 = ini.readNumEntry(QString("UBICACION"), QString("9_Y"), 2);
  int Y10 = ini.readNumEntry(QString("UBICACION"), QString("10_Y"), 2);
  int Y11 = ini.readNumEntry(QString("UBICACION"), QString("11_Y"), 2);
  int Y12 = ini.readNumEntry(QString("UBICACION"), QString("12_Y"), 2);
  int Y13 = ini.readNumEntry(QString("UBICACION"), QString("13_Y"), 2);
  int Y14 = ini.readNumEntry(QString("UBICACION"), QString("14_Y"), 2);
  int Y15 = ini.readNumEntry(QString("UBICACION"), QString("15_Y"), 2);
  int Y16 = ini.readNumEntry(QString("UBICACION"), QString("16_Y"), 2);
  int Y17 = ini.readNumEntry(QString("UBICACION"), QString("17_Y"), 2);
  int Y18 = ini.readNumEntry(QString("UBICACION"), QString("18_Y"), 2);
  int Y19 = ini.readNumEntry(QString("UBICACION"), QString("19_Y"), 2);
  int Y20 = ini.readNumEntry(QString("UBICACION"), QString("20_Y"), 2);
  int Y21 = ini.readNumEntry(QString("UBICACION"), QString("21_Y"), 2);
  int Y22 = ini.readNumEntry(QString("UBICACION"), QString("22_Y"), 2);
  int Y23 = ini.readNumEntry(QString("UBICACION"), QString("23_Y"), 2);
  int Y24 = ini.readNumEntry(QString("UBICACION"), QString("24_Y"), 2);
  int Y25 = ini.readNumEntry(QString("UBICACION"), QString("25_Y"), 2);
  int Y26 = ini.readNumEntry(QString("UBICACION"), QString("26_Y"), 2);
  int Y27 = ini.readNumEntry(QString("UBICACION"), QString("27_Y"), 2);
  int Y28 = ini.readNumEntry(QString("UBICACION"), QString("28_Y"), 2);
  int Y29 = ini.readNumEntry(QString("UBICACION"), QString("29_Y"), 2);
  int Y30 = ini.readNumEntry(QString("UBICACION"), QString("30_Y"), 2);
  int Y31 = ini.readNumEntry(QString("UBICACION"), QString("31_Y"), 2);
  int Y32 = ini.readNumEntry(QString("UBICACION"), QString("32_Y"), 2);
  int Y33 = ini.readNumEntry(QString("UBICACION"), QString("33_Y"), 2);
  int Y34 = ini.readNumEntry(QString("UBICACION"), QString("34_Y"), 2);
  int Y35 = ini.readNumEntry(QString("UBICACION"), QString("35_Y"), 2);
  int Y36 = ini.readNumEntry(QString("UBICACION"), QString("36_Y"), 2);

  /*************/


  if(!flag_loading)
    {
	QPixmap	pix;
	pix=myimg;
	QRect ur(0,0,240,320);
	ur=pe->rect();
	QPainter p(&pix);
	p.begin(&pix);
	QPainter p2(&pix);
	p2.begin(&pix);

	QPen pen(QColor(255,0,0) ); p.setPen(pen);
	QPen pen2(QColor(255,255,255) ); p2.setPen(pen2);

	if ( current == 1 ) { p.drawRect(X1, Y1, 40, 27); } 
	if ( current == 2 ) { p.drawRect(X2, Y2, 40, 27); } 
	if ( current == 3 ) { p.drawRect(X3, Y3, 40, 27); } 
	if ( current == 4 ) { p.drawRect(X4, Y4, 40, 27); } 
	if ( current == 5 ) { p.drawRect(X5, Y5, 40, 27); } 
	if ( current == 6 ) { p.drawRect(X6, Y6, 40, 27); } 
	if ( current == 7 ) { p.drawRect(X7, Y7, 40, 27); } 
	if ( current == 8 ) { p.drawRect(X8, Y8, 40, 27); } 
	if ( current == 9 ) { p.drawRect(X9, Y9, 40, 27); } 
	if ( current == 10 ) { p.drawRect(X10, Y10, 40, 27); } 
	if ( current == 11 ) { p.drawRect(X11, Y11, 40, 27); } 
	if ( current == 12 ) { p.drawRect(X12, Y12, 40, 27); } 
	if ( current == 13 ) { p.drawRect(X13, Y13, 40, 27); } 
	if ( current == 14 ) { p.drawRect(X14, Y14, 40, 27); } 
	if ( current == 15 ) { p.drawRect(X15, Y15, 40, 27); } 
	if ( current == 16 ) { p.drawRect(X16, Y16, 40, 27); } 
	if ( current == 17 ) { p.drawRect(X17, Y17, 40, 27); } 
	if ( current == 18 ) { p.drawRect(X18, Y18, 40, 27); } 
	if ( current == 19 ) { p.drawRect(X19, Y19, 40, 27); } 
	if ( current == 20 ) { p.drawRect(X20, Y20, 40, 27); } 
	if ( current == 21 ) { p.drawRect(X21, Y21, 40, 27); } 
	if ( current == 22 ) { p.drawRect(X22, Y22, 40, 27); } 
	if ( current == 23 ) { p.drawRect(X23, Y23, 40, 27); } 
	if ( current == 24 ) { p.drawRect(X24, Y24, 40, 27); } 
	if ( current == 25 ) { p.drawRect(X25, Y25, 40, 27); } 
	if ( current == 26 ) { p.drawRect(X26, Y26, 40, 27); } 
	if ( current == 27 ) { p.drawRect(X27, Y27, 40, 27); } 
	if ( current == 28 ) { p.drawRect(X28, Y28, 40, 27); } 
	if ( current == 29 ) { p.drawRect(X29, Y29, 40, 27); } 
	if ( current == 30 ) { p.drawRect(X30, Y30, 40, 27); } 
	if ( current == 31 ) { p.drawRect(X31, Y31, 40, 27); } 
	if ( current == 32 ) { p.drawRect(X32, Y32, 40, 27); } 
	if ( current == 33 ) { p.drawRect(X33, Y33, 40, 27); } 
	if ( current == 34 ) { p.drawRect(X34, Y34, 40, 27); } 
	if ( current == 35 ) { p.drawRect(X35, Y35, 40, 27); } 
	if ( current == 36 ) { p.drawRect(X36, Y36, 40, 27); } 

	if ( calcular==1 ) 
	{
		int valx; lista.clear();
		valx=1; Probabilidad(valx);
		valx=2; Probabilidad(valx);
		valx=4; Probabilidad(valx);
		valx=5; Probabilidad(valx);
		valx=7; Probabilidad(valx);
		valx=8; Probabilidad(valx);
		valx=10; Probabilidad(valx);
		valx=11; Probabilidad(valx);
		valx=13; Probabilidad(valx);
		valx=14; Probabilidad(valx);
		valx=16; Probabilidad(valx);
		valx=17; Probabilidad(valx);
		valx=19; Probabilidad(valx);
		valx=20; Probabilidad(valx);
		valx=22; Probabilidad(valx);
		valx=23; Probabilidad(valx);
		valx=25; Probabilidad(valx);
		valx=26; Probabilidad(valx);
		valx=28; Probabilidad(valx);
		valx=29; Probabilidad(valx);
		valx=31; Probabilidad(valx);
		valx=32; Probabilidad(valx);
		calcular=0;
	}

	int i=1; int j=1; QString stop="NO"; QString numero;
	while ( j<36 )
	{
		update();
		int number = Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(j), 0 ); numero=""; for(int k=0;k<number;++k) { numero+=QString("."); }
		if ( number != 0 ) { p2.drawText(22, (25*i)+i*2/3+3, QString("%1").arg(numero), 50, QPainter::NONE); }
		j++;
		number = Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(j), 0 ); numero=""; for(int k=0;k<number;++k) { numero+=QString("."); }
		if ( number != 0 ) { p2.drawText(61, (25*i)+i*2/3+3, QString("%1").arg(numero), 50, QPainter::NONE); }
		j++;
		number = Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(j), 0 ); numero=""; for(int k=0;k<number;++k) { numero+=QString("."); }
		if ( number != 0 ) { p2.drawText(99, (25*i)+i*2/3+3, QString("%1").arg(numero), 50, QPainter::NONE); }
		i++; j++;
		if ( j==36 ) { stop="YES"; }
	}
	
	QString pr1,pr2,pr3,pr4; int cp=1;
	for ( int j=0 ; j < lista.count(); ++j ) { 
	  if ( cp <= 4 ) { pr1 += QString("%1 ").arg(lista[j]); }
	  else if ( (cp>=5) && (cp<=8) ) { pr2 += QString("%1 ").arg(lista[j]); }
	  else if ( (cp>=9) && (cp<=12) ) { pr3 += QString("%1 ").arg(lista[j]); }
	  else if ( (cp>=13) && (cp<=16) ) { pr4 += QString("%1 ").arg(lista[j]); }
	  ++cp;
	}
	p2.drawText(136, 240, QString("%1").arg(pr1), 50, QPainter::NONE);
	p2.drawText(136, 265, QString("%1").arg(pr2), 50, QPainter::NONE);
	p2.drawText(136, 290, QString("%1").arg(pr3), 50, QPainter::NONE);
	p2.drawText(136, 315, QString("%1").arg(pr4), 50, QPainter::NONE);

	jugadas=0;
	for ( int k=1 ; k < 37; ++k ) { int number = Num.readNumEntry(QString("NUMEROS"), QString("%1").arg(k), 0 ); jugadas +=number; }
	p2.drawText(136, 165, QString("%1").arg(jugadas), 50, QPainter::NONE);

	p.end();
	p2.end();


	setWFlags(getWFlags() | Qt::WRepaintNoErase);
	bitBlt(   this,  start_x , start_y  ,  &pix,   0,   0,   -1,   -1   ); 
    }
	QWidget::paintEvent( pe);
}


void MyZKbMain::exit()
{

//system("cp start.cfg numeros.cfg");


qApp->quit();

}

