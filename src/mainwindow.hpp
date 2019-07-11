#pragma once
#include "imageview.hpp"
#include "networkview.hpp"
#include "standardnetworkmodel.hpp"
#include <QListView>
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private Q_SLOTS:
  void scaleUp();
  void scaleDown();
  void addNode();
  void showNetworkViewContextMenu(const QPoint &);
  void deleteSelectedNodes();
  void addConnection(QPersistentModelIndex fromConnector, QPersistentModelIndex toConnector);

private:
  QAction *deleteNodeAction;
  QAction *addNodeAction;
  QListView *listView;
  ImageView *imageView;
  NetworkView *networkView;
  StandardNetworkModel networkModel;
  int nodeIdCounter = 0;
};
