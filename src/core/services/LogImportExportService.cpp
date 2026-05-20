#include "LogImportExportService.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

#include "utils/DateTimeUtils.h"

LogImportExportService::LogImportExportService(ProductRepository& productRepository,
                                               MaterialRepository& materialRepository,
                                               BomRepository& bomRepository,
                                               InventoryEventRepository& eventRepository)
    : m_productRepository(productRepository),
      m_materialRepository(materialRepository),
      m_bomRepository(bomRepository),
      m_eventRepository(eventRepository)
{
}

bool LogImportExportService::exportJsonl(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "파일을 열 수 없습니다: " + filePath;
        return false;
    }

    QTextStream out(&file);

    for (const Product& product : m_productRepository.findAll()) {
        QJsonObject obj;
        obj["record_type"] = "product";
        obj["code"] = product.code;
        obj["name"] = product.name;
        obj["memo"] = product.memo;
        out << QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)) << '\n';
    }

    for (const Material& material : m_materialRepository.findAll()) {
        QJsonObject obj;
        obj["record_type"] = "material";
        obj["code"] = material.code;
        obj["name"] = material.name;
        obj["unit"] = material.unit;
        obj["location"] = material.location;
        obj["memo"] = material.memo;
        out << QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)) << '\n';
    }

    for (const BomItem& item : m_bomRepository.findAll()) {
        QJsonObject obj;
        obj["record_type"] = "bom_item";
        obj["product_code"] = item.productCode;
        obj["product_name"] = item.productName;
        obj["material_code"] = item.materialCode;
        obj["material_name"] = item.materialName;
        obj["unit"] = item.unit;
        obj["quantity_per_set"] = item.quantityPerSet;
        out << QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)) << '\n';
    }

    for (const InventoryEvent& event : m_eventRepository.findAll()) {
        QJsonObject obj;
        obj["record_type"] = "inventory_event";
        obj["event_uuid"] = event.eventUuid;
        obj["event_type"] = event.eventType;
        obj["material_code"] = event.materialCode;
        obj["material_name"] = event.materialName;
        obj["material_unit"] = event.materialUnit;
        obj["product_code"] = event.productCode;
        obj["product_name"] = event.productName;
        obj["quantity_delta"] = event.quantityDelta;
        obj["event_date"] = event.eventDate;
        obj["created_at"] = event.createdAt;
        obj["source_device"] = event.sourceDevice;
        obj["memo"] = event.memo;
        out << QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)) << '\n';
    }

    return true;
}

bool LogImportExportService::importJsonl(const QString& filePath, int* importedEventCount, int* skippedEventCount)
{
    if (importedEventCount) *importedEventCount = 0;
    if (skippedEventCount) *skippedEventCount = 0;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "파일을 열 수 없습니다: " + filePath;
        return false;
    }

    QList<QJsonObject> products;
    QList<QJsonObject> materials;
    QList<QJsonObject> bomItems;
    QList<QJsonObject> events;

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        const QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
        if (!doc.isObject()) continue;

        const QJsonObject obj = doc.object();
        const QString recordType = obj.value("record_type").toString();
        if (recordType == "product") products.append(obj);
        else if (recordType == "material") materials.append(obj);
        else if (recordType == "bom_item") bomItems.append(obj);
        else if (recordType == "inventory_event") events.append(obj);
    }

    for (const QJsonObject& obj : products) {
        Product product;
        product.code = obj.value("code").toString();
        product.name = obj.value("name").toString();
        product.memo = obj.value("memo").toString();
        if (product.code.trimmed().isEmpty()) continue;
        if (product.name.trimmed().isEmpty()) product.name = product.code;
        if (!m_productRepository.upsertByCode(product)) {
            m_lastError = m_productRepository.lastError();
            return false;
        }
    }

    for (const QJsonObject& obj : materials) {
        Material material;
        material.code = obj.value("code").toString();
        material.name = obj.value("name").toString();
        material.unit = obj.value("unit").toString("ea");
        material.location = obj.value("location").toString();
        material.memo = obj.value("memo").toString();
        if (material.code.trimmed().isEmpty()) continue;
        if (material.name.trimmed().isEmpty()) material.name = material.code;
        if (!m_materialRepository.upsertByCode(material)) {
            m_lastError = m_materialRepository.lastError();
            return false;
        }
    }

    for (const QJsonObject& obj : bomItems) {
        Product product = m_productRepository.findByCode(obj.value("product_code").toString());
        Material material = m_materialRepository.findByCode(obj.value("material_code").toString());
        if (!product.isValid() || !material.isValid()) continue;
        const double quantityPerSet = obj.value("quantity_per_set").toDouble();
        if (quantityPerSet <= 0.0) continue;
        if (!m_bomRepository.upsert(product.id, material.id, quantityPerSet)) {
            m_lastError = m_bomRepository.lastError();
            return false;
        }
    }

    for (const QJsonObject& obj : events) {
        const QString eventUuid = obj.value("event_uuid").toString();
        if (eventUuid.trimmed().isEmpty()) continue;
        if (m_eventRepository.existsByUuid(eventUuid)) {
            if (skippedEventCount) (*skippedEventCount)++;
            continue;
        }

        Material material = m_materialRepository.findByCode(obj.value("material_code").toString());
        if (!material.isValid()) {
            material.code = obj.value("material_code").toString();
            material.name = obj.value("material_name").toString(material.code);
            material.unit = obj.value("material_unit").toString("ea");
            if (!m_materialRepository.upsertByCode(material)) {
                m_lastError = m_materialRepository.lastError();
                return false;
            }
        }

        Product product;
        const QString productCode = obj.value("product_code").toString();
        if (!productCode.trimmed().isEmpty()) {
            product = m_productRepository.findByCode(productCode);
            if (!product.isValid()) {
                product.code = productCode;
                product.name = obj.value("product_name").toString(productCode);
                if (!m_productRepository.upsertByCode(product)) {
                    m_lastError = m_productRepository.lastError();
                    return false;
                }
            }
        }

        InventoryEvent event;
        event.eventUuid = eventUuid;
        event.eventType = obj.value("event_type").toString();
        event.materialId = material.id;
        event.materialCode = material.code;
        event.materialName = material.name;
        event.materialUnit = material.unit;
        event.productId = product.isValid() ? product.id : -1;
        event.productCode = product.isValid() ? product.code : QString();
        event.productName = product.isValid() ? product.name : QString();
        event.quantityDelta = obj.value("quantity_delta").toDouble();
        event.eventDate = obj.value("event_date").toString(DateTimeUtils::todayIsoDate());
        event.createdAt = obj.value("created_at").toString(DateTimeUtils::nowIsoString());
        event.sourceDevice = obj.value("source_device").toString();
        event.memo = obj.value("memo").toString();

        if (!m_eventRepository.insert(event)) {
            m_lastError = m_eventRepository.lastError();
            return false;
        }
        if (importedEventCount) (*importedEventCount)++;
    }

    return true;
}

QString LogImportExportService::lastError() const
{
    return m_lastError;
}
