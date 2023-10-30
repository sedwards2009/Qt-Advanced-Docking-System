/*******************************************************************************
 ** Qt Advanced Docking System
 ** Copyright (C) 2017 Uwe Kindler
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2.1 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

//============================================================================
/// \file   FloatingWidgetTitleBar.cpp
/// \author Uwe Kindler
/// \date   13.05.2019
/// \brief  Implementation of CFloatingWidgetTitleBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingWidgetTitleBar.h"

#include <iostream>

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QPixmap>
#include <QStyle>
#include <QMouseEvent>
#include <QWindow>

#include "ads_globals.h"
#include "ElidingLabel.h"
#include "FloatingDockContainer.h"

namespace ads
{

using tTabLabel = CElidingLabel;
using tCloseButton = QPushButton;
using tMaximizeButton  = QPushButton;
using tMinimizeButton = QPushButton;

/**
 * @brief Private data class of public interface CFloatingWidgetTitleBar
 */
struct FloatingWidgetTitleBarPrivate
{
	CFloatingWidgetTitleBar *_this; ///< public interface class
	QLabel *IconLabel = nullptr;
	tTabLabel *TitleLabel;
	tCloseButton *CloseButton = nullptr;
    tMinimizeButton* MinimizeButton = nullptr;
    tMaximizeButton* MaximizeButton = nullptr;
	CFloatingDockContainer *FloatingWidget = nullptr;
    QIcon MinimizeIcon;
    QIcon MaximizeIcon;
    QIcon NormalIcon;
    bool Maximized = false;

	FloatingWidgetTitleBarPrivate(CFloatingWidgetTitleBar *_public) :
		_this(_public)
	{
	}

	/**
	 * Creates the complete layout including all controls
	 */
	void createLayout(bool showTitle);
};

//============================================================================
void FloatingWidgetTitleBarPrivate::createLayout(bool showTitle)
{
	TitleLabel = new tTabLabel();
	TitleLabel->setElideMode(Qt::ElideRight);
	TitleLabel->setText("DockWidget->windowTitle()");
	TitleLabel->setObjectName("floatingTitleLabel");
    TitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	CloseButton = new tCloseButton();
	CloseButton->setObjectName("floatingTitleCloseButton");

	MinimizeButton = new tMinimizeButton();
	MinimizeButton->setObjectName("floatingTitleMinimizeButton");

	MaximizeButton = new tMaximizeButton();
	MaximizeButton->setObjectName("floatingTitleMaximizeButton");

	// The standard icons do does not look good on high DPI screens
	QIcon CloseIcon;
	QPixmap normalPixmap = _this->style()->standardPixmap(
	    QStyle::SP_TitleBarCloseButton, 0, CloseButton);
	CloseIcon.addPixmap(normalPixmap, QIcon::Normal);
	CloseIcon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25),
	    QIcon::Disabled);
	CloseButton->setIcon(
	    _this->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	CloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	CloseButton->setVisible(true);
	CloseButton->setFocusPolicy(Qt::NoFocus);
	_this->connect(CloseButton, SIGNAL(clicked()), SIGNAL(closeRequested()));

	MinimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	MinimizeButton->setVisible(true);
	MinimizeButton->setFocusPolicy(Qt::NoFocus);
	_this->connect(MinimizeButton, &QPushButton::clicked, _this, &CFloatingWidgetTitleBar::minimizeRequested);
	MinimizeButton->setIcon(MinimizeIcon);

	_this->setMaximizedIcon(false);
	MaximizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	MaximizeButton->setVisible(true);
	MaximizeButton->setFocusPolicy(Qt::NoFocus);
	_this->connect(MaximizeButton, &QPushButton::clicked, _this, &CFloatingWidgetTitleBar::maximizeRequested);

	QFontMetrics fm(TitleLabel->font());
	int Spacing = qRound(fm.height() / 4.0);

	// Fill the layout
	QBoxLayout *Layout = new QBoxLayout(QBoxLayout::LeftToRight);
	Layout->setContentsMargins(0, 0, 0, 0);
	Layout->setSpacing(0);
	_this->setLayout(Layout);
	if (showTitle)
	{
		Layout->addWidget(TitleLabel, 1);
		Layout->addSpacing(Spacing);
	}
	else
	{
		Layout->addStretch(1);
	}
	Layout->addWidget(MinimizeButton);
    Layout->addWidget(MaximizeButton);
	Layout->addWidget(CloseButton);
	Layout->setAlignment(Qt::AlignCenter);

	TitleLabel->setVisible(showTitle);
}

