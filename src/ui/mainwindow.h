#pragma once
#include "client/client.h"
#include "gfx/gfx.h"
#include "gfxopengl/opengl.h"
#include "imageview.h"
#include "img/imgnetwork.h"
#include "img/imgnode.h"
#include "img/rendertarget.h"
#include "networkview.h"
#include "node/network.h"
#include "standardnetworkmodel.h"
#include "ui/renderoutputview.h"
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QOpenGLWidget>

namespace ui {

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private Q_SLOTS:
  void exit();
  void scaleUp();
  void scaleDown();
  void showNetworkViewContextMenu(const QPoint &);
  void deleteSelectedNodes();
  void addConnection(node::Node *from, node::Output *output, node::Node *to,
                     node::Input *input);

  void addNode(node::NodeTemplate &blueprint);
  void saveNetwork();
  void registerNodes();

private:
  QAction *deleteNodeAct;
  QAction *exitAct;
  QAction *showRenderOutputAct;
  QAction *saveAct;

  QListView *listView;
  NetworkView *networkView;
  RenderOutputView *renderOutput;
  QWidget *paramPanel_;

  QLabel *connectionStatus;
  bool connected_ = false;

  int nodeIdCounter = 0;
  std::unique_ptr<client::RendergraphClient> client_;
  img::ImgNetwork::Ptr root_;
  img::RenderTargetCache::Ptr renderTargetCache_;
};

} // namespace ui
