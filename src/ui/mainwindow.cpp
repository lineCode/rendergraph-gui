#include "ui/mainwindow.h"
#include "QtAwesome/QtAwesome.h"
#include "img/imgshadernode.h"
#include "img/imgoutput.h"
#include "img/imgclear.h"
#include "ui/connectdialog.h"
#include "node/description.h"
#include "ui/nodes/nodeparams.h"
#include "util/log.h"
#include "util/jsonwriter.h"
#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QOpenGLContext>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <fstream>

using node::Input;
using node::Node;
using node::Output;

namespace ui {
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  renderOutput = new RenderOutputView;
  renderOutput->show();

  // root graph node
  root_ =
      std::make_unique<img::ImgNetwork>("root");

  networkView = new NetworkView{root_.get()};
  networkView->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(networkView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(showNetworkViewContextMenu(const QPoint &)));
  connect(networkView, &NetworkView::connectionRequest, this,
          &MainWindow::addConnection);

  // actions
  deleteNodeAct =
      new QAction(qtAwesome()->icon(fa::trasho), "Delete Node", this);
  connect(deleteNodeAct, SIGNAL(triggered()), this,
          SLOT(deleteSelectedNodes()));

  saveAct = new QAction{qtAwesome()->icon(fa::save), "Save Network...", this};
  connect(saveAct, &QAction::triggered, this, &MainWindow::saveNetwork);

  exitAct = new QAction{"Exit"};
  connect(exitAct, SIGNAL(triggered()), this, SLOT(exit()));

  showRenderOutputAct = new QAction{"Show Render Output"};
  connect(showRenderOutputAct, SIGNAL(triggered()), renderOutput, SLOT(show()));

  // menu
  auto fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);
  auto renderMenu = menuBar()->addMenu("&Render");
  renderMenu->addAction(showRenderOutputAct);

  // layout
  auto layout = new QVBoxLayout;
  layout->addWidget(networkView);

  // dock panels
  setDockNestingEnabled(true);

  auto networkViewDockWidget = new QDockWidget{tr("Network View")};
  auto networkViewPanel = new QWidget;
  networkViewPanel->setLayout(layout);
  networkViewDockWidget->setWidget(networkViewPanel);
  addDockWidget(Qt::RightDockWidgetArea, networkViewDockWidget);

  auto paramDockWidget = new QDockWidget{tr("Parameters")};
  paramPanel_ = new QWidget;
  // paramPanel->setLayout(layout);
  paramDockWidget->setWidget(paramPanel_);
  addDockWidget(Qt::LeftDockWidgetArea, paramDockWidget);

  // status bar
  connectionStatus = new QLabel{"Status: ?"};
  statusBar()->addPermanentWidget(connectionStatus);

  registerNodes();
}

void MainWindow::registerNodes() {
	// IMG nodes
	img::ImgShaderNode::registerNode();
	img::ImgOutput::registerNode();
	img::ImgClear::registerNode();
}

void MainWindow::exit() {
  renderOutput->close();
  close();
}

void MainWindow::saveNetwork() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Network"), QString(), tr("Rendergraph network (*.rnet)"));
  std::ofstream fileOut{ fileName.toStdString(), std::ios::trunc | std::ios::binary };
  util::JsonWriter writer{ fileOut };
  root_->save(writer);
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
	node::NodeDescriptions& currentNetworkNodes = root_->registeredNodes();
	int n = currentNetworkNodes.count();
	for (int i = 0; i < n; ++i) {
	  node::NodeDescription& bp = currentNetworkNodes.at(i);
	  auto act = contextMenu.addAction(QString::fromStdString(bp.friendlyName().to_string()));
	  connect(act, &QAction::triggered, this, [this, &bp] () {
		  addNode(bp);
	  });
	}

    contextMenu.exec(networkView->mapToGlobal(pos));
  }
}

void MainWindow::deleteSelectedNodes() {
  auto selectedNodes = networkView->selectedNodes();
  root_->deleteChildren(util::ArrayRef<Node *const>{
      selectedNodes.data(), (size_t)selectedNodes.size()});
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

void MainWindow::addNode(node::NodeDescription& blueprint) {
	auto typeName = blueprint.typeName().to_string();
	root_->createNode(typeName, typeName);
}

MainWindow::~MainWindow() {}

} // namespace ui