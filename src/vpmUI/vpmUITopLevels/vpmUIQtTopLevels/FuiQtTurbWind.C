// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtTurbWind.H"
#include "vpmUI/Fui.H"
#include "vpmUI/Pixmaps/turbWind.xpm"

extern const char* info_xpm[];

#include <QStandardPaths>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>


FuiTurbWind* FuiTurbWind::create(int xpos, int ypos, int width, int height,
                                 const char* title, const char* name)
{
  return new FuiQtTurbWind(xpos,ypos,width,height,title,name);
}


FuiQtTurbWind::FuiQtTurbWind(int xpos, int ypos, int width, int height,
                             const char* title, const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name), pProcess(NULL)
{
  QLabel* label = new QLabel(this);
  label->setObjectName("labTopImg");
  label->setGeometry(0, -2, 581, 111);
  label->setFrameShape(QFrame::NoFrame);
  label->setPixmap(QPixmap(turbWind_xpm));

  QGroupBox* frmTurbSpc = new QGroupBox(this);
  frmTurbSpc->setObjectName("frmTurbSpc");
  frmTurbSpc->setGeometry(10, 120, 256, 321);

  label = new QLabel("Hub height",frmTurbSpc);
  label->setObjectName("labHubHeight");
  label->setGeometry(10, 28, 81, 16);

  editHubHeight = new QLineEdit(frmTurbSpc);
  editHubHeight->setObjectName("editHubHeight");
  editHubHeight->setGeometry(100, 25, 126, 20);
  editHubHeight->setMaxLength(20);

  label = new QLabel("Grid height",frmTurbSpc);
  label->setObjectName("labGridHeight");
  label->setGeometry(10, 53, 71, 16);

  editGridHeight = new QLineEdit(frmTurbSpc);
  editGridHeight->setObjectName("editGridHeight");
  editGridHeight->setGeometry(100, 50, 126, 20);
  editGridHeight->setMaxLength(20);

  label = new QLabel("Grid width",frmTurbSpc);
  label->setObjectName("labGridWidth");
  label->setGeometry(10, 78, 71, 16);

  editGridWidth = new QLineEdit(frmTurbSpc);
  editGridWidth->setObjectName("editGridWidth");
  editGridWidth->setGeometry(100, 75, 126, 20);
  editGridWidth->setMaxLength(20);

  label = new QLabel("Num. grid Y",frmTurbSpc);
  label->setObjectName("labNumGridZ");
  label->setGeometry(10, 103, 71, 16);

  editNumGridZ = new QLineEdit(frmTurbSpc);
  editNumGridZ->setObjectName("editNumGridZ");
  editNumGridZ->setGeometry(100, 100, 126, 20);
  editNumGridZ->setMaxLength(20);

  label = new QLabel("Num. grid Z",frmTurbSpc);
  label->setObjectName("labNumGridY");
  label->setGeometry(10, 128, 71, 16);

  editNumGridY = new QLineEdit(frmTurbSpc);
  editNumGridY->setObjectName("editNumGridY");
  editNumGridY->setGeometry(100, 125, 126, 20);
  editNumGridY->setMaxLength(20);

  label = new QLabel("Time step",frmTurbSpc);
  label->setObjectName("labTimeStep");
  label->setGeometry(10, 153, 71, 16);

  editTimeStep = new QLineEdit(frmTurbSpc);
  editTimeStep->setObjectName("editTimeStep");
  editTimeStep->setGeometry(100, 150, 126, 20);
  editTimeStep->setMaxLength(20);

  label = new QLabel("Duration",frmTurbSpc);
  label->setObjectName("labDuration");
  label->setGeometry(10, 178, 71, 16);

  editDuration = new QLineEdit(frmTurbSpc);
  editDuration->setObjectName("editDuration");
  editDuration->setGeometry(100, 175, 126, 20);
  editDuration->setMaxLength(20);

  label = new QLabel("IEC turbine class",frmTurbSpc);
  label->setObjectName("labIECturbc");
  label->setGeometry(10, 203, 80, 16);

  cmbIECturbc = new QComboBox(frmTurbSpc);
  cmbIECturbc->setObjectName("cmbIECturbc");
  cmbIECturbc->setGeometry(100, 200, 126, 22);
  cmbIECturbc->setEditable(true);

  label = new QLabel(frmTurbSpc);
  label->setObjectName("labInfoA1");
  label->setGeometry(10, 229, 21, 16);
  label->setPixmap(QPixmap(info_xpm));
  label = new QLabel(frmTurbSpc);
  label->setObjectName("labInfoA2");
  label->setGeometry(30, 231, 46, 13);
  label->setText("<b>Note</b>");

  QLabel* labInfoA3 = new QLabel(frmTurbSpc);
  labInfoA3->setObjectName("labInfoA3");
  labInfoA3->setGeometry(10, 244, 231, 65);
  labInfoA3->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
  labInfoA3->setWordWrap(true);

  QGroupBox* frmGenerate = new QGroupBox(this);
  frmGenerate->setObjectName("frmGenerate");
  frmGenerate->setGeometry(279, 120, 291, 208);

  label = new QLabel("Template file",frmGenerate);
  label->setObjectName("labTemplateFile");
  label->setGeometry(10, 28, 71, 16);

  editTemplateFile = new QLineEdit(frmGenerate);
  editTemplateFile->setObjectName("editTemplateFile");
  editTemplateFile->setGeometry(100, 25, 156, 20);
  btnTemplateFile = new QPushButton(frmGenerate);
  btnTemplateFile->setObjectName("btnTemplateFile");
  btnTemplateFile->setGeometry(260, 23, 21, 23);

  label = new QLabel("Output folder",frmGenerate);
  label->setObjectName("labOutputFolder");
  label->setGeometry(10, 53, 71, 16);

  editOutputFolder = new QLineEdit(frmGenerate);
  editOutputFolder->setObjectName("editOutputFolder");
  editOutputFolder->setGeometry(100, 50, 156, 20);
  btnOutputFolder = new QPushButton(frmGenerate);
  btnOutputFolder->setObjectName("btnOutputFolder");
  btnOutputFolder->setGeometry(260, 48, 21, 23);

  label = new QLabel("Turbulence type",frmGenerate);
  label->setObjectName("labWindType");
  label->setGeometry(10, 78, 86, 16);

  cmbWindType = new QComboBox(frmGenerate);
  cmbWindType->setObjectName("cmbWindType");
  cmbWindType->setGeometry(100, 75, 156, 22);

  label = new QLabel("Wind speed",frmGenerate);
  label->setObjectName("labWindSpeed");
  label->setGeometry(10, 108, 71, 16);

  editWindSpeed = new QLineEdit(frmGenerate);
  editWindSpeed->setObjectName("editWindSpeed");
  editWindSpeed->setGeometry(100, 105, 56, 20);

  label = new QLabel("PLExp",frmGenerate);
  label->setObjectName("labPLExp");
  label->setGeometry(168, 108, 30, 16);

  editPLExp = new QLineEdit(frmGenerate);
  editPLExp->setObjectName("editPLExp");
  editPLExp->setGeometry(200, 105, 56, 20);

  label = new QLabel("Ref. height",frmGenerate);
  label->setObjectName("labRefHt");
  label->setGeometry(10, 133, 71, 16);

  editRefHt = new QLineEdit(frmGenerate);
  editRefHt->setObjectName("editRefHt");
  editRefHt->setGeometry(100, 130, 156, 20);
  editRefHt->setMaxLength(20);

  chkTowerPoints = new QCheckBox(frmGenerate);
  chkTowerPoints->setObjectName("chkTowerPoints");
  chkTowerPoints->setGeometry(100, 155, 156, 20);

  label = new QLabel("Random seed 1",frmGenerate);
  label->setObjectName("labRandSeed1");
  label->setGeometry(10, 183, 75, 16);

  editRandSeed1 = new QLineEdit(frmGenerate);
  editRandSeed1->setObjectName("editRandSeed1");
  editRandSeed1->setGeometry(100, 180, 156, 20);

  btnGenerate = new QPushButton(this);
  btnGenerate->setObjectName("btnGenerate");
  btnGenerate->setGeometry(280, 410, 91, 26);
  btnClose = new QPushButton(this);
  btnClose->setObjectName("btnClose");
  btnClose->setGeometry(380, 410, 91, 26);
  btnHelp = new QPushButton(this);
  btnHelp->setObjectName("btnHelp");
  btnHelp->setGeometry(480, 410, 91, 26);

  label = new QLabel(this);
  label->setObjectName("labInfoB1");
  label->setGeometry(280, 332, 21, 16);
  label->setPixmap(QPixmap(info_xpm));
  label = new QLabel(this);
  label->setObjectName("labInfoB2");
  label->setGeometry(300, 334, 46, 13);
  label->setText("<b>Note</b>");

  QLabel* labInfoB3 = new QLabel(this);
  labInfoB3->setObjectName("labInfoB3");
  labInfoB3->setGeometry(280, 347, 301, 65);
  labInfoB3->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
  labInfoB3->setWordWrap(false);

  textEdit = new QTextEdit(this);
  textEdit->setObjectName("textEdit");
  textEdit->setGeometry(10, 445, 561, 316);

  QWidget::setTabOrder(editHubHeight, editGridHeight);
  QWidget::setTabOrder(editGridHeight, editGridWidth);
  QWidget::setTabOrder(editGridWidth, editNumGridZ);
  QWidget::setTabOrder(editNumGridZ, editNumGridY);
  QWidget::setTabOrder(editNumGridY, editTimeStep);
  QWidget::setTabOrder(editTimeStep, editDuration);
  QWidget::setTabOrder(editDuration, cmbIECturbc);
  QWidget::setTabOrder(cmbIECturbc, editTemplateFile);
  QWidget::setTabOrder(editTemplateFile, btnTemplateFile);
  QWidget::setTabOrder(btnTemplateFile, editOutputFolder);
  QWidget::setTabOrder(editOutputFolder, btnOutputFolder);
  QWidget::setTabOrder(btnOutputFolder, cmbWindType);
  QWidget::setTabOrder(cmbWindType, editWindSpeed);
  QWidget::setTabOrder(editWindSpeed, editPLExp);
  QWidget::setTabOrder(editPLExp, editRefHt);
  QWidget::setTabOrder(editRefHt, chkTowerPoints);
  QWidget::setTabOrder(chkTowerPoints, editRandSeed1);
  QWidget::setTabOrder(editRandSeed1, btnGenerate);
  QWidget::setTabOrder(btnGenerate, btnClose);
  QWidget::setTabOrder(btnClose, btnHelp);
  QWidget::setTabOrder(btnHelp, textEdit);

  frmTurbSpc->setTitle("Turbine Specification for Wind");
  editHubHeight->setText("0.0");
  editGridHeight->setText("0.0");
  editGridWidth->setText("0.0");

  editNumGridY->setText("0");
  editNumGridZ->setText("0");
  editTimeStep->setText("0.0");
  editDuration->setText("0.0");
  cmbIECturbc->clear();
  cmbIECturbc->insertItems(0, { "A", "B", "C" });
  editRefHt->setText("0.0");
  labInfoA3->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">The 'Hub height' value is extracted from the Fedem model. You will typically not modify this value. The 'Ref. height' value is initially set to the same value as 'Hub height,' but you may wish to adjust this value.</span></p></body></html>");
  frmGenerate->setTitle("Generate Wind File");
  cmbWindType->clear();
  cmbWindType->insertItems(0,{ "NTM",
                               "ETM - class 1 turbine",
                               "ETM - class 2 turbine",
                               "ETM - class 3 turbine" });
  editWindSpeed->setText("0.0");
  editPLExp->setText("0.11");
  chkTowerPoints->setText("Include tower points");
  editRandSeed1->setText("1234567");
  btnTemplateFile->setText("...");
  btnOutputFolder->setText("...");
  btnGenerate->setText("Generate");
  btnClose->setText("Close");
  btnHelp->setText("Help");
  labInfoB3->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Please fill in the fields above before generating a wind file.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">- The template file is copied and filled in by this tool.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">- The output folder receives all output from the tool.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">- Additional parameters are available in the template file.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>");

  // Set up slots
  connect(btnTemplateFile, SIGNAL(clicked()), this, SLOT(onBtnTemplateFileClick()));
  connect(btnOutputFolder, SIGNAL(clicked()), this, SLOT(onBtnOutputFolderClick()));
  connect(btnGenerate, SIGNAL(clicked()), this, SLOT(onBtnGenerateClick()));
  connect(btnClose, SIGNAL(clicked()), this, SLOT(onBtnCloseClick()));
  connect(btnHelp, SIGNAL(clicked()), this, SLOT(onBtnHelpClick()));

  // Set up tooltips
  editHubHeight->setToolTip("The wind turbine hub height");
  editGridHeight->setToolTip("The height of the wind field grid");
  editGridWidth->setToolTip("The width of the wind field grid");
  editNumGridZ->setToolTip("Vertical grid-point matrix dimension");
  editNumGridY->setToolTip("Horizontal grid-point matrix dimension");
  editTimeStep->setToolTip("Time step");
  editDuration->setToolTip("Analysis and usable time duration");
  cmbIECturbc->setToolTip("IEC turbulence characteristics");
  editTemplateFile->setToolTip("Specifies the TurbSim INP-file");
  editOutputFolder->setToolTip("Specifies the target folder for output data");
  cmbWindType->setToolTip("IEC turbulence type");
  editWindSpeed->setToolTip("Mean wind speed at the reference height");
  editPLExp->setToolTip("Power law exponent");
  editRefHt->setToolTip("Height of the reference wind speed");
  chkTowerPoints->setToolTip("Generate binary tower time series");
  editRandSeed1->setToolTip("First random seed (-2147483648 to 2147483647)");
  btnGenerate->setToolTip("Click to generate wind files");
  btnClose->setToolTip("Click to close this tool");
  btnHelp->setToolTip("Click to view help file");

  this->setMinimumSize(580, 445);
  this->setMaximumSize(580, 770);
}


