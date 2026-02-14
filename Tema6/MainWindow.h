#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QTextEdit>
#include "Graph.h"

enum class DisplayMode { 
    Initial,
    FloydWarshall, 
    Complete, 
    Kruskal, 
    TSP 
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override; 

private slots:
    void OnFloydClicked();
    void OnCompleteClicked();
    void OnKruskalClicked();
    void OnTSPClicked();

private:
    void SetupUI();

    void CreateCentralWidget();
    void CreateLayouts();
    void CreateButtons();
    void CreateInfoPanel();
    void AssembleLayout();
    void ConnectSignals();

    void LogInfo(const QString& text);

    void DrawInitialGraph(QPainter& painter);
    void DrawCompleteGraph(QPainter& painter);
    void DrawMST(QPainter& painter);
    void DrawTSP(QPainter& painter);
    void DrawNodes(QPainter& painter);

    QPointF ScalePoint(const Point& cityCoords);

private:
    Graph m_graph;
    DisplayMode currentMode = DisplayMode::Initial;

    QPushButton* m_buttonFloyd;
    QPushButton* m_buttonComplete;
    QPushButton* m_buttonKruskal;
    QPushButton* m_buttonTSP;
    QTextEdit* m_infoPanel;

    QWidget* m_centralWidget = nullptr;
    QHBoxLayout* m_mainLayout = nullptr;
    QVBoxLayout* m_leftLayout = nullptr;
    QHBoxLayout* m_buttonsLayout = nullptr;
};
