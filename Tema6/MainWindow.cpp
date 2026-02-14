#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    resize(1000, 700);
    m_graph.LoadFromFile("cities.txt");
    SetupUI();
}

void MainWindow::CreateCentralWidget() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
}

void MainWindow::CreateLayouts() {
    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(10, 10, 0, 10);
    m_mainLayout->setSpacing(0);

    m_leftLayout = new QVBoxLayout();
    m_leftLayout->setContentsMargins(0, 0, 15, 0);

    m_buttonsLayout = new QHBoxLayout();
}

void MainWindow::CreateButtons() {
    m_buttonFloyd = new QPushButton("Floyd-Warshall", this);
    m_buttonComplete = new QPushButton("Complete Graph", this);
    m_buttonKruskal = new QPushButton("Kruskal (MST)", this);
    m_buttonTSP = new QPushButton("TSP Circuit", this);

    m_buttonsLayout->addWidget(m_buttonFloyd);
    m_buttonsLayout->addWidget(m_buttonComplete);
    m_buttonsLayout->addWidget(m_buttonKruskal);
    m_buttonsLayout->addWidget(m_buttonTSP);

    m_leftLayout->addLayout(m_buttonsLayout);
    m_leftLayout->addStretch();
}

void MainWindow::CreateInfoPanel() {
    m_infoPanel = new QTextEdit(this);
    m_infoPanel->setReadOnly(true);
    m_infoPanel->setFixedWidth(300);
    m_infoPanel->setStyleSheet(
        "font-family: 'Segoe UI'; font-size: 11pt; background: #ffffff;"
    );
    m_infoPanel->setLineWrapMode(QTextEdit::WidgetWidth);
    m_infoPanel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_infoPanel->setPlaceholderText("Algorithm info...");
}

void MainWindow::AssembleLayout() {
    m_mainLayout->addLayout(m_leftLayout, 1);
    m_mainLayout->addWidget(m_infoPanel);
}

void MainWindow::OnFloydClicked() {
    m_graph.ComputeFloydWarshall();
    currentMode = DisplayMode::FloydWarshall;
    m_infoPanel->clear();

    QString html =
        "<div style='font-family: Verdana, sans-serif; font-size: 10pt; color: #2c3e50;'>"
        "<div style='color:#27ae60; font-size:16pt; font-weight:bold; margin-bottom: 10px;'>"
        "Floyd-Warshall Results</div>";

    int n = (int)m_graph.cityNames.size();
    double inf = std::numeric_limits<double>::infinity();

    for (int i = 0; i < n; ++i) {
        html += QString("<div style='margin-top:%1;'><b>%2</b> connections:</div>")
            .arg(i == 0 ? "0px" : "15px")
            .arg(QString::fromStdString(m_graph.cityNames[i]));

        html += "<div>";
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            double dist = m_graph.distanceMatrix[i][j];
            QString distText = (dist == inf)
                ? "<span style='color:#e74c3c;'>No Path</span>"
                : QString("<b>%1 km</b>").arg(dist, 0, 'f', 2);

            html += QString(
                "<div style='margin-left:15px;'>&bull; to %1: %2</div>")
                .arg(QString::fromStdString(m_graph.cityNames[j]))
                .arg(distText);
        }
        html += "</div>";
    }

    html += "</div>";
    m_infoPanel->setHtml(html);
    update();
}


void MainWindow::OnCompleteClicked() {
    currentMode = DisplayMode::Complete;
    update();
}

void MainWindow::OnKruskalClicked() {
    m_graph.ComputeFloydWarshall();
    m_graph.ComputeKruskalMST();
    currentMode = DisplayMode::Kruskal;
    m_infoPanel->clear();

    QString html =
        "<div style='font-family: Verdana, sans-serif; font-size: 10pt; color: #2c3e50;'>"
        "<div style='color:#2980b9; font-size:16pt; font-weight:bold; margin-bottom:10px;'>"
        "Kruskal MST</div>";

    double totalCost = 0.0;
    for (const auto& e : m_graph.mstEdges) {
        html += QString(
            "<div style='margin-left:15px;'>&bull; %1 &harr; %2: %3 km</div>")
            .arg(QString::fromStdString(m_graph.cityNames[e.u]))
            .arg(QString::fromStdString(m_graph.cityNames[e.v]))
            .arg(e.weight, 0, 'f', 2);
        totalCost += e.weight;
    }

    html += QString(
        "<div style='margin-top:10px; color:#c0392b;'><b>Total MST Cost: %1 km</b></div>")
        .arg(totalCost, 0, 'f', 2);

    html += "</div>";
    m_infoPanel->setHtml(html);
    update();
}

