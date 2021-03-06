// edit_group.cpp
//
// Edit a Rivendell Group
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <rddb.h>
#include <qcolordialog.h>

#include <edit_group.h>
#include <rduser.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

EditGroup::EditGroup(QString group,QWidget *parent)
  : QDialog(parent,"",true)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  group_group=new RDGroup(group);

  setCaption(tr("Group: ")+group);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Group Name
  //
  group_name_edit=new QLineEdit(this);
  group_name_edit->setMaxLength(10);
  group_name_edit->setReadOnly(true);
  group_name_label=new QLabel(group_name_edit,tr("&Group Name:"),this);
  group_name_label->setFont(font);
  group_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Group Description
  //
  group_description_edit=new QLineEdit(this);
  group_description_edit->setMaxLength(255);
  group_description_edit->setValidator(validator);
  group_description_label=
    new QLabel(group_description_edit,tr("Group &Description:"),this);
  group_description_label->setFont(font);
  group_description_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Title
  //
  group_title_edit=new QLineEdit(this);
  group_title_edit->setMaxLength(255);
  group_title_edit->setValidator(validator);
  group_title_label=
    new QLabel(group_title_edit,tr("Default Import &Title:"),this);
  group_title_label->setFont(font);
  group_title_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Cart Type
  //
  group_carttype_box=new QComboBox(this);
  group_carttype_box->insertItem(tr("Audio"));
  group_carttype_box->insertItem(tr("Macro"));
  group_carttype_label=
    new QLabel(group_carttype_box,tr("Default Cart &Type:"),this);
  group_carttype_label->setFont(font);
  group_carttype_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Cart Numbers
  //
  group_lowcart_box=new QSpinBox(this);
  group_lowcart_box->setRange(0,999999);
  group_lowcart_box->setSpecialValueText(tr("None"));
  group_cartrange_label=
    new QLabel(group_lowcart_box,tr("Default Cart Number:"),this);
  group_cartrange_label->setFont(font);
  group_cartrange_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  group_highcart_box=new QSpinBox(this);
  group_highcart_box->setRange(1,999999);
  group_highcart_label=
    new QLabel(group_highcart_box,tr("to"),this);
  group_highcart_label->setFont(font);
  group_highcart_label->setAlignment(AlignCenter|ShowPrefix);
  connect(group_lowcart_box,SIGNAL(valueChanged(int)),
	  this,SLOT(lowCartChangedData(int)));

  //
  // Enforce Cart Range Checkbox
  //
  group_enforcerange_box=new QCheckBox(this);
  group_enforcerange_label=
    new QLabel(group_enforcerange_box,tr("Enforce Cart Range"),this);
  group_enforcerange_label->setFont(font);
  group_enforcerange_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Traffic Report Checkbox
  //
  group_traffic_check=new QCheckBox(this);
  group_traffic_label=
    new QLabel(group_traffic_check,tr("Include this group in Traffic reports"),
	       this);
  group_traffic_label->setFont(font);
  group_traffic_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Music Report Checkbox
  //
  group_music_check=new QCheckBox(this);
  group_music_label=new QLabel(group_music_check,tr("Include this group in Music reports"),
		   this);
  group_music_label->setFont(font);
  group_music_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Default Cut End DateTime
  //
  group_cutlife_check=new QCheckBox(this);
  connect(group_cutlife_check,SIGNAL(toggled(bool)),
	  this,SLOT(cutLifeEnabledData(bool)));
  group_cutlife_spin=new QSpinBox(this);
  group_cutlife_spin->setRange(0,999);
  group_cutlife_label=
    new QLabel(group_cutlife_check,tr("Set End Date/Time to"),this);
  group_cutlife_label->setFont(font);
  group_cutlife_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  group_cutlife_unit=
    new QLabel(group_cutlife_check,tr("days after cut creation"),this);
  group_cutlife_unit->setFont(font);
  group_cutlife_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Cut Auto Purging
  //
  group_shelflife_check=new QCheckBox(this);
  connect(group_shelflife_check,SIGNAL(toggled(bool)),
	  this,SLOT(purgeEnabledData(bool)));
  group_shelflife_spin=new QSpinBox(this);
  group_shelflife_spin->setRange(0,30);
  group_shelflife_label=
    new QLabel(group_shelflife_check,tr("Purge expired cuts after"),this);
  group_shelflife_label->setFont(font);
  group_shelflife_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  group_shelflife_unit=
    new QLabel(group_shelflife_check,tr("days"),this);
  group_shelflife_unit->setFont(font);
  group_shelflife_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  group_delete_carts_check=new QCheckBox(this);
  group_delete_carts_label=
    new QLabel(group_delete_carts_check,tr("Delete cart if empty"),this);
  group_delete_carts_label->setFont(font);
  group_delete_carts_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Now & Next Data Checkbox
  //
  group_nownext_check=new QCheckBox(this);
  group_nownext_label=
    new QLabel(group_nownext_check,tr("Transmit Now && Next data"),this);
  group_nownext_label->setFont(font);
  group_nownext_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Services Selector
  //
  group_svcs_sel=new RDListSelector(this);

  //
  //  Color Button
  //
  group_color_button=new QPushButton(this);
  group_color_button->setFont(font);
  group_color_button->setText(tr("C&olor"));
  connect(group_color_button,SIGNAL(clicked()),this,SLOT(colorData()));

  //
  //  Ok Button
  //
  group_ok_button=new QPushButton(this);
  group_ok_button->setDefault(true);
  group_ok_button->setFont(font);
  group_ok_button->setText(tr("&OK"));
  connect(group_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  group_cancel_button=new QPushButton(this);
  group_cancel_button->setFont(font);
  group_cancel_button->setText(tr("&Cancel"));
  connect(group_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  group_name_edit->setText(group_group->name());
  group_description_edit->setText(group_group->description());
  group_title_edit->setText(group_group->defaultTitle());
  group_carttype_box->setCurrentItem(group_group->defaultCartType()-1);
  group_lowcart_box->setValue(group_group->defaultLowCart());
  group_highcart_box->setValue(group_group->defaultHighCart());
  lowCartChangedData(group_group->defaultLowCart());
  group_enforcerange_box->setChecked(group_group->enforceCartRange());
  group_traffic_check->setChecked(group_group->exportReport(RDGroup::Traffic));
  group_music_check->setChecked(group_group->exportReport(RDGroup::Music));
  if(group_group->defaultCutLife()>=0) {
    group_cutlife_spin->setValue(group_group->defaultCutLife());
    group_cutlife_check->setChecked(true);
  }
  cutLifeEnabledData(group_cutlife_check->isChecked());
  if(group_group->cutShelflife()>=0) {
    group_shelflife_spin->setValue(group_group->cutShelflife());
    group_shelflife_check->setChecked(true);
    group_delete_carts_check->setChecked(group_group->deleteEmptyCarts());
  }
  purgeEnabledData(group_shelflife_check->isChecked());
  group_nownext_check->setChecked(group_group->enableNowNext());
  sql=QString().sprintf("select SERVICE_NAME from AUDIO_PERMS \
                         where GROUP_NAME=\"%s\"",
			(const char *)group_group->name());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    group_svcs_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select NAME from SERVICES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(group_svcs_sel->destFindItem(q->value(0).toString())==0) {
      group_svcs_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
  SetButtonColor(group_group->color());
}


EditGroup::~EditGroup()
{
  delete group_name_edit;
  delete group_description_edit;
  delete group_svcs_sel;
}


QSize EditGroup::sizeHint() const
{
  return QSize(400,493);
} 


QSizePolicy EditGroup::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditGroup::lowCartChangedData(int value)
{
  if(value==0) {
    group_highcart_box->setRange(0,0);
    group_highcart_label->setDisabled(true);
    group_highcart_box->setDisabled(true);
    group_enforcerange_box->setDisabled(true);
    group_enforcerange_label->setDisabled(true);
  }
  else {
    group_highcart_label->setEnabled(true);
    group_highcart_box->setEnabled(true);
    group_highcart_box->setRange(value,999999); 
    group_enforcerange_box->setEnabled(true);
    group_enforcerange_label->setEnabled(true);
 }
}


void EditGroup::colorData()
{
  QColor color=
    QColorDialog::getColor(group_color_button->palette().color(QPalette::Active,
			   QColorGroup::ButtonText),this);
  if(color.isValid()) {
    SetButtonColor(color);
  }
}


void EditGroup::cutLifeEnabledData(bool state)
{
  group_cutlife_spin->setEnabled(state);
  group_cutlife_unit->setEnabled(state);
}


void EditGroup::purgeEnabledData(bool state)
{
  group_shelflife_spin->setEnabled(state);
  group_shelflife_unit->setEnabled(state);
  group_delete_carts_check->setEnabled(state);
  group_delete_carts_label->setEnabled(state);
}


void EditGroup::okData()
{
  RDSqlQuery *q;
  QString sql;

  if(!CheckRange()) {
    return;
  }

  group_group->setDescription(group_description_edit->text());
  group_group->setDefaultTitle(group_title_edit->text());
  group_group->
    setDefaultCartType((RDCart::Type)(group_carttype_box->currentItem()+1));
  group_group->setDefaultLowCart(group_lowcart_box->value());
  group_group->setDefaultHighCart(group_highcart_box->value());
  if(group_lowcart_box->value()==0) {
    group_group->setEnforceCartRange(false);
  }
  else {
    group_group->setEnforceCartRange(group_enforcerange_box->isChecked());
  }
  group_group->
    setExportReport(RDGroup::Traffic,group_traffic_check->isChecked());
  group_group->setExportReport(RDGroup::Music,group_music_check->isChecked());
  if(group_cutlife_check->isChecked()) {
    group_group->setDefaultCutLife(group_cutlife_spin->value());
  }
  else {
    group_group->setDefaultCutLife(-1);
  }
  if(group_shelflife_check->isChecked()) {
    group_group->setCutShelflife(group_shelflife_spin->value());
    group_group->setDeleteEmptyCarts(group_delete_carts_check->isChecked());
  }
  else {
    group_group->setCutShelflife(-1);
    group_group->setDeleteEmptyCarts(false);
  }
  group_group->setEnableNowNext(group_nownext_check->isChecked());
  group_group->setColor(group_color_button->
			palette().color(QPalette::Active,
					QColorGroup::ButtonText));

  //
  // Add New Services
  //
  for(unsigned i=0;i<group_svcs_sel->destCount();i++) {
    sql=QString().sprintf("select SERVICE_NAME from AUDIO_PERMS \
where GROUP_NAME=\"%s\" && SERVICE_NAME=\"%s\"",
			  (const char *)group_group->name(),
			  (const char *)group_svcs_sel->destText(i));
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString().sprintf("insert into AUDIO_PERMS (GROUP_NAME,SERVICE_NAME) \
values (\"%s\",\"%s\")",
			    (const char *)group_group->name(),
			    (const char *)group_svcs_sel->destText(i));
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Services
  //
  sql=QString().sprintf("delete from AUDIO_PERMS where GROUP_NAME=\"%s\"",
			(const char *)group_group->name());
  for(unsigned i=0;i<group_svcs_sel->destCount();i++) {
    sql+=QString().sprintf(" && SERVICE_NAME<>\"%s\"",
			   (const char *)group_svcs_sel->destText(i));
  }
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditGroup::cancelData()
{
  done(-1);
}


void EditGroup::resizeEvent(QResizeEvent *e)
{
  group_name_edit->setGeometry(165,11,100,19);
  group_name_label->setGeometry(10,11,150,19);

  group_description_edit->setGeometry(165,32,size().width()-175,19);
  group_description_label->setGeometry(10,32,150,19);

  group_title_edit->setGeometry(165,53,size().width()-175,19);
  group_title_label->setGeometry(10,53,150,19);

  group_carttype_box->setGeometry(165,74,100,19);
  group_carttype_label->setGeometry(10,74,150,19);

  group_lowcart_box->setGeometry(165,95,70,19);
  group_cartrange_label->setGeometry(10,95,150,19);
  group_highcart_box->setGeometry(265,95,70,19);
  group_highcart_label->setGeometry(240,95,20,19);

  group_enforcerange_box->setGeometry(20,118,15,15);
  group_enforcerange_label->setGeometry(40,118,size().width()-50,19);

  group_traffic_check->setGeometry(20,145,15,15);
  group_traffic_label->setGeometry(40,143,size().width()-50,19);

  group_music_check->setGeometry(20,166,15,15);
  group_music_label->setGeometry(40,164,size().width()-50,19);

  group_cutlife_check->setGeometry(20,193,15,15);
  group_cutlife_label->setGeometry(40,193,140,19);
  group_cutlife_spin->setGeometry(185,191,45,19);
  group_cutlife_unit->setGeometry(245,193,size().width()-245,19);

  group_shelflife_check->setGeometry(20,214,15,15);
  group_shelflife_spin->setGeometry(200,212,40,19);
  group_shelflife_label->setGeometry(40,214,160,19);
  group_shelflife_unit->setGeometry(250,214,50,19);

  group_delete_carts_check->setGeometry(40,235,15,15);
  group_delete_carts_label->setGeometry(60,235,160,19);

  group_nownext_check->setGeometry(20,263,15,15);
  group_nownext_label->setGeometry(40,262,size().width()-50,19);

  group_svcs_sel->setGeometry(10,261,401,130);

  group_color_button->setGeometry(10,size().height()-60,80,50);

  group_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  group_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


bool EditGroup::CheckRange()
{
  if(group_lowcart_box->value()==0) {
    return true;
  }

  bool conflict_found=false;
  QString sql;
  RDSqlQuery *q;
  QString msg=
    tr("The selected cart range conflicts with the following groups:\n\n");

  sql=QString().sprintf("select NAME,DEFAULT_LOW_CART,DEFAULT_HIGH_CART\
                         from GROUPS where NAME!=\"%s\"",
			(const char *)group_name_edit->text());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(((group_lowcart_box->value()<=q->value(1).toInt())&&
	(group_highcart_box->value()>=q->value(1).toInt()))||
       ((group_lowcart_box->value()<=q->value(2).toInt())&&
	(group_highcart_box->value()>=q->value(2).toInt()))) {
      msg+=QString().sprintf("    %s\n",(const char *)q->value(0).toString());
      conflict_found=true;
    }
  }
  delete q;

  if(conflict_found) {
    msg+=tr("\nDo you still want to save?");
    switch(QMessageBox::warning(this,"Conflict Found",msg,
				QMessageBox::Yes,QMessageBox::No)) {
	case QMessageBox::No:
	case QMessageBox::NoButton:
	  return false;

	default:
	  break;
    }
  }
  return true;
}


void EditGroup::SetButtonColor(const QColor &color)
{
  QPalette p=group_color_button->palette();
  p.setColor(QColorGroup::ButtonText,color);
  group_color_button->setPalette(p);
}
