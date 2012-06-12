#ifndef GSDIALOGCREATESUBGRIDSLICE_H
#define GSDIALOGCREATESUBGRIDSLICE_H

#include <QDialog>

class GS3DGrid;

namespace Ui {
    class GSDialogCreateSubGridSlice;
}

class GSDialogCreateSubGridSlice : public QDialog
{
    Q_OBJECT

public:
    explicit GSDialogCreateSubGridSlice(GS3DGrid *p3DGrid, QWidget *parent = 0);
    ~GSDialogCreateSubGridSlice();

    QVector<int> getSelectedI();
    QVector<int> getSelectedJ();
    QVector<int> getSelectedK();

private:
    void initUi();

private slots:
    void slotListWidget1SelectionChanged();
    void slotListWidget2SelectionChanged();
    void slotListWidget3SelectionChanged();

private:
    Ui::GSDialogCreateSubGridSlice *ui;
    GS3DGrid *m_p3DGrid;
    QVector<int> m_vi, m_vj, m_vk;
};

#endif // GSDIALOGCREATESUBGRIDSLICE_H
