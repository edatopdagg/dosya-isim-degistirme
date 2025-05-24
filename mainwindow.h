#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <windows.h>
#include <QStyleFactory>
#include <QPalette>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectFiles();
    void onSelectFolder();
    void onPatternChanged();
    void onApplyChanges();
    void toggleTheme();

private:
    QWidget *centralWidget;
    QVBoxLayout *layout;
    QPushButton *btnSelectFiles;
    QPushButton *btnSelectFolder;
    QLineEdit *lineEditPattern;
    QListWidget *listPreview;
    QPushButton *btnApply;
    QPushButton *btnToggleTheme;
    QLabel *labelStatus;
    QStringList selectedFiles;
    bool isDarkTheme;

    QStringList generatePreview();
    void backupFile(const QString &filePath);
    bool fileExists(const QString &filePath);
    void applyTheme();
};

#endif // MAINWINDOW_H
