#include "ui/mainwindow.h"
#include "QtAwesome/QtAwesome.h"
#include "ui/connectdialog.h"
#include "util/log.h"
#include "ui/nodes/nodeparams.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QOpenGLContext>

namespace ui {
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  renderOutput = new RenderOutputView;
  renderOutput->show();


  // root graph node
  root_ = std::make_unique<render::Node>("root");

  networkView = new NetworkView{ root_.get() };
  networkView->setContextMenuPolicy(Qt::CustomContextMenu);
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

  // actions
  deleteNodeAct =
	  new QAction(qtAwesome()->icon(fa::trasho), "Delete node", this);
  connect(deleteNodeAct, SIGNAL(triggered()), this,
	  SLOT(deleteSelectedNodes()));
  addNodeAct = new QAction(qtAwesome()->icon(fa::plus), "Add node", this);
  connectToServerAct =
	  new QAction{ qtAwesome()->icon(fa::wifi), "Connect to server", this };
  connect(connectToServerAct, SIGNAL(triggered()), this,
	  SLOT(connectToServer()));
  exitAct = new QAction{ "Exit" };
  connect(exitAct, SIGNAL(triggered()), this, SLOT(exit()));
  showRenderOutputAct = new QAction{ "Show render output" };
  connect(showRenderOutputAct, SIGNAL(triggered()), renderOutput, SLOT(show()));

  // menu
  auto fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(connectToServerAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);
  auto renderMenu = menuBar()->addMenu("&Render");
  renderMenu->addAction(showRenderOutputAct);

  // layout
  auto layout = new QVBoxLayout;
  // layout->addWidget(listView);
  layout->addWidget(networkView);
  layout->addWidget(buttonAddNode);
  layout->addWidget(buttonScaleUp);
  layout->addWidget(buttonScaleDown);


  // dock panels
  setDockNestingEnabled(true);

  auto networkViewDockWidget = new QDockWidget{ tr("Network view") };
  auto networkViewPanel = new QWidget;
  networkViewPanel->setLayout(layout);
  networkViewDockWidget->setWidget(networkViewPanel);
  addDockWidget(Qt::RightDockWidgetArea, networkViewDockWidget);

  auto paramDockWidget = new QDockWidget{ tr("Network view") };
  paramPanel_ = new QWidget;
  //paramPanel->setLayout(layout);
  paramDockWidget->setWidget(paramPanel_);
  addDockWidget(Qt::LeftDockWidgetArea, paramDockWidget);
  
  // status bar
  connectionStatus = new QLabel{"Status: Disconnected"};
  statusBar()->addPermanentWidget(connectionStatus);

}

void MainWindow::exit() {
	renderOutput->close();
	close();
}

void MainWindow::connectToServer() {
  ui::ConnectDialog dialog;
  dialog.exec();

  QString endpoint;
  switch (dialog.transport()) {
  case 0:
    endpoint.append("tcp://");
    break;
  case 1:
  default:
    endpoint.append("ipc://");
    break;
  }

  endpoint.append(dialog.address());
  auto endpointStr = endpoint.toStdString();
  client_ = std::make_unique<client::RendergraphClient>(
      util::StringRef{endpointStr.c_str(), endpointStr.size()});
  client_->setTimeoutMs(250);
  try {
    auto versionReply = client_->send(client::method::GetVersion{});
    util::log("versionReply status={} version={}",
              static_cast<int>(versionReply.status), versionReply.version);
	// got a valid reply
  } catch (client::RendergraphClient::TimeoutError) {
    util::log("Timeout when establishing connection");
  }
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

  // TODO
  /*
  for (const QPersistentModelIndex &n : selectedNodes) {
	networkModel.removeNode(n.row());
  }*/
}

void MainWindow::addConnection(const Node* fromConnector, const Node* toConnector) {
  //networkModel.addConnection(fromConnector, toConnector);
}

void MainWindow::scaleUp() {
  // imageView->setScale(imageView->scale() * 2.0);
}

void MainWindow::scaleDown() {
  // imageView->setScale(imageView->scale() * 0.5);
}

void MainWindow::addNode() {
  auto nodeA = render::ScreenSpaceNode::make(root_.get(), "nodeA");
  auto nodeA_UI = ui::nodes::NodeParams::make(nodeA, *networkView);
  auto nodeB = render::ScreenSpaceNode::make(root_.get(), "nodeB");
  auto nodeB_UI = ui::nodes::NodeParams::make(nodeB, *networkView);
  render::Param::make(nodeA, "testParam", "Test parameter", 0.0);
  render::Param::make(nodeA, "testParam2", "Test parameter2", 0.0);
  nodeA_UI->rebuildParamUI(paramPanel_);
  networkView->nodeAdded(nodeA);
  networkView->nodeAdded(nodeB);
}

MainWindow::~MainWindow() {}

} // namespace ui