void MainWindow::OnTSPClicked() {
    if (m_graph.mstEdges.empty()) {
        m_graph.ComputeKruskalMST();
    }

    m_graph.ComputeFloydWarshall();
    m_graph.GenerateTSPSolution();

    currentMode = DisplayMode::TSP;
    m_infoPanel->clear();

    QString html =
        "<div style='font-family: Verdana, sans-serif; font-size: 10pt; color: #2c3e50;'>"
        "<div style='color:#e67e22; font-size:16pt; font-weight:bold; margin-bottom:10px;'>"
        "TSP Circuit</div>";

    QString route;
    double totalDist = 0.0;

    for (size_t i = 0; i < m_graph.tspRoute.size(); ++i) {
        int curr = m_graph.tspRoute[i];
        route += "<b>" + QString::fromStdString(m_graph.cityNames[curr]) + "</b>";

        if (i + 1 < m_graph.tspRoute.size()) {
            route += " &rarr; ";
            totalDist += m_graph.distanceMatrix[curr][m_graph.tspRoute[i + 1]];
        }
    }

    html += "<div>Full Route:</div>";
    html += QString(
        "<div style='margin-left:15px; background:#fdf2e9; padding:10px; "
        "border-left:4px solid #e67e22;'>%1</div>")
        .arg(route);

    html += QString(
        "<div style='margin-top:15px; color:#c0392b;'><b>Total TSP Distance: %1 km</b></div>")
        .arg(totalDist, 0, 'f', 2);

    html += "</div>";
    m_infoPanel->setHtml(html);
    update();
}


void MainWindow::ConnectSignals() {

    connect(m_buttonFloyd, &QPushButton::clicked, this, &MainWindow::OnFloydClicked);
    connect(m_buttonComplete, &QPushButton::clicked, this, &MainWindow::OnCompleteClicked);
    connect(m_buttonKruskal, &QPushButton::clicked, this, &MainWindow::OnKruskalClicked);
    connect(m_buttonTSP, &QPushButton::clicked, this, &MainWindow::OnTSPClicked);
}


void MainWindow::LogInfo(const QString& text)
{
    if (m_infoPanel) {
        m_infoPanel->append(text);
        m_infoPanel->moveCursor(QTextCursor::End);
    }
}

void MainWindow::SetupUI() {
    CreateCentralWidget();
    CreateLayouts();
    CreateButtons();
    CreateInfoPanel();
    AssembleLayout();
    ConnectSignals();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    update();
}

QPointF MainWindow::ScalePoint(const Point& cityCoords) {
    double minLat = 1e9, maxLat = -1e9, minLon = 1e9, maxLon = -1e9;
    for (const auto& p : m_graph.cityCoordinates) {
        if (p.x < minLat) minLat = p.x; if (p.x > maxLat) maxLat = p.x;
        if (p.y < minLon) minLon = p.y; if (p.y > maxLon) maxLon = p.y;
    }

    
    double padding = 60.0;          
    double topOffset = 100.0;       
    double rightPanelWidth = 350.0; 

    double availableWidth = width() - rightPanelWidth - (2 * padding);
    double availableHeight = height() - topOffset - (2 * padding);

    double rangeLat = (maxLat - minLat == 0) ? 1.0 : (maxLat - minLat);
    double rangeLon = (maxLon - minLon == 0) ? 1.0 : (maxLon - minLon);

    double x = padding + (cityCoords.y - minLon) * (availableWidth / rangeLon);

    double y = topOffset + padding + availableHeight - (cityCoords.x - minLat) * (availableHeight / rangeLat);

    return QPointF(x, y);
}

