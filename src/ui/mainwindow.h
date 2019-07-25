#pragma once
#include "imageview.h"
#include "networkview.h"
#include "standardnetworkmodel.h"
#include "ui/renderoutputview.h"
#include "gfx/gfx.h"
#include "gfxopengl/opengl.h"
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
  void addNode();
  void showNetworkViewContextMenu(const QPoint &);
  void deleteSelectedNodes();
  void addConnection(QPersistentModelIndex fromConnector,
                     QPersistentModelIndex toConnector);

private:
  QAction* connectToServerAct;
  QAction *deleteNodeAct;
  QAction *addNodeAct;
  QAction* exitAct;
  QAction* showRenderOutputAct;

  QListView *listView;
  NetworkView *networkView;
  RenderOutputView* renderOutput;

  QLabel* connectionStatus;
  StandardNetworkModel networkModel;
  int nodeIdCounter = 0;
  bool connected_ = false;  
  std::unique_ptr<client::RendergraphClient> client_;
};

} // namespace ui
