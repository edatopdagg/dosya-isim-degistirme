#include "mainwindow.h"
#include <QApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isDarkTheme(true)
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    layout = new QVBoxLayout();
    centralWidget->setLayout(layout);

    QLabel *titleLabel = new QLabel("Toplu Dosya Yeniden Adlandırma", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3b5998;");
    layout->addWidget(titleLabel);

    btnSelectFiles = new QPushButton("Dosya Seç", this);
    connect(btnSelectFiles, &QPushButton::clicked, this, &MainWindow::onSelectFiles);
    layout->addWidget(btnSelectFiles);

    btnSelectFolder = new QPushButton("Klasör Seç", this);
    connect(btnSelectFolder, &QPushButton::clicked, this, &MainWindow::onSelectFolder);
    layout->addWidget(btnSelectFolder);

    lineEditPattern = new QLineEdit(this);
    lineEditPattern->setPlaceholderText("Yeni dosya adını girin");
    connect(lineEditPattern, &QLineEdit::textChanged, this, &MainWindow::onPatternChanged);
    layout->addWidget(lineEditPattern);

    listPreview = new QListWidget(this);
    layout->addWidget(listPreview);

    btnApply = new QPushButton("Değişiklikleri Uygula", this);
    connect(btnApply, &QPushButton::clicked, this, &MainWindow::onApplyChanges);
    layout->addWidget(btnApply);

    btnToggleTheme = new QPushButton("Tema Değiştir", this);
    connect(btnToggleTheme, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    layout->addWidget(btnToggleTheme);

    labelStatus = new QLabel("Durum: Henüz dosya seçilmedi.", this);
    labelStatus->setStyleSheet("color: #808080;");
    layout->addWidget(labelStatus);

    applyTheme();
}

MainWindow::~MainWindow() {
}

QStringList MainWindow::generatePreview() {
    QStringList previewList;
    for (const QString &filePath : selectedFiles) {
        QFileInfo fileInfo(filePath);
        QString newName = lineEditPattern->text();
        newName.replace("{name}", fileInfo.baseName());
        newName.replace("{ext}", fileInfo.suffix());
        newName.replace("{date}", QDateTime::currentDateTime().toString("yyyyMMdd"));
        previewList.append(newName);
    }
    return previewList;
}

void MainWindow::backupFile(const QString &filePath) {
    QString backupDir = QFileInfo(filePath).absolutePath() + "/backup/";
    QDir().mkpath(backupDir);
    QString backupPath = backupDir + QFileInfo(filePath).fileName();
    CopyFileW(reinterpret_cast<LPCWSTR>(filePath.utf16()), reinterpret_cast<LPCWSTR>(backupPath.utf16()), FALSE);
}

bool MainWindow::fileExists(const QString &filePath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFileW(reinterpret_cast<LPCWSTR>(filePath.utf16()), &findFileData);
    bool exists = (hFind != INVALID_HANDLE_VALUE);
    if (exists) FindClose(hFind);
    return exists;
}

void MainWindow::onSelectFiles() {
    selectedFiles = QFileDialog::getOpenFileNames(this, "Dosya Seç", "", "Tüm Dosyalar");
    if (!selectedFiles.isEmpty()) {
        labelStatus->setText(QString("Seçilen %1 dosya listelendi.").arg(selectedFiles.size()));
        onPatternChanged();
    }
}

void MainWindow::onSelectFolder() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Klasör Seç");
    if (!folderPath.isEmpty()) {
        QDir dir(folderPath);
        QStringList files = dir.entryList(QDir::Files);
        selectedFiles.clear();

        for (const QString &file : files) {
            selectedFiles.append(folderPath + "/" + file);
        }

        labelStatus->setText(QString("Klasördeki %1 dosya listelendi.").arg(selectedFiles.size()));
        onPatternChanged();
    }
}

void MainWindow::onPatternChanged() {
    listPreview->clear();
    QStringList previewList = generatePreview();
    for (const QString &newName : previewList) {
        listPreview->addItem(newName);
    }
}

void MainWindow::onApplyChanges() {
    QStringList previewList = generatePreview();
    bool success = true;

    for (int i = 0; i < selectedFiles.size(); ++i) {
        QString oldPath = selectedFiles[i];
        QFileInfo fileInfo(oldPath);
        QString newPath = fileInfo.absolutePath() + "/" + previewList[i] + "." + fileInfo.suffix();

        backupFile(oldPath);

        int counter = 1;
        while (fileExists(newPath)) {
            newPath = fileInfo.absolutePath() + "/" + previewList[i] + "_kopya" + QString::number(counter) + "." + fileInfo.suffix();
            counter++;
        }

        if (!QFile::rename(oldPath, newPath)) {
            success = false;
        }
    }

    if (success) {
        QMessageBox::information(this, "Başarılı", "Tüm dosyalar başarıyla yeniden adlandırıldı.");
    } else {
        QMessageBox::warning(this, "Hata", "Bazı dosyalar yeniden adlandırılamadı.");
    }

    selectedFiles.clear();
    listPreview->clear();
    labelStatus->setText("Durum: İşlem tamamlandı.");
}

void MainWindow::toggleTheme() {
    isDarkTheme = !isDarkTheme;
    applyTheme();
}

void MainWindow::applyTheme() {
    if (isDarkTheme) {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        qApp->setPalette(palette);
    } else {
        qApp->setPalette(QApplication::style()->standardPalette());
    }
}