void MainWindow::DrawInitialGraph(QPainter& painter) {
    for (int i = 0; i < (int)m_graph.cityNames.size(); ++i) {
        for (int j = i + 1; j < (int)m_graph.cityNames.size(); ++j) {
          
            if (m_graph.initialEdges[i][j]) {
                painter.setPen(QPen(Qt::black, 2));
                QPointF p1 = ScalePoint(m_graph.cityCoordinates[i]);
                QPointF p2 = ScalePoint(m_graph.cityCoordinates[j]);
                painter.drawLine(p1, p2);

            
                painter.setPen(Qt::darkRed);
                painter.drawText((p1 + p2) / 2, QString::number(m_graph.distanceMatrix[i][j], 'f', 2));
            }
        }
    }
}

void MainWindow::DrawCompleteGraph(QPainter& painter) {
    painter.setPen(QPen(QColor(200, 200, 200, 150), 1));

    int n = (int)m_graph.cityCoordinates.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            
            painter.drawLine(ScalePoint(m_graph.cityCoordinates[i]),
                ScalePoint(m_graph.cityCoordinates[j]));
        }
    }
   
    DrawInitialGraph(painter);
}

void MainWindow::DrawMST(QPainter& painter) {
    painter.setPen(QPen(QColor(200, 200, 200, 150), 1, Qt::SolidLine));
    double inf = std::numeric_limits<double>::infinity();

    for (int i = 0; i < m_graph.nodeCount; ++i) {
        for (int j = i + 1; j < m_graph.nodeCount; ++j) {
          
            if (m_graph.distanceMatrix[i][j] != inf) {
                painter.drawLine(ScalePoint(m_graph.cityCoordinates[i]),
                    ScalePoint(m_graph.cityCoordinates[j]));
            }
        }
    }

    painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine)); 
    for (const auto& edge : m_graph.mstEdges) {
        QPointF p1 = ScalePoint(m_graph.cityCoordinates[edge.u]);
        QPointF p2 = ScalePoint(m_graph.cityCoordinates[edge.v]);
        painter.drawLine(p1, p2);

        painter.setPen(QPen(QColor(0, 0, 139)));
        painter.drawText((p1 + p2) / 2, QString::number(edge.weight, 'f', 1));
        painter.setPen(QPen(Qt::blue, 3));
    }

}

void MainWindow::DrawTSP(QPainter& painter) {
    painter.setPen(QPen(QColor(200, 200, 200, 150), 1, Qt::SolidLine));
    double inf = std::numeric_limits<double>::infinity();

    for (int i = 0; i < m_graph.nodeCount; ++i) {
        for (int j = i + 1; j < m_graph.nodeCount; ++j) {
            if (m_graph.distanceMatrix[i][j] != inf) {
                painter.drawLine(ScalePoint(m_graph.cityCoordinates[i]),
                    ScalePoint(m_graph.cityCoordinates[j]));
            }
        }
    }

    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine));
    if (m_graph.tspRoute.size() < 2) return;

    for (size_t i = 0; i < m_graph.tspRoute.size() - 1; ++i) {
        QPointF p1 = ScalePoint(m_graph.cityCoordinates[m_graph.tspRoute[i]]);
        QPointF p2 = ScalePoint(m_graph.cityCoordinates[m_graph.tspRoute[i + 1]]);
        painter.drawLine(p1, p2);

        painter.setPen(QPen(QColor(150, 0, 0))); 
        painter.drawText((p1 + p2) / 2, QString::number(m_graph.distanceMatrix[m_graph.tspRoute[i]][m_graph.tspRoute[i + 1]], 'f', 1));
        painter.setPen(QPen(Qt::red, 3));
    }
}

void MainWindow::DrawNodes(QPainter& painter) {
    for (int i = 0; i < m_graph.cityCoordinates.size(); ++i) {
        QPointF pos = ScalePoint(m_graph.cityCoordinates[i]);
        painter.setBrush(Qt::darkBlue);
        painter.setPen(Qt::black);
        painter.drawEllipse(pos, 6, 6);
        painter.drawText(pos + QPointF(10, -5), QString::fromStdString(m_graph.cityNames[i]));
    }
}

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (currentMode == DisplayMode::Initial) {
        DrawInitialGraph(painter);
    }
    else if (currentMode == DisplayMode::FloydWarshall) {
        DrawInitialGraph(painter);
    }
    else if (currentMode == DisplayMode::Complete) {
        DrawCompleteGraph(painter); 
    }
    else if (currentMode == DisplayMode::Kruskal) { 
        DrawMST(painter);  
    }
    else if (currentMode == DisplayMode::TSP) DrawTSP(painter);

    DrawNodes(painter);
}