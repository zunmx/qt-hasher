#ifndef COMP_WINDOW_H
#define COMP_WINDOW_H

#include "QGraphicsDropShadowEffect"
#include <QDialog>

namespace Ui {
class comp_window;
}

class comp_window : public QDialog
{
    Q_OBJECT

public:
    explicit comp_window(QWidget *parent = nullptr);
    void draw_comp_result();
    ~comp_window();
public slots:

    void set_hash_info(const QString &str, const QString &type);
private slots:
    void on_lineEdit_need_comp_textChanged(const QString &arg1);


private:
    Ui::comp_window *ui;
    QGraphicsDropShadowEffect * gen_style(int style);
};

#endif // COMP_WINDOW_H
