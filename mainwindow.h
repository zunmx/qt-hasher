#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void Calculate(QString path);
    auto gen_obj(QString text);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void columnSwitcher(int id, int flag);

private slots:

    void on_checkBox_SHA384_stateChanged(int arg1);
    void on_checkBox_SHA512_stateChanged(int arg1);
    void on_checkBox_SHA256_stateChanged(int arg1);
    void on_checkBox_SHA1_stateChanged(int arg1);
    void on_checkBox_MD5_stateChanged(int arg1);
    void on_checkBox_MD4_stateChanged(int arg1);
    void on_checkBox_CRC32_stateChanged(int arg1);
    void on_checkBox_calc_time_stateChanged(int arg1);
    void on_checkBox_file_name_stateChanged(int arg1);
    void on_checkBox_file_size_stateChanged(int arg1);
    void on_checkBox_file_path_stateChanged(int arg1);
signals:
    void send_hash_info(const QString &str, const QString &type);

private:
    Ui::MainWindow *ui;
    QList<QString> pathlist;
    void adjustColumnSize();
    virtual void resizeEvent(QResizeEvent* event) override;
    void open_comp_window(int type,int curr_row);
};

#endif // MAINWINDOW_H
