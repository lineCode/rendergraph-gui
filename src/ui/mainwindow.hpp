#pragma once
#include "imageview.hpp"
#include "networkview.hpp"
#include "standardnetworkmodel.hpp"
#include <QListView>
#include <QMainWindow>
#include <QLabel>

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

  QListView *listView;
  NetworkView *networkView;
  QLabel* connectionStatus;
  StandardNetworkModel networkModel;
  int nodeIdCounter = 0;
};

} // namespace ui
