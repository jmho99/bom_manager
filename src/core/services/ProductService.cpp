#include "ProductService.h"

ProductService::ProductService(ProductRepository& productRepository)
    : m_productRepository(productRepository)
{
}

QList<Product> ProductService::products(const QString& keyword)
{
    return m_productRepository.findAll(keyword);
}

Product ProductService::productById(int id)
{
    return m_productRepository.findById(id);
}

bool ProductService::save(Product& product)
{
    if (product.code.trimmed().isEmpty()) {
        m_lastError = "제품 코드를 입력하세요.";
        return false;
    }
    if (product.name.trimmed().isEmpty()) {
        m_lastError = "제품명을 입력하세요.";
        return false;
    }
    if (m_productRepository.existsByCode(product.code, product.id)) {
        m_lastError = "이미 존재하는 제품 코드입니다.";
        return false;
    }
    if (!m_productRepository.save(product)) {
        m_lastError = m_productRepository.lastError();
        return false;
    }
    return true;
}

bool ProductService::remove(int id)
{
    if (id < 0) {
        m_lastError = "삭제할 제품이 선택되지 않았습니다.";
        return false;
    }
    if (!m_productRepository.remove(id)) {
        m_lastError = m_productRepository.lastError();
        return false;
    }
    return true;
}

QString ProductService::lastError() const
{
    return m_lastError;
}
