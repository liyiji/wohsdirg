
#include "GSDialogCreateSubGridSlice.h"
#include "ui_GSDialogCreateSubGridSlice.h"
#include "GS3DGrid.h"

GSDialogCreateSubGridSlice::GSDialogCreateSubGridSlice(GS3DGrid *p3DGrid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSDialogCreateSubGridSlice)
{
    ui->setupUi(this);
    m_p3DGrid = p3DGrid;
    m_vi.clear();
    m_vj.clear();
    m_vk.clear();
    initUi();
}

GSDialogCreateSubGridSlice::~GSDialogCreateSubGridSlice()
{
    delete ui;
}

QVector<int> GSDialogCreateSubGridSlice::getSelectedI()
{
    return m_vi;
}

QVector<int> GSDialogCreateSubGridSlice::getSelectedJ()
{
    return m_vj;
}

QVector<int> GSDialogCreateSubGridSlice::getSelectedK()
{
    return m_vk;
}

void GSDialogCreateSubGridSlice::initUi()
{
    ui->lineEdit->setText(QString::number(0));
    ui->lineEdit_2->setText(QString::number(0));
    ui->lineEdit_3->setText(QString::number(0));

    ui->checkBox->setChecked(false);
    ui->checkBox_2->setChecked(false);
    ui->checkBox_3->setChecked(false);
    ui->listWidget->setEnabled(ui->checkBox->isChecked());
    ui->listWidget_2->setEnabled(ui->checkBox_2->isChecked());
    ui->listWidget_3->setEnabled(ui->checkBox_3->isChecked());
    ui->lineEdit->setEnabled(ui->checkBox->isChecked());
    ui->lineEdit_2->setEnabled(ui->checkBox_2->isChecked());
    ui->lineEdit_3->setEnabled(ui->checkBox_3->isChecked());
    connect(ui->checkBox, SIGNAL(toggled(bool)),
            ui->listWidget, SLOT(setEnabled(bool)));
    connect(ui->checkBox_2, SIGNAL(toggled(bool)),
            ui->listWidget_2, SLOT(setEnabled(bool)));
    connect(ui->checkBox_3, SIGNAL(toggled(bool)),
            ui->listWidget_3, SLOT(setEnabled(bool)));
    connect(ui->checkBox, SIGNAL(toggled(bool)),
            ui->lineEdit, SLOT(setEnabled(bool)));
    connect(ui->checkBox_2, SIGNAL(toggled(bool)),
            ui->lineEdit_2, SLOT(setEnabled(bool)));
    connect(ui->checkBox_3, SIGNAL(toggled(bool)),
            ui->lineEdit_3, SLOT(setEnabled(bool)));

    ui->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listWidget_2->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listWidget_3->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->listWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotListWidget1SelectionChanged()));
    connect(ui->listWidget_2, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotListWidget2SelectionChanged()));
    connect(ui->listWidget_3, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotListWidget3SelectionChanged()));

    if (m_p3DGrid)
    {
        int ni = m_p3DGrid->getICount();
        int nj = m_p3DGrid->getJCount();
        int nk = m_p3DGrid->getKCount();

        for (int i = 0; i < ni; i++)
        {
            ui->listWidget->addItem(QString::number(i));
        }
        for (int j = 0; j < nj; j++)
        {
            ui->listWidget_2->addItem(QString::number(j));
        }
        for (int k = 0; k < nk; k++)
        {
            ui->listWidget_3->addItem(QString::number(k));
        }
    }
}

void GSDialogCreateSubGridSlice::slotListWidget1SelectionChanged()
{
    m_vi.clear();
    QList<QListWidgetItem *> vec = ui->listWidget->selectedItems();
    for (int i = 0; i < vec.size(); i++)
    {
        m_vi.append(vec[i]->text().toInt());
    }
    ui->lineEdit->setText(QString::number(vec.size()));
}

void GSDialogCreateSubGridSlice::slotListWidget2SelectionChanged()
{
    m_vj.clear();
    QList<QListWidgetItem *> vec = ui->listWidget_2->selectedItems();
    for (int i = 0; i < vec.size(); i++)
    {
        m_vj.append(vec[i]->text().toInt());
    }
    ui->lineEdit_2->setText(QString::number(vec.size()));
}

void GSDialogCreateSubGridSlice::slotListWidget3SelectionChanged()
{
    m_vk.clear();
    QList<QListWidgetItem *> vec = ui->listWidget_3->selectedItems();
    for (int i = 0; i < vec.size(); i++)
    {
        m_vk.append(vec[i]->text().toInt());
    }
    ui->lineEdit_3->setText(QString::number(vec.size()));
}