void FuiQtTurbWind::setValues(double hubHeight, double gridSize,
                              double timeInc, double duration,
                              const char* outputPath)
{
  editHubHeight->setText(QString::number(hubHeight));
  editGridHeight->setText(QString::number(gridSize));
  editGridWidth->setText(QString::number(gridSize));
  editNumGridZ->setText("16");
  editNumGridY->setText("16");
  editTimeStep->setText(QString::number(timeInc));
  editDuration->setText(QString::number(duration));
  cmbIECturbc->setCurrentIndex(1); // "B"
  QString templateFile(templateDir.c_str());
  templateFile.replace(" ","%20");
  templateFile.replace("\\","/");
  templateFile.append("/IecKai.inp");
  editTemplateFile->setText(templateFile);
  QString outputDir(outputPath);
  outputDir.replace(" ","%20");
  outputDir.replace("\\","/");
  editOutputFolder->setText(outputDir);
  cmbWindType->setCurrentIndex(1); // "ETM - class 1 turbine"
  editWindSpeed->setText("18.2");
  editPLExp->setText("0.11");
  editRefHt->setText(QString::number(hubHeight));
  chkTowerPoints->setChecked(true);
  editRandSeed1->setText("1234567");
}


void FuiQtTurbWind::onBtnTemplateFileClick()
{
  // Show file open dialog
  QString fileName = QFileDialog::getOpenFileName(this, "Open File",
                                                  templateDir.c_str(),
                                                  "INP Files (*.inp);;All Files (*.*)");
  fileName.replace('\\','/');
  editTemplateFile->setText(fileName);
}


