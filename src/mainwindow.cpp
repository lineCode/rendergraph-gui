#include "mainwindow.hpp"
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	listView = new QListView;
	imageView = new ImageView;
	networkView = new NetworkView;

	QImage image;
	image.load("test.jpg");
	imageView->setImage(image);
	imageView->setMinimumHeight(200);
	auto buttonScaleUp = new QPushButton("+");
	auto buttonScaleDown = new QPushButton("-");
	connect(buttonScaleUp, SIGNAL(released()), this, SLOT(scaleUp()));
	connect(buttonScaleDown, SIGNAL(released()), this, SLOT(scaleDown()));
	networkView->setModel(&networkModel);
	auto buttonAddNode = new QPushButton("Add node");
	connect(buttonAddNode, SIGNAL(released()), this, SLOT(addNode()));

	networkModel.addNode(AbstractNetworkModel::NodeID{nodeIdCounter++});

	// layout
	auto layout = new QVBoxLayout;
	//layout->addWidget(listView);
	layout->addWidget(networkView);
	layout->addWidget(buttonAddNode);
	layout->addWidget(buttonScaleUp);
	layout->addWidget(buttonScaleDown);
	layout->addWidget(imageView);

	// setup central widget
	setCentralWidget(new QWidget);
	centralWidget()->setLayout(layout);
}

void MainWindow::scaleUp() {
	imageView->setScale(imageView->scale() * 2.0);
}

void MainWindow::scaleDown() {
	imageView->setScale(imageView->scale() * 0.5);
}

void MainWindow::addNode()
{
	networkModel.addNode(AbstractNetworkModel::NodeID{nodeIdCounter++});
}

MainWindow::~MainWindow() {
}