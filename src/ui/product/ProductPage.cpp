#include "ProductPage.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "ui/product/ProductDialog.h"
#include "utils/MessageBoxHelper.h"

ProductPage::ProductPage(ProductService& productService, QWidget* parent)
    : QWidget(parent),
      m_productService(productService)
{
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("제품 코드, 제품명, 메모 검색");
    auto* searchButton = new QPushButton("검색", this);
    auto* addButton = new QPushButton("제품 등록", this);
    auto* editButton = new QPushButton("수정", this);
    auto* deleteButton = new QPushButton("삭제", this);

    auto* topLayout = new QHBoxLayout;
    topLayout->addWidget(m_searchEdit, 1);
    topLayout->addWidget(searchButton);
    topLayout->addWidget(addButton);
    topLayout->addWidget(editButton);
    topLayout->addWidget(deleteButton);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "제품 코드", "제품명", "메모", "수정일"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addWidget(m_table);

    connect(searchButton, &QPushButton::clicked, this, &ProductPage::refresh);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &ProductPage::refresh);
    connect(addButton, &QPushButton::clicked, this, &ProductPage::addProduct);
    connect(editButton, &QPushButton::clicked, this, &ProductPage::editProduct);
    connect(deleteButton, &QPushButton::clicked, this, &ProductPage::deleteProduct);

    refresh();
}

void ProductPage::refresh()
{
    const QList<Product> products = m_productService.products(m_searchEdit->text());
    m_table->setRowCount(products.size());
    for (int row = 0; row < products.size(); ++row) {
        const Product& p = products[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(p.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(p.code));
        m_table->setItem(row, 2, new QTableWidgetItem(p.name));
        m_table->setItem(row, 3, new QTableWidgetItem(p.memo));
        m_table->setItem(row, 4, new QTableWidgetItem(p.updatedAt));
    }
    m_table->resizeColumnsToContents();
}

int ProductPage::selectedProductId() const
{
    const auto selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;
    return m_table->item(selected.first()->row(), 0)->text().toInt();
}

void ProductPage::addProduct()
{
    ProductDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;
    Product product = dialog.product();
    if (!m_productService.save(product)) {
        MessageBoxHelper::error(this, m_productService.lastError());
        return;
    }
    refresh();
    emit productDataChanged();
}

void ProductPage::editProduct()
{
    const int id = selectedProductId();
    if (id < 0) {
        MessageBoxHelper::error(this, "수정할 제품을 선택하세요.");
        return;
    }
    Product product = m_productService.productById(id);
    ProductDialog dialog(this);
    dialog.setProduct(product);
    if (dialog.exec() != QDialog::Accepted) return;
    Product edited = dialog.product();
    if (!m_productService.save(edited)) {
        MessageBoxHelper::error(this, m_productService.lastError());
        return;
    }
    refresh();
    emit productDataChanged();
}

void ProductPage::deleteProduct()
{
    const int id = selectedProductId();
    if (id < 0) {
        MessageBoxHelper::error(this, "삭제할 제품을 선택하세요.");
        return;
    }
    if (!MessageBoxHelper::confirm(this, "선택한 제품을 삭제하시겠습니까? BOM도 함께 삭제됩니다.")) return;
    if (!m_productService.remove(id)) {
        MessageBoxHelper::error(this, m_productService.lastError());
        return;
    }
    refresh();
    emit productDataChanged();
}
