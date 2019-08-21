#pragma once
#include "imageview.h"
#include "networkview.h"
#include "standardnetworkmodel.h"
#include "ui/renderoutputview.h"
#include "gfx/gfx.h"
#include "gfxopengl/opengl.h"
#include "node/network.h"
#include "img/rendertarget.h"
#include "img/screenspacenetwork.h"
#include "img/screenspacenode.h"
#include "client/client.h"
#include <QListView>
#include <QMainWindow>
#include <QLabel>
#include <QOpenGLWidget>

namespace ui {

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private Q_SLOTS:
	void connectToServer();
	void exit();
  void scaleUp();
  void scaleDown();
  void showNetworkViewContextMenu(const QPoint &);
  void deleteSelectedNodes();
  void addConnection(node::Node* from, node::Output* output, node::Node *to, node::Input* input);

  void addNode();
  void addOutputNode();

private:
  QAction* connectToServerAct;
  QAction *deleteNodeAct;
  QAction* exitAct;
  QAction* showRenderOutputAct;

  QAction *addNodeAct;
  QAction *addOutputNodeAct;

  QListView *listView;
  NetworkView *networkView;
  RenderOutputView* renderOutput;
  QWidget* paramPanel_;

  QLabel* connectionStatus;
  bool connected_ = false;

  int nodeIdCounter = 0;
  std::unique_ptr<client::RendergraphClient> client_;
  img::ImgNetwork::Ptr root_;
  img::RenderTargetCache::Ptr renderTargetCache_;
};

} // namespace ui
