#include "ui/mainwindow.hpp"
#include "ui/connectdialog.hpp"
#include "QtAwesome/QtAwesome.h"
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QMenuBar>

namespace ui {
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  deleteNodeAct =
      new QAction(qtAwesome()->icon(fa::trasho), "Delete node", this);
  connect(deleteNodeAct, SIGNAL(triggered()), this,
          SLOT(deleteSelectedNodes()));
  addNodeAct = new QAction(qtAwesome()->icon(fa::plus), "Add node", this);
  connectToServerAct = new QAction{ qtAwesome()->icon(fa::wifi), "Connect to server", this };
  connect(connectToServerAct, SIGNAL(triggered()), this, SLOT(connectToServer()));
  exitAct = new QAction{ "Exit" };
  connect(exitAct, SIGNAL(triggered()), this, SLOT(exit()));

  networkView = new NetworkView;
  networkView->setModel(&networkModel);
  networkView->setContextMenuPolicy(Qt::CustomContextMenu);
  networkModel.insertNode(0);
  connect(networkView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(showNetworkViewContextMenu(const QPoint &)));
  connect(
      networkView,
      SIGNAL(connectionRequest(QPersistentModelIndex, QPersistentModelIndex)),
      this, SLOT(addConnection(QPersistentModelIndex, QPersistentModelIndex)));

  auto buttonScaleUp = new QPushButton("+");
  connect(buttonScaleUp, SIGNAL(released()), this, SLOT(scaleUp()));

  auto buttonScaleDown = new QPushButton("-");
  connect(buttonScaleDown, SIGNAL(released()), this, SLOT(scaleDown()));

  auto buttonAddNode = new QPushButton("Add node");
  connect(buttonAddNode, SIGNAL(released()), this, SLOT(addNode()));

  // menu
  auto fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(connectToServerAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);


  // layout
  auto layout = new QVBoxLayout;
  // layout->addWidget(listView);
  layout->addWidget(networkView);
  layout->addWidget(buttonAddNode);
  layout->addWidget(buttonScaleUp);
  layout->addWidget(buttonScaleDown);

  // setup central widget
  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  // status bar
  connectionStatus = new QLabel{ "Status: Disconnected" };
  statusBar()->addPermanentWidget(connectionStatus);
}

void MainWindow::exit() {

}

void MainWindow::connectToServer() {
	ui::ConnectDialog dialog;
	dialog.exec();
}

void MainWindow::showNetworkViewContextMenu(const QPoint &pos) {
  // Show different menus if there is a selection or not.
  auto selectedNodes = networkView->selectedNodes();
  if (!selectedNodes.empty()) {
    QMenu contextMenu;
    contextMenu.addAction(deleteNodeAct);
    contextMenu.exec(networkView->mapToGlobal(pos));
  } else {
    QMenu contextMenu;
    contextMenu.addAction(addNodeAct);
    contextMenu.exec(networkView->mapToGlobal(pos));
  }
}

void MainWindow::deleteSelectedNodes() {
  auto selectedNodes = networkView->selectedNodes();
  for (const QPersistentModelIndex &n : selectedNodes) {
    networkModel.removeNode(n.row());
  }
}

void MainWindow::addConnection(QPersistentModelIndex fromConnector,
                               QPersistentModelIndex toConnector) {
  networkModel.addConnection(fromConnector, toConnector);
}

void MainWindow::scaleUp() {
  // imageView->setScale(imageView->scale() * 2.0);
}

void MainWindow::scaleDown() {
  // imageView->setScale(imageView->scale() * 0.5);
}

void MainWindow::addNode() {
  networkModel.insertNode(0);
  networkModel.insertInputConnector(networkModel.nodeIndex(0), 0);
  networkModel.insertInputConnector(networkModel.nodeIndex(0), 0);
  networkModel.insertInputConnector(networkModel.nodeIndex(0), 0);
  networkModel.insertInputConnector(networkModel.nodeIndex(0), 0);
  networkModel.insertInputConnector(networkModel.nodeIndex(0), 0);
  networkModel.insertOutputConnector(networkModel.nodeIndex(0), 0);
  networkModel.insertOutputConnector(networkModel.nodeIndex(0), 0);
}

MainWindow::~MainWindow() {}

} // namespace ui