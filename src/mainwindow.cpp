#include "mainwindow.hpp"
#include "QtAwesome/QtAwesome.h"
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  listView = new QListView;
  deleteNodeAction =
      new QAction(qtAwesome()->icon(fa::trasho), "Delete node", this);
  connect(deleteNodeAction, SIGNAL(triggered()), this,
          SLOT(deleteSelectedNodes()));
  addNodeAction = new QAction(qtAwesome()->icon(fa::plus), "Add node", this);

  imageView = new ImageView;
  QImage image;
  image.load("test.jpg");
  imageView->setImage(image);
  imageView->setMinimumHeight(200);

  networkView = new NetworkView;
  networkView->setModel(&networkModel);
  networkView->setContextMenuPolicy(Qt::CustomContextMenu);
  networkModel.insertNode(0);
  connect(networkView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(showNetworkViewContextMenu(const QPoint &)));

  auto buttonScaleUp = new QPushButton("+");
  connect(buttonScaleUp, SIGNAL(released()), this, SLOT(scaleUp()));

  auto buttonScaleDown = new QPushButton("-");
  connect(buttonScaleDown, SIGNAL(released()), this, SLOT(scaleDown()));

  auto buttonAddNode = new QPushButton("Add node");
  connect(buttonAddNode, SIGNAL(released()), this, SLOT(addNode()));

  // layout
  auto layout = new QVBoxLayout;
  // layout->addWidget(listView);
  layout->addWidget(networkView);
  layout->addWidget(buttonAddNode);
  layout->addWidget(buttonScaleUp);
  layout->addWidget(buttonScaleDown);
  layout->addWidget(imageView);

  // setup central widget
  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);
}

void MainWindow::showNetworkViewContextMenu(const QPoint &pos) {
  // Show different menus if there is a selection or not.
  auto selectedNodes = networkView->selectedNodes();
  if (!selectedNodes.empty()) {
    QMenu contextMenu;
    contextMenu.addAction(deleteNodeAction);
    contextMenu.exec(networkView->mapToGlobal(pos));
  } else {
    QMenu contextMenu;
    contextMenu.addAction(addNodeAction);
    contextMenu.exec(networkView->mapToGlobal(pos));
  }
}

void MainWindow::deleteSelectedNodes() {
  auto selectedNodes = networkView->selectedNodes();
  for (const QPersistentModelIndex &n : selectedNodes) {
    networkModel.removeNode(n.row());
  }
}

void MainWindow::addConnection(QPersistentModelIndex fromConnector, QPersistentModelIndex toConnector)
{
	networkModel.addConnection(fromConnector, toConnector);
}

void MainWindow::scaleUp() { imageView->setScale(imageView->scale() * 2.0); }

void MainWindow::scaleDown() { imageView->setScale(imageView->scale() * 0.5); }

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