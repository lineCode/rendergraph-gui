#pragma once
#include <QMainWindow>
#include <QListView>
#include "imageview.hpp"
#include "networkview.hpp"
#include "standardnetworkmodel.hpp"

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private Q_SLOTS:
	void scaleUp();
	void scaleDown();
	void addNode();

private:
	QListView* listView;
	ImageView* imageView;
	NetworkView* networkView;
	StandardNetworkModel networkModel;
	int nodeIdCounter = 0;
};
