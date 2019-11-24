/***************************************************************************
 *   Copyright 2010 by Christoph Thielecke <crissi99@gmx.de>               *
 *                                                                         *
 *   button-green_benji_park_01.svg, button-purple_benji_park_01.svg       *
 *   button-red_benji_park_01.svg and button-seagreen_benji_pa_01.svg      *
 *   from openclipart project                                              *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "virtual_hdd_led.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QSize>
#include <QColor>
#include <QWidget>
#include <KLocale>

#include <plasma/svg.h>
#include <plasma/theme.h>
#include <QGraphicsView>
#include <QDebug>
#include <QSpinBox>
#include <QFont>
#include <QFontMetrics>
#include <QRect>
#include <KConfigDialog>
#include <kcombobox.h>
#include <kcolorbutton.h>
#include <kcolordialog.h>

#include <Plasma/Applet>
#include <Plasma/ToolTipManager>


#include <unistd.h>


virtual_hdd_led::virtual_hdd_led(QObject *parent, const QVariantList &args)
: Plasma::Applet(parent, args),
    m_svg(this),
    m_icon("drive-harddisk")
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    m_svg.setImagePath("widgets/background");
    // setStatus(Plasma::ActiveStatus); // disabled for now
	StatusColor = Qt::lightGray;
	ReadColor = Qt::green;
	WriteColor = Qt::red;
	ReadWriteColor = Qt::darkMagenta;
	IdleColor = Qt::lightGray;
	UnknownColor = Qt::lightGray;
	Status = Unknown;
	diskFound = false;
	use_sysfs = true;
	use_icons = true;
	draw_devicename = true;
	draw_border = true;
	diskname = "sda";
	last_rx = 0;
	last_tx = 0;
	PollInterval = 500; // ms
	setAspectRatioMode(Plasma::ConstrainedSquare );
}


virtual_hdd_led::~virtual_hdd_led()
{
	testtimer.stop();
	//if (id)
	//	fclose(id);
	if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}

void virtual_hdd_led::init()
{
	KGlobal::locale()->insertCatalog("plasma_applet_virtual-hdd-led");
	
	setHasConfigurationInterface(true);
	configChanged();
    if (m_icon.isNull()) {
        setFailedToLaunch(true, i18n("Init failed"));
    }
    if (use_sysfs) {
		QString sysfs_path = "/sys/block/"+diskname+"/stat";
		id = fopen(sysfs_path.toLocal8Bit() , "r" );
	} else {
		id = fopen( "/proc/diskstats", "r" );
	}
	if (id) {
		fclose(id);
		testtimer.setInterval(PollInterval);
		connect ((QObject *)&testtimer, SIGNAL(timeout()), this, SLOT(hddDataUpdate()));
		testtimer.start();
	} else {
		qDebug("cant open proc/sysfs file!");
	}
	toolTipAboutToShow();
	if (use_icons) {
		ReadPixmap = KIcon("virtual_hdd_led-read").pixmap(QSize(22,22));
		WritePixmap = KIcon("virtual_hdd_led-write").pixmap(QSize(22,22));
		ReadWritePixmap = KIcon("virtual_hdd_led-readwrite").pixmap(QSize(22,22));
		IdlePixmap = KIcon("virtual_hdd_led-idle").pixmap(QSize(22,22));
		UnknownPixmap = KIcon("virtual_hdd_led-unknown").pixmap(QSize(22,22));
	}
	resize(22, 22);
}

void virtual_hdd_led::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	Q_UNUSED(option);
	//qDebug("repaint!");
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);
	
	int borderwidth = contentsRect.width()*0.1; // 10%
	//qDebug(QString("contentsRect.width(): "+QString().setNum(contentsRect.width())).toLocal8Bit());
	//qDebug(QString("borderwidth: "+QString().setNum(borderwidth)).toLocal8Bit());
	int x_pos = borderwidth;
	int y_pos = borderwidth;
	int my_width = contentsRect.width()-(borderwidth*2);
	int my_height = contentsRect.height()-(borderwidth*2);
	QRect disknamerect;
	
	//qDebug("StatusColor: "+StatusColor.name().toLocal8Bit());
	if (draw_devicename) {
		QFont myfont("Helvetica", 8);
		QFontMetrics fm(myfont);
		disknamerect = fm.boundingRect(diskname);
		x_pos = x_pos+(disknamerect.width()/2)-(borderwidth);
		my_height = my_height - disknamerect.height()+(borderwidth*2);
		my_width = my_width - disknamerect.width()+(borderwidth*2);
	}
	if  (use_icons) {
		// set the corresponding icon to pixmap pm here
		
		if (draw_devicename) {
			p->drawPixmap(x_pos, y_pos, StatusPixmap.scaled(my_width, my_height), 0,0, my_width, my_height);
			p->save();
			//qDebug(QString("x color: "+QString().setNum(x_pos)+", y color: "+QString().setNum(y_pos)).toLocal8Bit());
			p->drawText((contentsRect.width()/2)-(disknamerect.width()/2)-borderwidth/2, x_pos+my_height+(borderwidth*2), diskname);
			p->restore();
		} else {
			p->drawPixmap(x_pos, y_pos, StatusPixmap, 0,0, my_width, my_height);
		}
	} else {
// 		draw_devicename = false;

		//QPixmap pm( contentsRect.width()-10, contentsRect.height()-10 );
		//pm.fill( StatusColor );
		
		// we draw color
		p->fillRect(x_pos, y_pos, my_width, my_height, StatusColor);
		if (draw_border) {
			p->setPen(Qt::black);
			p->drawRect(x_pos, y_pos, my_width, my_height);
		}
		if (draw_devicename) {
			//qDebug(QString("x color: "+QString().setNum(x_pos)+", y color: "+QString().setNum(y_pos)).toLocal8Bit());
			p->drawText((contentsRect.width()/2)-(disknamerect.width()/2)-borderwidth/2, x_pos+my_height+(borderwidth*2), diskname);
		}
	}
}

void virtual_hdd_led::hddDataUpdate()
{
	char buff[500];
	char devicename_1[10];
	QString devicename;
	unsigned long major, minor, reads_completed, reads_merged, write_merged_bytes, sectors_read, writes_completed, writes_merged, sectors_written, write_ms, progress_io, ms_io, ms_io_weight;
	diskFound = false;
	
	// example line:
	// 42 174 864 284 0 0 0 0 0 256 284
	/*
	 * /usr/src/linux-2.6.34/Documentation/iostats.txt
	 Field  1 -- # of reads completed                                 *
	 This is the total number of reads completed successfully.
	 
	 Field  2 -- # of reads merged,
	 Reads and writes which are adjacent to each other may be merged for
	 efficiency.  Thus two 4K reads may become one 8K read before it is
	 ultimately handed to the disk, and so it will be counted (and queued)
	 as only one I/O.  This field lets you know how often this was done.
	 
	 Field  3 -- # of sectors read
	 This is the total number of sectors read successfully.
	 
	 Field  4 -- # of milliseconds spent reading
	 This is the total number of milliseconds spent by all reads (as
	 measured from __make_request() to end_that_request_last()).
	 
	 Field  5 -- # of writes completed
	 This is the total number of writes completed successfully.
	 
	 Field  6 -- # of writes merged
	 
	 Field  7 -- # of sectors written
	 This is the total number of sectors written successfully.
	 
	 Field  8 -- # of milliseconds spent writing
	 This is the total number of milliseconds spent by all writes (as
	 measured from __make_request() to end_that_request_last()).
	 
	 Field  9 -- # of I/Os currently in progress
	 The only field that should go to zero. Incremented as requests are
	 given to appropriate struct request_queue and decremented as they finish.
	 
	 Field 10 -- # of milliseconds spent doing I/Os
	 This field is increases so long as field 9 is nonzero.
	 
	 Field 11 -- weighted # of milliseconds spent doing I/Os
	 This field is incremented at each I/O start, I/O completion, I/O
	 merge, or read of these stats by the number of I/Os in progress
	 (field 9) times the number of milliseconds spent doing I/O since the
	 last update of this field.  This can provide an easy measure of both
	 I/O completion time and the backlog that may be accumulating.
	 */
	
	if (use_sysfs) {
		QString sysfs_path = "/sys/block/"+diskname+"/stat";
		id = fopen(sysfs_path.toLocal8Bit() , "r" );
		if (id) {
			fgets(buff, sizeof(buff), id);
			if ( sscanf(buff, "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
				&reads_completed, &reads_merged, &write_merged_bytes, &sectors_read, &writes_completed, &writes_merged, &sectors_written, &write_ms, &progress_io, &ms_io, &ms_io_weight) == 11) {
				diskFound  = true;
			} else {
				//qDebug("wrong data line");
			}
			fclose(id);
		} else {
			//qDebug("could not open sysfs file!");
		}
	} else {
		id = fopen( "/proc/diskstats", "r" );
		if (id) {
			//rewind(id);
			while (fgets(buff, sizeof(buff), id) != NULL) {
				// example line:
				// 8       4 sda4 42 174 864 284 0 0 0 0 0 256 284
				/*
				 * same as sysfs but major, minor, diskname prepended
				*/
				if (sscanf(buff, "%lu %lu %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
					&major, &minor, devicename_1, &reads_completed, &reads_merged, &write_merged_bytes, &sectors_read, &writes_completed, &writes_merged, &sectors_written, &write_ms, &progress_io, &ms_io, &ms_io_weight) == 14) {
					devicename = QString(devicename_1);
					if (devicename == diskname) {
						diskFound  = true;
						break;
					}
				} else {
					//qDebug("wrong data line");
				}
			}
			fclose(id);
		}
		else {
			//qDebug("could not open proc file!");
		}
	}
	if (diskFound) {
		/*
		 * reads_completed
		 * reads_merged
		 * write_merged_bytes
		 * sectors_read
		 * writes_completed
		 * writes_merged
		 * sectors_written
		 * write_ms
		 * progress_io
		 * ms_io
		 * ms_io_weight
		 */
		if (last_rx != 0 && last_tx != 0) {
			//qDebug("last_rx and last_tx != 0");
			if( reads_completed != last_rx ) {
				Status = Read;
				//qDebug("read");
			}
			else if ( writes_completed != last_tx ) {
				Status = Write;
				//qDebug("write");
			}
			else if ( reads_completed != last_rx && writes_completed != last_tx ) {
				Status = ReadWrite;
				//qDebug("write");
			}
			if (reads_completed == last_rx && writes_completed == last_tx) {
				Status = Idle;
				//qDebug("write");
			}
			//qDebug("act!");
			//qDebug(QString("status: "+QString().setNum(Status)).toLocal8Bit());
			//qDebug(QString(devicename+" read: "+QString().setNum(read_bytes)+", write: "+QString().setNum(write_bytes)).toLocal8Bit());
		}
		last_rx = reads_completed;
		last_tx = writes_completed;
		if (Status == Read) {
			StatusColor = Qt::green;
			if (use_icons) {
				StatusPixmap = ReadPixmap;
			}
		}
		else if (Status == Write) {
			StatusColor = WriteColor;
			if (use_icons) {
				StatusPixmap = WritePixmap;
			}
		}
		else if (Status == ReadWrite) {
			StatusColor = ReadWriteColor;
			if (use_icons) {
				StatusPixmap = ReadWritePixmap;
			}
		}
		else if (Status == Idle) {
			StatusColor = IdleColor;
			if (use_icons) {
				StatusPixmap = IdlePixmap;
			}
		}
		else {
			StatusColor = UnknownColor;
			if (use_icons) {
				StatusPixmap = UnknownPixmap;
			}
		}
		//qDebug("state change");
		update();
	} else {
		Status = Unknown;
		if (use_icons) {
			StatusPixmap = UnknownPixmap;
		}
		//qDebug("disk not found!");
	}
}

