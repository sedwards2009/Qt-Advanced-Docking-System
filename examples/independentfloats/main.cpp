#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include "DockManager.h"
#include "DockContainerWidget.h"
#include "../../examples/simple/MainWindow.h"


int counter = 0;

ads::CDockWidget* createTerminal(ads::CDockManager* dockManager, QWidget* mainWidget)
{
    ads::CDockWidget* dockWidget = new ads::CDockWidget(QString("[%1] /home/sbe").arg(counter));
    counter++;

    QWidget *widget = new QWidget();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    widget->setLayout(layout);

    QLabel* label = new QLabel();
    label->setText("Terminal contents");
    layout->addWidget(label);

    QPushButton* button = new QPushButton();
    button->setText("New Terminal");

    QObject::connect(button, &QAbstractButton::clicked, [=]() {
        ads::CDockWidget* term = createTerminal(dockManager, mainWidget);
        ads::CDockContainerWidget* dockContainer = dockWidget->dockContainer();
        dockManager->addDockWidgetToContainer(ads::CenterDockWidgetArea, term, dockContainer);
    });

    layout->addWidget(button);

    QPushButton* closeMainButton = new QPushButton();
    closeMainButton->setText("Close Main Window");
    QObject::connect(closeMainButton, &QAbstractButton::clicked, [=]() {
        mainWidget->hide();
    });
    layout->addWidget(closeMainButton);

    dockWidget->setWidget(widget);
    return dockWidget;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QWidget mainWindow;

    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FloatingContainerIndependent, true);

    ads::CDockManager dockManager(&mainWindow);

    ads::CDockWidget* term = createTerminal(&dockManager, &mainWindow);
    ads::CFloatingDockContainer* floating = dockManager.addDockWidgetFloating(term);
    floating->show();

    // mainWindow.show();

    return a.exec();
}
