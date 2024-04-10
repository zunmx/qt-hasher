#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <calculator.h>
#include <QDragEnterEvent>
#include <QFile>
#include <QDebug>
#include <QMimeData>
#include <QTimer>
#include <QString>
#include <QTime>
#include <QFileInfo>
#include <QCryptographicHash>
#include <qtconcurrentrun.h>
#include <QFutureWatcher>
#include <QPushButton>
#include <comp_window.h>
#include <QLineEdit>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>


QString global_status = "空闲";

QString get_file_size_string(double file_size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    double size = static_cast<double>(file_size);
    long long unsigned int i = 0;
    while (size >= 1024 && i < (sizeof(units) / sizeof(units[0]) - 1)) {
        size /= 1024;
        ++i;
    }
    return QString("%1 %2").arg(size).arg(units[i]);
}



void MainWindow::resizeEvent(QResizeEvent *event)
{
    adjustColumnSize();
}

double getProcess(){
    auto cursor = process.begin();
    int total = 0;
    double process_percent = 0;
    while(cursor!=process.end()){
        total++;
        double single = (cursor->second["crc32"]+
        cursor->second["md4"]+
        cursor->second["md5"]+
        cursor->second["sha1"]+
        cursor->second["sha256"]+
        cursor->second["sha384"]+
        cursor->second["sha512"])*1.0 / (calc_number) * 100;
        process_percent += single;
        cursor ++;
    }
    return process_percent / total;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
    QLabel * status = new QLabel();
    ui->statusbar->insertWidget(0,status);
    QLabel * calc_process = new QLabel();
    ui->statusbar->insertWidget(1,calc_process);
    QTimer * kernel = new QTimer();
    kernel->start(500);
    connect(kernel,&QTimer::timeout,[=](){
        double p = getProcess();
        if(int(p)==100){
            global_status = "已完成";
            process.clear();
        }
        status->setText("状态: " + global_status);

        if(global_status == "已完成" || global_status == "空闲" ){

            calc_process->setText("队列进度: 100%" );
        }else{

            calc_process->setText("队列进度: " + QString("%1%").arg(p));
        }
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
}


auto MainWindow::gen_obj(QString text){
    auto obj = new QLineEdit();
    obj->setText(text);
    obj->setReadOnly(true);
    obj->setStyleSheet("QLineEdit{border:0px solid gray;}QLineEdit:focus {border-width:2px;border-color:rgb(170, 170, 255);background:rgba(69, 63, 45, 255);}");


    // QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    // shadow_effect->setOffset(0, 0);
    // shadow_effect->setColor(Qt::red);
    // shadow_effect->setBlurRadius(20);
    // obj->setGraphicsEffect(shadow_effect);

    return obj;
}
void MainWindow::open_comp_window(int type, int curr_row){
    auto cursor = process.begin();
    while(cursor!=process.end()){
        if(cursor->first == QString("%1%2").arg(ui->tableWidget->item(curr_row,4)->text()).arg(curr_row)){
            double single = (cursor->second["crc32"]+
                             cursor->second["md4"]+
                             cursor->second["md5"]+
                             cursor->second["sha1"]+
                             cursor->second["sha256"]+
                             cursor->second["sha384"]+
                             cursor->second["sha512"])* 1.0 / (calc_number) * 100;
            if(single != 100){
                QMessageBox::warning(this,"稍等片刻","数据尚未计算完毕，请稍后再试");
                return;
            }
        }
        cursor++;
    }

    comp_window *comp_win = new comp_window();
    connect(this, &MainWindow::send_hash_info,comp_win, &comp_window::set_hash_info);
    emit send_hash_info(ui->tableWidget->item(curr_row,2)->text(), "file_name");
    emit send_hash_info(ui->tableWidget->item(curr_row,3)->text(), "file_size");
    emit send_hash_info(ui->tableWidget->item(curr_row,4)->text(), "file_path");
    emit send_hash_info(ui->tableWidget->item(curr_row,5)->text(), "CRC32");
    emit send_hash_info(ui->tableWidget->item(curr_row,6)->text(), "MD4");
    emit send_hash_info(ui->tableWidget->item(curr_row,7)->text(), "MD5");
    emit send_hash_info(ui->tableWidget->item(curr_row,8)->text(), "SHA1");
    emit send_hash_info(ui->tableWidget->item(curr_row,9)->text(), "SHA256");
    emit send_hash_info(ui->tableWidget->item(curr_row,10)->text(), "SHA384");
    emit send_hash_info(ui->tableWidget->item(curr_row,11)->text(), "SHA512");
    if(type==1){
        emit send_hash_info("对比", "type");
    }
    else if (type == 2){
        emit send_hash_info("查看", "type");
    }
    comp_win->show();

}
void MainWindow::Calculate(QString path){
    global_status = "正在计算";
    QFileInfo fileInfo(path);
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    QHBoxLayout *layout = new QHBoxLayout();
    auto * show_btn = new QPushButton();
    show_btn->setText("查看" );
    auto * comp_btn = new QPushButton();
    comp_btn->setText("比较");
    layout->insertWidget(0, comp_btn);
    layout->insertWidget(0, show_btn);

    connect(comp_btn,  &QPushButton::clicked,this, [&]() {
        QObject * obj = sender()->parent();
        QTableWidgetItem * item = (QTableWidgetItem *)obj;
        QWidget * widget =  (QWidget *)item;
        int x = widget->frameGeometry().x();
        int y = widget->frameGeometry().y();
        QModelIndex modelIndex = ui->tableWidget->indexAt(QPoint(x, y));
        int curr_row = modelIndex.row();
        int column = modelIndex.column();
        open_comp_window(1,curr_row);

    });

    connect(show_btn,  &QPushButton::clicked,this, [&]() {
        QObject * obj = sender()->parent();
        QTableWidgetItem * item = (QTableWidgetItem *)obj;
        QWidget * widget =  (QWidget *)item;
        int x = widget->frameGeometry().x();
        int y = widget->frameGeometry().y();
        QModelIndex modelIndex = ui->tableWidget->indexAt(QPoint(x, y));
        int curr_row = modelIndex.row();
        int column = modelIndex.column();
        open_comp_window(2,curr_row);
    });


    layout->setContentsMargins(0,0,0,0);
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(""));
    ui->tableWidget->setCellWidget(rowCount, 0,widget );


    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    ui->tableWidget->setCellWidget(rowCount, 1, gen_obj(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(fileInfo.fileName()));
    ui->tableWidget->setCellWidget(rowCount, 2, gen_obj(fileInfo.fileName()));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(get_file_size_string(fileInfo.size())));
    ui->tableWidget->setCellWidget(rowCount, 3, gen_obj(get_file_size_string(fileInfo.size())));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(fileInfo.filePath()));
    ui->tableWidget->setCellWidget(rowCount, 4, gen_obj(fileInfo.filePath()));
    adjustColumnSize();
    if(ui->checkBox_CRC32->isChecked()){

        QFuture<QString > future = QtConcurrent::run(GetCRC32, path,rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            auto item = new QTableWidgetItem(future.result());
            this->ui->tableWidget->setItem(rowCount, 5, item);
            ui->tableWidget->setCellWidget(rowCount, 5, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{

        process[QString("%1%2").arg(path).arg(rowCount)]["crc32"] = 1;
        this->ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 5, gen_obj("---"));
    }
    if(ui->checkBox_MD4->isChecked()){
        QFuture<QString > future = QtConcurrent::run(GetMD4, path,rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            this->ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(future.result()));
            ui->tableWidget->setCellWidget(rowCount, 6, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{
        process[QString("%1%2").arg(path).arg(rowCount)]["md4"] = 1;
        this->ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 6, gen_obj("---"));
    }
    if(ui->checkBox_MD5->isChecked()){
        QFuture<QString > future = QtConcurrent::run(GetMD5, path,rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            this->ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(future.result()));
            ui->tableWidget->setCellWidget(rowCount, 7, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{

        process[QString("%1%2").arg(path).arg(rowCount)]["md5"] = 1;
        this->ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 7, gen_obj("---"));
    }
    if(ui->checkBox_SHA1->isChecked()){
        QFuture<QString > future = QtConcurrent::run(GetSHA1, path,rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            this->ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem(future.result()));
            ui->tableWidget->setCellWidget(rowCount, 8, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{
        process[QString("%1%2").arg(path).arg(rowCount)]["sha1"] = 1;
        this->ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 8, gen_obj("---"));
    }
    if(ui->checkBox_SHA256->isChecked()){
        QFuture<QString > future = QtConcurrent::run(GetSHA256, path,rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            this->ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem(future.result()));
            ui->tableWidget->setCellWidget(rowCount, 9, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{
        process[QString("%1%2").arg(path).arg(rowCount)]["sha256"] = 1;
        this->ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 9, gen_obj("---"));
    }
    if(ui->checkBox_SHA384->isChecked()){
        QFuture<QString > future = QtConcurrent::run(GetSHA384, path,rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            this->ui->tableWidget->setItem(rowCount, 10, new QTableWidgetItem(future.result()));
            ui->tableWidget->setCellWidget(rowCount, 10, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{
        process[QString("%1%2").arg(path).arg(rowCount)]["sha384"] = 1;
        this->ui->tableWidget->setItem(rowCount, 10, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 10, gen_obj("---"));
    }


    if(ui->checkBox_SHA512->isChecked()){
        QFuture<QString > future = QtConcurrent::run(GetSHA512, path, rowCount);
        QFutureWatcher<QString> *m_watcher = new QFutureWatcher<QString>(this);
        m_watcher = new QFutureWatcher<QString>(this);
        m_watcher->setFuture(future);
        connect(m_watcher, &QFutureWatcher<QString>::finished, this, [=](){
            this->ui->tableWidget->setItem(rowCount, 11, new QTableWidgetItem(future.result()));
            ui->tableWidget->setCellWidget(rowCount, 11, gen_obj(future.result()));
            adjustColumnSize();
        });
    }else{
        process[QString("%1%2").arg(path).arg(rowCount)]["sha512"] = 1;
        this->ui->tableWidget->setItem(rowCount, 11, new QTableWidgetItem("---"));
        ui->tableWidget->setCellWidget(rowCount, 11, gen_obj("---"));
    }



}
void MainWindow::dropEvent(QDropEvent *e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;
    foreach (QUrl u, urls)
    {
        QString filepath = u.toLocalFile();
        pathlist.append(filepath);
    }
    for(int i=0;i<pathlist.size();++i)
    {
        QString path = pathlist.at(i);
        Calculate(path);
    }
    pathlist.clear();

}

void MainWindow::adjustColumnSize(){
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}
void MainWindow::columnSwitcher(int id, int flag){
    if(flag==2) ui->tableWidget->showColumn(id+4);
    else  ui->tableWidget->hideColumn(id+4);
}

void MainWindow::on_checkBox_SHA512_stateChanged(int arg1)
{
    columnSwitcher(7,arg1);
}


void MainWindow::on_checkBox_SHA384_stateChanged(int arg1)
{
    columnSwitcher(6,arg1);
}


void MainWindow::on_checkBox_SHA256_stateChanged(int arg1)
{
    columnSwitcher(5,arg1);
}


void MainWindow::on_checkBox_SHA1_stateChanged(int arg1)
{
    columnSwitcher(4,arg1);
}


void MainWindow::on_checkBox_MD5_stateChanged(int arg1)
{
    columnSwitcher(3,arg1);
}


void MainWindow::on_checkBox_MD4_stateChanged(int arg1)
{
    columnSwitcher(2,arg1);
}


void MainWindow::on_checkBox_CRC32_stateChanged(int arg1)
{
    columnSwitcher(1,arg1);
}


void MainWindow::on_checkBox_calc_time_stateChanged(int arg1)
{
    columnSwitcher(-3,arg1);
}


void MainWindow::on_checkBox_file_name_stateChanged(int arg1)
{
    columnSwitcher(-2,arg1);
}


void MainWindow::on_checkBox_file_size_stateChanged(int arg1)
{
    columnSwitcher(-1,arg1);
}


void MainWindow::on_checkBox_file_path_stateChanged(int arg1)
{
    columnSwitcher(0,arg1);
}


