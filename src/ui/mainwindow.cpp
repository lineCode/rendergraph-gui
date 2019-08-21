#include "ui/mainwindow.h"
#include "QtAwesome/QtAwesome.h"
#include "ui/connectdialog.h"
#include "ui/nodes/nodeparams.h"
#include "util/log.h"
#include "img/outputnode.h"
#include <QAction>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QOpenGLContext>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>

using node::Input;
using node::Node;
using node::Output;

namespace ui {
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  renderOutput = new RenderOutputView;
  renderOutput->show();

  // root graph node
  root_ = std::make_unique<img::ImgNetwork>(nullptr, "root", *renderTargetCache_);

  networkView = new NetworkView{root_.get()};
  networkView->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(networkView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(showNetworkViewContextMenu(const QPoint &)));
  connect(networkView, &NetworkView::connectionRequest, this,
          &MainWindow::addConnection);

  auto buttonScaleUp = new QPushButton("+");
  connect(buttonScaleUp, SIGNAL(released()), this, SLOT(scaleUp()));

  auto buttonScaleDown = new QPushButton("-");
  connect(buttonScaleDown, SIGNAL(released()), this, SLOT(scaleDown()));

  auto buttonAddNode = new QPushButton("Add Node");
  connect(buttonAddNode, SIGNAL(released()), this, SLOT(addNode()));

  // actions
  deleteNodeAct =
      new QAction(qtAwesome()->icon(fa::trasho), "Delete Node", this);
  connect(deleteNodeAct, SIGNAL(triggered()), this,
          SLOT(deleteSelectedNodes()));

  addNodeAct = new QAction(qtAwesome()->icon(fa::plus), "Add Node", this);
  connect(addNodeAct, &QAction::triggered, this, &MainWindow::addNode);

  addOutputNodeAct = new QAction(qtAwesome()->icon(fa::plus), "Add Output Node", this);
  connect(addOutputNodeAct, &QAction::triggered, this, &MainWindow::addOutputNode);

  connectToServerAct =
      new QAction{qtAwesome()->icon(fa::wifi), "Connect to Server", this};

  connect(connectToServerAct, SIGNAL(triggered()), this,
          SLOT(connectToServer()));

  exitAct = new QAction{"Exit"};
  connect(exitAct, SIGNAL(triggered()), this, SLOT(exit()));

  showRenderOutputAct = new QAction{"Show Render Output"};
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

  auto networkViewDockWidget = new QDockWidget{tr("Network View")};
  auto networkViewPanel = new QWidget;
  networkViewPanel->setLayout(layout);
  networkViewDockWidget->setWidget(networkViewPanel);
  addDockWidget(Qt::RightDockWidgetArea, networkViewDockWidget);

  auto paramDockWidget = new QDockWidget{tr("Network View")};
  paramPanel_ = new QWidget;
  // paramPanel->setLayout(layout);
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
	contextMenu.addAction(addOutputNodeAct);
    contextMenu.exec(networkView->mapToGlobal(pos));
  }
}

void MainWindow::deleteSelectedNodes() {
  auto selectedNodes = networkView->selectedNodes();
  root_->deleteChildren(util::ArrayRef<Node *const>{
      (size_t)selectedNodes.size(), selectedNodes.data()});
}

void MainWindow::addConnection(node::Node *from, node::Output *output,
                               node::Node *to, node::Input *input) {
  root_->addConnection(from, output, to, input);
}

void MainWindow::scaleUp() {
  // imageView->setScale(imageView->scale() * 2.0);
}

void MainWindow::scaleDown() {
  // imageView->setScale(imageView->scale() * 0.5);
}

void MainWindow::addOutputNode() {
	auto n = img::OutputNode::make(*root_, "output");
}

void MainWindow::addNode() {
	auto n = img::ImgNode::make(*root_, "screen");
	n->createParameter("testParam", "Test Parameter", 0.0);
	n->createParameter("testParam2", "Test Parameter 2", 0.0);
	n->createInput("input0");
	n->createInput("input1");
	n->createOutput("output0");
}

MainWindow::~MainWindow() {}

} // namespace ui