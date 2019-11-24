	/***************************************************************************
 *   Copyright 2010 by Christoph Thielecke <crissi99@gmx.de>               *
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

#ifndef VIRTUAL_HDD_LED_HEADER
#define VIRTUAL_HDD_LED_HEADER

#include <Plasma/Applet>
#include <Plasma/Svg>
#include <QColor>
#include <QTimer>
#include <QObject>
#include <KIcon>
#include <QPixmap>
#include "ui_virtual_hdd_ledConfig.h"

class QSizeF;

// Define our plasma Applet
class virtual_hdd_led : public Plasma::Applet
{
Q_OBJECT
    public:
        /** constructor */
        virtual_hdd_led(QObject *parent, const QVariantList &args);
		
		/** destructor */
        ~virtual_hdd_led();

        /** The paintInterface procedure paints the applet to screen */
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
		
		/** initial setup of the applet */
        void init();
		

    private:
        Plasma::Svg m_svg;
        KIcon m_icon;
		QPixmap pm; //< pixmap for drawing led/icon
		QColor StatusColor; //< active color
		QColor ReadColor; //< color when in read state
		QColor WriteColor; //< color when in write state
		QColor ReadWriteColor; //< color when in read and write state
		QColor IdleColor; //< color when in idle state
		QColor UnknownColor; //< color when in unknown (or disk not found) state
		
		/** status definitions */
		enum DiskStatus {
			Read = 0,
			Write = 1,
			Idle = 2,
			ReadWrite = 3,
			Unknown = 10
		};
		
		DiskStatus Status; //< status the disk
		bool diskFound; //< true if the selected disk was found
		bool use_sysfs; //< true if sysfs should used instead of proc
		bool use_icons; //< true if icons should be used (unfinished)
		bool draw_devicename; //< true if devicename should be draw under led
		bool draw_border; //< true if the led should be have a black border
		QTimer testtimer; //< timer for fetching data
		QString diskname; //< name of the selected disk (e.g. sda)
		unsigned long last_rx; //< last stored rx data
		unsigned long last_tx; //< last stored tx data
		FILE *id; //< file pointer
		int PollInterval; //< interval for polling data (milliseconds)
		// Configuration dialog
		Ui::virtual_hdd_ledConfig ui; //< gui
		QPixmap ReadPixmap; //< Pixmap for read status
		QPixmap WritePixmap; //< Pixmap for write status
		QPixmap ReadWritePixmap; //< Pixmap for read and write status
		QPixmap IdlePixmap; //< Pixmap for idle status
		QPixmap UnknownPixmap; //< Pixmap for unknown status
		QPixmap StatusPixmap; //< Pixmap for current status
		
public slots:
	
	/** update status, called by timer */
	void hddDataUpdate();
	
	/** load config */
	void configChanged();
	
	/** display config dialog */
	void createConfigurationInterface(KConfigDialog *parent);

protected slots:
	/** save config */
	void configAccepted();

public Q_SLOTS:
	/** set tooltip data */
	void toolTipAboutToShow();
};

K_EXPORT_PLASMA_APPLET(virtual_hdd_led, virtual_hdd_led)
#endif