void virtual_hdd_led::configChanged()
{
	KConfigGroup cg = config();
	PollInterval = QVariant(cg.readEntry("PollInterval", 500)).toInt();
	diskname = cg.readEntry("diskname", "sda");
	draw_devicename = QVariant(cg.readEntry("draw_devicename", false)).toBool();
	draw_border = QVariant(cg.readEntry("draw_border", true)).toBool();
	use_icons = QVariant(cg.readEntry("use_icons", false)).toBool();
	WriteColor = QVariant(cg.readEntry("WriteColor", QVariant(Qt::red).toString())).value<QColor>();
	ReadColor = QVariant(cg.readEntry("ReadColor", QVariant(Qt::green).toString())).value<QColor>();
	ReadWriteColor = QVariant(cg.readEntry("ReadWriteColor", QVariant(Qt::darkMagenta).toString())).value<QColor>();
	IdleColor = QVariant(cg.readEntry("IdleColor", QVariant(Qt::lightGray).toString())).value<QColor>();
	UnknownColor = QVariant(cg.readEntry("UnknownColor", QVariant(Qt::lightGray).toString())).value<QColor>();
	//qDebug(QString("PollInterval: "+QString().setNum(PollInterval)).toLocal8Bit());
	//qDebug(QString("diskname: "+diskname).toLocal8Bit());
	update(); // nessary if device name has changed
}