//============================================================================
CFloatingWidgetTitleBar::CFloatingWidgetTitleBar(CFloatingDockContainer *parent, bool showTitle) :
    QFrame(parent),
	d(new FloatingWidgetTitleBarPrivate(this))
{
	d->FloatingWidget = parent;
	d->createLayout(showTitle);

    auto normalPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarNormalButton, 0, d->MaximizeButton);
    d->NormalIcon.addPixmap(normalPixmap, QIcon::Normal);
    d->NormalIcon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);

    auto maxPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarMaxButton, 0, d->MaximizeButton);
    d->MaximizeIcon.addPixmap(maxPixmap, QIcon::Normal);
    d->MaximizeIcon.addPixmap(internal::createTransparentPixmap(maxPixmap, 0.25), QIcon::Disabled);
    setMaximizedIcon(d->Maximized);

    auto minPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarMinButton, 0, d->MinimizeButton);
    d->MinimizeIcon.addPixmap(minPixmap, QIcon::Normal);
    d->MinimizeIcon.addPixmap(internal::createTransparentPixmap(minPixmap, 0.25), QIcon::Disabled);
    setMinimizeIcon(d->MinimizeIcon);
}

//============================================================================
CFloatingWidgetTitleBar::~CFloatingWidgetTitleBar()
{
	delete d;
}

//============================================================================
void CFloatingWidgetTitleBar::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		d->FloatingWidget->window()->windowHandle()->startSystemMove();
		return;
	}
	Super::mousePressEvent(ev);
}

//============================================================================
void CFloatingWidgetTitleBar::enableCloseButton(bool Enable)
{
	d->CloseButton->setEnabled(Enable);
}

//============================================================================
void CFloatingWidgetTitleBar::setTitle(const QString &Text)
{
	d->TitleLabel->setText(Text);
}

//============================================================================
void CFloatingWidgetTitleBar::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

//============================================================================
void CFloatingWidgetTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        emit maximizeRequested();
        event->accept();
    }
    else
    {
        QWidget::mouseDoubleClickEvent(event);
    }
}

//============================================================================
void CFloatingWidgetTitleBar::setMaximizedIcon(bool maximized)
{
    d->Maximized = maximized;
    if (maximized)
    {
        d->MaximizeButton->setIcon(d->NormalIcon);
    }
    else
    {
        d->MaximizeButton->setIcon(d->MaximizeIcon);
    }
}

//============================================================================
void CFloatingWidgetTitleBar::setMinimizeIcon(const QIcon& Icon)
{
    d->MinimizeIcon = Icon;
    d->MinimizeButton->setIcon(d->MinimizeIcon);
}

//============================================================================
void CFloatingWidgetTitleBar::setMaximizeIcon(const QIcon& Icon)
{
    d->MaximizeIcon = Icon;
    if (d->Maximized)
    {
        setMaximizedIcon(d->Maximized);
    }
}

//============================================================================
void CFloatingWidgetTitleBar::setNormalIcon(const QIcon& Icon)
{
    d->NormalIcon = Icon;
    if (!d->Maximized)
    {
        setMaximizedIcon(d->Maximized);
    }
}

//============================================================================
QIcon CFloatingWidgetTitleBar::minimizeIcon() const
{
    return d->MinimizeIcon;
}

//============================================================================
QIcon CFloatingWidgetTitleBar::maximizeIcon() const
{
    return d->MaximizeIcon;
}

//============================================================================
QIcon CFloatingWidgetTitleBar::normalIcon() const
{
    return d->NormalIcon;
}

} // namespace ads