void FuiQtTurbWind::onBtnOutputFolderClick()
{
  // Get current path
  QString dir1 = editOutputFolder->text();

  // Get user's document directory
  if (dir1.isEmpty())
    dir1 = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

  // Show directory dialog
  QString dir2 = QFileDialog::getExistingDirectory(this, "Open Directory", dir1,
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  // Set field
  dir2.replace('\\', '/');
  editOutputFolder->setText(dir2);
}


static int locateEntry(const QByteArray& line, const char* szName)
{
  int pos1 = line.indexOf(szName);
  if (pos1 < 0) return -1; // Entry not found

  int pos2 = line.indexOf("-",pos1);
  if (pos2 <= pos1) return -2; // Entry is inside the comments section

  return pos1;
}

void setTurboSimLineEntryDouble(QByteArray& line, const char* szName, double value)
{
  int pos = locateEntry(line,szName);
  if (pos < 0) return;

  char sVal[32];
  sprintf(sVal,"%.8f   ",value);
  line.replace(0,pos,sVal);
}

void setTurboSimLineEntryInt(QByteArray& line, const char* szName, int value)
{
  int pos = locateEntry(line,szName);
  if (pos < 0) return;

  char sVal[32];
  sprintf(sVal,"%d   ",value);
  line.replace(0,pos,sVal);
}

void setTurboSimLineEntryStr(QByteArray& line, const char* szName, const QString& value, bool quote = true)
{
  int pos = locateEntry(line,szName);
  if (pos < 0) return;

  QString sVal;
  if (quote)
    sVal = "\"" + value + "\"   ";
  else
    sVal = value + "   ";
  std::string strVal(sVal.toStdString());
  line.replace(0,pos,strVal.c_str());
}


void FuiQtTurbWind::onBtnGenerateClick()
{
  // Get field values
  double fHubHeight = editHubHeight->text().toDouble();
  double fGridHeight = editGridHeight->text().toDouble();
  double fGridWidth = editGridWidth->text().toDouble();
  int    nNumGridZ = editNumGridZ->text().toInt();
  int    nNumGridY = editNumGridY->text().toInt();
  double fTimeStep = editTimeStep->text().toDouble();
  double fDuration = editDuration->text().toDouble();
  QString strIECturbc = cmbIECturbc->currentText();
  QString strTemplateFile = editTemplateFile->text();
  QString strOutputFolder = editOutputFolder->text();
  int    nTurbulenceType = cmbWindType->currentIndex();
  double fWindSpeed = editWindSpeed->text().toDouble();
  double fPLExp = editPLExp->text().toDouble();
  double fRefHt = editRefHt->text().toDouble();
  bool   bTowerPoints = chkTowerPoints->isChecked();
  int    nRandSeed1 = editRandSeed1->text().toInt();

  // Get strTurbulenceType
  QString strTurbulenceType;
  switch (nTurbulenceType) {
  case 0: strTurbulenceType += "NTM"; break;
  case 1: strTurbulenceType += "1ETM"; break;
  case 2: strTurbulenceType += "2ETM"; break;
  case 3: strTurbulenceType += "3ETM"; break;
  }

  // Check fields
  if (fHubHeight <= 0.0) {
    QMessageBox msgBox(QMessageBox::Warning,
                       "Error",
                       "<b>Illegal entry:</b><br>"
                       "Hub height must be greater than 0.",
                       QMessageBox::Ok | QMessageBox::Cancel,
                       this);
    msgBox.exec();
    return;
  }
  else if (fGridHeight <= 0.0 || fGridWidth <= 0.0) {
    QMessageBox msgBox(QMessageBox::Warning,
                       "Error",
                       "<b>Illegal entry:</b><br>"
                       "grid height and width must be greater than 0.",
                       QMessageBox::Ok | QMessageBox::Cancel,
                       this);
    msgBox.exec();
    return;
  }
  else if (strTemplateFile.isEmpty()) {
    QMessageBox msgBox(QMessageBox::Warning, "Error",
                       "<b>Illegal entry:</b><br>"
                       "You must specify a template file.",
                       QMessageBox::Ok | QMessageBox::Cancel,
                       this);
    msgBox.exec();
    return;
  }
  else if (strOutputFolder.isEmpty()) {
    QMessageBox msgBox(QMessageBox::Warning, "Error",
                       "<b>Illegal entry:</b><br>"
                       "You must specify a output folder.",
                       QMessageBox::Ok | QMessageBox::Cancel,
                       this);
    msgBox.exec();
    return;
  }

  // Set output path
  QString strOutputFile = strOutputFolder;
  if (!strOutputFolder.endsWith('/') && !strOutputFolder.endsWith('\\'))
    strOutputFile += "/";
  switch (nTurbulenceType) {
  case 0: strOutputFile += "NTM/"; break;
  case 1: strOutputFile += "1ETM/"; break;
  case 2: strOutputFile += "2ETM/"; break;
  case 3: strOutputFile += "3ETM/"; break;
  }

  // Make output path
  QDir dir;
  dir.mkpath(strOutputFile);

  // Set output file name
  char strFileName[32];
  sprintf(strFileName,"windSpeed-%.2f-rs-%d.inp",fWindSpeed,nRandSeed1);
  strOutputFile += strFileName;

  // Clone template file
  QFile fileIn(strTemplateFile);
  QFile fileOut(strOutputFile);
  if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox msgBox(QMessageBox::Warning, "Error",
                       "Unable to open template file.",
                       QMessageBox::Ok | QMessageBox::Cancel,
                       this);
    msgBox.exec();
    return;
  }
  else if (!fileOut.open(QIODevice::ReadWrite | QIODevice::Text)) {
    QMessageBox msgBox(QMessageBox::Warning, "Error",
                       "Unable to open output file.",
                       QMessageBox::Ok | QMessageBox::Cancel,
                       this);
    msgBox.exec();
    return;
  }
  while (!fileIn.atEnd()) {
    QByteArray line = fileIn.readLine();
    setTurboSimLineEntryDouble(line, "HubHt", fHubHeight);
    setTurboSimLineEntryDouble(line, "GridHeight", fGridHeight);
    setTurboSimLineEntryDouble(line, "GridWidth", fGridWidth);
    setTurboSimLineEntryInt(line, "NumGrid_Z", nNumGridZ);
    setTurboSimLineEntryInt(line, "NumGrid_Y", nNumGridY);
    setTurboSimLineEntryDouble(line, "TimeStep", fTimeStep);
    setTurboSimLineEntryDouble(line, "AnalysisTime", fDuration);
    setTurboSimLineEntryDouble(line, "UsableTime", fDuration);
    setTurboSimLineEntryStr(line, "IECturbc", strIECturbc);
    setTurboSimLineEntryStr(line, "IEC_WindType", strTurbulenceType);
    setTurboSimLineEntryDouble(line, "URef", fWindSpeed);
    setTurboSimLineEntryDouble(line, "PLExp", fPLExp);
    setTurboSimLineEntryDouble(line, "RefHt", fRefHt);
    setTurboSimLineEntryStr(line, "WrADTWR", QString(bTowerPoints ? "True" : "False"), false);
    setTurboSimLineEntryInt(line, "RandSeed1", nRandSeed1);
    fileOut.write(line);
  }
  fileIn.close();
  fileOut.close();

  // Show output field
  this->setMinimumSize(580,770);

  // Start process
  textEdit->append("Generating " + strOutputFile + ".. Please wait..");
  pProcess = new QProcess(this);
  connect(pProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()));
  connect(pProcess, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStandardError()));
  connect(pProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
  pProcess->start(turbsim_exe.c_str(), { strOutputFile });
  btnGenerate->setEnabled(false);
}


void FuiQtTurbWind::onBtnCloseClick()
{
  if (pProcess != NULL)
    pProcess->kill();

  this->close();
}


void FuiQtTurbWind::onBtnHelpClick()
{
  Fui::showCHM("dialogbox/windpower/3d-turbulent-wind.htm");
}


void FuiQtTurbWind::onReadyReadStandardOutput()
{
  QByteArray byteArray = pProcess->readAllStandardOutput();
  QStringList strLines = QString(byteArray).split("\n");
  foreach (QString line, strLines) {
    line.remove('\n');
    line.remove('\r');
    textEdit->append("<font color=#505050>" + line + "</font>");
  }
}


void FuiQtTurbWind::onReadyReadStandardError()
{
  QByteArray byteArray = pProcess->readAllStandardError();
  QStringList strLines = QString(byteArray).split("\n");
  foreach (QString line, strLines) {
    line.remove('\n');
    line.remove('\r');
    textEdit->append("<font color=#505050>" + line + "</font>");
  }
}


void FuiQtTurbWind::onFinished(int status, QProcess::ExitStatus)
{
  btnGenerate->setEnabled(true);
  textEdit->append(status ? "Failed!\n" : "Finished!\n");
}
