#include "include/ctabwidget.hpp"

CTabWidget::CTabWidget(const QString text, QWidget *parent){
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    this->setStyleSheet("QTabWidget::pane {border: 0;}");
    this->setStyleSheet("QTabBar::tab {background-color: #3a3a3a; color: #ebdab4; font: 16pt;}");
    this->setStyleSheet("QTabBar::tab:selected {background-color: #ebdab4; color: #3a3a3a; font: 16pt;}");
    this->setStyleSheet("QTabBar::tab:!selected {background-color: #3a3a3a; color: #ebdab4; font: 16pt;}");
    this->setStyleSheet("QTabBar::tab:hover {background-color: #ebdab4; color: #3a3a3a; font: 16pt;}");
    this->setStyleSheet("QTabBar::tab:!hover {background-color: #3a3a3a; color: #ebdab4; font: 16pt;}");
    this->setStyleSheet("QTabBar::tab:selected:hover {background-color: #ebdab4; color: #3a3a3a; font: 16pt;}");
    this->setTabPosition(QTabWidget::TabPosition::West);
    this->setTabShape(QTabWidget::TabShape::Rounded);
    this->setAutoFillBackground(false);
}

CTabWidget::~CTabWidget(){};