void virtual_hdd_led::configAccepted()
{
	PollInterval = ui.PollIntervalspinBox->value();
	diskname = ui.disknamekcombobox->currentText();
	draw_devicename = ui.draw_devicenamecheckBox->isChecked();
	draw_border = ui.draw_bordercheckBox->isChecked();
	use_icons = ui.use_iconscheckBox->isChecked();
	WriteColor = ui.Writekcolorbutton->color();
	ReadColor = ui.Readkcolorbutton->color();
	ReadWriteColor = ui.ReadWritekcolorbutton->color();
	IdleColor = ui.Idlekcolorbutton->color();
	UnknownColor = ui.Unknownkcolorbutton->color();
	
	KConfigGroup cg = config();
	cg.writeEntry("PollInterval", PollInterval);
	cg.writeEntry("diskname", diskname);
	cg.writeEntry("draw_devicename", draw_devicename);
	cg.writeEntry("draw_border", draw_border);
	cg.writeEntry("use_icons", use_icons);
	cg.writeEntry("WriteColor", QVariant(WriteColor).toString());
	cg.writeEntry("ReadColor", QVariant(ReadColor).toString());
	cg.writeEntry("ReadWriteColor", QVariant(ReadWriteColor).toString());
	cg.writeEntry("IdleColor", QVariant(IdleColor).toString());
	cg.writeEntry("UnknownColor", QVariant(UnknownColor).toString());
	
	if (use_icons) {
		ReadPixmap = KIcon("virtual_hdd_led-read").pixmap(QSize(22,22));
		WritePixmap = KIcon("virtual_hdd_led-write").pixmap(QSize(22,22));
		ReadWritePixmap = KIcon("virtual_hdd_led-readwrite").pixmap(QSize(22,22));
		IdlePixmap = KIcon("virtual_hdd_led-idle").pixmap(QSize(22,22));
		UnknownPixmap = KIcon("virtual_hdd_led-unknown").pixmap(QSize(22,22));
	}

    emit configNeedsSaving();
}

