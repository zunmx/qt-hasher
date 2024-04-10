#include "comp_window.h"
#include "ui_comp_window.h"
#include "QClipboard"

int comp_style = false;

comp_window::comp_window(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::comp_window)
{
    ui->setupUi(this);
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString originalText = clipboard->text().trimmed();
    ui->lineEdit_need_comp->setText(originalText);
    ui->lineEdit_need_comp->setFocus();
}

comp_window::~comp_window()
{
    comp_style = false;
    delete ui;
}
void comp_window::draw_comp_result(){
    QString need = ui->lineEdit_need_comp->text();
    QString CRC32 = ui->lineEdit_CRC32->text();
    QString MD4 = ui->lineEdit_MD4->text();
    QString MD5 = ui->lineEdit_MD5->text();
    QString SHA1 = ui->lineEdit_SHA1->text();
    QString SHA256 = ui->lineEdit_SHA256->text();
    QString SHA384 = ui->lineEdit_SHA384->text();
    QString SHA512 = ui->lineEdit_SHA512->text();
    if(need == CRC32){
        ui->lineEdit_CRC32->setGraphicsEffect(gen_style(1));
    }else{
        ui->lineEdit_CRC32->setGraphicsEffect(gen_style(2));
    }

    if(need == MD4){
        ui->lineEdit_MD4->setGraphicsEffect(gen_style(1));
    }else{
        ui->lineEdit_MD4->setGraphicsEffect(gen_style(2));
    }

    if(need == MD5){
        ui->lineEdit_MD5->setGraphicsEffect(gen_style(1));
    }else{
        ui->lineEdit_MD5->setGraphicsEffect(gen_style(2));
    }

    if(need == SHA1){
        ui->lineEdit_SHA1->setGraphicsEffect(gen_style(1));
    }else{
        ui->lineEdit_SHA1->setGraphicsEffect(gen_style(2));
    }

    if(need == SHA256){
        ui->lineEdit_SHA256->setGraphicsEffect(gen_style(1));
    }else{
        ui->lineEdit_SHA256->setGraphicsEffect(gen_style(2));
    }

    if(need == SHA384){
        ui->lineEdit_SHA384->setGraphicsEffect(gen_style(1));
    }else{

        ui->lineEdit_SHA384->setGraphicsEffect(gen_style(2));
    }

    if(need == SHA512){
        ui->lineEdit_SHA512->setGraphicsEffect(gen_style(1));
    }else{

        ui->lineEdit_SHA512->setGraphicsEffect(gen_style(2));
    }
}


QGraphicsDropShadowEffect * comp_window::gen_style(int style){
    QGraphicsDropShadowEffect * effect = new QGraphicsDropShadowEffect(this);

    effect->setOffset(0, 0);
    effect->setBlurRadius(10);
    if(style==1){
        effect->setColor(Qt::green);
    }
    if(style==2){
        effect->setColor(Qt::red);
    }
    return effect;
}
void comp_window::set_hash_info(const QString &str, const QString &type)
{

    this->setWindowTitle(str);
    if(type=="type" && str == "查看"){
        ui->label->hide();
        ui->lineEdit_need_comp->hide();
        ui->lineEdit_CRC32->setGraphicsEffect(gen_style(0));
        ui->lineEdit_MD5->setGraphicsEffect(gen_style(0));
        ui->lineEdit_MD4->setGraphicsEffect(gen_style(0));
        ui->lineEdit_SHA1->setGraphicsEffect(gen_style(0));
        ui->lineEdit_SHA256->setGraphicsEffect(gen_style(0));
        ui->lineEdit_SHA384->setGraphicsEffect(gen_style(0));
        ui->lineEdit_SHA512->setGraphicsEffect(gen_style(0));
    }
    if(type=="file_name")
        ui->lineEdit_file_name->setText(str);
    if(type=="file_size")
        ui->lineEdit_file_size->setText(str);
    if(type=="file_path")
        ui->lineEdit_file_path->setText(str);
    if(type=="CRC32")
        ui->lineEdit_CRC32->setText(str);
    if(type=="MD4")
        ui->lineEdit_MD4->setText(str);
    if(type=="MD5")
        ui->lineEdit_MD5->setText(str);
    if(type=="SHA1")
        ui->lineEdit_SHA1->setText(str);
    if(type=="SHA256")
        ui->lineEdit_SHA256->setText(str);
    if(type=="SHA384")
        ui->lineEdit_SHA384->setText(str);
    if(type=="SHA512")
        ui->lineEdit_SHA512->setText(str);

    if(type=="type" && str=="对比"){
        comp_style = true;
        draw_comp_result();
    }

}

void comp_window::on_lineEdit_need_comp_textChanged(const QString &arg1)
{
    if(comp_style)
        draw_comp_result();

}