void virtual_hdd_led::createConfigurationInterface(KConfigDialog *parent)
{
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	parent->addPage(widget, i18n("General"), Applet::icon());
	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
	ui.PollIntervalspinBox->setValue(PollInterval);
	int disknameindex = ui.disknamekcombobox->findText(diskname);
	if (disknameindex > -1)
		ui.disknamekcombobox->setCurrentIndex(disknameindex);
	else {
		ui.disknamekcombobox->insertItem(0, diskname);
		disknameindex = ui.disknamekcombobox->findText(diskname);
		ui.disknamekcombobox->setCurrentIndex(disknameindex);
	}
	ui.draw_devicenamecheckBox->setChecked(draw_devicename);
	ui.draw_bordercheckBox->setChecked(draw_border);
	ui.use_iconscheckBox->setChecked(use_icons);
	ui.Writekcolorbutton->setColor(WriteColor);
	ui.Readkcolorbutton->setColor(ReadColor);
	ui.ReadWritekcolorbutton->setColor(ReadWriteColor);
	ui.Idlekcolorbutton->setColor(IdleColor);
	ui.Unknownkcolorbutton->setColor(UnknownColor);
}

void virtual_hdd_led::toolTipAboutToShow()
{
	QString mainText;
	QString subText;
	
	mainText = i18n("Virtual HDD led");
	subText = i18n("Selected disk: %1", diskname);

    Plasma::ToolTipContent c(mainText, subText, KIcon("drive-harddisk"));
    Plasma::ToolTipManager::self()->setContent(this, c);
}


#include "virtual_hdd_led.moc"
